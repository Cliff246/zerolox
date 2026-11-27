"""Map current source spellings onto the legacy emulator's instruction forms."""
import re

from extra import IN_PLACE

from model import CURRENT_ISA, V3_ISA, legacy_encoding, Error, REGISTERS
from syntax import split_args
from values import expr, reg


ITERATOR_BRANCH = {'bxeqc': 0, 'bxneqc': 1, 'byeqc': 2, 'byneqc': 3}
MEMORY = r'(ld|st|lda|sta|ald|ast|ldsa|stsa|ld\+a|st\+a)(?:\.(8|16|32))?'


def accepts(op):
    return (op in ITERATOR_BRANCH or op in ('get', 'ret', 'retp', 'bieqc', 'bteqa', 'btneqa',
                                           'baeqz', 'baneqz', 'geta', 'get8', 'adda')
            or re.fullmatch(r'(?:[mn]\.)?' + MEMORY, op)
            or re.fullmatch(r'(push|pop)(8|16|32)', op.removeprefix('n.')))


def lower(op, args, symbols, pc, target=CURRENT_ISA):
    """Return an encoding mnemonic and operands without changing the object."""
    if target == V3_ISA and op in IN_PLACE and len(args) == 2:
        return 'm.' + op, args
    if op == 'get':
        if len(args) != 2:
            raise Error('get requires a register and an immediate')
        dest = reg(args[0])
        value = expr(args[1], symbols, pc)
        if dest == reg('ac') and -512 <= value <= 511:
            return 'm.geta', args[1:]
        if dest in range(reg('t0'), reg('t3') + 1) and 0 <= value <= 255:
            return 'm.get8', args
        if not legacy_encoding(target):
            if 0xffffffe0 <= value <= 0xffffffff:
                value -= 1 << 32
            if value == 0:
                return 'm.mv', [args[0], 'z0']
            if target == CURRENT_ISA and -32 <= value <= 32:
                return 'get5', [args[0], str(value)]
            if target == V3_ISA and -16 <= value <= 16:
                return 'm.get5', [args[0], str(value)]
        return ('n.get16' if 0 <= value <= 65535 else 'n.get32'), args

    if op in ('beq', 'bneq') and len(args) == 3:
        pair = {reg(args[0]), reg(args[1])}
        for iterator, counter, selector in [('ix', 'cx', 0), ('iy', 'cy', 2)]:
            if pair == {reg(iterator), reg(counter)}:
                offset = expr(args[2], symbols, pc)
                if -256 <= offset <= 254:
                    return 'm.bieqc', [str(selector + (op == 'bneq')), args[2]]
        if reg('ac') in pair and len(pair) == 2:
            other = (pair - {reg('ac')}).pop()
            if other == reg('z0') and -1024 <= expr(args[2], symbols, pc) <= 1022:
                return ('m.baeqz' if op == 'beq' else 'm.baneqz'), args[2:]
            if reg('t0') <= other <= reg('t3') and -256 <= expr(args[2], symbols, pc) <= 254:
                return ('m.bteqa' if op == 'beq' else 'm.btneqa'), [f'r{other}', args[2]]

    if op in ITERATOR_BRANCH:
        if len(args) != 1:
            raise Error(f'{op} requires a displacement')
        return 'm.bieqc', [str(ITERATOR_BRANCH[op]), args[0]]
    if op in ('bieqc', 'bteqa', 'btneqa', 'baeqz', 'baneqz', 'geta', 'get8', 'adda'):
        return 'm.' + op, args
    if op in ('add', 'sub') and len(args) == 2:
        return 'm.' + op, args
    if op in ('jret', 'n.jret') and not args:
        return op, ['ac']
    stack = re.fullmatch(r'(n\.)?(push|pop)(8|16|32)', op)
    if stack:
        prefix, name, width = stack.groups()
        return f'{prefix or ""}{name}.{width}', args

    prefix = op[:2] if op.startswith(('m.', 'n.')) else ''
    memory = re.fullmatch(MEMORY, op[len(prefix):])
    if memory:
        name, width = memory.groups()
        width = width or '8'
        if name in ('ld+a', 'st+a'):
            if prefix == 'n.' or len(args) != 1:
                raise Error(f'{op} requires one post-increment address')
            address = re.fullmatch(r'\[\s*([A-Za-z_][A-Za-z_0-9]*)\s*\{([^{}]+)\}\s*\]', args[0])
            if not address:
                raise Error(f'{op} requires [base{{increment}}]')
            return f"m.{name[:2]}plus.{width}", list(address.groups())
        if name in ('ld', 'st'):
            return f'{prefix}{name}.{width}', args
        if prefix == 'n.':
            raise Error(f'{op} has no natural form in the legacy emulator ISA')
        if name in ('ald', 'ast', 'ldsa', 'stsa') and args and args[-1].startswith('[') and args[-1].endswith(']'):
            fields = split_args(args[-1][1:-1])
            # The operand names the offset when the instruction implies the base.
            if len(fields) == 1 and fields[0].lower() not in REGISTERS:
                fields.insert(0, 'ac' if name in ('ald', 'ast') else 'sp')
            if name in ('ldsa', 'stsa'):
                if len(fields) not in (1, 2) or reg(fields[0]) != reg('sp'):
                    raise Error(f'{op} requires a stack address')
                args = args[:-1] + [fields[1] if len(fields) == 2 else '0']
            else:
                args = args[:-1] + ['[' + ', '.join(fields) + ']']
        return f'm.{name}.{width}', args
    return op, args
