#include "EMU_Context.h"
#include <stdio.h>
#include <MY_commons.h>

ZLX_EMU_Context_t *context;

int main(int argc, char *argv[])
{
	printf("hello world\n");

	context = ZLX_EMU_init_Context();

	ZLX_EMU_load_file_Context(context, "image.bin");


}

