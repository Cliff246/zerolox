#ifndef ZLX_ASM_SYNTAX_HEADER
#define ZLX_ASM_SYNTAX_HEADER

#include "pseudo/PSUDO_Operation.h"
#include "ASM_Statement.h"
#include <stdint.h>

ZLX_PSUDO_Operation_t ZLX_ASM_decode_syntax(ZLX_ASM_Statement_t statement);
ZLX_PSUDO_Operation_t ZLX_ASM_2_registers_minor(ZLX_ASM_Statement_t statement, int32_t index);

#endif
