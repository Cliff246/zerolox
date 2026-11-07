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




ZLX_ASM_Statement_t *ZLX_ASM_convert_token_to_statement_Assembler(ZLX_ASM_Assembler_t *assembler, int32_t start, int32_t count)
{
	if(start < 0 || count <= 0 )
		MY_FAULT("failed");
	int opcode_size = 0;
	char opcode[1024] = {0};
#define BUFFER_SIZE 128
	char buffer[BUFFER_SIZE] = {0};

	if(count >= TYPE_BUFFER_SIZE)
		MY_FAULT("count >= 16");
	MY_token_type_t type_list[TYPE_BUFFER_SIZE] = {MY_token_type_empty};


	for(int i = 0; i < count && i < TYPE_BUFFER_SIZE; ++i)
	{
		type_list[i] = MY_get_token_type_Lexer(assembler->p__lexer, i + start);

	}


	int first_group = next_space(type_list, 0);
	if(first_group == -1 || first_group == 0)
	{
		MY_TODO;
	}


	//ugh parser, or just be a dipshit and do all cases. i forgot how to do a parser.
	if(type_list[0] == MY_token_type_word)
	{

	}

#undef BUFFER_SIZE

	ZLX_ASM_Statement_t *statement = ZLX_ASM_init_Statement();

	MY_TODO;


	return statement;

}

#undef TYPE_BUFFER_SIZE

