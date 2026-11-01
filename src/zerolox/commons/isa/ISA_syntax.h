#ifndef ZLX_ISA_SYNTAX_HEADER
#define ZLX_ISA_SYNTAX_HEADER

#include "ISA_zerolox.h"
#include <stdbool.h>
#include <stdint.h>



#define ZLX_ISA_MNEMONIC_SIZE 16

typedef struct ZLX_ISA_Statement
{
	char a__mnemonic[ZLX_ISA_MNEMONIC_SIZE];
	uint64_t arguments_count;
	char **pap__arguments;
}ZLX_ISA_Statement_t;

ZLX_ISA_Statement_t *ZLX_ISA_init_Statement(void);

void ZLX_ISA_free_Statement(ZLX_ISA_Statement_t *statement);


typedef enum ZLX_ISA_Pattern_form
{
	ZLX_ISA_Pattern_form_2_register,
}ZLX_ISA_Pattern_form_t;


typedef struct ZLX_ISA_Mnemonic
{
	ZLX_ISA_Instruction_type_t type;
	ZLX_ISA_Pattern_form_t form;
	char a__mnemonic[ZLX_ISA_MNEMONIC_SIZE];
}ZLX_ISA_Mnemonic_t;



extern ZLX_ISA_Mnemonic_t ZLX_ISA_mnemonics[];

ZLX_ISA_Instruction_t ZLX_ISA_decode_syntax(ZLX_ISA_Statement_t statement);

#endif
