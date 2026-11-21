"""Read the same operation enums the C decoder and emulator use."""
from pathlib import Path
import re


def read_operations(header, kind):
    prefix = f'ZLX_ISA_{kind}_operation'
    body = re.search(rf'typedef\s+enum\s+{prefix}\s*\{{([^}}]+)\}}', header).group(1)
    operations = {}
    value = 0
    for entry in body.split(','):
        entry = entry.strip()
        if entry in ('', prefix + '_count'):
            continue
        match = re.fullmatch(rf'{prefix}_(\w+)(?:\s*=\s*(0[xX][0-9a-fA-F]+|\d+))?', entry)
        if not match:
            raise ValueError(f'Unsupported opcode enum entry: {entry}')
        name, assigned = match.groups()
        if assigned is not None:
            value = int(assigned, 0)
        operations[name] = value
        value += 1
    return operations


header = (Path(__file__).resolve().parents[3] / 'src/zerolox/commons/isa/ISA_zerolox.h').read_text()
header = re.sub(r'/\*.*?\*/|//[^\n]*', '', header, flags=re.S)
MAJOR = read_operations(header, 'major')
MINOR = read_operations(header, 'minor')


# These encoding numbers belong to existing legacy JSON objects.
LEGACY_MAJOR = {name: code for code, name in enumerate(
    'ld st push8 push16 push32 pushp pop8 pop16 pop32 popp get16 get32 '
    'reg_group imm_group inc dec addi bneq beq blt bge bltu bgeu call ret callp retp'.split())}
LEGACY_MINOR = {name: code for code, name in enumerate(
    'move add adda sub geta get8 b baeqz baneqz lda ald sta ast stsa ldsa ld_plus '
    'st_plus bieqc bteqa btneqa'.split())}
LEGACY_REG_SELECTOR = {name: n for n, name in enumerate('add sub sll srl sra xor or and'.split())}
LEGACY_IMM_SELECTOR = {name: n for n, name in enumerate('slli srli srai xori andi ori'.split())}
