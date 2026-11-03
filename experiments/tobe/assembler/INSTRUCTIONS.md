# Experimental instruction spellings

This is a review aid for the current C decoder and emulator, not a finalized ISA manual. Each listed form emits one instruction. `m.` and `n.` constrain the form; unprefixed spellings below are experimental selectable aliases. A register is `r`, a value expression is `v`, and branch displacement `d` is in bytes from the instruction start and must be even. Memory syntax is `[base, byte_offset]`.

| Forms | Arguments |
| --- | --- |
| `mv`, `m.mv` | `destination, source` |
| `add`, `sub`, `sll`, `srl`, `sra`, `xor`, `or`, `and` and `n.` versions | `destination, left, right` |
| `m.add`, `m.adda`, `m.sub` | `register, register`; implicit accumulator as defined by the ISA |
| `m.geta` | signed 10-bit immediate |
| `m.get8` | `t0..t3, unsigned byte` |
| `get16`, `n.get16`; `get32`, `n.get32` | `destination, unsigned value` |
| `addi`, `n.addi` | `destination, source, signed immediate` |
| `slli`, `srli`, `srai`, `xori`, `andi`, `ori` and `n.` versions | `destination, source, immediate` |
| `ld.W`, `st.W`, `n.ld.W`, `n.st.W` | `value register, [base, offset]` |
| `m.lda.W`, `m.sta.W` | `[base, offset]`, accumulator implicit |
| `m.ald.W`, `m.ast.W` | `value register, [ac, offset]` |
| `m.ldsa.W`, `m.stsa.W` | unsigned byte offset; stack pointer and accumulator implicit |
| `m.ldplus.W`, `m.stplus.W` | `base register, increment in access-size units` |
| `b`, `beqz`, `bnez`; `m.b`, `m.baeqz`, `m.baneqz` | `d`; zero branches test accumulator |
| `beq`, `bneq`, `blt`, `bge`, `bltu`, `bgeu` and `n.` versions | `left register, right register, d` |
| `m.bieqc` | `selector 0..3, d`: selector bit 0 selects equal/not equal, bit 1 selects x/y pair |
| `m.bteqa`, `m.btneqa` | `t0..t3, d` |
| `push.W`, `pop.W`, `n.push.W`, `n.pop.W` | register |
| `inc`, `dec`, `call`, `ret`, `callp`, `retp` and `n.` versions | register |

`W` is `8`, `16`, `32` or `p`; `p` is three bytes on zerolox one. `m.ldplus` and `m.stplus` accept only -4, -2, -1, 1, 2, 3, 4, 8 as their increment, matching the current emulator's encoded selector; the operation accesses the old address, then updates the base. `m.ldplus` rejects `ac` as base because it conflicts with its implicit accumulator destination. `ret` and `retp` have a register operand because the current emulator copies `ac` to it. These spellings do not imply an ABI.

The C code's comments say stack-relative accumulator operations subtract their offsets, but the implementation adds them; this prototype encodes the raw nonnegative field and does not assume the comment is correct. There may be other implementation discrepancies. Direct form coverage is based on the current enum, decoding and execution switches, not a claim that every operation has been independently emulator-tested. The C decoder exposes major format categories, not a separate opcode for every arithmetic operation; the names above select their suboperations. See README for layout, string and image limitations.
