#ifndef ZLX_ASM_FORM_HEADER
#define ZLX_ASM_FORM_HEADER


//something like 
//
//
//%reg(all); accepts all registers 
//%int(imm3,); or something accepts the immediates of a size
//%exp;
//%float;
//% ;
//m.ld+a [%reg(all);,%int({0,3});] 
//
//ends on semi colon, then splits that into unique argument forms, those arguments then goto something, idk. 
//it could try to fill the instruction, but the irregularity is just begging for mistakes. 
//plus future error checking and assembly yelling matters 
//
//ultimately the idea of the assembler is roughly syntax in the form of 
//
//mnemonic operations
//declerations
//labels
//semantic operations 
//data operationns
//
//ie 
//
//ld.8 t0, [t4, 10] | is a mnemonic operation
//m.lda.8 [t4, 10]  | is also a mnenomic operation
//ld.8 ac, [t4, 10] | is the same as m.lda.8 [t4, 10] because they are the same operation, it picks the smallest possible form for you based on what you select.
//%include "file"   | declerations are defined by 
//number:           | is a lablel 
//@jumpto &number   | is a semantic operations(this can become many instructions) 
//!db "hello world" | is a data operation that fills data
//
// symbols are defined by 
// & exact reference too, used like a c &
// $ value of so if there is a constant, $value is the term
// # relative address to, pc + distance. rougly speaking.
//
//with symbols auto filling based on best 'intent'
//
//the assembler should be the lowest form that the 'user programable' files can be commented in the toolchain
//the assembler does not output a binary image, not exactly. it publishes a psudoeoperation file. which is similar to object code
//the psuedo operations file is then linked with others into an image.
//ultimately the zlx-asm front end and back end can be detached but also in the same program(to avoid useless file pain and complexity)
//
//it should be scalable, maybe not fast. but quite strict, not a macro assembler. you write what you get, the assembler yells first, you thank it for catching this 
//

typedef struct ZLX_ASM_Form
{

	char a__form[64];
			
}ZLX_ASM_Form_t;

#endif
