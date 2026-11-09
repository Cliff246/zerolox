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
w	2 |	ori		| 3formi	|
w	2 |	andi	| 3formi	|
w	2 |	xori	| 3formi	|
w	2 |	slli	| 3formi	|
w	2 |	srli	| 3formi	|
w	2 |	srai	| 3formi	|
	1 | inc		|			|
	1 | dec		|			|
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
	2 | add		| 3form		|
	2 | sub		| 3form		|
	2 | sll		| 3form		|
	2 | srl		| 3form		|
	2 | sra		| 3from		|
	2 | xor		|			|
	2 | or		|			|
	2 | and		|			|




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
	1 | bteqa	|			|
	1 | btneqa	|			|






possible additions 

majors: 
	1 | sl1		|			| r1 <<= 1
	1 | sl2		|			| r1 <<= 2
	1 | sl3		|			| r1 <<= 3
	1 | sl4		|			| r1 <<= 4
	1 | sl5		|			| r1 <<= 5
	1 | sl6		|			| r1 <<= 6
	1 | sl7		|			| r1 <<= 7
	1 | sl8		|			| r1 <<= 8
	
	1 | sr1		|			| r1 >>= 1
	1 | sr2		|			| r1 >>= 2
	1 | sr3		|			| r1 >>= 3
	1 | sr4		|			| r1 >>= 4
	1 | sr5		|			| r1 >>= 5
	1 | sr6		|			| r1 >>= 6
	1 | sr7		|			| r1 >>= 7
	1 | sr8		|			| r1 >>= 8

	1 | sra1	|			| (int)r1 >>= 1
	1 | sra2	|			| (int)r1 >>= 2
	1 | sra3	|			| (int)r1 >>= 3
	1 | sra4	|			| (int)r1 >>= 4
	1 | sra5	|			| (int)r1 >>= 5
	1 | sra6	|			| (int)r1 >>= 6
	1 | sra7	|			| (int)r1 >>= 7
	1 | sra8	|			| (int)r1 >>= 8


	1 | sraa	|			| ac = (int)ac >> imm5
	1 | srla	|			| ac = ac >> imm5
	1 | slla	|			| ac = ac << imm5

	1 | anda	|			| ac = r1 & ac
	1 | ora		|			| ac = r1 | ac
	1 | xora	|			| ac = r1 ^ ac


	2 | mul		|			| r1 = r2 * r3
	2 | mulh	|			| r1(high) = r2 * r3
	2 | div		|			| r1 = r2 / r3
	2 | rem		|			| r1 = r2 % r3
	
	1 | andia	|			| ac = ac & imm5
	1 | oria	|			| ac = ac | imm5
	1 | xoria	|			| ac = ac | imm5

	1 | swpa    |			| swap ac r1
	1 | rev		|			| reverse r1	
	1 | inca	|			| acc += imm5
	2 | deca    |			| acc -= imm5
//haahahahahahh sext
	2 | sext	|			| 
//maybe not
	2 | cigetc  |			| clear itterator and get 16 bit counter 

//for now, cause i don't
	1 | rota	|			| rotate 
	1 | rotoa	|			| rotate other way 

	2 | ld2		|			| r1 = mem_size[r2, r3]
	2 | st2		|			| mem[r2, r3] = r1

	2 | push3	|			| push 3 registers of 3 sizes. 
	2 | pop3	|			| push 3 registers of 3 sizes. 


minors:
	1 | ald+	|			|	 
	2 | ast+    |			|

	1 | bieqc+	|[x|y][+/-] |
	1 | bineqc+	|[x|y][+/-]	|







