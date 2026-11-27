import unittest
from ..base import Builder, O, R, Z
from ..backends.in_order import InOrderBackend
from ..backends.settings import InOrderSettings
from ..frontends.packets import PacketFrontend
from ..frontends.zerolox import raw, family, minor
from ..settings import ZLX_SCALAR, ZLX_DUAL, RV32_SCALAR, RV32_DUAL


def rv(*words):
    return b''.join(w.to_bytes(4,'little') for w in words)

def addi(d,a,i):
    return (i&4095)<<20 | a<<15 | d<<7 | 0x13

def add(d,a,b):
    return b<<20 | a<<15 | d<<7 | 0x33

class TestShared(unittest.TestCase):
    def test_direct_packets(self):
        q=Builder(0,2); q.emit(O.ADD,R(8),Z,q.imm(5)); q.emit(O.ADD,R(9),Z,q.imm(6))
        q2=Builder(2,2); q2.emit(O.ADD,R(10),R(8),R(9))
        a,b=(InOrderBackend(PacketFrontend([q.finish(),q2.finish()]),InOrderSettings(issue_width=w)).run() for w in (1,2))
        self.assertEqual(a.registers,b.registers)
        self.assertEqual(b.registers[10],11)
        self.assertEqual(b.uops,3)
        self.assertLess(b.cycles,a.cycles)

    def test_cross_isa(self):
        z=raw(family(7,7,8),family(7,9,9),minor(1,8,9))
        r=rv(addi(8,0,7),addi(9,0,9),add(1,8,9))
        for zcfg,rcfg in ((ZLX_SCALAR,RV32_SCALAR),(ZLX_DUAL,RV32_DUAL)):
            a,b=zcfg.build(z).run(),rcfg.build(r).run()
            self.assertEqual(a.registers,b.registers)
            self.assertEqual(a.registers[1],16)
            self.assertEqual((a.retired,a.uops),(3,3))
            self.assertEqual((b.retired,b.uops),(3,3))
            self.assertEqual(a.cycles,b.cycles)

    def test_rv_signed_load(self):
        # LB x5, 1(x2); LBU x6, 1(x2)
        program=rv(1<<20|2<<15|5<<7|3,1<<20|2<<15|4<<12|6<<7|3)
        for cfg in (RV32_SCALAR,RV32_DUAL):
            c=cfg.build(program); c.registers[2]=20;c.memory[21]=0x80;c.run()
            self.assertEqual(c.registers[5:7],[0xffffff80,0x80])

    def test_rv_jalr_alias(self):
        # JALR x1, 0(x1); skip second word and land on third.
        program=rv(1<<15|1<<7|0x67,addi(8,0,9),addi(9,0,7))
        c=RV32_DUAL.build(program); c.registers[1]=8;c.run()
        self.assertEqual((c.registers[1],c.registers[8],c.registers[9]),(4,0,7))

    def test_rv_taken_branch(self):
        # BEQ x0,x0,+8
        c=RV32_SCALAR.build(rv(4<<8|0x63,addi(8,0,7),addi(9,0,3))).run()
        self.assertEqual(c.registers[8:10],[0,3])
        self.assertEqual(c.retired,2)

    def test_frontend_width(self):
        from ..frontends.settings import FrontendSettings
        code=rv(*(addi(8+i,0,i+1) for i in range(8)))
        a=RV32_DUAL.build(code).run()
        b=RV32_DUAL.build(code,settings=FrontendSettings(packets_per_cycle=2)).run()
        self.assertEqual(a.registers,b.registers)
        self.assertEqual((a.cycles,b.cycles),(12,8))
