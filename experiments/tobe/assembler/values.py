"""Value expressions and data declaration validation."""
import ast
import re
from model import Error, REGISTERS
from syntax import split_args

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

def reg(s):
    try: return REGISTERS[s.lower()]
    except KeyError: raise Error(f'expected register, got {s}') from None

def data_size(args, width, symbols, here, validate=True):
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
        if validate:
            v=expr(arg,symbols,here)
            if not 0 <= v < (1 << (8*width)): raise Error(f'value {v} outside unsigned {width*8}-bit range')
        return width
    return sum(item(arg) for arg in args)

