#ifndef ZLX_ASM_STATEMENT_HEADER
#define ZLX_ASM_STATEMENT_HEADER

#include "ASM_Argument.h"
#include "ASM_Assembler.h"
#include "syntax/SYTX_syntax.h"
#include <stdint.h>



//too get a statement it really should consume a token list. or at least something above -> statement or such, idk.
typedef struct ZLX_ASM_Statement
{
	char a__mnemonic[ZLX_SYTX_MNEMONIC_SIZE];
	uint64_t arguments_count;
	//hah hah this is actually the single dumbest idea ever, every single fucking instruction and argument with a malloc, brilliant
	ZLX_ASM_Argument_t **pap__arguments;
}ZLX_ASM_Statement_t;

ZLX_ASM_Statement_t *ZLX_ASM_init_Statement(void);
void ZLX_ASM_free_Statement(ZLX_ASM_Statement_t *statement);

ZLX_ASM_Statement_t *ZLX_ASM_convert_token_to_statement_Assembler(ZLX_ASM_Assembler_t *assembler, int32_t start, int32_t end);


#endif
