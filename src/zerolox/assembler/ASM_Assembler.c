
#include "ASM_Assembler.h"
#include "ASM_Statement.h"
#include "MY_commons.h"
#include "MY_Lexer.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ZLX_ASM_Assembler_t *ZLX_ASM_init_Assembler(void)
{
	

	ZLX_ASM_Assembler_t *assembler = calloc(1,sizeof(ZLX_ASM_Assembler_t));
	if(!assembler)
		MY_FAULT("failed malloc");


	return assembler;
}


void ZLX_ASM_load_string_Assembler(ZLX_ASM_Assembler_t *_assembler, char *_argument)
{
	
	if(!_assembler)
		MY_FAULT("didn't even pass the assembler pepga");



	char *dup = strdup(_argument);
	if(!dup)
		MY_FAULT("failed to malloc and duplicate string");

	size_t length = strlen(dup);

	_assembler->pa__contents = dup;
	_assembler->contents_size = length;
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

	
	assembler->contents_size = size;
	assembler->pa__contents = result;
	assembler->pa__path = path_dup;

	return assembler;
}


//lex this assembler
void ZLX_ASM_lex_Assembler(ZLX_ASM_Assembler_t *assembler)
{
	if(assembler->pa__contents == NULL)
		//trash spelling
		MY_FAULT("no contents avilable");

	MY_ENTRY("lex assembler");
	//this isn't of the name, should be MY_init_Lexer, but i failed, oh well
	MY_Lexer_t *lexer = MY_init_Lexer(assembler->pa__contents);

	MY_lex_Lexer(lexer);
	MY_print_Lexer(lexer);
	assembler->p__lexer = lexer;
	
	return;
}



void ZLX_ASM_test_Assembler(ZLX_ASM_Assembler_t *_assembler)
{
	MY_Token_group_t group = MY_init_Token_group(_assembler->p__lexer, 0, _assembler->p__lexer->token_count); 

	ZLX_ASM_statement_type_t type = ZLX_ASM_classify_statement(group);
	printf("%d type", type);
}

