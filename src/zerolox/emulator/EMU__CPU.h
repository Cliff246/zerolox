#ifndef ZLX_EMU_CPU_HEADER
#define ZLX_EMU_CPU_HEADER

#include <stdint.h>
#include <stdbool.h>

#include "isa/ISA_zerolox.h"

typedef struct ZLX_EMU_Machine ZLX_EMU_Machine_t; 




typedef struct ZLX_EMU__CPU_parameters
{
	uint8_t source_1;
	uint8_t source_2;
	uint8_t source_3;
	uint8_t source_4;

	//these can be mutally exclusive
	uint8_t imm1_x;
	uint8_t imm1_y;
	

	//selection options
	uint8_t imm2;	
	uint8_t imm3;
	uint8_t imm5;
	uint8_t imm6;


	//
	uint8_t imm8;
	uint16_t imm9;
	uint16_t imm10;
	uint16_t imm11;
	
	uint16_t imm16;


	uint32_t imm32;

}ZLX_EMU__CPU_parameters_t;

//this is genuinely so stupid
typedef struct ZLX_EMU__CPU_output
{


	bool use_destination;
	bool use_destination_1;
	bool use_destination_2;

	uint8_t destination_1;
	uint8_t destination_2;

	uint32_t destination_1_data;	
	uint32_t destination_2_data;	

	
	bool pc_changed;
	bool pc_set_or_add;

	int32_t pc_value;

	
	
	bool memory_operation;
	bool memory_load;
	bool memory_store;

	uint32_t memory_address;
	uint32_t memory_value;



}ZLX_EMU__CPU_output_t;;



typedef ZLX_EMU__CPU_output_t (*ZLX_EMU__CPU_operation_t)(ZLX_EMU_Machine_t *machine, ZLX_EMU__CPU_parameters_t paramaters);

typedef struct ZLX_EMU__CPU
{
	uint32_t a__registers[32];
	uint32_t program_counter;



	

}ZLX_EMU__CPU_t;






ZLX_EMU__CPU_output_t ZLX_EMU_minor_execution(ZLX_EMU_Machine_t *machine, ZLX_ISA_Minor_t minor);
ZLX_EMU__CPU_output_t ZLX_EMU_major_execution(ZLX_EMU_Machine_t *machine, ZLX_ISA_Major_t major);
//this is all in that for a reason



void ZLX_EMU_decode_instruction(ZLX_EMU_Machine_t *machine);

void ZLX_EMU_step(ZLX_EMU_Machine_t *machine);

uint32_t ZLX_EMU_get_pc__CPU(ZLX_EMU__CPU_t *cpu);
uint32_t ZLX_EMU_get_register__CPU(ZLX_EMU__CPU_t *cpu, uint8_t id);
void ZLX_EMU_set_regiser__CPU(ZLX_EMU__CPU_t *cpu, uint8_t id, uint32_t value);

#endif
