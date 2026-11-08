#include "MY_Lexer.h"
#include "MY_commons.h"
#include <_ctype.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define BASE_TOKENS_ALLOCATE 10

char *MY_token_type_names[] =
{
	MY_TOKEN_TYPE_LIST(MY_TOKEN_TYPE_STRING)
};


static void allocate_tokens(MY_Lexer_t *lexer)
{
	if(lexer->token_allocated == 0)
	{
		lexer->token_allocated = BASE_TOKENS_ALLOCATE;
		lexer->token_count = 0;

		lexer->ap__token_buffer = malloc(sizeof(MY_Token_t) * lexer->token_allocated);
		if(!lexer->ap__token_buffer)
			exit(1);

	}
	else
	{
		if(lexer->token_allocated > (INT_MAX - 1) / 2)
		{
			exit(1);
		}
		lexer->token_allocated = (lexer->token_allocated * 2) + 1;
		void *temp = realloc(lexer->ap__token_buffer, sizeof(MY_Token_t) *  lexer->token_allocated);
		if(temp == NULL)
			exit(1);
		lexer->ap__token_buffer = temp;
	}
}


static void add_token(MY_Lexer_t *lexer, MY_Token_t token)
{
	if(lexer->token_count >= lexer->token_allocated)
	{
		allocate_tokens(lexer);
	}
	lexer->ap__token_buffer[lexer->token_count++] = token;
}

static void print_token(MY_Lexer_t *lexer, int position)
{
	if(position < lexer->token_count && position >= 0)
	{
		MY_Token_t token = lexer->ap__token_buffer[position];
		if(token.start < 0)
		{
			return;
		}
		if(token.length + token.start > lexer->string_length)
		{
			return;
		}
		printf("token %s{%d:%d} [%d,%d]: <",MY_token_type_names[token.type], token.row, token.column,  token.start, token.length);
		for(int i = token.start; i < token.length + token.start; ++i)
		{
			if(lexer->ap__string[i] == '\n')
			{
				putchar('\\');
				putchar('n');
			}
			else
			{
				putchar(lexer->ap__string[i]);
			}
		}
		printf(">\n");
	}

}

static MY_Token_t init_token(MY_token_type_t type, int start, int stop, int _column, int _row)
{
	return (MY_Token_t){.type = type, .start = start, .length = stop - start, .column = _column, .row = _row};

}



static char peek(MY_Lexer_t *lexer)
{

	if(lexer->position >= lexer->string_length)
		return 0;
	else
	{
		char ch = lexer->ap__string[lexer->position];
		if(lexer->print_each == true)
		{
			printf("%c", ch);
		}
		return ch;

	}
}

static char advance(MY_Lexer_t *lexer)
{
	char consumed = peek(lexer);
	if(lexer->position >= lexer->string_length)
	{
		return 0;
	}
	lexer->position++;
	char pk = peek(lexer);
	if(consumed == '\n')
	{
		lexer->row++;
		lexer->column = 0;
	}
	else if(consumed == '\t')
	{
		lexer->column += 4;
	}
	else
	{
		lexer->column++;
	}
	return pk;
}

static int position(MY_Lexer_t *lexer)
{
	return lexer->position;
}


static void add_token_start_word(MY_Lexer_t *lexer, MY_token_type_t type, int start)
{
	int stop = position(lexer);
	MY_Token_t range = init_token(type, start, stop, lexer->tcolumn, lexer->trow);

	add_token(lexer, range);
}


static void has_character_ctype(MY_Lexer_t *lexer, int (*ctype)(int c))
{
	char scroll = peek(lexer);
	while(ctype((unsigned char)scroll))
	{
		scroll = advance(lexer);
	}
}

static void has_character_string(MY_Lexer_t *lexer, char *string)
{
	char scroll = peek(lexer);
	while(strchr(string, scroll) && scroll != 0)
	{
		scroll = advance(lexer);
	}
}

static void has_character_ctype_or_string(MY_Lexer_t *lexer, int (*ctype)(int c), char *string)
{
	char scroll = peek(lexer);
	while(ctype((unsigned char)scroll) || (strchr(string, scroll) && scroll != 0))
	{
		scroll = advance(lexer);
	}
}


int MY_get_token_string_Lexer(MY_Lexer_t *_lexer, int _position, char *_buffer, int _blength)
{
	if(_position < _lexer->token_count && _position >= 0)
	{
		MY_Token_t token = _lexer->ap__token_buffer[_position];
		if(token.start < 0)
		{
			return -1;
		}
		if(token.length + token.start > _lexer->string_length || token.length >= _blength)
		{
			return -1;
		}
		int c = 0;
		for(int i = token.start; i < token.length + token.start && c < _blength; ++i, ++c)
		{
			_buffer[c] = _lexer->ap__string[i];
		}
		_buffer[c] = '\0';
		return c;
	}
	return -1;
}

