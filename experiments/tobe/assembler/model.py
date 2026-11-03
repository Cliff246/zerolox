"""Shared types and register definitions."""

REGISTERS = {'z0': 0, 'ac': 1, 'sp': 2, 'rp': 3, 'ix': 4, 'iy': 5, 'cx': 6, 'cy': 7}
REGISTERS.update({f't{i}': 8+i for i in range(6)})
REGISTERS.update({f's{i}': 14+i for i in range(10)})
REGISTERS.update({f'a{i}': 24+i for i in range(8)})
# rN is a prototype alias for a physical register number.
REGISTERS.update({f'r{i}': i for i in range(32)})

class Error(Exception): pass

def mnemonic(record):
    """Render the operation and explicit form constraint for selection."""
    return (record['form'] + '.' if record['form'] else '') + record['operation'] + ('.' + record['width'] if record['width'] else '')

