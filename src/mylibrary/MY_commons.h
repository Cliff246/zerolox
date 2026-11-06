#ifndef __MY_COMMONS_HEADER__
#define __MY_COMMONS_HEADER__

#include <stdint.h>
#include <stdio.h>


typedef struct MY_List
{
	uint32_t count;
	uint64_t alloc;
	void **pap__array;
}MY_List_t;

int32_t MY_get_filename_start(char *path);

void MY_todo(char *file, int32_t line);
void MY_fault(char *message, char *file, int32_t line);

#define MY_TODO { MY_todo(__FILE__, __LINE__); }
#define MY_FAULT(X) { MY_fault(X, __FILE__, __LINE__); } 


#define MY_SET_BIT(n, k) (n | (1 << (k - 1)))
#define MY_CLR_BIT(n, k) (n & (~(1 << (k - 1))))
#define MY_GET_BIT(n, k) ((n >> k) & 1)
#define MY_ARRAY_SIZE(ary) sizeof(ary)/sizeof(*ary)

uint64_t MY_cap_to_bytes(int64_t data, uint8_t bytes);
int32_t MY_sign_extend32(uint32_t imm, uint8_t bits);



void *MY_realloc(void *ptr, uint64_t grow, uint64_t size);
int64_t MY_split_string(char *input, char ***buffer, char delim);
void MY_free_string_array(char **strings, uint64_t count);

//cringe file reader before MY_File is built

int64_t MY_get_length__STD_File(FILE *file);


char *MY_read_file(char *path, uint64_t *size);


#endif
