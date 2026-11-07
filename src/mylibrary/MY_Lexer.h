#ifndef MY_LEXER_H
#define MY_LEXER_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>


#define MY_TOKEN_TYPE_LIST(X)			\
	X(EMPTY)							\
	X(INVALID)							\
	X(SPACE)							\
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

#define MY_TOKEN_TYPE_ENUM_NAME(X) MY_TOKEN_ ## X
#define MY_TOKEN_TYPE_ENUM(X) MY_TOKEN_TYPE_ENUM_NAME(X),

//this needs to be totally renamed, it doesn't follow the proper naming format
//
//
//
//
//

typedef enum MY_token_type
{
	MY_TOKEN_TYPE_LIST(MY_TOKEN_TYPE_ENUM)
}MY_token_type_t;

#define MY_TOKEN_TYPE_STRING(X) ("MY_TOKEN_"#X),

extern char *MY_token_type_names[];



typedef struct MY_token
{
	MY_token_type_t type;
	int start;
	int length;
	int column;
	int row;
}MY_Token_t;

typedef struct MY_lexer
{
	char *ap__string;
	int string_length;

	int token_allocated;
	int token_count;
	MY_Token_t *ap__token_buffer;

	int position;
	int row, column;
	int trow, tcolumn;
	bool print_each;
}MY_Lexer_t;

MY_Lexer_t *MY_init_Lexer(char *_string);
int MY_get_token_string_Lexer(MY_Lexer_t *_lexer, int _position, char *_buffer, int _blength);
int MY_get_substring_Lexer(MY_Lexer_t *_lexer, int _start, int _length, char *_buffer, int _blength);
MY_token_type_t MY_get_token_type_Lexer(MY_Lexer_t *_lexer, int _index); 
void MY_lex_Lexer(MY_Lexer_t *_lexer);
void MY_print_Lexer(MY_Lexer_t *lexer);
void MY_free_Lexer(MY_Lexer_t *_lexer);



//---- before i kill myself, i am going to add a token group that is nice and simple, basically just a subset of lexer_t


//p = ptr
//pa = ptr, array
//pap = ptr, array of ptr's
//cpap = constant ptr, array of ptr's
//cpapu = constant ptr, array of ptr unions (ie needs a dedicated free)
//bleh bleh bleh... idk this is supposed to show you what needs to die and what not.
//it should tell you what can be just raw free'd and what cannot be... 
//i am cringe
//a means [index]
//p means ptr. 
//i need another for has complex free. idk, honest to god
// fuck ,e
typedef struct MY_Token_group
{
	//i actually need to remember shit... it's ptr, array of ptr's not 	
	MY_Lexer_t *cp__lexer;	
	int start, length;
}MY_Token_group_t;

//in tokens, ie start token, to length of last
#endif
