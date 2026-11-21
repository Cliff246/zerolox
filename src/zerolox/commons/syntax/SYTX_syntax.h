#ifndef ZLX_SYTX_SYNTAX_HEADER
#define ZLX_SYTX_SYNTAX_HEADER

#include "isa/ISA_zerolox.h"
#include <stddef.h>
#include <stdint.h>



typedef enum ZLX_SYTX_instruction_prefix
{
	ZLX_SYTX_instruction_prefix_invalid,
	ZLX_SYTX_instruction_prefix_none,
	ZLX_SYTX_instruction_prefix_minor,

	ZLX_SYTX_instruction_prefix_natural,
}ZLX_SYTX_instruction_prefix_t;


typedef enum ZLX_SYTX_instruction_postfix
{
	ZLX_SYTX_instruction_postfix_invalid, 
	ZLX_SYTX_instruction_postfix_none,
	ZLX_SYTX_instruction_postfix_8,
	ZLX_SYTX_instruction_postfix_16,
	ZLX_SYTX_instruction_postfix_32,
	ZLX_SYTX_instruction_postfix_24,
	ZLX_SYTX_instruction_postfix_p
}ZLX_SYTX_instruction_postfix_t;

extern char *ZLX_SYTX_instruction_postfix_keywords[];

extern const size_t ZLX_SYTX_instruction_postfix_count;
#define ZLX_SYTX_MNEMONIC_SIZE 16

typedef enum ZLX_SYTX_pattern_form
{
	ZLX_SYTX_pattern_form_2_register,
	ZLX_SYTX_pattern_form_register,
	ZLX_SYTX_pattern_form_accumulator_immediate,
	ZLX_SYTX_pattern_form_register_register_immediate,
	ZLX_SYTX_pattern_form_register_ultra,
	ZLX_SYTX_pattern_form_iterator_branch,
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

int ZLX_SYTX_get_mnemonic_code(char *string);
int ZLX_SYTX_find_mnemonic(char *string, ZLX_SYTX_instruction_prefix_t prefix);
ZLX_SYTX_instruction_postfix_t ZLX_SYTX_get_instruction_postfix(char *string);
ZLX_SYTX_instruction_prefix_t ZLX_SYTX_get_instruction_prefix(char *string);

#endif
