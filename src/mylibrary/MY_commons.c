#include "MY_commons.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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


void *MY_realloc(void *ptr, uint64_t grow, uint64_t size)
{
	if(size == 0 || grow == 0)
	{
		MY_FAULT("bad arguments for realloc");
	}
	void *new = NULL;
	if(ptr == NULL)
	{
		new = malloc(grow * size);
		if(!new)
			MY_FAULT("malloc failed");
		return ptr;
	}
	else
	{

		void *copy = realloc(ptr, grow * size);
		if(!copy)
		{
			MY_FAULT("failed to realloc");
			return ptr;
		}

		return copy;
	}


}


int64_t MY_split_string(char *input, char ***buffer, char delim)
{

	int64_t count = 0; 
	int64_t alloc = 16;	
	char **splits = MY_realloc(NULL, alloc, sizeof(char **));
	char *last = input;
	//i need to make an alloc or list thing, fuck
	for(char *scroll = input; *scroll != 0; scroll++)
	{
		if(*scroll == delim)
		{
			int32_t size = scroll - last;
			char *sub = malloc(size + 1);
			if(sub == NULL)
				MY_FAULT("malloc failed");
			memcpy(sub, last, size);
			sub[size] = 0; 	
			if(count == alloc)
				splits = MY_realloc(splits, alloc = alloc * 2 + 1, sizeof(char **));	
			splits[count++] = sub;
		}
	}
	
	*buffer = splits; 
	return count;
}


void MY_free_string_array(char **strings, uint64_t count)
{
	for(int i = 0; i < count; ++i)
	{
		if(strings[i] != NULL)
		{
			free(strings[i]);
		}
	}
	free(strings);
}


int64_t MY_get_length__STD_File(FILE *file)
{
	if(!file)
	   return -1;	
	int64_t original = ftell(file);
	fseek(file, 0l, SEEK_END);
	int64_t size = ftell(file);
	fseek(file, original, SEEK_SET);
	return size;
}



//
char *MY_read_file(char *path, uint64_t *size)
{
	if(!path || !size)
	{
		MY_FAULT("gave a bad input");
	}


	FILE *fp = fopen(path, "rb");
	if(!fp)
		MY_FAULT("file could not be open");
	
	int64_t fsize = MY_get_length__STD_File(fp);
	if(fsize < 0)
		MY_FAULT("file could not get length");

	char *string = malloc(fsize + 1);
	if(!string)
		MY_FAULT("malloc failed");
	string[fsize] = 0;
	if(fread(string, 1, fsize, fp) != fsize)
	{
		MY_FAULT("fread failed");
	}
	*size = fsize;
	fclose(fp);
	return string;
}

