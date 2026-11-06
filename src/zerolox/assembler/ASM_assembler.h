#ifndef ZLX_ASM_ASSEMBLER_HEADER
#define ZLX_ASM_ASSEMBLER_HEADER

#include "MY_lexer.h"
#include <stdint.h>
#include <stdbool.h>

typedef struct ZLX_ASM_Assembler
{

	char *p__path;
	
	char *p__file_contents;
	uint64_t file_contents_size; 

	MY_lexer_t *p__lexer;


}ZLX_ASM_Assembler_t;


ZLX_ASM_Assembler_t *ZLX_ASM_init_Assembler(void);
bool ZLX_ASM_load_file_Assembler(ZLX_ASM_Assembler_t *assembler, const char *path);


void ZLX_ASM_lex_Assembler(ZLX_ASM_Assembler_t *assembler);

#endif
