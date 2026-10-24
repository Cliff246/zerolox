#ifndef ZLX_EMU_MEMORY_HEADER
#define ZLX_EMU_MEMORY_HEADER

#include <stdint.h>

typedef struct ZLX_EMU_Machine ZLX_EMU_Machine_t;


typedef struct ZLX_EMU_Memory
{
	uint8_t *ap__memory;

	uint64_t size;
	
}ZLX_EMU_Memory_t;

ZLX_EMU_Memory_t *ZLX_EMU_init_Memory(uint64_t _size);

void ZLX_EMU_set_Memory(ZLX_EMU_Memory_t *_memory, char *_buffer, uint64_t _size);

void ZLX_EMU_free_Memory(ZLX_EMU_Memory_t *_memory);

uint8_t ZLX_EMU_read_8_Memory(ZLX_EMU_Memory_t *memory, uint32_t address);
uint16_t ZLX_EMU_read_16_Memory(ZLX_EMU_Memory_t *memory, uint32_t address);
uint32_t ZLX_EMU_read_32_Memory(ZLX_EMU_Memory_t *memory, uint32_t address);
uint32_t ZLX_EMU_read_24_Memory(ZLX_EMU_Memory_t *memory, uint32_t address);

void ZLX_EMU_write_8_Memory(ZLX_EMU_Memory_t *memory, uint32_t address, uint8_t value);
void ZLX_EMU_write_16_Memory(ZLX_EMU_Memory_t *memory, uint32_t address, uint16_t value);
void ZLX_EMU_write_32_Memory(ZLX_EMU_Memory_t *memory, uint32_t address, uint32_t value);
void ZLX_EMU_write_24_Memory(ZLX_EMU_Memory_t *memory, uint32_t address, uint32_t value);




#endif
