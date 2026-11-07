# Zerolox C Naming and Formatting Guide

Version: 0.3 (enum values and all-uppercase class clarification)  
Scope: Zerolox C source, shared libraries, and related code.  
Status: Naming principles approved for a conservative cleanup; unresolved conventions below are **not** authorization for speculative renames.

## 1. Purpose

Names should convey **namespace, purpose, and class identity** without growing needlessly long. Capitalization has meaning: the capitalized class at the end of a function name distinguishes class-specific behavior from a function belonging directly to the namespace.

A mechanical cleanup must not change behavior, architecture, binary layout, allocation semantics, or unfinished work.

## 2. Namespaces: the last two levels

Namespace prefixes identify the **immediate parent and current namespace**, not the full ancestry. The first project/subsystem pair is typically:

- `ZLX_ISA` — Zerolox / ISA
- `ZLX_ASM` — Zerolox / assembler
- `ZLX_EMU` — Zerolox / emulator
- `ZLX_SYTX` — Zerolox / syntax

For a namespace nested below ISA, use `ISA_THAT`, **not** `ZLX_ISA_THAT`. Its child could be `THAT_OTHER`, not `ZLX_ISA_THAT_OTHER`.

The namespace pair must remain distinguishable from other namespaces. `MY` is an existing standalone library prefix. Do not mechanically concatenate every ancestor's name.

## 3. Functions

### Namespace-level functions

Use `NAMESPACE_verb_noun`, or a similarly clear verb-led description, with **no capitalized class ending**. Examples currently in Zerolox:

    ZLX_ISA_get_register_number(...)
    MY_sign_extend32(...)
    MY_cap_to_bytes(...)

### Class-specific functions

Use `NAMESPACE_verb_[noun_or_details_]Class`.

The class name **starts with a capital letter**, so the class boundary is visible:

    ZLX_ASM_init_Statement(...)
    ZLX_ASM_free_Statement(...)
    ZLX_EMU_read_8_Memory(...)
    ZLX_EMU_write_32_Memory(...)

Verb-first naming is the general rule. `init` and `free` are the conventional lifecycle verbs and ordinarily appear first in the class's API. Not all classes need both.

### All-uppercase class names

When the class itself is **entirely uppercase**, use **two underscores** at the class boundary. This is a general rule for *any* all-uppercase class (for example, `CPU` or `FPGA`), not a special case for the CPU:

    ZLX_EMU_init__CPU(...)
    ZLX_EMU_free__CPU(...)
    ZLX_EMU_get_pc__CPU(...)
    EXAMPLE_NS_init__FPGA(...)

The related type and filename may also preserve it: `ZLX_EMU__CPU_t`, `EMU__CPU.c`.

**Do not collapse these double underscores or replace them with single underscores.**

### Static functions

Internal `static` functions are **exempt** from the namespace/class naming requirements. They can have concise, locally clear names. They are still subject to ordinary formatting.

## 4. Types, enums, and files

Struct/class types use an **uppercase-initial class name** and an existing `_t` typedef suffix:

    ZLX_ASM_Statement_t
    ZLX_ASM_Operation_t
    ZLX_EMU_Memory_t
    ZLX_EMU__CPU_t

Enum type/category names should be **lowercase**, rather than using class-style capitalization. Example intended spelling:

    ZLX_ASM_statement_type_t

**Enumerator values begin with the exact enum type name without the `_t` suffix**, followed by a descriptive value suffix. Both enum type names and the corresponding value prefix are lowercase after the uppercase namespace pair:

    ZLX_ASM_statement_type_t
    ZLX_ASM_statement_type_invalid
    ZLX_ASM_statement_type_operation

    ZLX_ISA_minor_operation_t
    ZLX_ISA_minor_operation_add

For enums defined through X-macros, keep the generated value names consistent with the enum type as well. Apply renames consistently to all references; preserve the underlying numeric values and declaration order.

Class-specific filenames use capitalized class identifiers:

    ASM_Statement.c / ASM_Statement.h
    EMU_Memory.c / EMU_Memory.h
    EMU__CPU.c / EMU__CPU.h

Namespace-level utility files can use lowercase names:

    ISA_zerolox.c
    SYTX_syntax.c
    MY_commons.c

Folders represent subsystem organization. Filename conventions and symbol naming should agree, without forcing unnecessary directory restructuring.

## 5. Parameters and clarity

Function **parameters** should generally start with `_`:

    uint8_t ZLX_EMU_read_8_Memory(
        ZLX_EMU_Memory_t *_memory,
        uint32_t _address
    );

This is a preference, not a requirement to rewrite every local variable. Local variables and members do not automatically get the parameter prefix.

If a shortened name could reasonably mean different things, **spell it out**. Keep well-understood terms such as CPU, ISA, and ASM. Choose the shortest name that remains unambiguous; do not make names longer simply to make them longer.

## 6. Field and pointer prefixes

Field prefixes are a separate convention from namespace and class naming. Current code uses forms such as:

    a__registers
    p__machine
    pa__file_contents
    pap__arguments
    cp__lexer

These have been used to signal pointer/array structure and/or the cleanup procedure. **The exact prefix grammar, especially complex-free behavior, is not yet finalized.** Do not invent ownership meanings or rewrite the prefixes automatically. Pointer shape alone does not prove how something should be freed.

## 7. Formatting baseline

The existing codebase generally uses:

- Tabs for indentation.
- Opening braces on new lines for functions, declarations, and control blocks.
- Consistent whitespace after commas and around operators.
- Clear grouping of related includes, declarations, and definitions.
- Matching `.c` and `.h` names where appropriate.
- Preservation of comments, TODOs, and deliberately incomplete code.

Avoid sweeping formatting of unrelated files where that would obscure meaningful diffs. Do not change expression evaluation, control flow, layout, or data ownership under the guise of formatting.

## 8. Rules for applying a cleanup

1. Preserve architecture, functionality, instruction semantics, and unfinished placeholders.
2. Treat names as structured: namespace, action, optional details, and (when present) class.
3. Preserve intentional case distinctions and `__CPU`-style boundaries.
4. Apply **last-two-level** namespaces, not full recursive prefixes.
5. Leave `static` helper naming alone unless a clear local improvement is requested.
6. Use `_` for parameters where the edit is safe and consistent.
7. Spell out genuinely ambiguous names.
8. Update definitions, declarations, call sites, includes, and build references together.
9. Keep mechanical renaming, formatting, and functional bug fixes separately reviewable.
10. Prefer small, inspectable diffs. Do not delete files, TODOs, or code because it is unfinished.
11. Leave unspecified or ambiguous naming cases unchanged for review.

## 9. Still to be specified

- Union and auxiliary-type naming.
- Full pointer/array/complex-cleanup prefix grammar.
- Exact formatting defaults in areas where established files differ.

These are explicitly **open questions**, not implicit permissions to choose a new convention.
