#include "ASM_Statement.h"
#include "ASM_Operation.h"
#include "MY_commons.h"
#include "ASM_commons.h"
#include "MY_Lexer.h"
#include "MY_commons.h"
#include <assert.h>
#include <stdlib.h>

ZLX_ASM_Statement_t *ZLX_ASM_init_Statement()
{
	ZLX_ASM_Statement_t *statement = calloc(1, sizeof(*statement));
	if(!statement)
		MY_FAULT("malloc failed");


	return statement;
}

void ZLX_ASM_free_Statement(ZLX_ASM_Statement_t *statement)
{
	MY_TODO;
}



enum MnemonicState
{
    EXPECT_WORD,
    EXPECT_DOT_VALUE,
    EXPECT_SEPARATOR
};



static bool valid_form(const MY_token_type_t *tokens, int count) 
{
    enum MnemonicState state = EXPECT_WORD;
    for (int i = 0; i < count; i++) 
	{
        MY_token_type_t t = tokens[i];
        switch (state) 
		{
        case EXPECT_WORD:
            if (t != MY_token_type_word)
                return false;
            state = EXPECT_SEPARATOR;
            break;
        case EXPECT_DOT_VALUE:
            if (t != MY_token_type_word &&
                t != MY_token_type_int)
                return false;
            state = EXPECT_SEPARATOR;
            break;
        case EXPECT_SEPARATOR:
            if (t == MY_token_type_period)
                state = EXPECT_DOT_VALUE;
            else if (t == MY_token_type_add)
                state = EXPECT_WORD;
            else
                return false;
            break;
        }
    }
    return state == EXPECT_SEPARATOR;
}


static int next_space(MY_token_type_t buf[ZLX_ASM_TYPE_BUFFER_SIZE], int index)
{

	if(index < 0 || index >= ZLX_ASM_TYPE_BUFFER_SIZE)
	{
		return -2;
	}
	int space = -1;
	for(int i = index; i < ZLX_ASM_TYPE_BUFFER_SIZE; ++i)
	{
		if(buf[i] == MY_token_type_space)
		{
			space = i;
			break;
		}
		else if(buf[i] == MY_token_type_empty)
		{
			break;
		}
	}
	return space;
}


//the form for the mnemonic can roughly be
//word dot word plus word dot word
//it cannot be word dot
//dot word dot
//etc
//
//basically [word], [word dot word], [word dot word dot word] [word plus word] [word dot word plus word]
//shit
//
//isolating what's in and what's out
//
//start: [word dot]
//end: [dot word]
//middle: [word], [word plus word]
//




static ZLX_ASM_Statement_information_t get_statement_tokentypes(MY_Token_group_t _group)
{
	//MY_ENTRY("get statement type");
	int count = _group.length;
	if(count >= ZLX_ASM_TYPE_BUFFER_SIZE)
		MY_FAULT("count >= 16");
	ZLX_ASM_Statement_information_t statement_information =
	{
		.group = _group,
	};

	for(int i = 0; i < count && i < ZLX_ASM_TYPE_BUFFER_SIZE; ++i)
	{
		statement_information.a__token_types[i] = MY_get_token_type_Token_group(_group, i);
	}


	return statement_information;	
}	


static ZLX_ASM_statement_type_t classify_statement(ZLX_ASM_Statement_information_t statement_information)
{
	MY_token_type_t first = statement_information.a__token_types[0];
	if(first == MY_token_type_modulo)
		return ZLX_ASM_statement_type_directive;
	else if(first == MY_token_type_at)
		return ZLX_ASM_statement_type_operation_semantic;
	
	int last = 0;
	for(int i = 0; i < ZLX_ASM_TYPE_BUFFER_SIZE; ++i)
	{
		MY_token_type_t type = statement_information.a__token_types[i];
		if(type == MY_token_type_space || type == MY_token_type_empty)
		{
			break;
		}
		last++;
	}	
	
	if(statement_information.a__token_types[last - 1] == MY_token_type_colon)
		return ZLX_ASM_statement_type_label;


	if(valid_form(statement_information.a__token_types, last ))
		return ZLX_ASM_statement_type_operation_mnemonic;
	return ZLX_ASM_statement_type_invalid;

}

ZLX_ASM_statement_type_t ZLX_ASM_classify_statement(MY_Token_group_t _group)
{
	//MY_ENTRY("classify statement");
	ZLX_ASM_Statement_information_t statement_information = get_statement_tokentypes(_group);	
	return classify_statement(statement_information);
}



ZLX_ASM_Statement_t *ZLX_ASM_convert_token_to_statement_Assembler(ZLX_ASM_Assembler_t *assembler, MY_Token_group_t _group)
{
	if(!assembler)
		return NULL;


	ZLX_ASM_Statement_information_t information = get_statement_tokentypes(_group); 

	
	
	MY_print_token_group(_group);
	ZLX_ASM_statement_type_t type = classify_statement(information);

	if(type == ZLX_ASM_statement_type_invalid)
	{
		return NULL;
	}
	//this should be removed 
	ZLX_ASM_Statement_t *statement = calloc(1, sizeof(ZLX_ASM_Statement_t));
	if(!statement)
		MY_FAULT("statement malloc failed");

	//should be temporary
	statement->type = type;
	switch(type)
	{
		case ZLX_ASM_statement_type_label:
			MY_FAULT("labels not solved yet");
			break;
		case ZLX_ASM_statement_type_operation_mnemonic:
			statement->u__operation = ZLX_ASM_init_mnemonic_Operation(information);	
			
			break;
		case ZLX_ASM_statement_type_operation_semantic:
			MY_FAULT("semantics not solved yet");
			break;
		case ZLX_ASM_statement_type_directive:
			MY_FAULT("directives not solved yet");
			break;
		case ZLX_ASM_statement_type_invalid:
			MY_FAULT("invalid for some reason got here");
			break;
		default:
			MY_FAULT("invalid");
	}


	

	return statement;
}

#undef TYPE_BUFFER_SIZE

