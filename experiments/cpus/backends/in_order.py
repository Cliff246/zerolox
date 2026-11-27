"""Five-stage ordered reference backend: D, II, EX, MEM, WB."""
from dataclasses import dataclass, field
from collections import deque
from ..base import *
from .settings import InOrderSettings

@dataclass(eq=False)
class Group:
    packet:Packet; immed:list; latches:list; writes:dict=field(default_factory=dict); stores:list=field(default_factory=list)
@dataclass(eq=False)
class Work:
    u:MicroInstruction; group:Group; last:bool; value:int|None=None; a:int=0; b:int=0; store:tuple|None=None; target:int|None=None
    @property
    def dest(self): return self.u.dst if self.u.dst_type in (T.REGISTER,T.LATCH) and self.u.dst!=Z else None

class InOrderBackend:
    """Scalar reference by default; issue_width=2 tests ordered P/C-like issue.
    II never bypasses blocked head. One memory op per issue batch. No speculative
    fetch across branches. EX->II and MEM->II forwarding configurable.
    """
    def __init__(self,frontend: Frontend,settings: InOrderSettings | None=None):
        settings = settings or InOrderSettings()
        self.settings = settings
        issue_width, buffer_size, memory_size, forward = (settings.issue_width, settings.buffer_size, settings.memory_size, settings.forward)
        self.frontend=frontend; self.pc=frontend.base; self.arch_pc=self.pc
        self.immediates=[0]*32; self.registers=[0]*32; self.latches=[0]*32
        self.memory=bytearray(memory_size); self.free_i=set(range(32)); self.free_l=set(range(32))
        self.queue=deque(); self.publisher=deque(); self.pending={}; self.groups=[]
        self.ii=[]; self.ex=[]; self.mem=[]; self.wb=[]; self.width=issue_width; self.capacity=buffer_size; self.forward=forward
        self.block=False; self.eof=False; self.cycles=0; self.retired=0; self.uops=0; self.trace=[]
    def get_paramater(self,ptype,pval):
        fits5bits(pval)
        if (ptype,pval)==Z or ptype==T.INVALID:return 0
        return {T.REGISTER:self.registers,T.IMMEDIATE:self.immediates,T.LATCH:self.latches}[ptype][pval]
    def read(self,p):
        w=self.pending.get(p)
        if w is None: return self.get_paramater(*p)
        return w.value if self.forward or w not in self.ex+self.mem+self.wb else None
    def publish(self):
        for _ in range(self.frontend.settings.packets_per_cycle):
            while self.publisher and len(self.queue)<self.capacity:self.queue.append(self.publisher.popleft())
            if self.publisher or self.block or self.eof or len(self.queue)>=self.capacity:break
            p=self.frontend.packet(self.pc)
            if p is None: self.eof=True;break
            elif len(p.constants)<=len(self.free_i) and p.latches<=len(self.free_l):
                if not p.ops: raise ValueError('empty packet')
                ii=sorted(self.free_i)[:len(p.constants)]; ll=sorted(self.free_l)[:p.latches]
                self.free_i.difference_update(ii); self.free_l.difference_update(ll)
                for i,v in zip(ii,p.constants): self.immediates[i]=v
                for i in ll:self.latches[i]=0
                g=Group(p,ii,ll); self.groups.append(g)
                def remap(x):
                    t,i=x
                    return (t,ii[i] if t==T.IMMEDIATE else ll[i] if t==T.LATCH else i)
                for n,u in enumerate(p.ops): self.publisher.append(Work(uop(u.op,remap(u.dst),remap(u.src1),remap(u.src2),u.extra),g,n==len(p.ops)-1))
                self.pc=p.next_pc; self.block=p.branch
            else:break # immediate/latch allocation backpressure
        while self.publisher and len(self.queue)<self.capacity:self.queue.append(self.publisher.popleft())
    def execute(self,w):
        o=w.u.op; a=w.a; b=w.b
        if o in (O.LD,O.ST): return
        if o==O.JMP:w.target=a;return
        if o==O.JMPC:w.target=b if a else None;return
        funcs={O.ADD:lambda:a+b,O.SUB:lambda:a-b,O.SLL:lambda:a<<(b&31),O.SRL:lambda:a>>(b&31),O.SRA:lambda:sx(a,32)>>(b&31),O.CEQ:lambda:a==b,O.CNE:lambda:a!=b,O.CLT:lambda:sx(a,32)<sx(b,32),O.CGE:lambda:sx(a,32)>=sx(b,32),O.CLTU:lambda:a<b,O.CGEU:lambda:a>=b,O.AND:lambda:a&b,O.XOR:lambda:a^b,O.OR:lambda:a|b}
        w.value=int(funcs[o]())&MASK
    def memory_step(self,w):
        if w.u.op not in (O.LD,O.ST):return
        if w.u.extra not in (0,1,2): raise ValueError('width')
        size=1<<w.u.extra; a=w.a
        if a+size>len(self.memory): raise ValueError(f'memory fault {a:#x}')
        if w.u.op==O.ST:w.store=(a,(w.b&((1<<(8*size))-1)).to_bytes(size,'little'));return
        data=bytearray(self.memory[a:a+size])
        # Forward older uncommitted packet stores, in order. WB precedes MEM.
        for g in self.groups:
            for addr,raw in g.stores:
                for j,v in enumerate(raw):
                    if a<=addr+j<a+size:data[addr+j-a]=v
            if g is w.group:break
        w.value=int.from_bytes(data,'little')
    def writeback(self,w):
        g=w.group; self.uops+=1
        if w.dest is not None:
            if w.dest[0]==T.LATCH:self.latches[w.dest[1]]=w.value
            else:g.writes[w.dest]=w.value
        if w.store:g.stores.append(w.store)
        if w.last:
            if w.target is not None and w.target % self.frontend.instruction_alignment: raise ValueError("unaligned jump target")
            for (_,i),v in g.writes.items():self.registers[i]=v
            for addr,data in g.stores:self.memory[addr:addr+len(data)]=data
            for p,producer in list(self.pending.items()):
                if producer.group is g:del self.pending[p]
            self.free_i.update(g.immed);self.free_l.update(g.latches);self.groups.remove(g)
            self.arch_pc=g.packet.next_pc if w.target is None else w.target
            if g.packet.branch:self.pc=self.arch_pc;self.block=False
            self.retired+=1
    @property
    def done(self):return self.eof and not (self.queue or self.publisher or self.ii or self.ex or self.mem or self.wb)
    def step(self):
        self.cycles+=1
        show=lambda ws:' '.join(f'{w.group.packet.pc:04x}:{w.u.op.name}' for w in ws) or '-'
        rec={'cycle':self.cycles,'WB':show(self.wb),'MEM':show(self.mem),'EX':show(self.ex)}
        for w in self.wb:self.writeback(w)
        for w in self.mem:self.memory_step(w)
        self.wb=self.mem
        for w in self.ex:self.execute(w)
        self.mem=self.ex;self.ex=[]
        memory_used=False;stall=''
        while self.ii and len(self.ex)<self.width:
            w=self.ii[0]; ismem=w.u.op in (O.LD,O.ST)
            if ismem and memory_used:stall='shared MEM';break
            a=self.read(w.u.src1); b=self.read(w.u.src2)
            if a is None or b is None:stall='RAW';break
            w.a=a;w.b=b
            if w.dest is not None:self.pending[w.dest]=w
            self.ex.append(self.ii.pop(0));memory_used|=ismem
        rec['II']=show(self.ex);rec['stall']=stall
        self.publish(); decoded=[]
        while self.queue and len(self.ii)<self.width:
            w=self.queue.popleft();self.ii.append(w);decoded.append(w)
        rec['D']=show(decoded);self.trace.append(rec)
    def run(self,max_cycles=100000):
        while not self.done and self.cycles<max_cycles:self.step()
        if not self.done:raise RuntimeError('cycle limit')
        return self

