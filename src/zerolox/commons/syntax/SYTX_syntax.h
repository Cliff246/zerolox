#ifndef ZLX_SYTX_SYNTAX_HEADER
#define ZLX_SYTX_SYNTAX_HEADER

#include "isa/ISA_zerolox.h"
#include "pseudo/PSUDO_operation.h"
#include <stdbool.h>
#include <stdint.h>



#define ZLX_SYTX_MNEMONIC_SIZE 16

//this will probably move

typedef enum ZLX_SYTX_Argument_type
{
	ZLX_SYTX_Argument_type_number,
	ZLX_SYTX_Argument_type_string,
	ZLX_SYTX_Argument_type_reference,
	ZLX_SYTX_Argument_type_register,
}ZLX_SYTX_Argument_type_t;


typedef struct ZLX_SYTX_Argument
{
	ZLX_SYTX_Argument_type_t type;
	
	union
	{
		uint64_t u__number;
		char *up__string;
		
	};
}ZLX_SYTX_Argument_t;


void ZLX_SYTX_free_Argument(ZLX_SYTX_Argument_t *argument);

typedef struct ZLX_SYTX_Statement
{
	char a__mnemonic[ZLX_SYTX_MNEMONIC_SIZE];
	uint64_t arguments_count;
	ZLX_SYTX_Argument_t **pap__arguments;
}ZLX_SYTX_Statement_t;

ZLX_SYTX_Statement_t *ZLX_SYTX_init_Statement(void);

void ZLX_SYTX_free_Statement(ZLX_SYTX_Statement_t *statement);


typedef enum ZLX_SYTX_Pattern_form
{
	ZLX_SYTX_Pattern_form_2_register,
}ZLX_SYTX_Pattern_form_t;


typedef struct ZLX_SYTX_Mnemonic
{
	ZLX_ISA_Instruction_type_t type;
	ZLX_SYTX_Pattern_form_t form;
	uint16_t opcode;
	char a__mnemonic[ZLX_SYTX_MNEMONIC_SIZE];
}ZLX_SYTX_Mnemonic_t;



extern ZLX_SYTX_Mnemonic_t ZLX_SYTX_mnemonics[];

ZLX_PSUDO_Operation_t ZLX_SYTX_decode_syntax(ZLX_SYTX_Statement_t statement);

//this is not really done yet but still
ZLX_PSUDO_Operation_t ZLX_SYTX_2_registers_minor(ZLX_SYTX_Statement_t statement, int32_t index)

#endif
