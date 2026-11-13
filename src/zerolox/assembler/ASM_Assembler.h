#ifndef ZLX_ASM_ASSEMBLER_HEADER
#define ZLX_ASM_ASSEMBLER_HEADER

#include "MY_Lexer.h"
#include <stdint.h>
#include <stdbool.h>

typedef struct ZLX_ASM_Assembler
{

	bool has_content, has_lexed;
	char *pa__path;
	//ptr array 
	char *pa__contents;
	size_t contents_size; 

	MY_Lexer_t *p__lexer;


}ZLX_ASM_Assembler_t;


ZLX_ASM_Assembler_t *ZLX_ASM_init_Assembler(void);


void ZLX_ASM_load_string_Assembler(ZLX_ASM_Assembler_t *_assembler, char *_argument);
bool ZLX_ASM_load_file_Assembler(ZLX_ASM_Assembler_t *assembler, const char *path);




void ZLX_ASM_lex_Assembler(ZLX_ASM_Assembler_t *assembler);
void ZLX_ASM_clear_assembler(ZLX_ASM_Assembler_t *_assembler);

void ZLX_ASM_temp_frontend(ZLX_ASM_Assembler_t *_assembler);


void ZLX_ASM_test_Assembler(ZLX_ASM_Assembler_t *_assembler);

#endif
