import unittest
from ..base import *
from ..frontends.zerolox import *
from ..backends.in_order import InOrderBackend
from ..backends.settings import InOrderSettings

def CPU(frontend, issue_width=1):
    return InOrderBackend(frontend, InOrderSettings(issue_width=issue_width))

class Test(unittest.TestCase):
    def cpu(self,code,width=1,setup=None):
        c=CPU(ZeroloxFrontend(code),issue_width=width)
        if setup:setup(c)
        return c.run()
    def test_format(self):
        u=uop(O.ADD,R(31),I(30),L(29),31)
        self.assertEqual(MicroInstruction.decode(u.encode()),u)
    def test_five_stages(self):
        c=self.cpu(raw(family(7,7,8)))
        self.assertEqual(c.cycles,5)
        self.assertEqual(c.registers[8],7)
        self.assertEqual([k for k in ('D','II','EX','MEM','WB') if c.trace[('D','II','EX','MEM','WB').index(k)][k]!='-'],['D','II','EX','MEM','WB'])
    def test_dependency(self):
        code=raw(family(7,7,8),family(7,9,9),minor(1,8,9),minor(2,10,9))
        for w in (1,2):
            c=self.cpu(code,w);self.assertEqual(c.registers[10],25);self.assertEqual(c.retired,4)
    def test_move_atomic(self):
        code=raw(family(7,3,1),family(7,4,8),major('add',9),(10<<11)|(8<<6)|(9<<1))
        c=CPU(ZeroloxFrontend(code),issue_width=2)
        while not c.done:
            c.step()
            if c.retired<3:self.assertEqual(c.registers[9],0)
        self.assertEqual(c.registers[9:11],[3,7])
    def test_stack(self):
        code=raw(major('push3',8), (36<<11 & 0xffff)|(9<<6)|(10<<1)|1,major('pop3',11),(36<<11 & 0xffff)|(12<<6)|(13<<1)|1)
        # width codes 0,1,2 = 36, top width bit in parcel bit 0.
        def setup(c):c.registers[2]=100;c.registers[8:11]=[0x123,0x12345,0x12345678]
        for w in (1,2):
            c=self.cpu(code,w,setup);self.assertEqual(c.registers[2],100);self.assertEqual(c.registers[11:14],[0x23,0x2345,0x12345678]);self.assertEqual(c.retired,2)
    def test_post_load(self):
        code=raw((15<<11)|(2<<9)|(1<<6)|(8<<1)|1)
        def setup(c):c.registers[8]=20;c.memory[20:24]=(123).to_bytes(4,'little')
        for w in (1,2):
            c=self.cpu(code,w,setup);self.assertEqual((c.registers[1],c.registers[8]),(123,24))
    def test_branch(self):
        code=raw((6<<11)|(2<<1)|1,family(7,9,8),family(7,3,9))
        c=self.cpu(code,2);self.assertEqual(c.registers[8:10],[0,3]);self.assertEqual(c.retired,2)
    def test_iterator(self):
        code=raw((21<<11)|(255<<1)|1) # branch -1 parcel when unequal, not taken with ix=cx
        c=self.cpu(code);self.assertEqual(c.registers[4],1);self.assertEqual(c.retired,1)
    def test_dual_compound(self):
        code=raw(family(7,3,1),family(7,4,8),major('add',8),(10<<11)|(8<<6)|(9<<1))
        a,b=(self.cpu(code,w) for w in (1,2))
        self.assertEqual(a.registers,b.registers)
        self.assertEqual(b.registers[9:11],[3,8])
        self.assertEqual((a.cycles,b.cycles),(8,7))
        self.assertTrue(any(len(t['II'].split())==2 for t in b.trace))
    def test_independent(self):
        # frontend publication is intentionally limited to one packet/cycle.
        code=raw(*(family(7,i+1,8+i) for i in range(8)))
        self.assertEqual(self.cpu(code,1).registers,self.cpu(code,2).registers)
if __name__=='__main__':unittest.main(verbosity=2)
