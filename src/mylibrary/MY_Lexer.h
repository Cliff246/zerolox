#ifndef MY_LEXER_H
#define MY_LEXER_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>


#define MY_TOKEN_TYPE_LIST(X)			\
	X(empty)							\
	X(invalid)							\
	X(space)							\
	X(newline)							\
	X(word)								\
	X(int)								\
	X(float)							\
	X(hexdecimal)						\
	X(symbol)							\
	X(string)							\
	X(exclaim)							\
	X(exclaim_equals)					\
	X(parenthesis_open)					\
	X(parenthesis_closed)				\
	X(curlybrace_open)					\
	X(curlybrace_closed)				\
	X(squarebrace_open)					\
	X(squarebrace_closed)				\
	X(add)								\
	X(add_add)							\
	X(add_equals)						\
	X(minus)							\
	X(minus_minus)						\
	X(minus_equals)						\
	X(slash)							\
	X(slash_slash)						\
	X(slash_equals)						\
	X(star)								\
	X(star_star)						\
	X(star_equals)						\
	X(comma)							\
	X(colon)							\
	X(semicolon)						\
	X(equals)							\
	X(equals_equals)					\
	X(equals_less_than)					\
	X(less_than_equals)					\
	X(less_than)						\
	X(less_than_less_than)				\
	X(less_than_less_than_equals)		\
	X(greater_than)						\
	X(greater_than_equals)				\
	X(greater_than_greater_than)		\
	X(greater_than_greater_than_equals)	\
	X(vertical_line)					\
	X(vertical_line_vertical_line)		\
	X(vertical_line_equals)				\
	X(ampersand)						\
	X(ampersand_ampersand)				\
	X(ampersand_equals)					\
	X(modulo)							\
	X(modulo_equals)					\
	X(hat)								\
	X(hat_equals)						\
	X(question_mark)					\
	X(period)							\
	X(at)								\
	X(hashtag)							\

#define MY_TOKEN_TYPE_ENUM_NAME(X) MY_token_type_ ## X
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

#define MY_TOKEN_TYPE_STRING(X) ("MY_token_type_"#X),

extern char *MY_token_type_names[];



typedef struct MY_Token
{
	MY_token_type_t type;
	int start;
	int length;
	int column;
	int row;
}MY_Token_t;

typedef struct MY_Lexer
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
void MY_print_Lexer(MY_Lexer_t *_lexer);
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
//
typedef struct MY_Token_group
{
	//i actually need to remember shit... it's ptr, array of ptr's not
	MY_Lexer_t *cp__lexer;
	int start, length;
}MY_Token_group_t;

MY_Token_group_t MY_init_Token_group(MY_Lexer_t *_lexer, int _start, int _length);

MY_token_type_t MY_get_token_type_Token_group(MY_Token_group_t _group, int _index);
int MY_get_token_string_Token_group(MY_Token_group_t _group, int _index, char *_buffer, int _size);

//in tokens, ie start token, to length of last
#endif
