#include "ASM_Statement.h"
#include "MY_commons.h"

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
