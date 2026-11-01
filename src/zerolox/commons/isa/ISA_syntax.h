#ifndef ZLX_ISA_SYNTAX_HEADER
#define ZLX_ISA_SYNTAX_HEADER

#include "ISA_zerolox.h"
#include <stdbool.h>
#include <stdint.h>



#define ZLX_ISA_MNEMONIC_SIZE 16

//this will probably move

typedef enum ZLX_ISA_Argument_type
{
	ZLX_ISA_Argument_type_number,
	ZLX_ISA_Argument_type_string,
	ZLX_ISA_Argument_type_reference,
	ZLX_ISA_Argument_type_register,
}ZLX_ISA_Argument_type_t;


typedef struct ZLX_ISA_Argument
{
	ZLX_ISA_Argument_type_t type;
	
	union
	{
		uint64_t u__number;
		char *up__string;
		
	};
}ZLX_ISA_Argument_t;


void ZLX_ISA_free_Argument(ZLX_ISA_Argument_t *argument);

typedef struct ZLX_ISA_Statement
{
	char a__mnemonic[ZLX_ISA_MNEMONIC_SIZE];
	uint64_t arguments_count;
	ZLX_ISA_Argument_t **pap__arguments;
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
	uint16_t opcode;
	char a__mnemonic[ZLX_ISA_MNEMONIC_SIZE];
}ZLX_ISA_Mnemonic_t;



extern ZLX_ISA_Mnemonic_t ZLX_ISA_mnemonics[];

ZLX_ISA_Instruction_t ZLX_ISA_decode_syntax(ZLX_ISA_Statement_t statement);

//this is not really done yet but still
ZLX_ISA_Instruction_t ZLX_ISA_55_minor(ZLX_ISA_Statement_t statement, int32_t index);

#endif
