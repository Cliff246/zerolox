"""Zerolox R32 binary frontend; wire definitions from 0541f51."""
from ..base import *

AC,SP,RP=map(R,range(1,4))

# Current ISA wire values from ISA_zerolox.h / ISA_zerolox.c at master.
MAJOR='ld st push8 push16 push32 pushp pop8 pop16 pop32 popp get16 get32 addi bneq beq blt bge bltu bgeu jlink reserved jcall jret add sub sll srl sra xor or and slli srli srai xori andi ori sext anda xora ora aand axor aor clt cge cltu cgeu ceq cneq j andn extb bcall21 blink21 b21 addpc push3 pop3'.split()
ALU={k.lower():v for k,v in O.__members__.items() if v.value in range(3,17)}; ALU['cneq']=O.CNE
BR={'bneq':O.CNE,'beq':O.CEQ,'blt':O.CLT,'bge':O.CGE,'bltu':O.CLTU,'bgeu':O.CGEU}
from .settings import FrontendSettings

class ZeroloxFrontend:
    """Raw little-endian instruction parcels. Program is read-only in this model."""
    instruction_alignment = 2
    def __init__(self,code,base=0,settings=None):
        self.settings = settings or FrontendSettings(base=base)
        self.code=bytes(code); self.base=self.settings.base; self.end=self.base+len(code)
        if self.base % 2 or len(self.code) % 2: raise ValueError("unaligned Zerolox code")
    def parcel(self,pc):
        i=pc-self.base
        if i<0 or i+2>len(self.code) or i%2: raise ValueError(f'bad fetch {pc:#x}')
        return int.from_bytes(self.code[i:i+2],'little')
    def packet(self,pc):
        if pc==self.end: return None
        f=self.parcel(pc); a=(f>>6)&31; b=(f>>1)&31
        if f&1:
            op=f>>11; w=(f>>9)&3; imm8=(f>>1)&255; imm10=(f>>1)&1023; mag=(f>>6)&7
            q=Builder(pc,2); target=lambda x,n:q.imm(pc+2*sx(x,n))
            if op==0: q.mv(R(a),R(b))
            elif op==1: q.emit(O.ADD,AC,R(a),R(b))
            elif op==2: q.emit(O.ADD,R(a),AC,R(b))
            elif op==3: q.emit(O.SUB,AC,R(a),R(b))
            elif op==4: q.mv(AC,q.imm(sx(imm10,10)))
            elif op==5: q.mv(R(8+w),q.imm(imm8))
            elif op==6: q.jump(target(imm10,10))
            elif op in (7,8): q.jump(target(imm10,10),q.cond(O.CEQ if op==7 else O.CNE,AC,Z))
            elif 9<=op<=16:
                if w==3: raise ValueError('reserved width')
                if op in (9,10,11,12):
                    base=R(b) if op in (9,11) else AC; data=AC if op in (9,11) else R(b); off=mag*(1<<w)
                    (q.ld if op in (9,10) else q.st)(data,base,off,w)
                elif op in (13,14): (q.st if op==13 else q.ld)(AC,SP,imm8,w)
                else:
                    if op==15 and b==1: raise ValueError('ambiguous double destination ld+a ac')
                    (q.ld if op==15 else q.st)(AC,R(b),0,w)
                    delta={0:4,5:8}.get(mag,sx(mag,3))*(1<<w)
                    q.emit(O.ADD,R(b),R(b),q.imm(delta))
            elif op==17:
                it=R(4+(w>>1)); counter=R(6+(w>>1)); q.jump(target(imm8,8),q.cond(O.CNE if w&1 else O.CEQ,it,counter))
            elif op in (18,19): q.jump(target(imm8,8),q.cond(O.CEQ if op==18 else O.CNE,R(8+w),AC))
            elif op in (20,21):
                it=R(4+(w>>1)); counter=R(6+(w>>1)); c=q.cond(O.CEQ if op==20 else O.CNE,it,counter)
                q.emit(O.ADD,it,it,q.imm(-1 if w&1 else 1)); q.jump(target(imm8,8),c)
            elif op in (25,26):
                if op==26: self.push(q,RP,2)
                q.mv(RP,q.imm(pc+2)); q.jump(target(imm10,10))
            else: raise NotImplementedError(f'minor {op}')
            return q.finish()
        cls=f>>11; reg=R(b)
        if 2<=cls<=8:
            q=Builder(pc,2); mag=a or 32
            if cls in (4,5,6) and a==0: raise ValueError('reserved shift')
            if cls in (7,8): q.mv(reg,q.imm(mag if cls==7 else -mag))
            else: q.emit({2:O.ADD,3:O.SUB,4:O.SLL,5:O.SRL,6:O.SRA}[cls],reg,reg,q.imm(mag))
            return q.finish()
        idx=f>>6
        if cls>1 or idx>=len(MAJOR): raise ValueError(f'reserved major {idx}')
        name=MAJOR[idx]
        one={'push8','push16','push32','pop8','pop16','pop32','j','jlink','jcall','jret','addpc','anda','xora','ora','aand','axor','aor'}
        if name in ('pushp','popp','reserved'): raise ValueError('retired or reserved')
        size=2 if name in one else 6 if name=='get32' else 4
        q=Builder(pc,size); s=self.parcel(pc+2) if size>=4 else 0
        right=R((s>>6)&31); dst=R(s>>11); mov=R((s>>1)&31); direction=s&1
        imm11=(s>>11)|(((s>>6)&31)<<5)|((s&1)<<10)
        if name in ALU or name=='andn':
            md,ms=(AC,mov) if direction else (mov,AC)
            if md!=Z and md==dst: raise ValueError('conflicting destinations')
            if md!=Z: q.mv(md,ms)
            if name=='andn':
                t=q.tmp(); q.emit(O.XOR,t,right,q.imm(MASK)); q.emit(O.AND,dst,reg,t)
            else: q.emit(ALU[name],dst,reg,right)
        elif name in ('ld','st'):
            width=(s>>9)&3
            if width==3: raise ValueError('reserved width')
            off=sx((s>>11)|(((s>>6)&7)<<5)|((s&1)<<8),9)
            (q.ld if name=='ld' else q.st)(mov,reg,off,width)
        elif name in ('get16','get32'): q.mv(reg,q.imm(s if name=='get16' else (s<<16)|self.parcel(pc+4)))
        elif name in ('addi','slli','srli','srai','xori','andi','ori'):
            op=ALU[name[:-1]]; q.emit(op,mov,reg,q.imm(sx(imm11,11)))
        elif name in ('extb','sext'):
            if name=='extb':
                if imm11>=32: raise ValueError('bit index')
                t=q.tmp(); q.emit(O.SRL,t,reg,q.imm(imm11)); q.emit(O.AND,mov,t,q.imm(1))
            else:
                if not 1<=imm11<=32: raise ValueError('sign extension width')
                if imm11==32: q.mv(mov,reg)
                else:
                    t=q.tmp(); sh=q.imm(32-imm11); q.emit(O.SLL,t,reg,sh); q.emit(O.SRA,mov,t,sh)
        elif name in BR: q.jump(q.imm(pc+2*sx(imm11,11)),q.cond(BR[name],reg,mov))
        elif name=='addpc': q.emit(O.ADD,reg,AC,q.imm(pc))
        elif name.startswith(('push','pop')):
            push=name.startswith('push')
            if name.endswith('3'):
                ws=(s>>11)|((s&1)<<5); regs=[reg,right,mov]; widths=[(ws>>(2*i))&3 for i in range(3)]
                if 3 in widths: raise ValueError('reserved width')
                if not push and (SP in regs or len([x for x in regs if x!=Z])!=len(set(x for x in regs if x!=Z))): raise ValueError('conflicting pop')
                # Snapshot push sources before SP changes. Compute each address from old SP.
                if push:
                    off=0
                    for r,w in zip(regs,widths): off-=1<<w; q.st(r,SP,off,w)
                else:
                    off=0
                    for r,w in reversed(list(zip(regs,widths))): q.ld(r,SP,off,w); off+=1<<w
                q.emit(O.ADD,SP,SP,q.imm(off))
            else:
                w={'8':0,'16':1,'32':2}[name[4:] if push else name[3:]]
                if push: self.push(q,reg,w)
                else:
                    if reg==SP: raise ValueError('conflicting pop SP')
                    q.ld(reg,SP,0,w); q.emit(O.ADD,SP,SP,q.imm(1<<w))
        elif name in ('b21','blink21','bcall21'):
            if name=='bcall21': self.push(q,RP,2)
            if name!='b21': q.mv(RP,q.imm(pc+4))
            q.jump(q.imm(pc+2*sx((b<<16)|s,21)))
        elif name in ('j','jlink','jcall','jret'):
            t=q.tmp(); q.mv(t,RP if name=='jret' else reg)
            if name=='jcall': self.push(q,RP,2)
            if name in ('jlink','jcall'): q.mv(RP,q.imm(pc+2))
            if name=='jret':
                if reg in (RP,SP): raise ValueError('conflicting jret destination')
                q.ld(RP,SP,0,2); q.emit(O.ADD,SP,SP,q.imm(4)); q.mv(reg,AC)
            q.jump(t)
        elif name in ('anda','xora','ora','aand','axor','aor'):
            if name in ('anda','xora','ora'): q.emit(ALU[name[:-1]],AC,AC,reg)
            else: q.emit(ALU[name[1:]],reg,AC,reg)
        else: raise NotImplementedError(name)
        return q.finish()
    @staticmethod
    def push(q,r,w): q.st(r,SP,-(1<<w),w); q.emit(O.ADD,SP,SP,q.imm(-(1<<w)))


# Minimal raw parcel encoders for tests; not an assembler.
def minor(op,a,b):return (op<<11)|(a<<6)|(b<<1)|1
def family(cls,mag,reg):return (cls<<11)|(mag<<6)|(reg<<1)
def major(op,a):return (MAJOR.index(op)<<6)|(a<<1)
def raw(*words):return b''.join(w.to_bytes(2,'little') for w in words)
