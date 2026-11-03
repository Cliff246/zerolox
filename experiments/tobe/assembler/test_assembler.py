import json
import unittest
from assembler import Error, assemble, link
from image import build_image

class DesignCases(unittest.TestCase):
    def test_selection_and_roundtrip(self):
        source = 'add ac, r8, r9\nadd r8, ac, r9\nadd r8, r9, r10\nld.16 ac, [r8, 3s]\nld.16 ac, [r8, 3]\nld.32 ac, [r8, -8]\n'
        obj = assemble(source)
        a = link(obj)
        self.assertEqual(a, link(json.loads(json.dumps(obj))))
        self.assertEqual([x[2] for x in a[0]], ['m.add','m.adda','n.add','m.lda.16','n.ld.16','n.ld.32'])

    def test_strict_minor_and_types(self):
        for source in ('m.lda.16 [r8, 3]', 'X = r1 + 10\nld.8 ac, [r8, $X]', 'ld.8 ac, [r8, r9]'):
            with self.subTest(source=source), self.assertRaises(Error): link(assemble(source))

    def test_bytes_and_branch(self):
        listing, symbols = link(assemble('start:\nb #start\n!db "hi"\nend:\n'))
        self.assertEqual(listing[0][:3], (0,2,'m.b'))
        self.assertEqual(symbols['end']['value'], 5)

    def test_odd_instruction_rejected(self):
        with self.assertRaisesRegex(Error, 'odd byte index'):
            link(assemble('!db 1\nadd ac, r8, r9'))

    def test_data_and_alignment(self):
        _, symbols = link(assemble('COUNT = 3\n!db { 65, 66, 0 }\n%align 4\nword:\n!dw { 0b1010, $COUNT * 10 + 5 }\nend:\n'))
        self.assertEqual(symbols['word']['value'], 4)
        self.assertEqual(symbols['end']['value'], 12)

    def test_string_rules(self):
        for source in ('!dw "hello"', '!db "é"', '!db 256', '!dw 4294967296', '!db u8"hi"'):
            with self.subTest(source=source), self.assertRaises(Error): link(assemble(source))
        _, symbols = link(assemble('!db "A\\xff"\nend:'))
        self.assertEqual(symbols['end']['value'], 3)

    def test_memory_forms_and_forward_data(self):
        source = ('ld.16 t0, [ac, 2s]\n'
                  'st.16 ac, [t0, 2s]\n'
                  'st.16 t0, [ac, 2s]\n'
                  'st.16 t0, [t1, -1]\n'
                  '!dw &end\nend:\n')
        listing, symbols = link(assemble(source))
        self.assertEqual([x[2] for x in listing],
                         ['m.ald.16', 'm.sta.16', 'm.ast.16', 'n.st.16'])
        self.assertEqual(symbols['end']['value'], 14)

    def test_example_program(self):
        from pathlib import Path
        source = Path(__file__).with_name('program.zlx').read_text()
        listing, symbols = link(assemble(source))
        self.assertGreater(len(listing), 10)
        self.assertEqual(symbols['values']['value'] % 4, 0)

    def test_image_bytes(self):
        image = build_image(assemble('m.get8 t0, 7\nm.geta -1\n!db "A"\n!dw 0x12345678'))
        self.assertEqual(image, bytes.fromhex('0f 28 ff 27 41 00 78 56 34 12'))

    def test_invalid_image_does_not_succeed(self):
        with self.assertRaises(Error):
            build_image(assemble('m.lda.16 [t0, 3]'))

    def test_emulator_programs_if_available(self):
        import os, subprocess, tempfile
        from pathlib import Path
        emulator = os.environ.get('ZEROLOX_EMULATOR')
        if not emulator: self.skipTest('set ZEROLOX_EMULATOR to a built C emulator')
        cases = [
            ('m.get8 t0, 7\nm.get8 t1, 3\nadd ac, t0, t1\nadd t2, ac, t1\nadd t3, t0, t1\n', 5, {1:10, 10:13, 11:10}),
            ('m.get8 t0, 64\nm.get8 t1, 123\nst.8 t1, [t0]\nld.8 ac, [t0]\n', 4, {1:123}),
            ('m.geta 0\nbeqz #target\nm.geta 99\ntarget:\nm.geta 7\n', 3, {1:7}),
            ('m.get8 t0, 80\nm.get8 t1, 77\nst.8 t1, [t0, -1]\nld.8 ac, [t0, -1]\n', 4, {1:77}),
            ('m.geta 80\nm.get8 t1, 42\nst.8 t1, [ac, 2]\nld.8 t2, [ac, 2]\n', 4, {10:42}),
        ]
        with tempfile.TemporaryDirectory() as directory:
            image_path=Path(directory)/'image.bin'
            for source, steps, expected in cases:
                with self.subTest(source=source):
                    image_path.write_bytes(build_image(assemble(source)))
                    result=subprocess.run([emulator,str(image_path),str(steps)],check=True,capture_output=True,text=True)
                    registers=dict(line.split('=',1) for line in result.stdout.splitlines() if line.startswith('r'))
                    for register,value in expected.items(): self.assertEqual(int(registers[f'r{register}']),value)

if __name__ == '__main__': unittest.main()
