#ifndef ZLX_ASM_ARGUMENT_HEADER
#define ZLX_ASM_ARGUMENT_HEADER

#include <stdint.h>

typedef enum ZLX_ASM_Argument_type
{
	ZLX_ASM_Argument_type_number,
	ZLX_ASM_Argument_type_string,
	ZLX_ASM_Argument_type_reference,
	ZLX_ASM_Argument_type_register,
}ZLX_ASM_Argument_type_t;

typedef struct ZLX_ASM_Argument
{
	ZLX_ASM_Argument_type_t type;

	union
	{
		uint64_t u__number;
		char *up__string;
	};
}ZLX_ASM_Argument_t;

void ZLX_ASM_free_Argument(ZLX_ASM_Argument_t *argument);

#endif
