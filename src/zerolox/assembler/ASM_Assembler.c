
#include "ASM_assembler.h"
#include "MY_commons.h"
#include "MY_lexer.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

ZLX_ASM_Assembler_t *ZLX_ASM_init_Assembler(void)
{
	

	ZLX_ASM_Assembler_t *assembler = calloc(1,sizeof(ZLX_ASM_Assembler_t));
	if(!assembler)
		MY_FAULT("failed malloc");


	return assembler;
}


//this is split so if it fails one day i will yell back
bool ZLX_ASM_load_file_Assembler(ZLX_ASM_Assembler_t *assembler, const char *path)
{
	if(!path)
		return false;
	if(!assembler)
		return false;


	char *path_dup = strdup(path);	
	if(!path_dup)
		MY_FAULT("failed to duplicate string");
	uint64_t size = 0;	

	char *result = MY_read_file(path_dup, &size);
	if(result == NULL)
		MY_FAULT("failed to load");

	
	assembler->file_contents_size = size;
	assembler->p__file_contents = result;
	assembler->p__path = path_dup;

	return assembler;
}


//lex this assembler
void ZLX_ASM_lex_Assembler(ZLX_ASM_Assembler_t *assembler)
{
	if(assembler->p__file_contents == NULL)
		//trash spelling
		MY_FAULT("no contents avilable");

	//this isn't of the name, should be MY_init_Lexer, but i failed, oh well
	MY_lexer_t *lexer = MY_initialize_lexer(assembler->p__file_contents);

	MY_lex(lexer);

	
	return;
}
