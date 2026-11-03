"""Experimental instruction selection and iterative layout."""
import re
from model import Error
from syntax import split_args
from values import expr, reg, data_size

def select(rec,symbols,pc):
    op,args=rec['op'],rec['args']
    if op in ('mv', 'm.mv'):
        if len(args)!=2: raise Error(f'{op} requires two registers')
        list(map(reg,args)); return 'm.mv',2
    if op in ('add','m.add','m.adda','n.add'):
        if op in ('m.add','m.adda'):
            if len(args)!=2: raise Error(f'{op} requires two registers')
            list(map(reg,args)); return op,2
        if len(args)!=3: raise Error(f'{op} requires three registers')
        d,a,b=map(reg,args)
        if op=='n.add': return op,4
        if d==1: return 'm.add',2
        if a==1: return 'm.adda',2
        return 'n.add',4
    if op in ('b','m.b','beqz','bnez','m.baeqz','m.baneqz'):
        if len(args)!=1: raise Error(f'{op} requires one offset')
        off=expr(args[0],symbols,pc)
        if off%2: raise Error(f'branch offset {off} is not parcel aligned')
        if not -1024<=off<=1022: raise Error(f'm.b offset {off} outside -1024..1022 bytes')
        chosen={'b':'m.b','beqz':'m.baeqz','bnez':'m.baneqz'}.get(op,op)
        return chosen,2
    match=re.fullmatch(r'(m\.(?:lda|ald|sta|ast)|n\.(?:ld|st)|ld|st)\.(8|16|32|p)',op)
    if not match: raise Error('unsupported instruction')
    form,width=match.groups(); size={'8':1,'16':2,'32':4,'p':3}[width]
    if len(args)!=(1 if form in ('m.lda','m.sta') else 2): raise Error(f'{op} has wrong number of arguments')
    value_reg=1 if form in ('m.lda','m.sta') else reg(args[0]); mem=args[-1]
    if not (mem.startswith('[') and mem.endswith(']')): raise Error('expected [base, offset] memory operand')
    fields=split_args(mem[1:-1]);
    if len(fields) not in (1,2): raise Error('memory operand needs a base and optional offset')
    base=reg(fields[0]); offset=expr(fields[1],symbols,pc) if len(fields)==2 else 0
    fits_minor=offset>=0 and offset%size==0 and offset//size<=7
    if form.startswith('m.'):
        if not fits_minor: raise Error(f'{form} displacement {offset} bytes must be a multiple of {size} in 0..{7*size}')
        if form in ('m.ald','m.ast') and base != 1: raise Error(f'{form} requires ac as base')
        return op,2
    if form in ('ld','st') and fits_minor:
        if value_reg==1: return f"m.{'lda' if form=='ld' else 'sta'}.{width}",2
        if base==1: return f"m.{'ald' if form=='ld' else 'ast'}.{width}",2
    if not -256<=offset<=255: raise Error(f'non-minor displacement {offset} outside -256..255 bytes')
    return f"n.{'ld' if form in ('ld','n.ld') else 'st'}.{width}",4

def link(obj):
    records=obj['records']; sizes=[4 if r['kind']=='instruction' else 0 for r in records]; seen=set()
    for iteration in range(100):
        key=tuple(sizes)
        if key in seen: raise Error('layout did not converge (repeated instruction sizes)')
        seen.add(key); symbols={}; pc=0; positions=[]
        for r in records:
            if r['kind']=='constant': symbols[r['name']]={'kind':'constant','value':r['value']}
        for i,r in enumerate(records):
            positions.append(pc); kind=r['kind']
            try:
                if kind=='label': symbols[r['name']]={'kind':'label','value':pc}
                elif kind=='instruction': pc+=sizes[i]
                elif kind=='data': pc+=data_size(r['args'],r['width'],symbols,pc,validate=False)
                elif kind=='align':
                    n=expr(r['value'],symbols,pc)
                    if n<=0: raise Error('alignment must be positive')
                    pc+=(-pc)%n
                elif kind=='jumpto':
                    target=expr(r['value'],symbols,pc)
                    if target<pc: raise Error('@jumpto cannot move backwards in this prototype')
                    pc=target
            except Error as e: raise Error(f"{r['file']}:{r['line']}: {e}") from None
        new=sizes.copy(); listing=[]; errors=[]
        for i,r in enumerate(records):
            if r['kind']=='data':
                try: data_size(r['args'],r['width'],symbols,positions[i])
                except Error as e: errors.append(f"{r['file']}:{r['line']}: {e}")
            if r['kind']!='instruction': continue
            try:
                if positions[i]%2: raise Error(f'instruction at odd byte index {positions[i]}')
                chosen,new[i]=select(r,symbols,positions[i]); listing.append((positions[i],new[i],chosen,r['line']))
            except Error as e: errors.append(f"{r['file']}:{r['line']}: {e}")
        if errors: raise Error('\n'.join(errors))
        if new==sizes:
            constant_errors=[]
            for r in records:
                if r['kind']=='constant':
                    try: expr('$'+r['name'],symbols,0)
                    except Error as e: constant_errors.append(f"{r['file']}:{r['line']}: {e}")
            if constant_errors: raise Error('\n'.join(constant_errors))
            return listing,symbols
        sizes=new
    raise Error('layout exceeded iteration limit')

