"""Additional ISA forms, spelled explicitly while syntax remains experimental."""
from model import Error, CURRENT_ISA, V3_ISA, legacy_encoding
from values import expr, reg

from opcodes import MAJOR, MAJOR_CLASS, MINOR, V3_MINOR, LEGACY_MAJOR, LEGACY_MINOR, LEGACY_IMM_SELECTOR

COMPARE = {'clt', 'cge', 'cltu', 'cgeu', 'ceq', 'cneq'}
REG_ALU = COMPARE | {'add', 'sub', 'sll', 'srl', 'sra', 'xor', 'or', 'and', 'andn'}
IMM = {'slli', 'srli', 'srai', 'xori', 'andi', 'ori', 'sext', 'extb'}
IN_PLACE = {'addi', 'slli', 'srli', 'srai'}
CLASS_NAMES = {'inc': 'inc', 'dec': 'dec', 'slli': 'shift_left', 'srli': 'shift_right',
               'srai': 'shift_arith', 'getpos': 'get_pos', 'getneg': 'get_neg'}
COMPACT_IMMEDIATE = CLASS_NAMES.keys() | {'addi', 'get5'}
LEGACY_STEP = {'inc': 'inc', 'inc1': 'inc', 'dec': 'dec', 'dec1': 'dec'}
ONE = {name: name for name in ('j', 'jlink', 'jcall', 'jret',
                                'anda', 'xora', 'ora', 'aand', 'axor', 'aor')}
for name in ('push', 'pop'):
    for width in ('8', '16', '32', 'p'):
        ONE[f'{name}.{width}'] = name + width
ITER = {'bieqc+': 'bieqc_update', 'bineqc+': 'bineqc_update'}
BRANCH = {'bneq', 'beq', 'blt', 'bge', 'bltu', 'bgeu'}
MINOR28 = {'ldsa', 'stsa', 'bieqc', 'bteqa', 'btneqa'}
PLUS = {'ldplus': 'ld_plus', 'stplus': 'st_plus'}
WIDTH = {'8':0,'16':1,'32':2,'p':3}

def names(target=CURRENT_ISA):
    steps = set(LEGACY_STEP) if legacy_encoding(target) else set()
    added = set()
    if not legacy_encoding(target):
        added.add('get5')
        if target == CURRENT_ISA:
            added.update(('bcall10', 'm.bcall10', 'blink10', 'm.blink10', 'bcall21', 'blink21'))
        else:
            added.update(('calli', 'm.calli'))
    if target == CURRENT_ISA:
        added.update(CLASS_NAMES)
        added.update('n.' + k for k in CLASS_NAMES)
        added.add('n.get5')
    if target == V3_ISA:
        added.update('m.' + k for k in IN_PLACE | {'get5'})
    return (added | set(ITER) | {'m.'+k for k in ITER} | steps | {'n.'+k for k in steps} |
            {'n.'+k for k in ONE} | set(ONE) | {'n.get16','n.get32','get16','get32','n.addi','addi'} |
            {'n.'+k for k in IMM} | set(IMM) | {'n.'+k for k in BRANCH} | set(BRANCH) |
            {'m.'+k+'.'+w for k in ('ldsa','stsa','ldplus','stplus') for w in WIDTH} |
            {'m.'+k for k in ('bieqc','bteqa','btneqa')})

def parse(op,args,symbols,pc,target=CURRENT_ISA):
    """Return (form, parcel fields) after strict operand checks."""
    legacy = legacy_encoding(target)
    major, minor = (LEGACY_MAJOR, LEGACY_MINOR) if legacy else (MAJOR, MINOR)
    if target == V3_ISA:
        minor = V3_MINOR
    name=op.removeprefix('n.')
    if legacy and ((name in IMM and name not in LEGACY_IMM_SELECTOR) or op.removeprefix('m.') in ITER
                   or (name in ONE and ONE[name] not in major)):
        raise Error(f'{op} is unavailable for the legacy target')
    def count(n):
        if len(args)!=n: raise Error(f'{op} requires {n} arguments')
    def immediate(s,lo,hi):
        v=expr(s,symbols,pc)
        if not lo<=v<=hi: raise Error(f'{op} value {v} outside {lo}..{hi}')
        return v
    if legacy and name in LEGACY_STEP:
        count(1)
        return 'one', (major[LEGACY_STEP[name]], reg(args[0]))
    if op in ('bcall10', 'm.bcall10', 'blink10', 'm.blink10', 'calli', 'm.calli'):
        count(1)
        offset = immediate(args[0], -1024, 1022)
        if offset % 2:
            raise Error('calli displacement must be parcel aligned')
        return 'minor10', (minor[op.removeprefix('m.')], (offset // 2) & 1023)
    if name in ('bcall21', 'blink21'):
        count(1)
        offset = immediate(args[0], -2097152, 2097150)
        if offset % 2: raise Error('branch displacement must be parcel aligned')
        return 'branch21', (major[name], (offset // 2) & 0x1fffff)
    if target == CURRENT_ISA and name in COMPACT_IMMEDIATE and len(args) == 2:
        if name in ('get5', 'addi'):
            value = immediate(args[1], -32, 32)
            if not value:
                raise Error(f'{op} requires a nonzero immediate')
            if name == 'get5':
                family = 'get_pos' if value > 0 else 'get_neg'
            else:
                family = 'inc' if value > 0 else 'dec'
            amount = abs(value)
        else:
            family = CLASS_NAMES[name]
            amount = immediate(args[1], 1, 31 if name in ('slli', 'srli', 'srai') else 32)
        opcode = (MAJOR_CLASS[family] << 5) | (amount & 31)
        return 'one', (opcode, reg(args[0]))
    if target == V3_ISA and op == 'get5':
        op = 'm.get5'
    if op == 'm.get5' or (op.startswith('m.') and op[2:] in IN_PLACE):
        if target != V3_ISA:
            raise Error(f'{op} is unavailable for this target')
        count(2)
        if op in ('m.addi', 'm.get5'):
            value = immediate(args[1], -16, 16)
            if value == 0:
                raise Error(f'{op} requires a nonzero immediate')
            field = 0 if value == 16 else value & 31
        else:
            field = immediate(args[1], 0, 31)
        return 'minor55', (minor[op[2:]], field, reg(args[0]))
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
        if name == 'extb': lo, hi = 0, 31
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
    if form=='branch21': return 4
    return 4 if form in ('imm11', 'imm6') else 2

def encode(form, f):
    def p(n): return n.to_bytes(2,'little')
    if form=='branch21': return p((f[0]<<6)|((f[1]>>16)<<1))+p(f[1]&65535)
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
    if form=='minor10': return p((f[0]<<11)|(f[1]<<1)|1)
    if form=='minor55': return p((f[0]<<11)|(f[1]<<6)|(f[2]<<1)|1)
    if form=='minor28': return p((f[0]<<11)|(f[1]<<9)|(f[2]<<1)|1)
    if form=='minor235': return p((f[0]<<11)|(f[1]<<9)|(f[2]<<6)|(f[3]<<1)|1)
    raise Error('unknown instruction format')
