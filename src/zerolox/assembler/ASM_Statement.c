#include "ASM_Statement.h"
#include "MY_commons.h"
#include "MY_Lexer.h"

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

#define TYPE_BUFFER_SIZE 16

static int next_space(MY_token_type_t buf[TYPE_BUFFER_SIZE], int index)
{

	if(index < 0 || index >= TYPE_BUFFER_SIZE)
	{
		return -2;
	}
	int space = -1;
	for(int i = index; i < TYPE_BUFFER_SIZE; ++i)
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


struct StatementInformation
{
	MY_Token_group_t group;
	MY_token_type_t token_types[TYPE_BUFFER_SIZE];
};



static struct StatementInformation get_statement_tokentypes(MY_Token_group_t _group)
{
	int count = _group.length;
	if(count >= TYPE_BUFFER_SIZE)
		MY_FAULT("count >= 16");
	struct StatementInformation statement_information =
	{
		.group = _group,
	};

	for(int i = 0; i < count && i < TYPE_BUFFER_SIZE; ++i)
	{
		statement_information.token_types[i] = MY_get_token_type_Token_group(_group, i);
	}


	return statement_information;	
}	

ZLX_ASM_statement_type_t ZLX_ASM_classify_statement(MY_Token_group_t _group)
{
	struct StatementInformation statement_information = get_statement_tokentypes(_group);	


	MY_token_type_t first = statement_information.token_types[0];


	if(first == MY_token_type_modulo)
		return ZLX_ASM_statement_type_directive;
	else if(first == MY_token_type_ampersand)
		return ZLX_ASM_statement_type_operation_semantic;
	
	int last = 0;
	for(int i = 1; i < TYPE_BUFFER_SIZE; ++i)
	{
		if(statement_information.token_types[i] == MY_token_type_space)
		{
			break;
		}
		last++;
	}	
	
	if(statement_information.token_types[last] == MY_token_type_colon)
		return ZLX_ASM_statement_type_label;


	if(valid_form(statement_information.token_types, last))
		return ZLX_ASM_statement_type_operation_mnemonic;
	return ZLX_ASM_statement_type_invalid;
}



ZLX_ASM_Statement_t *ZLX_ASM_convert_token_to_statement_Assembler(ZLX_ASM_Assembler_t *assembler, int32_t start, int32_t count)
{
	if(start < 0 || count <= 0 )
		MY_FAULT("failed");
	int opcode_size = 0;
	char opcode[1024] = {0};
#define BUFFER_SIZE 128
	char buffer[BUFFER_SIZE] = {0};



}

#undef TYPE_BUFFER_SIZE

