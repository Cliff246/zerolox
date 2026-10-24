#ifndef __MY_COMMONS_HEADER__
#define __MY_COMMONS_HEADER__

#include <stdint.h>
#include <stdio.h>
#include <sys/syslimits.h>


int32_t MY_get_filename_start(char *path);

void MY_todo(char *file, int32_t line);
void MY_fault(char *message, char *file, int32_t line);

#define MY_TODO { MY_todo(__FILE__, __LINE__); }
#define MY_FAULT(X) { MY_fault(X, __FILE__, __LINE__); } 


uint64_t MY_cap_to_bytes(int64_t data, uint8_t bytes);
int32_t MY_sign_extend32(uint32_t imm, uint8_t bits);

//cringe file reader before MY_File is built

char *MY_read_file(char *path, uint64_t *size);


#endif
