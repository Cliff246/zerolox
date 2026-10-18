#include "EMU_Machine.h"
#include "EMU_Memory.h"
#include <stdlib.h>
#include <MY_commons.h>





ZLX_EMU_Machine_t *ZLX_EMU_init_Machine()
{
	ZLX_EMU_Machine_t *machine = malloc(sizeof(ZLX_EMU_Machine_t));
	if(!machine)
		MY_FAULT("malloc failed");

	ZLX_EMU_Memory_t *memory = ZLX_EMU_init_Memory(ZLX_MACHINE_DEFAULT_MEMORY_ALLOC);
	if(!memory)
		MY_FAULT("memory could not be allocated");

	machine->p__memory = memory;

	MY_TODO;
		
	return machine;
}
