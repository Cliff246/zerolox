"""Additional ISA forms, spelled explicitly while syntax remains experimental."""
from model import Error
from values import expr, reg

ONE = {'push.8':2,'push.16':3,'push.32':4,'push.p':5,
       'pop.8':6,'pop.16':7,'pop.32':8,'pop.p':9,
       'inc':14,'dec':15,'call':23,'ret':24,'callp':25,'retp':26}
IMM = {'slli':0,'srli':1,'srai':2,'xori':3,'andi':4,'ori':5}
BRANCH = {'bneq':17,'beq':18,'blt':19,'bge':20,'bltu':21,'bgeu':22}
MINOR28 = {'ldsa':14,'stsa':13,'bieqc':17,'bteqa':18,'btneqa':19}
PLUS = {'ldplus':15,'stplus':16}
WIDTH = {'8':0,'16':1,'32':2,'p':3}

def names():
    return ({'n.'+k for k in ONE} | set(ONE) | {'n.get16','n.get32','get16','get32','n.addi','addi'} |
            {'n.'+k for k in IMM} | set(IMM) | {'n.'+k for k in BRANCH} | set(BRANCH) |
            {'m.'+k+'.'+w for k in ('ldsa','stsa','ldplus','stplus') for w in WIDTH} |
            {'m.'+k for k in ('bieqc','bteqa','btneqa')})

def parse(op,args,symbols,pc):
    """Return (form, parcel fields) after strict operand checks."""
    name=op.removeprefix('n.')
    def count(n):
        if len(args)!=n: raise Error(f'{op} requires {n} arguments')
    def immediate(s,lo,hi):
        v=expr(s,symbols,pc)
        if not lo<=v<=hi: raise Error(f'{op} value {v} outside {lo}..{hi}')
        return v
    if name in ONE:
        count(1); return 'one',(ONE[name],reg(args[0]))
    if name in ('get16','get32'):
        count(2); bits=16 if name=='get16' else 32
        return 'get',(10 if bits==16 else 11,reg(args[0]),immediate(args[1],0,(1<<bits)-1),bits)
    if name=='addi':
        count(3); return 'imm11',(16,reg(args[1]),reg(args[0]),immediate(args[2],-1024,1023)&2047)
    if name in IMM:
        count(3)
        lo,hi=(0,31) if name in ('slli','srli','srai') else (-32,31)
        return 'imm6',(reg(args[1]),reg(args[0]),IMM[name],immediate(args[2],lo,hi)&63)
    if name in BRANCH:
        count(3); offset=immediate(args[2],-2048,2046)
        if offset%2: raise Error('branch offset must be parcel aligned')
        return 'imm11',(BRANCH[name],reg(args[0]),reg(args[1]),(offset//2)&2047)
    if op.startswith('m.'):
        parts=op.split('.'); instruction=parts[1]
        if instruction in ('ldsa','stsa'):
            count(1); return 'minor28',(MINOR28[instruction],WIDTH[parts[2]],immediate(args[0],0,255))
        if instruction in PLUS:
            count(2); step=immediate(args[1],-4,8)
            mapping={4:0,1:1,2:2,3:3,-4:4,8:5,-2:6,-1:7}
            if step not in mapping: raise Error('increment must be -4, -2, -1, 1, 2, 3, 4, or 8 access units')
            base=reg(args[0])
            if instruction=='ldplus' and base==1: raise Error('m.ldplus cannot write both base and accumulator to ac')
            return 'minor235',(PLUS[instruction],WIDTH[parts[2]],mapping[step],base)
        if instruction in ('bieqc','bteqa','btneqa'):
            count(2)
            if instruction=='bieqc': selector=immediate(args[0],0,3)
            else:
                selector=reg(args[0])-8
                if selector not in range(4): raise Error(f'{op} requires t0..t3')
            offset=immediate(args[1],-256,254)
            if offset%2: raise Error('branch offset must be parcel aligned')
            return 'minor28',(MINOR28[instruction],selector,(offset//2)&255)
    raise Error(f'unsupported instruction {op}')

def size(form, fields):
    if form=='get': return 4 if fields[-1]==16 else 6
    return 4 if form in ('imm11','imm6') else 2

def encode(form, f):
    def p(n): return n.to_bytes(2,'little')
    if form=='one': return p((f[0]<<6)|(f[1]<<1))
    if form=='get':
        op,r,v,bits=f
        return p((op<<6)|(r<<1))+(p(v) if bits==16 else p(v>>16)+p(v&65535))
    if form=='imm11':
        op,a,c,v=f
        return p((op<<6)|(a<<1))+p(((v&31)<<11)|(((v>>5)&31)<<6)|(c<<1)|((v>>10)&1))
    if form=='imm6':
        a,d,selector,v=f
        return p((13<<6)|(a<<1))+p(((v&31)<<11)|(d<<6)|(selector<<1)|((v>>5)&1))
    if form=='minor28': return p((f[0]<<11)|(f[1]<<9)|(f[2]<<1)|1)
    if form=='minor235': return p((f[0]<<11)|(f[1]<<9)|(f[2]<<6)|(f[3]<<1)|1)
    raise Error('unknown instruction format')
