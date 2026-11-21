"""Additional ISA forms, spelled explicitly while syntax remains experimental."""
from model import Error, CURRENT_ISA
from values import expr, reg

from opcodes import MAJOR, MINOR, LEGACY_MAJOR, LEGACY_MINOR, LEGACY_IMM_SELECTOR

COMPARE = {'clt', 'cge', 'cltu', 'cgeu', 'ceq', 'cneq'}
REG_ALU = COMPARE | {'add', 'sub', 'sll', 'srl', 'sra', 'xor', 'or', 'and'}
IMM = {'slli', 'srli', 'srai', 'xori', 'andi', 'ori', 'sext'}
ACC_IMM = {'inca', 'deca', 'srra', 'srla', 'slla'}
ONE = {name: name for name in ('inc', 'dec', 'j', 'call', 'ret', 'callp', 'retp',
                                'anda', 'xora', 'ora', 'aand', 'axor', 'aor')}
ONE.update(inc1='inc', dec1='dec')
for name in ('push', 'pop'):
    for width in ('8', '16', '32', 'p'):
        ONE[f'{name}.{width}'] = name + width
for stem, first in [('inc', 2), ('dec', 2), ('sl', 1), ('sr', 1), ('sra', 1)]:
    for count in range(first, 9):
        name = f'{stem}{count}'
        ONE[name] = name
ITER = {'bieqc+': 'bieqc_update', 'bineqc+': 'bineqc_update'}
BRANCH = {'bneq', 'beq', 'blt', 'bge', 'bltu', 'bgeu'}
MINOR28 = {'ldsa', 'stsa', 'bieqc', 'bteqa', 'btneqa'}
PLUS = {'ldplus': 'ld_plus', 'stplus': 'st_plus'}
WIDTH = {'8':0,'16':1,'32':2,'p':3}

def names():
    return (set(ITER) | {'m.'+k for k in ITER} | set(ACC_IMM) | {'n.'+k for k in ACC_IMM} |
            {'n.'+k for k in ONE} | set(ONE) | {'n.get16','n.get32','get16','get32','n.addi','addi'} |
            {'n.'+k for k in IMM} | set(IMM) | {'n.'+k for k in BRANCH} | set(BRANCH) |
            {'m.'+k+'.'+w for k in ('ldsa','stsa','ldplus','stplus') for w in WIDTH} |
            {'m.'+k for k in ('bieqc','bteqa','btneqa')})

def parse(op,args,symbols,pc,target=CURRENT_ISA):
    """Return (form, parcel fields) after strict operand checks."""
    legacy = target != CURRENT_ISA
    major, minor = (LEGACY_MAJOR, LEGACY_MINOR) if legacy else (MAJOR, MINOR)
    name=op.removeprefix('n.')
    if legacy and (name in ACC_IMM or name == 'sext' or op.removeprefix('m.') in ITER
                   or (name in ONE and ONE[name] not in major)):
        raise Error(f'{op} is unavailable for the legacy target')
    def count(n):
        if len(args)!=n: raise Error(f'{op} requires {n} arguments')
    def immediate(s,lo,hi):
        v=expr(s,symbols,pc)
        if not lo<=v<=hi: raise Error(f'{op} value {v} outside {lo}..{hi}')
        return v
    if name in ACC_IMM:
        count(1)
        return 'one', (major[name], immediate(args[0], 0, 31))
    branch = op.removeprefix('m.')
    if branch in ITER:
        count(2)
        modes = {'x+':0, 'x-':1, 'y+':2, 'y-':3}
        if args[0] not in modes:
            raise Error(f'{op} requires x+, x-, y+ or y-')
        offset = immediate(args[1], -256, 254)
        if offset % 2: raise Error('branch offset must be parcel aligned')
        return 'minor28', (minor[ITER[branch]], modes[args[0]], (offset // 2) & 255)
    if name in ONE:
        count(1); return 'one',(major[ONE[name]],reg(args[0]))
    if name in ('get16','get32'):
        count(2); bits=16 if name=='get16' else 32
        return 'get',(major[name],reg(args[0]),immediate(args[1],0,(1<<bits)-1),bits)
    if name=='addi':
        count(3); return 'imm11',(major['addi'],reg(args[1]),reg(args[0]),immediate(args[2],-1024,1023)&2047)
    if name in IMM:
        count(3)
        if legacy:
            lo, hi = (0, 31) if name in ('slli','srli','srai') else (-32, 31)
            value = immediate(args[2], lo, hi) & 63
            return 'imm6', (major['imm_group'], reg(args[1]), reg(args[0]), LEGACY_IMM_SELECTOR[name], value)
        lo, hi = (-1024, 1023)
        if name in ('slli','srli','srai'): lo, hi = 0, 2047
        if name == 'sext': lo, hi = 1, 32
        return 'imm11', (major[name], reg(args[1]), reg(args[0]), immediate(args[2], lo, hi) & 2047)
    if name in BRANCH:
        count(3); offset=immediate(args[2],-2048,2046)
        if offset%2: raise Error('branch offset must be parcel aligned')
        return 'imm11',(major[name],reg(args[0]),reg(args[1]),(offset//2)&2047)
    if op.startswith('m.'):
        parts=op.split('.'); instruction=parts[1]
        if instruction in ('ldsa','stsa'):
            count(1); return 'minor28',(minor[instruction],WIDTH[parts[2]],immediate(args[0],0,255))
        if instruction in PLUS:
            count(2); step=immediate(args[1],-4,8)
            mapping={4:0,1:1,2:2,3:3,-4:4,8:5,-2:6,-1:7}
            if step not in mapping: raise Error('increment must be -4, -2, -1, 1, 2, 3, 4, or 8 access units')
            base=reg(args[0])
            if instruction=='ldplus' and base==1: raise Error('m.ldplus cannot write both base and accumulator to ac')
            return 'minor235',(minor[PLUS[instruction]],WIDTH[parts[2]],mapping[step],base)
        if instruction in ('bieqc','bteqa','btneqa'):
            count(2)
            if instruction=='bieqc': selector=immediate(args[0],0,3)
            else:
                selector=reg(args[0])-8
                if selector not in range(4): raise Error(f'{op} requires t0..t3')
            offset=immediate(args[1],-256,254)
            if offset%2: raise Error('branch offset must be parcel aligned')
            return 'minor28',(minor[instruction],selector,(offset//2)&255)
    raise Error(f'unsupported instruction {op}')

def size(form, fields):
    if form=='get': return 4 if fields[-1]==16 else 6
    return 4 if form in ('imm11', 'imm6') else 2

def encode(form, f):
    def p(n): return n.to_bytes(2,'little')
    if form=='one': return p((f[0]<<6)|(f[1]<<1))
    if form=='get':
        op,r,v,bits=f
        return p((op<<6)|(r<<1))+(p(v) if bits==16 else p(v>>16)+p(v&65535))
    if form=='imm6':
        op,a,d,selector,v=f
        return p((op<<6)|(a<<1))+p(((v&31)<<11)|(d<<6)|(selector<<1)|((v>>5)&1))
    if form=='imm11':
        op,a,c,v=f
        return p((op<<6)|(a<<1))+p(((v&31)<<11)|(((v>>5)&31)<<6)|(c<<1)|((v>>10)&1))
    if form=='minor28': return p((f[0]<<11)|(f[1]<<9)|(f[2]<<1)|1)
    if form=='minor235': return p((f[0]<<11)|(f[1]<<9)|(f[2]<<6)|(f[3]<<1)|1)
    raise Error('unknown instruction format')