void MY_print_Lexer(MY_Lexer_t *_lexer)
{

	for(int i = 0; i < _lexer->token_count; ++i)
	{
		print_token(_lexer, i);
	}
}


int MY_get_substring_Lexer(MY_Lexer_t *_lexer, int _start, int _length, char *_buffer, int _blength)
{
	if(_start < 0 || _length < 0 || _blength <= _length ||
		_start > _lexer->string_length ||
		_length > _lexer->string_length - _start)
	{
		return -1;
	}
	memcpy(_buffer, _lexer->ap__string + _start, (size_t)_length);
	_buffer[_length] = '\0';
	int c = _length;
	return c;
}


MY_token_type_t MY_get_token_type_Lexer(MY_Lexer_t *_lexer, int _index)
{

	if(_index < 0 || _index >= _lexer->token_count)
	{
		return MY_token_type_invalid;
	}
	return _lexer->ap__token_buffer[_index].type;
}

MY_Lexer_t *MY_init_Lexer(char *_string)
{
	if(!_string)
		exit(1);
	MY_Lexer_t *lexer = (MY_Lexer_t *)malloc(sizeof(MY_Lexer_t));
	if(!lexer)
		exit(1);
	lexer->token_allocated = 0;
	allocate_tokens(lexer);

	size_t length = strlen(_string);
	if(length > INT_MAX)
	{
		exit(1);
	}
	char *copy = malloc(length + 1);
	if(!copy)
	{
		free(lexer->ap__token_buffer);
		free(lexer);
		exit(1);
	}
	memcpy(copy, _string, length + 1);
	lexer->string_length = strlen(copy);
	lexer->ap__string = copy;
	lexer->position = 0;
	lexer->row = 1;
	lexer->column = 0;
	lexer->print_each = false;
	return lexer;
}

