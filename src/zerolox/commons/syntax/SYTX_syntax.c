#include "MY_commons.h"
#include "isa/ISA_zerolox.h"
#include "SYTX_syntax.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


ZLX_SYTX_Mnemonic_t ZLX_SYTX_mnemonics[] = 
{
	{.type = ZLX_ISA_instruction_type_minor, .form = ZLX_SYTX_pattern_form_2_register, .a__mnemonic = "mv", .opcode = ZLX_ISA_minor_operation_move},
	{.type = ZLX_ISA_instruction_type_minor, .a__mnemonic = "add", .opcode = ZLX_ISA_minor_operation_add },
	{.type = ZLX_ISA_instruction_type_minor, .a__mnemonic = "adda",.opcode = ZLX_ISA_minor_operation_adda},
	{.type = ZLX_ISA_instruction_type_minor, .a__mnemonic = "sub", .opcode = ZLX_ISA_minor_operation_sub},
	{.type = ZLX_ISA_instruction_type_minor, .a__mnemonic = "geta", .opcode = ZLX_ISA_minor_operation_geta}, 
	{.type = ZLX_ISA_instruction_type_minor, .a__mnemonic = "get8", .opcode = ZLX_ISA_minor_operation_get8},
	{.type = ZLX_ISA_instruction_type_minor, .a__mnemonic = "b", .opcode = ZLX_ISA_minor_operation_b}, 
	{.type = ZLX_ISA_instruction_type_minor, .a__mnemonic = "baeqz", .opcode = ZLX_ISA_minor_operation_baeqz },
	{.type = ZLX_ISA_instruction_type_minor, .a__mnemonic = "baneqz", .opcode = ZLX_ISA_minor_operation_baneqz},
	{.type = ZLX_ISA_instruction_type_minor, .a__mnemonic = "lda", .opcode = ZLX_ISA_minor_operation_lda},  
	{.type = ZLX_ISA_instruction_type_minor, .a__mnemonic = "ald", .opcode = ZLX_ISA_minor_operation_ald},
	{.type = ZLX_ISA_instruction_type_minor, .a__mnemonic = "sta", .opcode = ZLX_ISA_minor_operation_sta},
	{.type = ZLX_ISA_instruction_type_minor, .a__mnemonic = "ast", .opcode =ZLX_ISA_minor_operation_ast},	
	{.type = ZLX_ISA_instruction_type_minor, .a__mnemonic = "stsa", .opcode = ZLX_ISA_minor_operation_stsa},
	{.type = ZLX_ISA_instruction_type_minor, .a__mnemonic = "ldsa",.opcode = ZLX_ISA_minor_operation_ldsa},
	{.type = ZLX_ISA_instruction_type_minor, .a__mnemonic = "ld+a", .opcode = ZLX_ISA_minor_operation_ld_plus}, 
	{.type = ZLX_ISA_instruction_type_minor, .a__mnemonic = "st+a", .opcode = ZLX_ISA_minor_operation_st_plus},  
	{.type = ZLX_ISA_instruction_type_minor, .a__mnemonic = "bieqc", .opcode = ZLX_ISA_minor_operation_bieqc},
	{.type = ZLX_ISA_instruction_type_minor, .a__mnemonic = "bteqa", .opcode = ZLX_ISA_minor_operation_bteqa},
	{.type = ZLX_ISA_instruction_type_minor, .a__mnemonic = "btneqa", .opcode = ZLX_ISA_minor_operation_btneqa},

};

const size_t ZLX_SYTX_mnemonics_count = MY_ARRAY_SIZE(ZLX_SYTX_mnemonics);
