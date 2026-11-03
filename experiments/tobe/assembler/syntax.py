"""Parser and structured object creation for the syntax experiment."""
import re
from model import Error, REGISTERS

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
                elif op in ('add','m.add','m.adda','n.add','sub','m.sub','n.sub','sll','srl','sra','xor','or','and','n.sll','n.srl','n.sra','n.xor','n.or','n.and','mv','m.mv','m.geta','m.get8','b','m.b','beqz','bnez','m.baeqz','m.baneqz') or re.fullmatch(r'(?:m\.(?:lda|ald|sta|ast)|n\.(?:ld|st)|ld|st)\.(?:8|16|32|p)',op):
                    parts=op.split('.')
                    form=parts.pop(0) if parts[0] in ('m','n') else None
                    rec.update(kind='instruction',operation=parts[0],form=form,width=parts[1] if len(parts)>1 else None,args=args)
                else: raise Error(f'unknown operation {op}')
            records.append(rec)
        except Error as e: errors.append(f'{filename}:{line_no}: {e}')
    if errors: raise Error('\n'.join(errors))
    return {'format':'zerolox-experiment-2','target':'zerolox1','records':records}

