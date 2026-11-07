#include <stdint.h>
#include "MY_commons.h"
#include "ASM_Assembler.h"

//we have been fucking this so badly, every single int32 or int64 is unportable to zerolox in 32 bit


const char *my_bitchass_const_path = "asm.asm";

int32_t main(int32_t argc, char *argv[])
{

	
	
	ZLX_ASM_Assembler_t *assembler = ZLX_ASM_init_Assembler();

	if(!ZLX_ASM_load_file_Assembler(assembler, my_bitchass_const_path))
	{
		MY_FAULT("failed to load file");
	}
	
	ZLX_ASM_lex_Assembler(assembler);

	return 0;
}