void MY_lex_Lexer(MY_Lexer_t *_lexer)
{
	bool space = false;

	while(_lexer->position < _lexer->string_length)
	{
		_lexer->tcolumn = _lexer->column;
		_lexer->trow = _lexer->row;
		int start = position(_lexer);
		char current = peek(_lexer);
		char scroll = advance(_lexer);

		int stop;
		if(current == ' ' || current == '\t')
		{
			if(space == false)
			{
				add_token_start_word(_lexer, MY_token_type_space, start);
				space = true;
			}
		}
		else
		{
			if(current == '\n' || current == '\r')
			{

			}

			else if(isalpha((unsigned char)current) || current == '_')
			{
				has_character_ctype_or_string(_lexer, isalnum, "_");

				add_token_start_word(_lexer, MY_token_type_word, start);
			}
			else if(current == '\"')
			{
				while(scroll != '\0' && scroll != '\n' && scroll != '\"')
				{
					scroll = advance(_lexer);
				}
				if(scroll == '\"')
				{
					advance(_lexer);
				}
				else
				{
					stop = position(_lexer);
					add_token_start_word(_lexer, MY_token_type_invalid, start);
					continue;
				}
				stop = position(_lexer);
				MY_Token_t range = init_token(MY_token_type_string, start + 1, stop - 1, _lexer->tcolumn, _lexer->trow);
				add_token(_lexer, range);
			}
			else if(current == '!')
			{
				if(scroll == '=')
				{
					advance(_lexer);
					add_token_start_word(_lexer, MY_token_type_exclaim_equals, start);

				}
				else
					add_token_start_word(_lexer, MY_token_type_exclaim, start);

			}
			else if(isdigit((unsigned char)current))
			{
				bool has_hexcode = false, has_zero_start = false;
				if(scroll == 'x' || scroll == 'X')
				{
					has_hexcode = true;
				}

				if(current == '0')
				{
					has_zero_start = true;
				}

				if(has_hexcode && has_zero_start)
				{
					scroll = advance(_lexer);
					const char *hex_chars = "0123456789aAbBcCdDeEfF";
				//printf("%c\n", scroll);
					if(!strchr(hex_chars, scroll) || scroll == 0)
					{
						add_token_start_word(_lexer, MY_token_type_invalid, start);

					}
					else
					{
						has_character_string(_lexer, (char *)hex_chars);

						add_token_start_word(_lexer, MY_token_type_hexdecimal, start);
					}
				}
				else if(has_hexcode && !has_zero_start)
				{
					add_token_start_word(_lexer, MY_token_type_invalid, start);
				}
				else if(scroll == '.')
				{
					//floating point
					advance(_lexer);
					has_character_ctype(_lexer, isdigit);
					add_token_start_word(_lexer, MY_token_type_float, start);
				}
				else
				{
					//integer
					bool has_found_dot = false;
					while(isdigit((unsigned char)scroll) && scroll != '\0' && scroll != '\n')
					{
				 		scroll = advance(_lexer);
						if(scroll == '.')
							has_found_dot = true;
					}
					if(has_found_dot == true)
					{
						advance(_lexer);
						has_character_ctype(_lexer, isdigit);
						add_token_start_word(_lexer, MY_token_type_float, start);

					}
					else
					{
						add_token_start_word(_lexer, MY_token_type_int, start);
					}
				}
			}
			else if(current == '.')
			{
				add_token_start_word(_lexer, MY_token_type_period, start);
			}
			else if(current == '(')
			{
				add_token_start_word(_lexer, MY_token_type_parenthesis_open, start);
			}
			else if(current == ')')
			{
				add_token_start_word(_lexer, MY_token_type_parenthesis_closed, start);
			}
			else if(current == '[')
			{
				add_token_start_word(_lexer, MY_token_type_squarebrace_open, start);
			}
			else if(current == ']')
			{
				add_token_start_word(_lexer, MY_token_type_squarebrace_closed, start);

			}
			else if(current == '{')
			{
				add_token_start_word(_lexer, MY_token_type_curlybrace_open, start);
			}
			else if(current == '}')
			{
				add_token_start_word(_lexer, MY_token_type_curlybrace_closed, start);
			}
			else if(current == '+')
			{
				if(scroll == '=')
				{
					advance(_lexer);
					add_token_start_word(_lexer, MY_token_type_add_equals, start);
				}
				else if(scroll == '+')
				{
					advance(_lexer);
					add_token_start_word(_lexer, MY_token_type_add_add, start);
				}
				else
				{
					add_token_start_word(_lexer, MY_token_type_add, start);
				}
			}
			else if(current == '-')
			{
				if(scroll == '=')
				{
					advance(_lexer);
					add_token_start_word(_lexer, MY_token_type_minus_equals, start);
				}
				else if(scroll == '-')
				{
					advance(_lexer);
					add_token_start_word(_lexer, MY_token_type_minus_minus, start);
				}
				else
				{
					add_token_start_word(_lexer, MY_token_type_minus, start);
				}
			}
			else if(current == '/')
			{
				if(scroll == '/')
				{
					advance(_lexer);
					add_token_start_word(_lexer, MY_token_type_slash_slash, start);
				}
				else if(scroll == '=')
				{
					advance(_lexer);
					add_token_start_word(_lexer, MY_token_type_slash_equals, start);
				}
				else if(scroll == ':')
				{
					//printf("found this\n");
					//_lexer->print_each = true;
					advance(_lexer);
					while(scroll != '\0' && scroll != '\n' )
					{
						scroll = advance(_lexer);
					}
				//_lexer->print_each = false;

				}
				else
				{
					add_token_start_word(_lexer, MY_token_type_slash, start);
				}
			}
			else if(current == '*')
			{
				if(scroll == '*')
				{
					advance(_lexer);
					add_token_start_word(_lexer, MY_token_type_star_star, start);
				}
				else if(scroll == '=')
				{
					advance(_lexer);
					add_token_start_word(_lexer, MY_token_type_star_equals, start);
				}
				else
				{
					add_token_start_word(_lexer, MY_token_type_star, start);
				}
			}
			else if(current == ',')
			{
				add_token_start_word(_lexer, MY_token_type_comma, start);

			}
			else if(current == ':')
			{
				add_token_start_word(_lexer, MY_token_type_colon, start);
			}
			else if(current == ';')
			{
				add_token_start_word(_lexer, MY_token_type_semicolon, start);
			}
			else if(current == '@')
			{
				add_token_start_word(_lexer, MY_token_type_at, start);
			}
			else if(current == '#')
			{
				add_token_start_word(_lexer, MY_token_type_hashtag, start);
			}
			else if(current == '=')
			{
				if(scroll == '=')
				{
					advance(_lexer);
					add_token_start_word(_lexer, MY_token_type_equals_equals, start);
				}
				else if(scroll == '>')
				{
					advance(_lexer);
					add_token_start_word(_lexer, MY_token_type_equals_less_than, start);
				}
				else
				{
					add_token_start_word(_lexer, MY_token_type_equals, start);
				}
			}
			else if(current == '<')
			{
				if(scroll == '=')
				{
					scroll = advance(_lexer);
					add_token_start_word(_lexer, MY_token_type_less_than_equals, start);
				}
				else if(scroll == '<')
				{
					scroll = advance(_lexer);
					if(scroll == '=')
					{
						advance(_lexer);
						add_token_start_word(_lexer, MY_token_type_less_than_less_than_equals, start);
					}
					else
						add_token_start_word(_lexer, MY_token_type_less_than_less_than, start);
				}
				else
					add_token_start_word(_lexer, MY_token_type_less_than, start);
			}
			else if(current == '>')
			{
				if(scroll == '=')
				{
					advance(_lexer);
					add_token_start_word(_lexer, MY_token_type_greater_than_equals, start);
				}
				else if(scroll == '>')
				{
					scroll = advance(_lexer);
					if(scroll == '=')
					{
						advance(_lexer);
						add_token_start_word(_lexer, MY_token_type_greater_than_greater_than_equals, start);
					}
					else
						add_token_start_word(_lexer, MY_token_type_greater_than_greater_than, start);
				}
				else
					add_token_start_word(_lexer, MY_token_type_greater_than, start);
			}
			else if(current == '&')
			{
				if(scroll == '&')
				{
					advance(_lexer);
					add_token_start_word(_lexer, MY_token_type_ampersand_ampersand, start);
				}
				else if(scroll == '=')
				{
					advance(_lexer);
					add_token_start_word(_lexer, MY_token_type_ampersand_equals, start);
				}
				else
				{
					add_token_start_word(_lexer, MY_token_type_ampersand, start);
				}
			}
			else if(current == '|')
			{
				if(scroll == '|')
				{
					advance(_lexer);
					add_token_start_word(_lexer, MY_token_type_vertical_line_vertical_line, start);
				}
				else if(scroll == '=')
				{
					advance(_lexer);
					add_token_start_word(_lexer, MY_token_type_vertical_line_equals, start);
				}
				else
				{
					add_token_start_word(_lexer, MY_token_type_vertical_line, start);
				}
			}
			else if(current == '%')
			{
				if(scroll == '=')
				{
					advance(_lexer);
					add_token_start_word(_lexer, MY_token_type_modulo_equals, start);
				}
				else
				{
					add_token_start_word(_lexer, MY_token_type_modulo, start);
				}
			}
			else if(current == '^')
			{
				if(scroll == '=')
				{
					advance(_lexer);
					add_token_start_word(_lexer, MY_token_type_hat_equals, start);
				}
				else
				{
					add_token_start_word(_lexer, MY_token_type_hat, start);

				}
			}
			else if(current == '?')
			{
				add_token_start_word(_lexer, MY_token_type_question_mark, start);
			}
			else
			{
				add_token_start_word(_lexer, MY_token_type_invalid, start);

			}
			space = false;
		}

		//advance(lexer);

	}

}

