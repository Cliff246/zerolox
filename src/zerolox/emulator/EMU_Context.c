#include "MY_commons.h"
#include "EMU_Context.h"
#include "EMU_Machine.h"
#include <stdlib.h>



ZLX_EMU_Context_t *ZLX_EMU_init_Context()
{
	ZLX_EMU_Context_t *context = malloc(sizeof(ZLX_EMU_Context_t));
	if(!context)
		MY_FAULT("malloc failed");
	ZLX_EMU_Machine_t *machine = ZLX_EMU_init_Machine(); 
	if(!machine)
		MY_FAULT("machine ptr null");
	context->p__machine = machine;
	return context;
}




void ZLX_EMU_load_file_Context(ZLX_EMU_Context_t *context, char *path)
{



	uint64_t size = 0;
	char *data = MY_read_file(path, &size);

	ZLX_EMU_image_Machine(context->p__machine, data, size);

	free(data);	

}

