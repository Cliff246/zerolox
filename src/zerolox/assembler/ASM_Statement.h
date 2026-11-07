#ifndef ZLX_ASM_STATEMENT_HEADER
#define ZLX_ASM_STATEMENT_HEADER

#include "ASM_Operation.h"
#include "ASM_Argument.h"
#include "ASM_Assembler.h"
#include "syntax/SYTX_syntax.h"
#include <stdint.h>


typedef enum ZLX_ASM_Statement_type
{
	ZLX_ASM_Statement_type_invalid,	
	ZLX_ASM_Statement_type_operation,	
	ZLX_ASM_Statement_type_label,	
	ZLX_ASM_Statement_type_directive,	
}ZLX_ASM_Statement_type_t;

//will probably go to it's own file eventually
typedef struct ZLX_ASM_Directive
{

}ZLX_ASM_Directive_t;

//will probably go to it's own file eventually
typedef struct ZLX_ASM_Label
{

}ZLX_ASM_Label_t;

//too get a statement it really should consume a token list. or at least something above -> statement or such, idk.
typedef struct ZLX_ASM_Statement
{
	ZLX_ASM_Statement_type_t type;	
	union 
	{
		ZLX_ASM_Operation_t u__operation;
		ZLX_ASM_Directive_t u__directive;
		ZLX_ASM_Label_t u__label;
	};	
}ZLX_ASM_Statement_t;

ZLX_ASM_Statement_t *ZLX_ASM_init_Statement(void);
void ZLX_ASM_free_Statement(ZLX_ASM_Statement_t *statement);

ZLX_ASM_Statement_t *ZLX_ASM_convert_token_to_statement_Assembler(ZLX_ASM_Assembler_t *assembler, int32_t start, int32_t count);


#endif
