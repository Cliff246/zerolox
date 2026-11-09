#ifndef ZLX_ASM_COMMONS_HEADER
#define ZLX_ASM_COMMONS_HEADER

#include <MY_Lexer.h>

#define ZLX_ASM_TYPE_BUFFER_SIZE 16

typedef struct ZLX_ASM_Statement_information
{
	MY_Token_group_t group;
	MY_token_type_t a__token_types[ZLX_ASM_TYPE_BUFFER_SIZE];
}ZLX_ASM_Statement_information_t;



#endif
