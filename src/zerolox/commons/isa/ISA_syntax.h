#ifndef ZLX_ISA_SYNTAX_HEADER
#define ZLX_ISA_SYNTAX_HEADER

#include "ISA_zerolox.h"


typedef struct ZLX_ISA_Statement
{
	char a__mnemonic[16];
	uint64_t arguments_count;
	char *pa__arguments;
}ZLX_ISA_Statement_t;



typedef enum ZLX_ISA_Pattern_form
{
	ZLX_ISA_Pattern_form_2_register,
}ZLX_ISA_Pattern_form_t;

typedef struct ZLX_ISA_Mnemonic
{
	ZLX_ISA_Pattern_form_t form;
	char a__mnemonic[16];
}ZLX_ISA_Mnemonic_t;



extern ZLX_ISA_Mnemonic_t ZLX_ISA_major_mnemonic[];
extern ZLX_ISA_Mnemonic_t ZLX_ISA_minor_mnemonic[];

ZLX_ISA_Instruction_t ZLX_ISA_decode_syntax(ZLX_ISA_Statement_t statement);

#endif
