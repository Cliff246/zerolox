#include "ASM_Argument.h"

#include <stdlib.h>

void ZLX_ASM_free_Argument(ZLX_ASM_Argument_t *argument)
{
	if(!argument)
		return;

	// Text payload ownership is not defined yet; do not free up__string here.
	free(argument);
}
