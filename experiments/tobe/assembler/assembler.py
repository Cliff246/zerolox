#!/usr/bin/env python3
"""Experimental zerolox syntax and linker model; not a machine-code encoder."""
import argparse
import ast
import json
import re
import sys
from pathlib import Path

REGISTERS = {'z0': 0, 'ac': 1, 'sp': 2, 'rp': 3, 'ix': 4, 'iy': 5, 'cx': 6, 'cy': 7}
REGISTERS.update({f't{i}': 8+i for i in range(6)})
REGISTERS.update({f's{i}': 14+i for i in range(10)})
REGISTERS.update({f'a{i}': 24+i for i in range(8)})
# rN is a prototype alias for a physical register number.
REGISTERS.update({f'r{i}': i for i in range(32)})

class Error(Exception): pass

def split_args(s):
    parts, start, depth, quote, escape = [], 0, 0, None, False
    for i, c in enumerate(s):
        if quote:
            if escape: escape = False
            elif c == '\\': escape = True
            elif c == quote: quote = None
        elif c in "'\"": quote = c
        elif c in '([{': depth += 1
        elif c in ')]}':
            depth -= 1
            if depth < 0: raise Error('unmatched closing bracket')
        elif c == ',' and depth == 0:
            parts.append(s[start:i].strip()); start = i+1
    if quote or depth: raise Error('unterminated string or bracket')
    if s.strip(): parts.append(s[start:].strip())
    if any(not p for p in parts): raise Error('empty argument')
    return parts

def strip_comment(s):
    quote, escape = None, False
    for i,c in enumerate(s):
        if quote:
            if escape: escape = False
            elif c == '\\': escape = True
            elif c == quote: quote = None
        elif c in "'\"": quote = c
        elif c == ';': return s[:i]
    return s

def expr(text, symbols, here, stack=()):
    # Replace the deliberately small expression language with safe Python AST nodes.
    def unit(m): return str(int(m.group(1), 0) * {'b':1,'s':2,'w':4,'l':8,'p':3}[m.group(2)])
    text = re.sub(r'\b(0x[0-9a-fA-F]+|\d+)([bswlp])\b', unit, text)
    refs = {}
    def ref(m):
        key = f'__ref{len(refs)}'; refs[key] = (m.group(1),m.group(2)); return key
    text = re.sub(r'([&#$])([A-Za-z_][A-Za-z_0-9]*)', ref, text)
    try: tree = ast.parse(text.strip(), mode='eval')
    except SyntaxError: raise Error('invalid value expression') from None
    def walk(n):
        if isinstance(n, ast.Constant) and type(n.value) is int: return n.value
        if isinstance(n, ast.Name) and n.id in refs:
            kind,name = refs[n.id]
            if name not in symbols: raise Error(f'undefined symbol {name}')
            entry = symbols[name]
            if kind == '$':
                if entry['kind'] != 'constant': raise Error(f'${name} requires a constant')
                if name in stack: raise Error(f'cyclic constant {name}')
                return expr(entry['value'], symbols, here, stack+(name,))
            if entry['kind'] != 'label': raise Error(f'{kind}{name} requires a label')
            return entry['value'] - here if kind == '#' else entry['value']
        if isinstance(n, ast.UnaryOp) and isinstance(n.op, (ast.USub,ast.UAdd)):
            v=walk(n.operand); return -v if isinstance(n.op,ast.USub) else v
        if isinstance(n, ast.BinOp) and isinstance(n.op,(ast.Add,ast.Sub,ast.Mult)):
            a,b=walk(n.left),walk(n.right)
            return a+b if isinstance(n.op,ast.Add) else a-b if isinstance(n.op,ast.Sub) else a*b
        raise Error('invalid expression: use integers, references, +, -, and *')
    return walk(tree.body)

def assemble(source, filename='<input>'):
    records=[]; names=set(); errors=[]
    for line_no, raw in enumerate(source.splitlines(),1):
        line=strip_comment(raw).strip()
        if not line: continue
        try:
            rec={'line':line_no, 'file':filename}
            if re.fullmatch(r'[A-Za-z_][A-Za-z_0-9]*:',line):
                name=line[:-1]
                if name.lower() in REGISTERS or name in names: raise Error(f'duplicate or reserved symbol {name}')
                names.add(name); rec.update(kind='label',name=name)
            elif (definition := re.fullmatch(r'([A-Za-z_][A-Za-z_0-9]*)\s*=\s*(.+)', line)):
                name, value = definition.groups()
                if name.lower() in REGISTERS or name in names: raise Error(f'duplicate or reserved symbol {name}')
                names.add(name); rec.update(kind='constant',name=name,value=value)
            else:
                pieces = line.split(None, 1)
                op, tail = pieces[0], pieces[1] if len(pieces) == 2 else ''
                op=op.lower(); args=split_args(tail.strip())
                if op in ('!db', '!dw'):
                    if not args: raise Error(f'{op} requires data')
                    rec.update(kind='data',width=1 if op=='!db' else 4,args=args)
                elif op in ('@jumpto','%align'):
                    if len(args)!=1: raise Error(f'{op} requires one value')
                    rec.update(kind=op[1:],value=args[0])
                elif op in ('add','m.add','m.adda','n.add','b','m.b') or re.fullmatch(r'(?:m\.lda|n\.ld|ld)\.(?:8|16|32|p)',op):
                    rec.update(kind='instruction',op=op,args=args)
                else: raise Error(f'unknown operation {op}')
            records.append(rec)
        except Error as e: errors.append(f'{filename}:{line_no}: {e}')
    if errors: raise Error('\n'.join(errors))
    return {'format':'zerolox-experiment-1','target':'zerolox1','records':records}

