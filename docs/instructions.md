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
	1 | pushp	|			|
	1 | pop8	|			|
    1 | pop16	|			|
	1 | pop32	|			|
	1 | popp	|			|
	2 | get16	|			|
	3 | get32	|			|
    2 | ori, andi, xori     | 5_b5 | signed 11-bit immediate
    2 | slli, srli, srai    | 5_b5 | low 5 bits of the 11-bit shift count
    2 | sext                | 5_b5 | sign-extend the low 1..32 bits of the source
    1 | inc1..8, dec1..8    | 5    | inc and dec are aliases for inc1 and dec1
    1 | sl1..8, sr1..8, sra1..8 | 5 | shift the named register in place
    1 | anda, xora, ora     | 5    | accumulator receives the result
    1 | aand, axor, aor     | 5    | named register receives the result
    1 | inca, deca          | 5    | add or subtract imm5 from ac
    1 | srra, srla, slla    | 5    | arithmetic right, logical right or left shift of ac by imm5
	2 | addi	|			|
	2 | bneq	|			|
	2 | beq		|			|
	2 | blt		|			|
	2 | blt		|			|
	2 | bge		|			|
	2 | bltu	|			|
	2 | bgeu	|			|
    1 | call	|			|
    1 | ret		|			|
	1 | callp	|			|
	1 | retp	|			|
    2 | add, sub, sll, srl, sra, xor, or, and | 5_5551 | optional accumulator move

Each of these eight register majors has its own provisional opcode. In parcel two,
the low six bits are the move register followed by the direction bit. Direction
zero copies ac to the named register. Direction one copies that register to ac.
Six zero bits discard the copy into z0.

The move latches its source first. The ALU then reads its operands, including the
moved value if it reads the move destination. Both writes commit together. Two
writes to the same nonzero register fault before either write commits.

minors 
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
	
	
//might be worth adding a m.addi where r1 = r1 + sext(imm5)
minors:
	1 | calli	| imm10		|






