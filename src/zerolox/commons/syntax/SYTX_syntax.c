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
	{.type = ZLX_ISA_instruction_type_minor, .form = ZLX_SYTX_pattern_form_iterator_branch, .a__mnemonic = "bieqc+", .opcode = ZLX_ISA_minor_operation_bieqc_update},
	{.type = ZLX_ISA_instruction_type_minor, .form = ZLX_SYTX_pattern_form_iterator_branch, .a__mnemonic = "bineqc+", .opcode = ZLX_ISA_minor_operation_bineqc_update},
#define MAJOR_MNEMONIC(name, form_name) \
	{.type = ZLX_ISA_instruction_type_major, .form = ZLX_SYTX_pattern_form_##form_name, \
	 .a__mnemonic = #name, .opcode = ZLX_ISA_major_operation_##name}
	MAJOR_MNEMONIC(j, register),
	MAJOR_MNEMONIC(add, register_ultra),
	MAJOR_MNEMONIC(sub, register_ultra),
	MAJOR_MNEMONIC(sll, register_ultra),
	MAJOR_MNEMONIC(srl, register_ultra),
	MAJOR_MNEMONIC(sra, register_ultra),
	MAJOR_MNEMONIC(xor, register_ultra),
	MAJOR_MNEMONIC(or, register_ultra),
	MAJOR_MNEMONIC(and, register_ultra),
	MAJOR_MNEMONIC(clt, register_ultra),
	MAJOR_MNEMONIC(cge, register_ultra),
	MAJOR_MNEMONIC(cltu, register_ultra),
	MAJOR_MNEMONIC(cgeu, register_ultra),
	MAJOR_MNEMONIC(ceq, register_ultra),
	MAJOR_MNEMONIC(cneq, register_ultra),
	MAJOR_MNEMONIC(slli, register_register_immediate),
	MAJOR_MNEMONIC(srli, register_register_immediate),
	MAJOR_MNEMONIC(srai, register_register_immediate),
	MAJOR_MNEMONIC(xori, register_register_immediate),
	MAJOR_MNEMONIC(andi, register_register_immediate),
	MAJOR_MNEMONIC(ori, register_register_immediate),
	MAJOR_MNEMONIC(sext, register_register_immediate),
	MAJOR_MNEMONIC(inc2, register),
	MAJOR_MNEMONIC(inc3, register),
	MAJOR_MNEMONIC(inc4, register),
	MAJOR_MNEMONIC(inc5, register),
	MAJOR_MNEMONIC(inc6, register),
	MAJOR_MNEMONIC(inc7, register),
	MAJOR_MNEMONIC(inc8, register),
	MAJOR_MNEMONIC(dec2, register),
	MAJOR_MNEMONIC(dec3, register),
	MAJOR_MNEMONIC(dec4, register),
	MAJOR_MNEMONIC(dec5, register),
	MAJOR_MNEMONIC(dec6, register),
	MAJOR_MNEMONIC(dec7, register),
	MAJOR_MNEMONIC(dec8, register),
	MAJOR_MNEMONIC(sl1, register),
	MAJOR_MNEMONIC(sl2, register),
	MAJOR_MNEMONIC(sl3, register),
	MAJOR_MNEMONIC(sl4, register),
	MAJOR_MNEMONIC(sl5, register),
	MAJOR_MNEMONIC(sl6, register),
	MAJOR_MNEMONIC(sl7, register),
	MAJOR_MNEMONIC(sl8, register),
	MAJOR_MNEMONIC(sr1, register),
	MAJOR_MNEMONIC(sr2, register),
	MAJOR_MNEMONIC(sr3, register),
	MAJOR_MNEMONIC(sr4, register),
	MAJOR_MNEMONIC(sr5, register),
	MAJOR_MNEMONIC(sr6, register),
	MAJOR_MNEMONIC(sr7, register),
	MAJOR_MNEMONIC(sr8, register),
	MAJOR_MNEMONIC(sra1, register),
	MAJOR_MNEMONIC(sra2, register),
	MAJOR_MNEMONIC(sra3, register),
	MAJOR_MNEMONIC(sra4, register),
	MAJOR_MNEMONIC(sra5, register),
	MAJOR_MNEMONIC(sra6, register),
	MAJOR_MNEMONIC(sra7, register),
	MAJOR_MNEMONIC(sra8, register),
	MAJOR_MNEMONIC(anda, register),
	MAJOR_MNEMONIC(xora, register),
	MAJOR_MNEMONIC(ora, register),
	MAJOR_MNEMONIC(aand, register),
	MAJOR_MNEMONIC(axor, register),
	MAJOR_MNEMONIC(aor, register),
	MAJOR_MNEMONIC(inca, accumulator_immediate),
	MAJOR_MNEMONIC(deca, accumulator_immediate),
	MAJOR_MNEMONIC(srra, accumulator_immediate),
	MAJOR_MNEMONIC(srla, accumulator_immediate),
	MAJOR_MNEMONIC(slla, accumulator_immediate),
#undef MAJOR_MNEMONIC
	{.type = ZLX_ISA_instruction_type_major, .form = ZLX_SYTX_pattern_form_register, .a__mnemonic = "inc", .opcode = ZLX_ISA_major_operation_inc},
	{.type = ZLX_ISA_instruction_type_major, .form = ZLX_SYTX_pattern_form_register, .a__mnemonic = "inc1", .opcode = ZLX_ISA_major_operation_inc},
	{.type = ZLX_ISA_instruction_type_major, .form = ZLX_SYTX_pattern_form_register, .a__mnemonic = "dec", .opcode = ZLX_ISA_major_operation_dec},
	{.type = ZLX_ISA_instruction_type_major, .form = ZLX_SYTX_pattern_form_register, .a__mnemonic = "dec1", .opcode = ZLX_ISA_major_operation_dec},

};




