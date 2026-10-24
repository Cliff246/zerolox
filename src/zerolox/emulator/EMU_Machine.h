#ifndef __ZLX_EMU_MACHINE_HEADER__
#define __ZLX_EMU_MACHINE_HEADER__

#include "EMU__CPU.h"
#include "EMU_Memory.h"

#define ZLX_MACHINE_DEFAULT_MEMORY_ALLOC 1024 * 1024 


//this machine is hard coded for simplicity, 
typedef struct ZLX_EMU_Machine
{

	
	ZLX_EMU__CPU_t *p__cpu;
	ZLX_EMU_Memory_t *p__memory;

}ZLX_EMU_Machine_t;

ZLX_EMU_Machine_t *ZLX_EMU_init_Machine();


void ZLX_EMU_image_Machine(ZLX_EMU_Machine_t *machine, char *_image, uint64_t size);

#endif