void MY_free_Lexer(MY_Lexer_t *_lexer)
{
	if(!_lexer)
	{
		return;
	}
	free(_lexer->ap__string);
	free(_lexer->ap__token_buffer);
	free(_lexer);
}




//don't fucking free this without the lexer
//don't use this without the lexer,
//fuck don't do this
MY_Token_group_t MY_init_Token_group(MY_Lexer_t *_lexer, int _start, int _length)
{
	MY_Token_group_t token_group;
	token_group.cp__lexer = _lexer;
	token_group.start = _start;
	token_group.length = _length;

	return token_group;

}


//probably fucked so idk
MY_token_type_t MY_get_token_type_Token_group(MY_Token_group_t _group, int _index)
{
	if(_group.length >= _index || _group.cp__lexer->token_count >= _index + _group.start || index < 0)
	{
		return MY_token_type_invalid;
	}

	return MY_get_token_type_Lexer(_group.cp__lexer, _index);

}

int MY_get_token_string_Token_group(MY_Token_group_t _group, int _index, char *_buffer, int _size)
{
	int together = _group.start + _index;
	if(together > _group.start || together + _group.length >= _group.cp__lexer->token_count)
	{
		MY_FAULT("fucked");
		return -1;	
	}
	if(_index < 0)
	{
		MY_FAULT("fucked");
		return -1;
	}

	if(!_buffer || _size < 0)
	{
		MY_FAULT("buffer cannot fit string");
	}	



	return MY_get_token_string_Lexer(_group.cp__lexer, together, _buffer, _size);
}