char *ZLX_SYTX_instruction_postfix_keywords[] = 
{
	[ZLX_SYTX_instruction_postfix_invalid] = NULL, 
	[ZLX_SYTX_instruction_postfix_none] = NULL,
	[ZLX_SYTX_instruction_postfix_8] = "8",
	[ZLX_SYTX_instruction_postfix_16] = "16",
	[ZLX_SYTX_instruction_postfix_32] = "32",
	[ZLX_SYTX_instruction_postfix_24] = "24",
	[ZLX_SYTX_instruction_postfix_p] = "p"

};

const size_t ZLX_SYTX_instruction_postfix_count = MY_ARRAY_SIZE(ZLX_SYTX_instruction_postfix_keywords);
const size_t ZLX_SYTX_mnemonics_count = MY_ARRAY_SIZE(ZLX_SYTX_mnemonics);




int ZLX_SYTX_find_mnemonic(char *string, ZLX_SYTX_instruction_prefix_t prefix)
{
	for (size_t i = 0; i < ZLX_SYTX_mnemonics_count; ++i)
	{
		const ZLX_SYTX_Mnemonic_t *entry = &ZLX_SYTX_mnemonics[i];
		if (prefix == ZLX_SYTX_instruction_prefix_minor && entry->type != ZLX_ISA_instruction_type_minor)
			continue;
		if (prefix == ZLX_SYTX_instruction_prefix_natural && entry->type != ZLX_ISA_instruction_type_major)
			continue;
		if (strcmp(entry->a__mnemonic, string) == 0)
			return (int)i;
	}
	return -1;
}

int ZLX_SYTX_get_mnemonic_code(char *string)
{
	return ZLX_SYTX_find_mnemonic(string, ZLX_SYTX_instruction_prefix_none);
}

ZLX_SYTX_instruction_postfix_t ZLX_SYTX_get_instruction_postfix(char *string)
{
	if(string == NULL)
		return ZLX_SYTX_instruction_postfix_invalid;
	int id = ZLX_SYTX_instruction_postfix_invalid;
	for(int i = 2; i < ZLX_SYTX_instruction_postfix_count; ++i)
	{
		
		if(strcmp(ZLX_SYTX_instruction_postfix_keywords[i], string) == 0)
		{
			id = i;
			break;
		}
	}
	return id;

}
ZLX_SYTX_instruction_prefix_t ZLX_SYTX_get_instruction_prefix(char *string)
{
	if(strcmp("m", string) == 0)
		return ZLX_SYTX_instruction_prefix_minor;
	else if(strcmp("n", string) == 0)
		return ZLX_SYTX_instruction_prefix_natural;
	else if(string[0] == '\0')
		return ZLX_SYTX_instruction_prefix_invalid;
	else
		return ZLX_SYTX_instruction_prefix_none;
}

