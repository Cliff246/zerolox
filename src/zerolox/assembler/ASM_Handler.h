#ifndef ZLX_ASM_HANDLER_HEADER
#define ZLX_ASM_HANDLER_HEADER

//fucking rename this file
#include "pseudo/PSUDO_operation.h"
#include "ASM_Statement.h"
#include <stdint.h>

typedef ZLX_PSUDO_Operation_t (*ZLX_ASM_Handler_fn_t)(ZLX_ASM_Statement_t *statement, int32_t opcode);

typedef struct ZLX_ASM_Handler
{
	ZLX_ASM_Handler_fn_t p__handler_fn;
	//idk what else belongs in here but probably shit i will wish

}ZLX_ASM_Handler_t;



#endif
