# Zerolox assembler design experiment

Run `python3 experiments/tobe/assembler/assembler.py experiments/tobe/assembler/example.zlx --object /tmp/example.json -o /tmp/example.bin` from the repository root. The output lists selected instruction forms and byte indexes. JSON is the serialized structured object; `assemble()` and `link()` also accept and return Python structures directly. `-o` writes an experimental flat image for the current emulator.

The default target is `zerolox1-emu-v2`. Its opcode numbers come from `src/zerolox/commons/isa/ISA_zerolox.h` in the checkout. New JSON objects record this target and the source syntax `zerolox-source-v1`. The CLI reports both IDs. Flat binaries carry no version field.

To reproduce images for the original emulator, select `--target zerolox1-legacy-emu-v1`. Existing objects tagged `zerolox1` or `zerolox1-legacy-emu-v1` use that encoding, including the grouped register and six-bit immediate majors. Reassemble source for the default target to run it on the updated emulator. See [docs/instructions.md](../../../docs/instructions.md) for the ISA inventory.

Current source examples:

| Source | Meaning |
| --- | --- |
| `ld t1, [t0, 5]` | Load one byte from the address in `t0` plus five bytes. |
| `lda.16 [t0, 6]` | Load a 16-bit value into the implied accumulator. |
| `ld+a [t4{1}]` | Load one byte into the accumulator, then advance `t4` by one access unit. |
| `st+a.p [s0{-1}]` | Store the accumulator, then move `s0` back one pointer-sized unit. |
| `beq ix, cx, #target` | Select the compact iterator comparison when its displacement fits. |
| `bxeqc #target` | Explicit compact X iterator and counter comparison. |
| `get t0, 44` | Select the smallest available immediate load that fits. |
| `ret` | Return with the accumulator as the implied result register. |

The X/Y and equality/inequality iterator aliases share the legacy `bieqc` opcode. Width defaults to 8 bits for memory operations. Address offsets count bytes while the legacy post-increment field counts access units. Older experimental spellings remain accepted for existing sources.

This is a deliberately incomplete syntax, layout and image experiment. It supports `NAME = expression`, labels, `!db`, `!dw`, `%align` and `@jumpto`, move, add, direct `m.geta` and `m.get8`, several load/store forms, and minor unconditional and accumulator-zero branches. `r0` through `r31` are experimental aliases alongside the names in the ISA. Expressions accept decimal, hexadecimal and binary integer literals, size suffixes, `$constant`, `&label`, `#label`, parentheses, `+`, `-`, and `*`. `#` measures from the start of the containing instruction, matching the current emulator's branch PC behavior. The prototype rejects register-offset addressing because no corresponding form has been implemented. The larger `program.zlx` is a syntax exercise, not a defined ABI.

Line comments start with `//`, `/:`, or the existing experimental `;` delimiter. Comment markers inside quoted strings and character literals remain literal text.

Mnemonic records share one pseudo-instruction structure: `operation`, `args`, optional `width`, and `form` (`null`, `m`, or `n`). A null form permits selection; an explicit form constrains it. Direct operations such as `m.adda` retain their own operation name and operand form. The object format is experimental version 2 and does not read version 1 objects. Subtract, shifts, and bitwise operations are also available; their selectable and direct spellings are experimental.

`!db` writes byte-sized values, `!dw` describes 32-bit values, and `{ ... }` groups data elements. Double-quoted byte strings add a zero byte; single-quoted character literals are one value without a terminator. Plain string source is ASCII with byte escapes. Literal parsing currently uses Python's string escapes, which are not a complete C escape specification; Unicode-prefixed strings, floating point and arbitrary-length bit strings are not implemented. Binary integer literals are supported. The experimental image uses little-endian data and zero-filled gaps and alignment, matching the current emulator's byte order; padding policy is not a finalized language rule. `%align` must precede a label when the label should name the aligned position.

Layout starts with large instruction sizes, reselects forms, and detects repeated states. It is not a complete relaxation algorithm. Forward references in fixed-width data are supported; forward references in layout operations are not, and a size-dependent reference can still fail if fitting a short form requires first assuming the short form itself. `@jumpto` only moves forward. No major unconditional branch is assumed. Direct forms use experimental spellings such as `m.add r1, r2`, `m.adda r1, r2`, and `m.lda.16 [r1, offset]`; `m.ald` and `m.ast` require an explicit `[ac, offset]`.

Pointer size is fixed to three bytes for zerolox one. The prototype has no cross-file linking, external symbols, or general target configuration. Run `ZEROLOX_EMULATOR=/path/to/ZLX_emu python3 -m unittest discover -s experiments/tobe/assembler -v` to run the emulator integration cases. The emulator runs a specified number of instructions; images have no automatic halt.

See `INSTRUCTIONS.md` for the current complete enum-form inventory, experimental operand spellings, and known emulator ambiguities. Form coverage does not mean every operation has been independently executed in the emulator.
