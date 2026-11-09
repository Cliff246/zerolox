#ifndef ZLX_ASM_OPERATION_HEADER
#define ZLX_ASM_OPERATION_HEADER

#include "ASM_Argument.h"
#include "ASM_commons.h"
#include "syntax/SYTX_syntax.h"


#include <stdbool.h>

typedef enum ZLX_ASM_operation_type
{
	ZLX_ASM_operation_type_invalid,
	ZLX_ASM_operation_type_mnemonic,
	ZLX_ASM_operation_type_semantic,
}ZLX_ASM_operation_type_t;

typedef struct ZLX_ASM_Operation_mnemonic
{
	//waste of space for the has post and prefix
	bool has_prefix, has_postfix;
	char a__prefix[4];
	char a__mnemonic[8];
	char a__postfix[4];


}ZLX_ASM_Operation_mnemonic_t;

typedef struct ZLX_ASM_Operation_semantic
{

}ZLX_ASM_Operation_semantic_t;


typedef struct ZLX_ASM_Operation
{

	ZLX_ASM_operation_type_t type;
	union
	{
		ZLX_ASM_Operation_mnemonic_t u__mnemonic;
		ZLX_ASM_Operation_semantic_t u__semantic;
	};

	char a__identifier[ZLX_SYTX_MNEMONIC_SIZE];
	size_t arguments_count;
	//hah hah this is actually the single dumbest idea ever, every single fucking instruction and argument with a malloc, brilliant
	ZLX_ASM_Argument_t **pap__arguments;



}ZLX_ASM_Operation_t;

ZLX_ASM_Operation_t ZLX_ASM_init_mnemonic_Operation(ZLX_ASM_Statement_information_t _statement_information);


#endif
