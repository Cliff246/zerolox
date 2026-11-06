#include "MY_commons.h"
#include "isa/ISA_zerolox.h"
#include "SYTX_syntax.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//TODO
void ZLX_SYTX_free_Argument(ZLX_SYTX_Argument_t *argument)
{
	if(!argument)
		return;
	// Text payload ownership is not defined yet; do not free up__string here.
	free(argument);
}

ZLX_SYTX_Statement_t *ZLX_SYTX_init_Statement(void)
{
	ZLX_SYTX_Statement_t *statement = calloc(1, sizeof(*statement));
	if(!statement)
		MY_FAULT("malloc failed");





	return statement;
}


void ZLX_SYTX_free_Statement(ZLX_SYTX_Statement_t *statement)
{
	if(!statement)
	{
		return;
	}
	if(statement->arguments_count > 0)
	{
		for(int i = 0; i < statement->arguments_count; ++i)
		{
			if(statement->pap__arguments)
				ZLX_SYTX_free_Argument(statement->pap__arguments[i]);	
		}
	}
	if(statement->pap__arguments != NULL)
	{
		free(statement->pap__arguments);
	}	
	free(statement);
	return;
}





ZLX_SYTX_Mnemonic_t ZLX_SYTX_mnemonics[] = 
{
	{.type = ZLX_ISA_Minor_type, .form = ZLX_SYTX_Pattern_form_2_register, .a__mnemonic = "mv", .opcode = ZLX_ISA_Minor_operation_move},
	{.type = ZLX_ISA_Minor_type, .a__mnemonic = "add", .opcode = ZLX_ISA_Minor_operation_add },
	{.type = ZLX_ISA_Minor_type, .a__mnemonic = "adda",.opcode = ZLX_ISA_Minor_operation_adda},
	{.type = ZLX_ISA_Minor_type, .a__mnemonic = "sub", .opcode = ZLX_ISA_Minor_operation_sub},
	{.type = ZLX_ISA_Minor_type, .a__mnemonic = "geta", .opcode = ZLX_ISA_Minor_operation_geta}, 
	{.type = ZLX_ISA_Minor_type, .a__mnemonic = "get8", .opcode = ZLX_ISA_Minor_operation_get8},
	{.type = ZLX_ISA_Minor_type, .a__mnemonic = "b", .opcode = ZLX_ISA_Minor_operation_b}, 
	{.type = ZLX_ISA_Minor_type, .a__mnemonic = "baeqz", .opcode = ZLX_ISA_Minor_operation_baeqz },
	{.type = ZLX_ISA_Minor_type, .a__mnemonic = "baneqz", .opcode = ZLX_ISA_Minor_operation_baneqz},
	{.type = ZLX_ISA_Minor_type, .a__mnemonic = "lda", .opcode = ZLX_ISA_Minor_operation_lda},  
	{.type = ZLX_ISA_Minor_type, .a__mnemonic = "ald", .opcode = ZLX_ISA_Minor_operation_ald},
	{.type = ZLX_ISA_Minor_type, .a__mnemonic = "sta", .opcode = ZLX_ISA_Minor_operation_sta},
	{.type = ZLX_ISA_Minor_type, .a__mnemonic = "ast", .opcode =ZLX_ISA_Minor_operation_ast},	
	{.type = ZLX_ISA_Minor_type, .a__mnemonic = "stsa", .opcode = ZLX_ISA_Minor_operation_stsa},
	{.type = ZLX_ISA_Minor_type, .a__mnemonic = "ldsa",.opcode = ZLX_ISA_Minor_operation_ldsa},
	{.type = ZLX_ISA_Minor_type, .a__mnemonic = "ld+a", .opcode = ZLX_ISA_Minor_operation_ld_plus}, 
	{.type = ZLX_ISA_Minor_type, .a__mnemonic = "st+a", .opcode = ZLX_ISA_Minor_operation_st_plus},  
	{.type = ZLX_ISA_Minor_type, .a__mnemonic = "bieqc", .opcode = ZLX_ISA_Minor_operation_bieqc},
	{.type = ZLX_ISA_Minor_type, .a__mnemonic = "bteqa", .opcode = ZLX_ISA_Minor_operation_bteqa},
	{.type = ZLX_ISA_Minor_type, .a__mnemonic = "btneqa", .opcode = ZLX_ISA_Minor_operation_btneqa},

};

const size_t ZLX_SYTX_mnemonics_count = MY_ARRAY_SIZE(ZLX_SYTX_mnemonics);
