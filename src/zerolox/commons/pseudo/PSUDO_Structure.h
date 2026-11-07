#ifndef ZLX_PSUDO_STRUCTURE_HEADER
#define ZLX_PSUDO_STRUCTURE_HEADER

#include "PSUDO_Operation.h"

typedef struct ZLX_PSUDO_Structure
{
	ZLX_PSUDO_Operation_t pa__operations;
	uint64_t operations_size;
}ZLX_PSUDO_Structure_t;

#endif
