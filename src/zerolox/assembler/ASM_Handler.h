#ifndef ZLX_ASM_HANDLER_HEADER
#define ZLX_ASM_HANDLER_HEADER

#include "ASM_Argument.h"
#include "ASM_Form.h"
#include "pseudo/PSUDO_operation.h"
#include <stdint.h>

typedef ZLX_PSUDO_Operation_t (*ZLX_ASM_Handler_fn_t)(ZLX_ASM_Argument_t argument, int32_t opcode);

typedef struct ZLX_ASM_Handler
{
	ZLX_ASM_Form_t *p__form;
	ZLX_ASM_Handler_fn_t p__handler_fn;

}ZLX_ASM_Handler_t;



#endif
