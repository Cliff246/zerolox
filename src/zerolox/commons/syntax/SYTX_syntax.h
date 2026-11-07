#ifndef ZLX_SYTX_SYNTAX_HEADER
#define ZLX_SYTX_SYNTAX_HEADER

#include "isa/ISA_zerolox.h"
#include <stddef.h>
#include <stdint.h>



#define ZLX_SYTX_MNEMONIC_SIZE 16

typedef enum ZLX_SYTX_pattern_form
{
	ZLX_SYTX_pattern_form_2_register,
}ZLX_SYTX_pattern_form_t;


typedef struct ZLX_SYTX_Mnemonic
{
	ZLX_ISA_instruction_type_t type;
	ZLX_SYTX_pattern_form_t form;
	uint16_t opcode;
	char a__mnemonic[ZLX_SYTX_MNEMONIC_SIZE];
}ZLX_SYTX_Mnemonic_t;



extern ZLX_SYTX_Mnemonic_t ZLX_SYTX_mnemonics[];

extern const size_t ZLX_SYTX_mnemonics_count;

#endif
