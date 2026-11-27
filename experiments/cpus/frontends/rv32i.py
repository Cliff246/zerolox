"""Minimal RV32I binary frontend for testing the common backend.
No compressed instructions, CSR, traps, FENCE, M extension or system calls.
"""
from ..base import Builder, O, R, Z, MASK, sx

from .settings import FrontendSettings

class RV32IFrontend:
    instruction_alignment = 4

    def __init__(self, code, base=0, settings=None):
        self.settings = settings or FrontendSettings(base=base)
        base = self.settings.base
        self.code = bytes(code)
        self.base = base
        self.end = base + len(self.code)
        if base % 4 or len(self.code) % 4:
            raise ValueError('RV32I code must be word aligned')

    def packet(self, pc):
        if pc == self.end:
            return None
        i = pc - self.base
        if i < 0 or i+4 > len(self.code) or i % 4:
            raise ValueError(f'bad RV32I fetch {pc:#x}')
        w = int.from_bytes(self.code[i:i+4], 'little')
        op, f3, f7 = w & 127, (w>>12)&7, w>>25
        d, a, b = R((w>>7)&31), R((w>>15)&31), R((w>>20)&31)
        q = Builder(pc, 4)
        imm = sx(w>>20, 12)
        alu = (O.ADD,O.SLL,O.CLT,O.CLTU,O.XOR,O.SRL,O.OR,O.AND)
        if op == 0x33:
            if f7 == 0:
                q.emit(alu[f3],d,a,b)
            elif f7 == 32 and f3 in (0,5):
                q.emit(O.SUB if f3==0 else O.SRA,d,a,b)
            else:
                raise NotImplementedError(f'RV32I register operation {w:#x}')
        elif op == 0x13:
            if f3 in (1,5):
                if f7 not in ((0,32) if f3==5 else (0,)):
                    raise ValueError('invalid shift encoding')
                q.emit(O.SRA if f7==32 else alu[f3],d,a,q.imm((w>>20)&31))
            else:
                q.emit(alu[f3],d,a,q.imm(imm))
        elif op in (0x37,0x17):
            q.mv(d,q.imm((w & 0xfffff000)+(pc if op==0x17 else 0)))
        elif op == 0x03:
            if f3 not in (0,1,2,4,5): raise ValueError('invalid load width')
            width = f3&3
            if f3 in (0,1):
                t = q.tmp(); q.ld(t,a,imm,width)
                sh = q.imm(32-(8<<width))
                q.emit(O.SLL,t,t,sh); q.emit(O.SRA,d,t,sh)
            else:
                q.ld(d,a,imm,width)
        elif op == 0x23:
            if f3>2: raise ValueError('invalid store width')
            q.st(b,a,sx((w>>25)<<5|((w>>7)&31),12),f3)
        elif op == 0x63:
            ops={0:O.CEQ,1:O.CNE,4:O.CLT,5:O.CGE,6:O.CLTU,7:O.CGEU}
            if f3 not in ops: raise ValueError('invalid branch')
            off=sx(((w>>31)<<12)|(((w>>7)&1)<<11)|(((w>>25)&63)<<5)|(((w>>8)&15)<<1),13)
            q.jump(q.imm(pc+off),q.cond(ops[f3],a,b))
        elif op == 0x6f:
            off=sx(((w>>31)<<20)|(((w>>12)&255)<<12)|(((w>>20)&1)<<11)|(((w>>21)&1023)<<1),21)
            if d!=Z: q.mv(d,q.imm(pc+4))
            q.jump(q.imm(pc+off))
        elif op == 0x67 and f3==0:
            t=q.tmp(); q.emit(O.ADD,t,a,q.imm(imm)); q.emit(O.AND,t,t,q.imm(MASK-1))
            if d!=Z: q.mv(d,q.imm(pc+4))
            q.jump(t)
        else:
            raise NotImplementedError(f'RV32I operation {w:#x}')
        return q.finish()
