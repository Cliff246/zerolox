#include "ASM_Operation.h"
#include "ASM_Argument.h"
#include "ASM_commons.h"
#include "MY_Lexer.h"
#include "MY_commons.h"
#include "syntax/SYTX_syntax.h"
#include <math.h>
#include <string.h>

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

	//print_type_buffer(information.a__token_types);
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
	//print_segments(seg);
	return seg; 
}


ZLX_ASM_Operation_mnemonic_t ZLX_ASM_collect_mnemonic_Operation_mnemonic(ZLX_ASM_Statement_information_t information, int start, int length)
{
	int dots[2] =
	{
		-1, -1
	};
	char dots_count = -1;

	int i = start;	
	for(; i < ZLX_ASM_TYPE_BUFFER_SIZE && i < length; ++i)
	{

		MY_token_type_t type = information.a__token_types[i];
		if(type == MY_token_type_period)
		{
			if(dots_count == 1)
				MY_FAULT("too many dots in an opcode");
			dots[++dots_count] = i;

		}
		
	}
	//printf("0:%d 1:%d\n", dots[0], dots[1]);	
	
	char buffer1[16] = {0};
	int opcode = -1;
	ZLX_SYTX_instruction_prefix_t prefix = ZLX_SYTX_instruction_prefix_invalid;
	ZLX_SYTX_instruction_postfix_t postfix = ZLX_SYTX_instruction_postfix_invalid;
	if(dots_count == -1)
	{
		//this one is always the opcode		
		int test_size1 = MY_get_substring_Token_group(information.group, start, length, buffer1, 16);  

		if(test_size1 < 0)
			MY_FAULT("could not fit substring in buffer");
		
		prefix = ZLX_SYTX_instruction_prefix_none;		
		opcode = ZLX_SYTX_find_mnemonic(buffer1, prefix);
		postfix = ZLX_SYTX_instruction_postfix_none;
		//printf("0:%s\n", buffer1);
		//printf("dots count -1\n");
	}
	else if(dots_count == 0)
	{
		int test_size1 = MY_get_substring_Token_group(information.group, start, dots[0], buffer1, 16);  

		if(test_size1 < 0)
			MY_FAULT("could not fit substring in buffer");

		
		int test = ZLX_SYTX_get_mnemonic_code(buffer1);
		
		if(test == -1)
		{
			prefix = ZLX_SYTX_get_instruction_prefix(buffer1);	
			//the other one is the opcode
			test_size1 = MY_get_substring_Token_group(information.group, dots[0] + 1, length - dots[0] - 1, buffer1, 16);
			if(test_size1 < 0)
				MY_FAULT("could not fit substring in buffer");
			test = ZLX_SYTX_find_mnemonic(buffer1, prefix);
			if(test == -1)
			{
				MY_FAULT("opcode not found");
				opcode = -1;
			}
			else 
				opcode = test;
			postfix = ZLX_SYTX_instruction_postfix_none;
		}
		else 
		{
			prefix = ZLX_SYTX_instruction_prefix_none;
			opcode = test;
			char temp[16];
			//copy out 
			memcpy(temp, buffer1, 16);
			test_size1 = MY_get_substring_Token_group(information.group, dots[0] + 1, length - dots[0] - 1, buffer1, 16);
			if(test_size1 < 0)
				MY_FAULT("could not fit substring in buffer");
			postfix = ZLX_SYTX_get_instruction_postfix(buffer1);			
			if(postfix == ZLX_SYTX_instruction_postfix_invalid)
				MY_FAULT("failed to do a postfix correctly");
			//copy in
			memcpy(buffer1, temp, 16);
		}

		//printf("dots count 0\n");
	}
	else if(dots_count == 1)
	{
		//prefix
		char buffer_prefix[16] = {0};
		int test_prefix = MY_get_substring_Token_group(information.group, start, dots[0], buffer_prefix, 16); 
		if(test_prefix < 0)
			MY_FAULT("failed to get proper prefix");

		prefix = ZLX_SYTX_get_instruction_prefix(buffer_prefix);
		if(prefix == ZLX_SYTX_instruction_prefix_invalid)
			MY_FAULT("invalid prefix");
		//----------------------
		//opcode
		int test_size1 = MY_get_substring_Token_group(information.group, dots[0] + 1, dots[1] - dots[0] - 1, buffer1, 16);  
		if(test_size1 < 0)
			MY_FAULT("could not fit substring in buffer");
		//--------------------
		//prefix
		char buffer_postfix[16] = {0};
		int test_postfix = MY_get_substring_Token_group(information.group, dots[1] + 1, length - dots[1] - 1, buffer_postfix, 16);
		if(test_postfix < 0)
			MY_FAULT("could not fit substring in buffer");
		postfix = ZLX_SYTX_get_instruction_postfix(buffer_postfix);
		if(postfix == ZLX_SYTX_instruction_postfix_invalid)
			MY_FAULT("invalid prefix");

	

		
		opcode = ZLX_SYTX_find_mnemonic(buffer1, prefix);
		//printf("dots count 1\n");
	}

	//printf("%s\n", buffer1);	
	/* Both major forms accept n.; distinguish their operand counts after
	 * reading the prefix. Two-operand signed aliases resolve at emission. */
	if (opcode >= 0 &&
	    (ZLX_SYTX_mnemonics[opcode].form == ZLX_SYTX_pattern_form_register_immediate ||
	     ZLX_SYTX_mnemonics[opcode].form == ZLX_SYTX_pattern_form_signed_immediate_pair))
	{
		unsigned commas = 0;
		for (int j = start + length; j < information.group.length; ++j)
		{
			MY_token_type_t type = information.a__token_types[j];
			if (type == MY_token_type_semicolon || type == MY_token_type_newline)
				break;
			if (type == MY_token_type_comma)
				++commas;
		}
		if (commas == 2)
		{
			for (size_t i = 0; i < ZLX_SYTX_mnemonics_count; ++i)
				if (ZLX_SYTX_mnemonics[i].form == ZLX_SYTX_pattern_form_register_register_immediate &&
				    strcmp(buffer1, ZLX_SYTX_mnemonics[i].a__mnemonic) == 0)
				{
					opcode = (int)i;
					break;
				}
		}
	}
	if(opcode == -1)
	{
		MY_FAULT("opcode does not match any");
		
	}

	ZLX_ASM_Operation_mnemonic_t mnemonic = {0};
	mnemonic.opcode = opcode;
	mnemonic.prefix = prefix;
	mnemonic.postfix = postfix;
	return mnemonic;
}




ZLX_ASM_Operation_t ZLX_ASM_init_mnemonic_Operation(ZLX_ASM_Statement_information_t _statement_information)
{

	struct segments seg = find_segments(_statement_information); 

	ZLX_ASM_Operation_t operation = {.type = ZLX_ASM_operation_type_mnemonic};
	operation.u__mnemonic = ZLX_ASM_collect_mnemonic_Operation_mnemonic(
		_statement_information, seg.segments[0], seg.length[0]);
	return operation;
}



ZLX_ASM_Operation_t ZLX_ASM_init_semantic_Operation(ZLX_ASM_Statement_information_t _statement_information)
{
	ZLX_ASM_Operation_t operation = {0};
	return operation;
}
