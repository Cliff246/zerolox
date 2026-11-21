import os
from pathlib import Path
import subprocess
import tempfile
import unittest

from assembler import Error, assemble
from image import build_image


def binary(source):
    return build_image(assemble(source))


class EncodingTests(unittest.TestCase):
    def test_ultra_encoding(self):
        plain = binary('n.add t1, t2, t3')
        self.assertEqual(len(plain), 4)
        self.assertEqual(plain[2] & 63, 0)
        self.assertEqual(plain, binary('add t1, t2, t3 ; amv z0'))
        for move, bits in [('amv t0', 16), ('mva t0', 17)]:
            self.assertEqual(binary('add t1, t2, t3 ; ' + move)[2] & 63, bits)

    def test_invalid_combinations(self):
        for source in ('n.add t0, t1, t2 ; amv t0', 'add ac, t1, t2 ; mva t0',
                       'm.add t0, t1 ; amv t2', 'andi t0, ac, 4 ; amv t1',
                       'inc t0 ; amv t2', 'bieqc+ x+, 2 ; amv t1', 'sext ac, t1, 0',
                       'sext ac, t1, 33', 'bineqc+ x, 2', 'bieqc+ y-, 3'):
            with self.subTest(source=source), self.assertRaises(Error):
                binary(source)


@unittest.skipUnless(os.environ.get('ZEROLOX_EMULATOR'), 'set ZEROLOX_EMULATOR to a built C emulator')
class ExecutionTests(unittest.TestCase):
    def execute(self, data, steps):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / 'image.bin'
            path.write_bytes(data)
            return subprocess.run([os.environ['ZEROLOX_EMULATOR'], str(path), str(steps)],
                                  text=True, capture_output=True)

    def check_program(self, source, expected, steps=None):
        result = self.execute(binary(source), steps or len(source.splitlines()))
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        regs = dict(line.split('=') for line in result.stdout.splitlines() if line.startswith('r'))
        self.assertEqual({r: int(regs[f'r{r}']) for r in expected}, expected)

    def test_move_order_and_forwarding(self):
        self.check_program('m.geta 7\nm.get8 t1, 3\nadd ac, t0, t1 ; amv t0',
                           {1: 10, 8: 7})
        self.check_program('m.geta 19\nm.get8 t0, 5\nsub t0, ac, t0 ; mva t0',
                           {1: 5, 8: 0})
        self.check_program('m.geta 7\nadd t1, t0, t0 ; amv t0\nn.add ac, z0, t1',
                           {0: 0, 1: 14, 8: 7, 9: 14})
        self.check_program('m.geta 7\nn.add z0, ac, ac\nadd t0, ac, ac ; mva z0',
                           {0: 0, 1: 0, 8: 0})

    def test_invalid_raw_images(self):
        base = int.from_bytes(binary('n.add t0, t1, t2')[2:], 'little')
        faults = [(binary('n.add t0, t1, t2')[:2] + (base | 16).to_bytes(2,'little'), 'w1 == w2'),
                  (binary('n.add ac, t1, t2')[:2] + ((base & 2047) | (1<<11) | 17).to_bytes(2,'little'), 'w1 == w2')]
        for major in (12, 13, 1023):
            faults.append(((major << 6).to_bytes(2,'little'), 'could not decode'))
        faults.append((((31 << 11) | 1).to_bytes(2,'little'), 'could not decode'))
        for width in (0,33):
            second = ((width & 31) << 11) | ((width >> 5) << 6)
            faults.append((binary('sext z0, ac, 1')[:2] + second.to_bytes(2,'little'), 'sext width'))
        for data, reason in faults:
            with self.subTest(image=data.hex()):
                result = self.execute(data, 1)
                self.assertNotEqual(result.returncode, 0)
                self.assertIn(reason, result.stdout + result.stderr)

    def test_fixed_counts(self):
        for n in range(1, 9):
            for op, result in [('sl', 0x80000001 << n), ('sr', 0x80000001 >> n),
                               ('sra', -2147483647 >> n), ('inc', 0x80000001 + n),
                               ('dec', 0x80000001 - n)]:
                with self.subTest(op=op, n=n):
                    self.check_program(f'get32 t0, 0x80000001\n{op}{n} t0', {8: result & 0xffffffff})

    def test_accumulator_and_immediates(self):
        for op, value in [('and', 2), ('or', 15), ('xor', 13)]:
            self.check_program(f'm.geta 10\nm.get8 t0, 7\n{op}a t0', {1:value, 8:7})
            self.check_program(f'm.geta 10\nm.get8 t0, 7\na{op} t0', {1:10, 8:value})
        self.check_program('m.geta 0\ndeca 1\nsrra 31\nsrla 31\nslla 3\ninca 31', {1:39})
        self.check_program('get32 t0, 0xffffff70\nsext t1, t0, 8\nsext t2, t0, 7\nsext t3, t0, 32\n'
                           'ori t4, z0, -1024\nxori t5, z0, 1023\nslli ac, t5, 2047',
                           {1:1<<31, 9:112, 10:0xfffffff0, 11:0xffffff70, 12:0xfffffc00, 13:1023})

    def test_iterator_branch_uses_old_value(self):
        for pair, ir, cr in [('x', 4, 6), ('y', 5, 7)]:
            for op in ('bieqc+', 'bineqc+'):
                for sign, delta in [('+', 1), ('-', -1)]:
                    for counter in (0, delta):
                        taken = (counter == 0) == (op == 'bieqc+')
                        source = (f'get32 r{cr}, {counter & 0xffffffff}\n'
                                  f'{op} {pair}{sign}, #done\nm.geta 9\ndone:\ninc t0')
                        self.check_program(source, {1:0 if taken else 9, ir:delta & 0xffffffff,
                                                    cr:counter & 0xffffffff, 8:1}, 3 if taken else 4)
