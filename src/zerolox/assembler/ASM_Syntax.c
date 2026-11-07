#include "ASM_Syntax.h"
#include "MY_commons.h"

#include <string.h>

//this is not really done yet but still

ZLX_PSUDO_Operation_t ZLX_ASM_decode_syntax(ZLX_ASM_Statement_t statement)
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
		for(int32_t i = 0; i < ZLX_SYTX_mnemonics_count; ++i)	
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
	
	ZLX_PSUDO_Operation_t operation;
	MY_TODO;

	#undef MAX_OVERLAPS
	return operation;
}


ZLX_PSUDO_Operation_t ZLX_ASM_2_registers_minor(ZLX_ASM_Statement_t statement, int32_t index)
{

	ZLX_ISA_Instruction_t instruction = {0};

	if(statement.arguments_count != 2)
	{
		MY_FAULT("2 form minors must have 2 arguments");
	}
	if(statement.pap__arguments[0]->type != ZLX_ASM_Argument_type_register)
	{
		MY_FAULT("argument 1 is not a source register");
	}
	if(statement.pap__arguments[1]->type != ZLX_ASM_Argument_type_register)
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
