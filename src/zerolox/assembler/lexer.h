#ifndef __HEART_LEXER_HEADER__
#define __HEART_LEXER_HEADER__

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include "HRT_commons.h"


#define HRT_TOKEN_TYPE_LIST(X)			\
	X(EMPTY)							\
	X(INVALID)							\
	X(WORD)								\
	X(INT)								\
	X(FLOAT)							\
	X(HEXDECIMAL)						\
	X(SYMBOL)							\
	X(STRING)							\
	X(EXCLAIM)							\
	X(EXCLAIM_EQUALS)					\
	X(PARENTHESIS_OPEN)					\
	X(PARENTHESIS_CLOSED)				\
	X(CURLYBRACE_OPEN)					\
	X(CURLYBRACE_CLOSED)				\
	X(SQUAREBRACE_OPEN)					\
	X(SQUAREBRACE_CLOSED)				\
	X(ADD)								\
	X(ADD_ADD)							\
	X(ADD_EQUALS)						\
	X(MINUS)							\
	X(MINUS_MINUS)						\
	X(MINUS_EQUALS)						\
	X(SLASH)							\
	X(SLASH_SLASH)						\
	X(SLASH_EQUALS)						\
	X(STAR)								\
	X(STAR_STAR)						\
	X(STAR_EQUALS)						\
	X(COMMA)							\
	X(COLON)							\
	X(SEMICOLON)						\
	X(EQUALS)							\
	X(EQUALS_EQUALS)					\
	X(EQUALS_LESS_THAN)					\
	X(LESS_THAN_EQUALS)					\
	X(LESS_THAN)						\
	X(LESS_THAN_LESS_THAN)				\
	X(LESS_THAN_LESS_THAN_EQUALS)		\
	X(GREATER_THAN)						\
	X(GREATER_THAN_EQUALS)				\
	X(GREATER_THAN_GREATER_THAN)		\
	X(GREATER_THAN_GREATER_THAN_EQUALS)	\
	X(VERTICAL_LINE)					\
	X(VERTICAL_LINE_VERTICAL_LINE)		\
	X(VERTICAL_LINE_EQUALS)				\
	X(AMPERSAND)						\
	X(AMPERSAND_AMPERSAND)				\
	X(AMPERSAND_EQUALS)					\
	X(MODULO)							\
	X(MODULO_EQUALS)					\
	X(HAT)								\
	X(HAT_EQUALS)						\
	X(QUESTION_MARK)					\
	X(PERIOD)							\
	X(AT)								\
	X(HASHTAG)							\

#define HRT_TOKEN_TYPE_ENUM_NAME(X) HRT_TOKEN_ ## X
#define HRT_TOKEN_TYPE_ENUM(X) HRT_TOKEN_TYPE_ENUM_NAME(X),


typedef enum HRT_Token_type
{
	HRT_TOKEN_TYPE_LIST(HRT_TOKEN_TYPE_ENUM)
}HRT_Token_type_t;

#define HRT_TOKEN_TYPE_STRING(X) ("HRT_TOKEN_"#X),

extern char *HRT_token_types_String[];



typedef struct HRT_Token
{
	HRT_Token_type_t m_type;
	int m_start;
	int m_length;
	int m_column;
	int m_row;
}HRT_Token_t;

typedef struct HRT_Lexer
{
	char *m_string;
	int m_string_length;

	int m_token_allocated;
	int m_token_count;
	HRT_Token_t *m_token_buffer;

	int m_position;
	int m_row, m_column;
	int m_trow, m_tcolumn;
	bool m_print_each;
}HRT_Lexer_t;

HRT_Lexer_t *HRT_initialize_Lexer(char *_string);
int HRT_get_token_string_Lexer(HRT_Lexer_t *_lexer, int _position, char *_buffer, int _blength);
int HRT_get_substring_Lexer(HRT_Lexer_t *_lexer, int _start, int _length, char *_buffer, int _blength);
void HRT_lex_Lexer(HRT_Lexer_t *_lexer);
void HRT_print_Lexer(HRT_Lexer_t *lexer);
void HRT_free_Lexer(HRT_Lexer_t *_lexer);
#endif