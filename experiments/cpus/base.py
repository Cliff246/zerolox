"""Shared 32-bit microinstruction format and packet construction."""
from dataclasses import dataclass
from enum import IntEnum
from typing import Protocol

MASK = 0xffffffff

def fits5bits(val):
    if not 0 <= val <= 31: raise ValueError('5-bit index')
    return val

def sx(x, n):
    x &= (1 << n)-1
    return (x ^ (1 << (n-1))) - (1 << (n-1))

class ParamaterType(IntEnum):
    INVALID=0; REGISTER=1; IMMEDIATE=2; LATCH=3
class MicroOp(IntEnum):
    INVALID=0; LD=1; ST=2; ADD=3; SUB=4; SLL=5; SRL=6; SRA=7
    CEQ=8; CNE=9; CLT=10; CGE=11; CLTU=12; CGEU=13; AND=14; XOR=15
    OR=16; JMP=17; JMPC=18
T=ParamaterType; O=MicroOp
@dataclass(frozen=True)
class MicroInstruction:
    op: MicroOp; extra: int
    dst_type: ParamaterType; dst_val: int
    src1_type: ParamaterType; src1_val: int
    src2_type: ParamaterType; src2_val: int
    def __post_init__(self):
        for x in (self.extra,self.dst_val,self.src1_val,self.src2_val): fits5bits(x)
        if self.op == O.INVALID or self.dst_type == T.IMMEDIATE: raise ValueError('invalid microinstruction')
    @property
    def dst(self): return self.dst_type,self.dst_val
    @property
    def src1(self): return self.src1_type,self.src1_val
    @property
    def src2(self): return self.src2_type,self.src2_val
    def encode(self):
        a,b,c=(int(t)*32+i for t,i in (self.dst,self.src1,self.src2))
        return (int(self.op)<<26)|(self.extra<<21)|(a<<14)|(b<<7)|c
    @classmethod
    def decode(cls, word):
        if not 0<=word<=MASK: raise ValueError('not a 32-bit word')
        def operand(shift): return (T((word>>(shift+5))&3),(word>>shift)&31)
        return cls(O(word>>26),(word>>21)&31,*operand(14),*operand(7),*operand(0))

def R(i): return T.REGISTER,fits5bits(i)
def L(i): return T.LATCH,fits5bits(i)
def I(i): return T.IMMEDIATE,fits5bits(i)
N=(T.INVALID,0)
Z=R(0)

def uop(op,d=N,a=N,b=N,extra=0): return MicroInstruction(op,extra,*d,*a,*b)

@dataclass(frozen=True)
class Packet:
    pc:int; next_pc:int; ops:tuple; constants:tuple=(); latches:int=0
    @property
    def branch(self): return any(u.op in (O.JMP,O.JMPC) for u in self.ops)

class Builder:
    def __init__(self,pc,size): self.pc=pc; self.size=size; self.ops=[]; self.constants=[]; self.latches=0
    def imm(self,v):
        v &= MASK
        if v not in self.constants: self.constants.append(v)
        return I(self.constants.index(v))
    def tmp(self):
        t=L(self.latches); self.latches+=1; return t
    def emit(self,op,d=N,a=N,b=N,extra=0): self.ops.append(uop(op,d,a,b,extra))
    def mv(self,d,a): self.emit(O.ADD,d,a,Z)
    def addr(self,base,offset):
        if offset==0: return base
        t=self.tmp(); self.emit(O.ADD,t,base,self.imm(offset)); return t
    def ld(self,d,base,offset,width): self.emit(O.LD,d,self.addr(base,offset),extra=width)
    def st(self,d,base,offset,width): self.emit(O.ST,N,self.addr(base,offset),d,extra=width)
    def cond(self,op,a,b):
        t=self.tmp(); self.emit(op,t,a,b); return t
    def jump(self,target,cond=None): self.emit(O.JMP if cond is None else O.JMPC,N,target if cond is None else cond,N if cond is None else target)
    def finish(self): return Packet(self.pc,self.pc+self.size,tuple(self.ops),tuple(self.constants),self.latches)

class Frontend(Protocol):
    """The backend sees packets, not architectural opcodes."""
    base: int
    instruction_alignment: int
    settings: object
    def packet(self, pc: int) -> Packet | None: ...

class Backend(Protocol):
    cycles: int
    retired: int
    uops: int
    @property
    def done(self) -> bool: ...
    def step(self): ...
    def run(self, max_cycles: int = 100000): ...
