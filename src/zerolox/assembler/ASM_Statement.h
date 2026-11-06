#ifndef ZLX_ASM_STATEMENT_HEADER
#define ZLX_ASM_STATEMENT_HEADER

#include "ASM_Argument.h"
#include "syntax/SYTX_syntax.h"
#include <stdint.h>

typedef struct ZLX_ASM_Statement
{
	char a__mnemonic[ZLX_SYTX_MNEMONIC_SIZE];
	uint64_t arguments_count;
	ZLX_ASM_Argument_t **pap__arguments;
}ZLX_ASM_Statement_t;

ZLX_ASM_Statement_t *ZLX_ASM_init_Statement(void);
void ZLX_ASM_free_Statement(ZLX_ASM_Statement_t *statement);

#endif
