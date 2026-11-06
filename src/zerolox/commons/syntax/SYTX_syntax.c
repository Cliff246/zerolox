#include "MY_commons.h"
#include "isa/ISA_zerolox.h"
#include "pseudo/PSUDO_operation.h"
#include "SYTX_syntax.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//TODO
void ZLX_SYTX_free_Argument(ZLX_SYTX_Argument_t *argument)
{
	MY_TODO;
	free(argument);
}

ZLX_SYTX_Statement_t *ZLX_SYTX_init_Statement(void)
{
	ZLX_SYTX_Statement_t *statement = malloc(sizeof(ZLX_SYTX_Statement_t));
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


//this is not really done yet but still

ZLX_PSUDO_Operation_t ZLX_SYTX_decode_syntax(ZLX_SYTX_Statement_t statement)
{
	if(statement.a__mnemonic[ZLX_SYTX_MNEMONIC_SIZE - 1] != 0)
	{
		MY_FAULT("statements are true strings")
	}
	

	char **splits = NULL;
	int splits_count = MY_split_string(statement.a__mnemonic, &splits, '.');	

	
	#define MAX_OVERLAPS 4	
	
	int32_t overlap[splits_count][MAX_OVERLAPS];
	memset(overlap, 0, splits_count * MAX_OVERLAPS * sizeof(int32_t));
	uint32_t found_mnemonic = 0;	

	for(int p = 0; p < splits_count; ++p)
	{
		for(int32_t i = 0; i < MY_ARRAY_SIZE(ZLX_SYTX_mnemonics); ++i)	
		{
		
		
			int compare = strcmp(splits[p], ZLX_SYTX_mnemonics[i].a__mnemonic);
			if(compare == 0)
			{
				if(found_mnemonic == MAX_OVERLAPS)
				{
					//fix this, add more overlap capaacity 
					MY_TODO;

				}
				overlap[p][found_mnemonic++] = i;
			}
		}
	}
	if(found_mnemonic == 0)
	{
		MY_FAULT("invalid operation");
	}
	else if(found_mnemonic == 1)
	{

	}
	
	ZLX_ISA_Instruction_t inst;
	ZLX_PSUDO_Operation_t operation;
	MY_TODO;

	#undef MAX_OVERLAPS
	return operation;
}


ZLX_PSUDO_Operation_t ZLX_SYTX_55_minor(ZLX_SYTX_Statement_t statement, int32_t index)
{

	ZLX_SYTX_Mnemonic_t mnemonic = ZLX_SYTX_mnemonics[index];
	
	ZLX_ISA_Instruction_t instruction = {0};

	if(statement.arguments_count != 2)
	{
		MY_FAULT("2 form minors must have 2 arguments");
	}
	if(statement.pap__arguments[0]->type != ZLX_SYTX_Argument_type_register)
	{
		MY_FAULT("argument 1 is not a source register");
	}
	if(statement.pap__arguments[1]->type != ZLX_SYTX_Argument_type_register)
	{

		MY_FAULT("argument 2 is not a source register");
	}

	char *source1 = statement.pap__arguments[0]->up__string;
	char *source2 = statement.pap__arguments[1]->up__string;

	ZLX_ISA_Register_identifier_t r1 = ZLX_ISA_get_register_number(source1);
	ZLX_ISA_Register_identifier_t r2 = ZLX_ISA_get_register_number(source2);


	instruction.size = 1;
	instruction.type = ZLX_ISA_Minor_type; 
	instruction.u__minor.type = ZLX_ISA_Minor_format_type_55;
	instruction.u__minor.operation = ZLX_SYTX_mnemonics[index].opcode;
	instruction.u__minor.u__format_55.a5 = r1;
	instruction.u__minor.u__format_55.b5 = r2;

	ZLX_PSUDO_Operation_t operation = {0};
	operation.instruction = instruction;
	return operation;




}
