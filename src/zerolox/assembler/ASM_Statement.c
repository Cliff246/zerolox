#include "ASM_Statement.h"
#include "MY_commons.h"
#include "MY_lexer.h"

#include <assert.h>
#include <stdlib.h>

ZLX_ASM_Statement_t *ZLX_ASM_init_Statement(void)
{
	ZLX_ASM_Statement_t *statement = calloc(1, sizeof(*statement));
	if(!statement)
		MY_FAULT("malloc failed");

	return statement;
}

void ZLX_ASM_free_Statement(ZLX_ASM_Statement_t *statement)
{
	if(!statement)
		return;

	if(statement->arguments_count > 0)
	{
		for(uint64_t i = 0; i < statement->arguments_count; ++i)
		{
			if(statement->pap__arguments)
				ZLX_ASM_free_Argument(statement->pap__arguments[i]);
		}
	}

	free(statement->pap__arguments);
	free(statement);
}



ZLX_ASM_Statement_t *ZLX_ASM_convert_token_to_statement_Assembler(ZLX_ASM_Assembler_t *assembler, int32_t start, int32_t count)
{
	int opcode_size = 0;	
	char opcode[1024] = {0};
#define BUFFER_SIZE 128
	char buffer[BUFFER_SIZE] = {0};

	//the form for the mnemonic can roughly be 
	//word dot word plus word dot word

	//it cannot be word dot
	//dot word dot
	//etc 
	//
	//basically [word], [word dot word], [word dot word dot word] [word plus word] [word dot word plus word] 
	//shit
	for(int i = start; i < count; ++i)
	{	
		MY_token_type_t type = MY_get_token_type_Lexer(assembler->p__lexer, i);
		//ugh, fucker... this is really bad and unportable, but ultimately idc
		if(type == MY_TOKEN_SPACE)
		{
			int result = MY_get_token_string(assembler->p__lexer, i, buffer, BUFFER_SIZE); 
		}
	}

#undef BUFFER_SIZE
	
	ZLX_ASM_Statement_t *statement = ZLX_ASM_init_Statement();

	MY_TODO;


	return statement;

}
