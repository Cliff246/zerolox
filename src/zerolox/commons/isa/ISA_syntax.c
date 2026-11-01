#include "MY_commons.h"
#include "ISA_zerolox.h"
#include "ISA_syntax.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/_types/_sigaltstack.h>

ZLX_ISA_Statement_t *ZLX_ISA_init_Statement(void)
{
	ZLX_ISA_Statement_t *statement = malloc(sizeof(ZLX_ISA_Statement_t));
	if(!statement)
		MY_FAULT("malloc failed");





	return statement;
}


void ZLX_ISA_free_Statement(ZLX_ISA_Statement_t *statement)
{
	if(!statement)
	{
		return;
	}
	if(statement->arguments_count > 0)
	{
		for(int i = 0; i < statement->arguments_count; ++i)
		{
	
			if(statement->pap__arguments == NULL)
			{
				MY_FAULT("broken argument");	
			}
			free(statement->pap__arguments[i]);
		}
	}
	if(statement->pap__arguments != NULL)
	{
		free(statement->pap__arguments);
	}	
	free(statement);
	return;
}





ZLX_ISA_Mnemonic_t ZLX_ISA_mnemonics[] = 
{
	[ZLX_ISA_Minor_operation_move] = {.type = ZLX_ISA_Minor_type, .form = ZLX_ISA_Pattern_form_2_register, .a__mnemonic = "mv"},
	[ZLX_ISA_Minor_operation_add] = {.type = ZLX_ISA_Minor_type, .a__mnemonic = "add"},
	[ZLX_ISA_Minor_operation_adda] = {.type = ZLX_ISA_Minor_type, .a__mnemonic = "adda"},
	[ZLX_ISA_Minor_operation_sub] = {.type = ZLX_ISA_Minor_type, .a__mnemonic = "sub"},
	[ZLX_ISA_Minor_operation_geta] = {.type = ZLX_ISA_Minor_type, .a__mnemonic = "geta"}, 
	[ZLX_ISA_Minor_operation_get8] = {.type = ZLX_ISA_Minor_type, .a__mnemonic = "get8"},
	[ZLX_ISA_Minor_operation_b] = {.type = ZLX_ISA_Minor_type, .a__mnemonic = "b"}, 
	[ZLX_ISA_Minor_operation_baeqz] = {.type = ZLX_ISA_Minor_type, .a__mnemonic = "baeqz"},
	[ZLX_ISA_Minor_operation_baneqz] = {.type = ZLX_ISA_Minor_type, .a__mnemonic = "baneqz"},
	[ZLX_ISA_Minor_operation_lda] = {.type = ZLX_ISA_Minor_type, .a__mnemonic = "lda"},  
	[ZLX_ISA_Minor_operation_ald] = {.type = ZLX_ISA_Minor_type, .a__mnemonic = "ald"},
	[ZLX_ISA_Minor_operation_sta] = {.type = ZLX_ISA_Minor_type, .a__mnemonic = "sta"},
	[ZLX_ISA_Minor_operation_ast] = {.type = ZLX_ISA_Minor_type, .a__mnemonic = "ast"},	
	[ZLX_ISA_Minor_operation_stsa] = {.type = ZLX_ISA_Minor_type, .a__mnemonic = "stsa"},
	[ZLX_ISA_Minor_operation_ldsa] = {.type = ZLX_ISA_Minor_type, .a__mnemonic = "ldsa"},
	[ZLX_ISA_Minor_operation_ld_plus] = {.type = ZLX_ISA_Minor_type, .a__mnemonic = "ldpa"}, 
	[ZLX_ISA_Minor_operation_st_plus] = {.type = ZLX_ISA_Minor_type, .a__mnemonic = "stpa"},  
	[ZLX_ISA_Minor_operation_bieqc] = {.type = ZLX_ISA_Minor_type, .a__mnemonic = "bieqc"},
	[ZLX_ISA_Minor_operation_bteqa] = {.type = ZLX_ISA_Minor_type, .a__mnemonic = "bteqa"},
	[ZLX_ISA_Minor_operation_btneqa] = {.type = ZLX_ISA_Minor_type, .a__mnemonic = "btneqa"},

};


ZLX_ISA_Instruction_t ZLX_ISA_decode_syntax(ZLX_ISA_Statement_t statement)
{
	if(statement.a__mnemonic[ZLX_ISA_MNEMONIC_SIZE - 1] != 0)
	{
		MY_FAULT("statements are true strings")
	}
	int32_t overlap[32] = {-1};
	

	uint32_t found_operations = 0;
	for(int32_t i = 0; i < MY_ARRAY_SIZE(ZLX_ISA_mnemonics); ++i)
	{
		
		int compare = strcmp(statement.a__mnemonic, ZLX_ISA_mnemonics[i].a__mnemonic);
		if(compare == 0)
		{
			overlap[found_operations++] = i;
		}
	}

	


	ZLX_ISA_Instruction_t inst;



	return inst;
}


