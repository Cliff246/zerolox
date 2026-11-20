#include "ASM_Operation.h"
#include "ASM_Argument.h"
#include "ASM_commons.h"
#include "MY_Lexer.h"
#include "MY_commons.h"

static void print_type_buffer(MY_token_type_t type[ZLX_ASM_TYPE_BUFFER_SIZE])
{
	for(int i = 0; i < ZLX_ASM_TYPE_BUFFER_SIZE; ++i)
	{
		printf("[%d] = %s\n",	i, MY_token_type_names[type[i]]);
	}
}



struct segments
{
	int last;
	int count;

	short segments[ZLX_ASM_TYPE_BUFFER_SIZE];
	short length[ZLX_ASM_TYPE_BUFFER_SIZE];
};

static void print_segments(struct segments seg)
{
	for(int i = 0; i < ZLX_ASM_TYPE_BUFFER_SIZE; ++i)
	{
		if(seg.segments[i] == -1)
			break;
		printf("[%d] = %d, %d\n", i, seg.segments[i], seg.length[i]);
	}
}
static struct segments init_segments()
{
	struct segments seg = {0};
	for(int i = 0; i < ZLX_ASM_TYPE_BUFFER_SIZE; ++i)
	{
		seg.segments[i] = -1;
		seg.length[i] = -1;
	}
	seg.count = 0;
	seg.last = 0;
	return seg;
}

static struct segments find_segments(ZLX_ASM_Statement_information_t information)
{
	struct segments seg = init_segments();
	bool inside = false;
	seg.count = 0;
	if(information.a__token_types[0] != MY_token_type_space)
	{
		inside = true;
		seg.segments[seg.count] = 0;
		seg.length[seg.count] = 1;

	}

	print_type_buffer(information.a__token_types);
	for(int i = 1; i < ZLX_ASM_TYPE_BUFFER_SIZE; ++i)
	{
		MY_token_type_t type = information.a__token_types[i];
				
		if(type == MY_token_type_space)
		{
			inside = false;
			seg.last = i;
			continue;
		}	
		else if(type == MY_token_type_newline || type == MY_token_type_empty)
		{
			seg.last = i;
			break;	
		}
		else if(inside == false)
		{
			seg.count++;
			seg.segments[seg.count] = i;
			seg.length[seg.count] = 1;
			inside = true;
		}
		else if(inside)
		{
			seg.length[seg.count]++;
		}
		
	}
	print_segments(seg);
	return seg; 
}


ZLX_ASM_Operation_mnemonic_t ZLX_ASM_collect_mnemonic_Operation_mnemonic(ZLX_ASM_Statement_information_t information, int start, int end)
{
	
	char buffer[256] = {0};
	
	int size = MY_get_substring_Token_group(information.group, start, end, buffer, 256); 
	if(size < 0)
		MY_FAULT("could not fit substring in buffer");

	printf("%s\n", buffer);
	
	ZLX_ASM_Operation_mnemonic_t mnemonic = {0};
	return mnemonic;
}




ZLX_ASM_Operation_t ZLX_ASM_init_mnemonic_Operation(ZLX_ASM_Statement_information_t _statement_information)
{

	struct segments seg = find_segments(_statement_information); 

	ZLX_ASM_collect_mnemonic_Operation_mnemonic(_statement_information, seg.segments[0], seg.length[0] + seg.segments[0]); 	
	ZLX_ASM_Argument_t argument; 


	ZLX_ASM_Operation_t operation = {0};
	return operation;
}



ZLX_ASM_Operation_t ZLX_ASM_init_semantic_Operation(ZLX_ASM_Statement_information_t _statement_information)
{
	ZLX_ASM_Operation_t operation = {0};
	return operation;
}
