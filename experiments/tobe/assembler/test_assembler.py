import json
import unittest
from assembler import Error, assemble, link

class DesignCases(unittest.TestCase):
    def test_selection_and_roundtrip(self):
        source = 'add ac, r8, r9\nadd r8, ac, r9\nadd r8, r9, r10\nld.16 ac, [r8, 3s]\nld.16 ac, [r8, 3]\nld.32 ac, [r8, -8]\n'
        obj = assemble(source)
        a = link(obj)
        self.assertEqual(a, link(json.loads(json.dumps(obj))))
        self.assertEqual([x[2] for x in a[0]], ['m.add','m.adda','n.add','m.lda.16','n.ld.16','n.ld.32'])

    def test_strict_minor_and_types(self):
        for source in ('m.lda.16 [r8, 3]', '!eq X, r1 + 10\nld.8 ac, [r8, $X]', 'ld.8 ac, [r8, r9]'):
            with self.subTest(source=source), self.assertRaises(Error): link(assemble(source))

    def test_bytes_and_branch(self):
        listing, symbols = link(assemble('start:\nb #start\n!db "hi", 0\nend:\n'))
        self.assertEqual(listing[0][:3], (0,2,'m.b'))
        self.assertEqual(symbols['end']['value'], 5)

    def test_odd_instruction_rejected(self):
        with self.assertRaisesRegex(Error, 'odd byte index'):
            link(assemble('!db 1\nadd ac, r8, r9'))

if __name__ == '__main__': unittest.main()
