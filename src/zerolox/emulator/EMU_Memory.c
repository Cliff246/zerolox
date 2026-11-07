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


void ZLX_EMU_free_Memory(ZLX_EMU_Memory_t *_memory)
{
	free(_memory->ap__memory);
	free(_memory);
}


uint8_t ZLX_EMU_read_8_Memory(ZLX_EMU_Memory_t *_memory, uint32_t _address)
{

	uint32_t size = _memory->size;
	if(_address >= size  || size - _address < 1)
	{
		MY_FAULT("cannoy read memory unallocated")
	}

	uint8_t data = _memory->ap__memory[_address];
   	return data;	
}

uint16_t ZLX_EMU_read_16_Memory(ZLX_EMU_Memory_t *_memory, uint32_t _address)
{
	uint32_t size = _memory->size;
	if(_address >= size || size - _address < 2)
	{
		MY_FAULT("cannoy read memory unallocated")
	}
	uint8_t byte1 = _memory->ap__memory[_address];
	uint8_t byte2 = _memory->ap__memory[_address + 1];
	uint16_t data = (((uint16_t) byte2) << 8) + byte1; 
	return data;
}
uint32_t ZLX_EMU_read_32_Memory(ZLX_EMU_Memory_t *_memory, uint32_t _address)
{

	uint32_t size = _memory->size;
	if(_address >= size || size - _address < 4)

	{
		MY_FAULT("cannot read memory unallocated");	
	}
	uint8_t byte1 = _memory->ap__memory[_address];
	uint8_t byte2 = _memory->ap__memory[_address + 1];
	uint16_t short1 = (((uint16_t) byte2) << 8) + byte1; 

	uint8_t byte3 = _memory->ap__memory[_address + 2];
	uint8_t byte4 = _memory->ap__memory[_address + 3];
	uint16_t short2 = (((uint16_t) byte4) << 8) + byte3; 

	uint32_t data = ((uint32_t) short2 << 16) + short1;

	return data;
}

uint32_t ZLX_EMU_read_24_Memory(ZLX_EMU_Memory_t *_memory, uint32_t _address)
{
	
	uint32_t size = _memory->size;
	if(_address >= size || size - _address < 3)
	{

		MY_FAULT("cannot read memory unallocated");	
	}

	uint8_t byte1 = _memory->ap__memory[_address];
	uint8_t byte2 = _memory->ap__memory[_address + 1];

	uint8_t byte3 = _memory->ap__memory[_address + 2];



	uint32_t data = ((uint32_t)byte1) | 
					((uint32_t)byte2 << 8) | 
					((uint32_t)byte3 << 16);	
	return data;

}

void ZLX_EMU_write_8_Memory(ZLX_EMU_Memory_t *_memory, uint32_t _address, uint8_t _value)
{

	uint32_t size = _memory->size;
	if(_address >= size || size - _address < 1 )
	{

		MY_FAULT("cannot write memory unallocated");	
	}

	_memory->ap__memory[_address ] = _value;
	
}



void ZLX_EMU_write_16_Memory(ZLX_EMU_Memory_t *_memory, uint32_t _address, uint16_t _value)
{
	uint32_t size = _memory->size;

	if(_address >= size || size - _address < 2 )
	{

		MY_FAULT("cannot write memory unallocated");	
	}

	_memory->ap__memory[_address ] = (uint8_t)(_value & 0xff);
	_memory->ap__memory[_address + 1] = (uint8_t)(_value >> 8);
}


void ZLX_EMU_write_32_Memory(ZLX_EMU_Memory_t *_memory, uint32_t _address, uint32_t _value)
{

	uint32_t size = _memory->size;
	if(_address >= size || size - _address < 4 )
	{

		MY_FAULT("cannot write memory unallocated");	
	}

	_memory->ap__memory[_address ] = (uint8_t)(_value & 0xff);
	_memory->ap__memory[_address + 1] = (uint8_t)(_value >> 8);
	_memory->ap__memory[_address + 2] = (uint8_t)((_value >> 16) & 0xff);
	_memory->ap__memory[_address + 3] = (uint8_t)((_value >> 24) & 0xff);
}


void ZLX_EMU_write_24_Memory(ZLX_EMU_Memory_t *_memory, uint32_t _address, uint32_t _value)
{
	
	uint32_t size = _memory->size;

	if(_address >= size || size - _address < 3)
	{
		MY_FAULT("cannot write memory unallocated");	
	}

	_memory->ap__memory[_address ] = _value;
	_memory->ap__memory[_address + 1] = (uint8_t)(_value >> 8);
	_memory->ap__memory[_address + 2] = (uint8_t)((_value >> 16) & 0xff);
}


