#include "MY_commons.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int32_t MY_get_filename_start(char *path)
{
	
	int32_t last_slash = 0;
	for(char *scroll = path; *scroll != 0; ++scroll)
	{
		if(*scroll == '\\' || *scroll == '/')
		{
			last_slash = scroll - path;
		}
	}
	if(path[last_slash + 1] != 0)
	{
		last_slash++;
	}
	return last_slash;
}

void MY_todo(char *file, int32_t line)
{

	fprintf(stdout, "TODO[%s:%d]\n", file + MY_get_filename_start(file), line);
	exit(EXIT_FAILURE);
}

void MY_fault(char *message, char *file, int32_t line)
{
	
	fprintf(stdout, "FAULT[%s:%d] %s \n", file + MY_get_filename_start(file), line, message);

	exit(EXIT_FAILURE);
}



uint64_t MY_cap_to_bytes(int64_t data, uint8_t bytes)
{
	if(bytes == 0 || bytes > 8)
		return 0;	
	uint64_t mask = UINT64_MAX >> (64 - bytes * 8);
	return data & mask;	

}




int32_t MY_sign_extend32(uint32_t imm, uint8_t bits)
{
	uint32_t sign = 1u << (bits - 1);
   	return (int32_t)((imm ^ sign) - sign);	
}
