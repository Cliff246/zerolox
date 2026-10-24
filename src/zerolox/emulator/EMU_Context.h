#ifndef ZLX_EMU_CONTEXT_HEADER
#define ZLX_EMU_CONTEXT_HEADER

#include "EMU_Machine.h"

typedef struct ZLX_EMU_Context
{
	ZLX_EMU_Machine_t *p__machine;		
}ZLX_EMU_Context_t;

ZLX_EMU_Context_t *ZLX_EMU_init_Context();


void ZLX_EMU_load_file_Context(ZLX_EMU_Context_t *context, char *path);

#endif
