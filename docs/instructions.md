currently_in:

w = watched and may be redone
p = pending
f = future(may be redone)


majors:

	2 | ld		|			|
	2 | st		|			|
	1 | push8	|			|
	1 | push16	|			|
	1 | push32	|			|
	1 | retired pushp slot | Faults on ZLX-1.
	1 | pop8	|			|
    1 | pop16	|			|
	1 | pop32	|			|
	1 | retired popp slot  | Faults on ZLX-1.
	2 | get16	|			|
	3 | get32	|			|
    2 | ori, andi, xori     | 5_b5 | signed 11-bit immediate
    2 | slli, srli, srai    | 5_b5 | low 5 bits of the 11-bit shift count
    2 | extb                | 5_b5 | rd = (rs >> index) & 1, bit 0 is least significant, ZLX-1 accepts 0..31
    2 | sext                | 5_b5 | sign-extend the low 1..32 bits of the source
    1 | anda, xora, ora     | 5    | accumulator receives the result
    1 | aand, axor, aor     | 5    | named register receives the result
	2 | addi	|			|
	2 | bneq	|			|
	2 | beq		|			|
	2 | blt		|			|
	2 | blt		|			|
	2 | bge		|			|
	2 | bltu	|			|
	2 | bgeu	|			|
    1 | j       | 5         | jump to the even byte address in the register and preserve all registers
    1 | jlink | register | link through rp and jump to the absolute register address
	1 | jcall	|			|
	1 | jret	|			|
    2 | add, sub, sll, srl, sra, xor, or, and | 5_5551 | optional accumulator move
    2 | andn | 5_5551 | rd = lhs & ~rhs, optional accumulator move
    2 | clt, cge, cltu, cgeu, ceq, cneq | 5_5551 | register comparison with optional accumulator move

Comparisons take `rd, lhs, rhs` and write 1 for true or 0 for false to `rd`.
`clt` and `cge` compare signed 32-bit integers. The `u` forms compare unsigned
values. `ceq` and `cneq` compare all 32 bits for equality and inequality.

Each register major above has its own provisional opcode. In parcel two,
the low six bits are the move register followed by the direction bit. Direction
zero copies ac to the named register. Direction one copies that register to ac.
Six zero bits discard the copy into z0.

The move latches its source first. The ALU then reads its operands, including the
moved value if it reads the move destination. Both writes commit together. Two
writes to the same nonzero register fault before either write commits.

The one-parcel major `55` format uses `[class5][magnitude5][register5][0]`.

| Class | Instruction | Magnitude |
| --- | --- | --- |
| 2, 3 | inc, dec | 1…32. Field zero encodes 32. |
| 4, 5, 6 | slli, srli, srai in place | 1…31. Field zero is reserved on ZLX-1. |
| 7, 8 | getpos, getneg | 1…32. Write the positive or negative magnitude. |

Memory width codes 0, 1, and 2 select 8, 16, and 32 bits. Code 3 faults on ZLX-1 and is reserved for 64-bit transfers. The retired push and pop slots are reserved for 64-bit stack operations.

`bcall10`, `bcall21`, and `jcall` save the full 32-bit old rp in a four-byte stack slot. `jret` restores it and advances sp by four bytes. Rebuild code that assumes the old three-byte stack layout.

The assembler accepts signed two-operand `addi` and `get5`. Each selects the
corresponding positive or negative family. Ordinary `get` also accepts zero.

Fetch checks the minor tag first. For a major, the upper five bits select a
class. Classes 2…8 declare one parcel independently of the immediate. Classes
0 and 1 defer to the assigned suboperation's format to determine length.
Unassigned classes have no declared length. A known fetch length does not
authorize execution of reserved encodings.

minors
    1 | bcall10 disp | Push old rp, link and branch by signed imm10 parcels.
    1 | blink10 disp | Link and branch by signed imm10 parcels without pushing rp.
    2 | b21 | Branch by a signed 21-bit parcel displacement.
    1 | addpc rd | rd = address of this instruction + ac (32-bit wrap).
    2 | bcall21, blink21 | Corresponding signed imm21-parcel branches. High five bits precede low sixteen bits, as in get32.
    Reserved for W hardware: mvw, wmv and wmvw (slots 22, 23 and 24). ZLX-1 faults on both.
    27 slots allocated including the three W reservations. Five slots remain free.
	1 |	mv		|			| 
	1 | add		|			|
	1 | adda	|			|
	1 | sub		|			|
	1 | geta	|			|
	1 | get8	|			|
	1 | b		|			|
	1 | baeqz	|			|
	1 | baneqz	|			|
	1 | lda		|			|
	1 | ald		|			|
	1 | sta		|			|
	1 | ast		|			|
	1 | stsa	|			|
	1 | ldsa	|			|
	1 | ld+a	|			|
	1 | st+a	|			|
	1 | bieqc	|			| ix == cx, iy == cy, ix != cx, iy != cy  
    1 | bieqc+, bineqc+ | 28 | compare the old iterator and counter, then update the iterator
	1 | bteqa	|			|
	1 | btneqa	|			|






possible additions 

majors: 
	2 | mul		|			| r1 = r2 * r3
	2 | mulh	|			| r1(high) = r2 * r3
	2 | div		|			| r1 = r2 / r3
	2 | rem		|			| r1 = r2 % r3
	
	1 | andia	|			| ac = ac & imm5
	1 | oria	|			| ac = ac | imm5
	1 | xoria	|			| ac = ac | imm5

	1 | swpa    |			| swap ac r1
	1 | rev		|			| reverse r1	
//maybe not
	2 | cigetc  |			| clear itterator and get 16 bit counter 

//for now, cause i don't
	1 | rota	|			| rotate 
	1 | rotoa	|			| rotate other way 

	2 | ld2		|			| r1 = mem_size[r2, r3]
	2 | st2		|			| mem[r2, r3] = r1

	2 | push3	|			| push 3 registers of 3 sizes. 
	2 | pop3	|			| push 3 registers of 3 sizes. 
	1 | auipc	|			|
