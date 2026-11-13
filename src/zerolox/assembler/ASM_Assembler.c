
#include "ASM_Assembler.h"
#include "ASM_Statement.h"
#include "MY_commons.h"
#include "MY_Lexer.h"
#include <assert.h>
#include <stdbool.h>
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
	_assembler->has_content = true;
}


//this is split so if it fails one day i will yell back
bool ZLX_ASM_load_file_Assembler(ZLX_ASM_Assembler_t *assembler, const char *path)
{
	
	if(!path)
		return false;
	if(!assembler)
		return false;
	if(assembler->has_content == true)
		MY_FAULT("already has content");

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
	assembler->has_content = true;

	return assembler;
}


//lex this assembler
void ZLX_ASM_lex_Assembler(ZLX_ASM_Assembler_t *assembler)
{
	if(assembler->pa__contents == NULL || assembler->has_content == false)
		//trash spelling
		MY_FAULT("no contents avilable ");

	MY_ENTRY("lex assembler");
	//this isn't of the name, should be MY_init_Lexer, but i failed, oh well
	MY_Lexer_t *lexer = MY_init_Lexer(assembler->pa__contents);

	MY_lex_Lexer(lexer);
	assembler->p__lexer = lexer;
	assembler->has_lexed = true;	
	return;
}

void ZLX_ASM_clear_assembler(ZLX_ASM_Assembler_t *_assembler)
{
	if(_assembler->has_content == true)
	{
		_assembler->contents_size = 0;
		free(_assembler->pa__contents);
		_assembler->pa__contents = NULL;
		if(_assembler->p__lexer && _assembler->has_lexed)
		{
			MY_free_Lexer(_assembler->p__lexer);
			_assembler->p__lexer = NULL;
			_assembler->has_lexed = false;
		}
		if(_assembler->pa__path)
		{
			free(_assembler->pa__path);
			_assembler->pa__path = NULL;
		}
	}
}


void ZLX_ASM_temp_frontend(ZLX_ASM_Assembler_t *_assembler)
{
	if(!_assembler->has_content || !_assembler->has_lexed)
		MY_FAULT("has no content || has not lexed");	


	int start_line = 0, first_space = 0;
	int line_count = 0;
	bool new_line_started = true, has_first_space = false;
	for(int i = 0; i < _assembler->p__lexer->token_count; ++i)
	{
		
		MY_token_type_t type = MY_get_token_type_Lexer(_assembler->p__lexer, i);	
		char buffer[255] = {0};
		

		MY_get_token_string_Lexer(_assembler->p__lexer, i, buffer, 255);		
		printf("{[%s][%d]} \n", buffer, type);
		if(type == MY_token_type_space && has_first_space == false)
		{
			if(new_line_started == true)
			{
				first_space = i;	
				has_first_space = true;
			}
		}
		else if(type == MY_token_type_newline)
		{
			int length;
			if(has_first_space)
			{
				length = first_space - start_line;	
			}
			else
			{
				length = i - start_line;
			}

			MY_Token_group_t group = MY_init_Token_group(_assembler->p__lexer, start_line, length); 

			ZLX_ASM_statement_type_t type = ZLX_ASM_classify_statement(group);
		
			printf("[%d] = %d type\n", line_count++,type);

			start_line = i;
			new_line_started = true;
			has_first_space = false;
		}

	}



}



void ZLX_ASM_test_Assembler(ZLX_ASM_Assembler_t *_assembler)
{
	
	MY_Token_group_t group = MY_init_Token_group(_assembler->p__lexer, 0, _assembler->p__lexer->token_count); 

	ZLX_ASM_statement_type_t type = ZLX_ASM_classify_statement(group);
	printf("%d type", type);
}

