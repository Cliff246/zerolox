#include "EMU_Memory.h"
#include "EMU_Machine.h"
#include <stdlib.h>
#include <MY_commons.h>
#include <string.h>



ZLX_EMU_Memory_t *ZLX_EMU_init_Memory(uint64_t _size)
{
	ZLX_EMU_Memory_t *memory = malloc(sizeof(ZLX_EMU_Memory_t));
	if(!memory)
		MY_FAULT("malloc failed");


	uint8_t *bytes = malloc(_size);
	if(!bytes)
		MY_FAULT("malloc failed");
	
	memory->ap__memory = bytes;
	memory->size = _size;
	return memory;
}



void ZLX_EMU_set_Memory(ZLX_EMU_Memory_t *_memory, char *_buffer, uint64_t _size)
{
	if(!_memory || !_buffer) 
		MY_FAULT("bad arguments")
	
	if(_memory->size < _size)
		MY_FAULT("file is too big for memory");
		
	
	memcpy(_memory->ap__memory, _buffer, _size);
	


}



uint8_t ZLX_EMU_read_8_Memory(ZLX_EMU_Memory_t *memory, uint32_t address)
{

	uint32_t size = memory->size;
	if(address >= size  || size - address < 1)
	{
		MY_FAULT("cannoy read memory unallocated")
	}

	uint8_t data = memory->ap__memory[address];
   	return data;	
}

uint16_t ZLX_EMU_read_16_Memory(ZLX_EMU_Memory_t *memory, uint32_t address)
{
	uint32_t size = memory->size;
	if(address >= size || size - address < 2)
	{
		MY_FAULT("cannoy read memory unallocated")
	}
	uint8_t byte1 = memory->ap__memory[address];
	uint8_t byte2 = memory->ap__memory[address + 1];
	uint16_t data = (((uint16_t) byte2) << 8) + byte1; 
	return data;
}
uint32_t ZLX_EMU_read_32_Memory(ZLX_EMU_Memory_t *memory, uint32_t address)
{

	uint32_t size = memory->size;
	if(address >= size || size - address < 4)

	{
		MY_FAULT("cannot read memory unallocated");	
	}
	uint8_t byte1 = memory->ap__memory[address];
	uint8_t byte2 = memory->ap__memory[address + 1];
	uint16_t short1 = (((uint16_t) byte2) << 8) + byte1; 

	uint8_t byte3 = memory->ap__memory[address + 2];
	uint8_t byte4 = memory->ap__memory[address + 3];
	uint16_t short2 = (((uint16_t) byte4) << 8) + byte3; 

	uint32_t data = ((uint32_t) short2 << 16) + short1;

	return data;
}

uint32_t ZLX_EMU_read_24_Memory(ZLX_EMU_Memory_t *memory, uint32_t address)
{
	
	uint32_t size = memory->size;
	if(address >= size || size - address < 3)
	{

		MY_FAULT("cannot read memory unallocated");	
	}

	uint8_t byte1 = memory->ap__memory[address];
	uint8_t byte2 = memory->ap__memory[address + 1];

	uint8_t byte3 = memory->ap__memory[address + 2];



	uint32_t data = ((uint32_t)byte1) | 
					((uint32_t)byte2 << 8) | 
					((uint32_t)byte3 << 16);	
	return data;

}

void ZLX_EMU_write_8_Memory(ZLX_EMU_Memory_t *memory, uint32_t address, uint8_t value)
{

	uint32_t size = memory->size;
	if(address >= size || size - address < 1 )
	{

		MY_FAULT("cannot write memory unallocated");	
	}

	memory->ap__memory[address ] = value;
	
}



void ZLX_EMU_write_16_Memory(ZLX_EMU_Memory_t *memory, uint32_t address, uint16_t value)
{
	uint32_t size = memory->size;

	if(address >= size || size - address < 2 )
	{

		MY_FAULT("cannot write memory unallocated");	
	}

	memory->ap__memory[address ] = (uint8_t)(value & 0xff);
	memory->ap__memory[address + 1] = (uint8_t)(value >> 8);
}


void ZLX_EMU_write_32_Memory(ZLX_EMU_Memory_t *memory, uint32_t address, uint32_t value)
{

	uint32_t size = memory->size;
	if(address >= size || size - address < 4 )
	{

		MY_FAULT("cannot write memory unallocated");	
	}

	memory->ap__memory[address ] = (uint8_t)(value & 0xff);
	memory->ap__memory[address + 1] = (uint8_t)(value >> 8);
	memory->ap__memory[address + 2] = (uint8_t)((value >> 16) & 0xff);
	memory->ap__memory[address + 3] = (uint8_t)((value >> 24) & 0xff);
}


void ZLX_EMU_write_24_Memory(ZLX_EMU_Memory_t *memory, uint32_t address, uint32_t value)
{
	
	uint32_t size = memory->size;

	if(address >= size || size - address < 3)
	{
		MY_FAULT("cannot write memory unallocated");	
	}

	memory->ap__memory[address ] = value;
	memory->ap__memory[address + 1] = (uint8_t)(value >> 8);
	memory->ap__memory[address + 2] = (uint8_t)((value >> 16) & 0xff);
}