def reg(s):
    try: return REGISTERS[s.lower()]
    except KeyError: raise Error(f'expected register, got {s}') from None

def data_size(args, width, symbols, here):
    """Validate data and return its size; multi-byte endianness is not decided."""
    def item(arg):
        if arg.startswith('{') and arg.endswith('}'):
            members=split_args(arg[1:-1])
            if not members: raise Error('empty data list')
            return sum(item(member) for member in members)
        if arg.startswith(('"', "'")):
            try: value=ast.literal_eval(arg)
            except (SyntaxError,ValueError): raise Error('invalid character or string literal') from None
            if not isinstance(value,str): raise Error('expected character or string')
            if arg[0]=="'":
                if len(value)!=1 or ord(value)>255: raise Error('character literal must be one byte')
                return width
            if width!=1: raise Error('byte string requires !db')
            if any(ord(c)>255 for c in value): raise Error('byte string contains a non-byte character')
            # Unescaped non-ASCII text is rejected; byte escapes such as \xff are allowed.
            if any(ord(c)>127 for c in arg): raise Error('plain strings require ASCII text or byte escapes')
            return len(value)+1
        if arg.startswith(('u8"','u16"','u32"')):
            raise Error('Unicode string encoding is not specified in this prototype')
        v=expr(arg,symbols,here)
        if not 0 <= v < (1 << (8*width)): raise Error(f'value {v} outside unsigned {width*8}-bit range')
        return width
    return sum(item(arg) for arg in args)

def select(rec,symbols,pc):
    op,args=rec['op'],rec['args']
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
    if op in ('b','m.b'):
        if len(args)!=1: raise Error(f'{op} requires one offset')
        off=expr(args[0],symbols,pc)
        if off%2: raise Error(f'branch offset {off} is not parcel aligned')
        if not -1024<=off<=1022: raise Error(f'm.b offset {off} outside -1024..1022 bytes')
        return 'm.b',2
    match=re.fullmatch(r'(m\.lda|n\.ld|ld)\.(8|16|32|p)',op)
    if not match: raise Error('unsupported instruction')
    form,width=match.groups(); size={'8':1,'16':2,'32':4,'p':3}[width]
    if len(args)!=(1 if form=='m.lda' else 2): raise Error(f'{op} has wrong number of arguments')
    dest=1 if form=='m.lda' else reg(args[0]); mem=args[-1]
    if not (mem.startswith('[') and mem.endswith(']')): raise Error('expected [base, offset] memory operand')
    fields=split_args(mem[1:-1]);
    if len(fields) not in (1,2): raise Error('memory operand needs a base and optional offset')
    reg(fields[0]); offset=expr(fields[1],symbols,pc) if len(fields)==2 else 0
    minor=dest==1 and offset>=0 and offset%size==0 and offset//size<=7
    if form=='m.lda':
        if not minor: raise Error(f'm.lda displacement {offset} bytes must be a multiple of {size} in 0..{7*size}')
        return op,2
    if form=='ld' and minor: return f'm.lda.{width}',2
    if not -256<=offset<=255: raise Error(f'n.ld displacement {offset} outside -256..255 bytes')
    return f'n.ld.{width}',4

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
                elif kind=='data': pc+=data_size(r['args'],r['width'],symbols,pc)
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
            if r['kind']!='instruction': continue
            try:
                if positions[i]%2: raise Error(f'instruction at odd byte index {positions[i]}')
                chosen,new[i]=select(r,symbols,positions[i]); listing.append((positions[i],new[i],chosen,r['line']))
            except Error as e: errors.append(f"{r['file']}:{r['line']}: {e}")
        if errors: raise Error('\n'.join(errors))
        if new==sizes: return listing,symbols
        sizes=new
    raise Error('layout exceeded iteration limit')

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('source',type=Path); parser.add_argument('--object',type=Path,help='write structured JSON object')
    args=parser.parse_args()
    try:
        obj=assemble(args.source.read_text(),str(args.source))
        if args.object: args.object.write_text(json.dumps(obj,indent=2)+'\n')
        listing,symbols=link(obj)
        for pc,size,op,line in listing: print(f'{pc:06x}  {size} bytes  {op:12}  line {line}')
        for name,s in symbols.items():
            if s['kind']=='label': print(f"{name}: {s['value']:#x}")
    except (Error,OSError,ValueError) as e: print(e,file=sys.stderr); return 1
    return 0

if __name__=='__main__': sys.exit(main())
