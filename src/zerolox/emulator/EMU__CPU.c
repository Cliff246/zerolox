#include "EMU__CPU.h"
#include <MY_commons.h>
#include <stdlib.h>
#include "EMU_Machine.h"
#include "EMU_Memory.h"
#include "isa/ISA_zerolox.h"




typedef ZLX_EMU__CPU_output_t (*ZLX_EMU__CPU_operation_t)(ZLX_EMU_Machine_t *machine, ZLX_EMU__CPU_parameters_t paramaters);


#define OPERATION(name) static ZLX_EMU__CPU_output_t name(ZLX_EMU_Machine_t *machine, ZLX_EMU__CPU_parameters_t paramaters) 



#define OPERATION_SETUP \
	ZLX_EMU__CPU_t *cpu = machine->p__cpu;\
	ZLX_EMU__CPU_output_t output = {0};\
	ZLX_EMU_Memory_t *memory = machine->p__memory;\



enum {return_pointer_slot_bytes = 4};

static uint32_t imm2_size_to_bytes(uint8_t imm2)
{
	imm2 &= 0x3;

	uint32_t size = 0;
	switch(imm2)
	{
		// size = 8
		case 0:
			size = 1;
			break;
		// size = 16
		case 1:	
			size = 2;
			break;
		// size = 32
		case 2: 
			size = 4;
			break;
		default:
			MY_TODO;
			break;
	}	
	return size;

}	


//this will be capped to the max size of the type
static uint64_t load_variable_size_data(ZLX_EMU_Memory_t *memory, uint32_t address, uint8_t size)
{
	uint64_t data = 0;	
	if(size == 0 || size > 4)
		MY_FAULT("size must be x >= 1 && x <= 4");
	switch(size)
	{
		case 1:
			data = ZLX_EMU_read_8_Memory(memory, address);
			break;
		case 2: 
			data = ZLX_EMU_read_16_Memory(memory, address);
			break;
		case 4:
			data = ZLX_EMU_read_32_Memory(memory, address);
			break;

		default:
			MY_FAULT("invalid set of  size");
			break;
	}
	uint64_t cap = MY_cap_to_bytes(data, size);
	return cap;
}

//this will be capped to the max size of the type
static void store_variable_size_data(ZLX_EMU_Memory_t *memory, uint32_t address, uint64_t data, uint8_t size)
{
	if(size == 0 || size > 4)
		MY_FAULT("size must be x >= 1 && x <= 4");
	switch(size)
	{
		case 1:
			ZLX_EMU_write_8_Memory(memory, address, data);
			break;
		case 2: 
			ZLX_EMU_write_16_Memory(memory, address, data);
			break;
		case 4:
			ZLX_EMU_write_32_Memory(memory, address, data);
			break;

		default:
			MY_FAULT("invalid set of  size");
			break;
	}
}





//this takes the 2 bit size form 
//ie 
//0 = 8
//1 = 16
//2 = 32
//3 is reserved
//
static int32_t imm3_size_solution(uint8_t immediate, uint8_t size)
{
	size &= 0x3;
	immediate &= 0x7;
	
	//basically imm3 * size_in_bytes 
	uint32_t size_in_bytes = imm2_size_to_bytes(size);	

	uint32_t output = immediate * size_in_bytes;	
	
	return output;
}

//m.mov d1 s1
//d1 = s1
OPERATION(minor_move)
{
	OPERATION_SETUP;
	
	uint32_t data_1 = ZLX_EMU_get_register__CPU(cpu, paramaters.source_2);
	
		
	output.use_destination = true;
	output.use_destination_1 = true;;
	output.destination_1 = paramaters.source_1; 
	output.destination_1_data = data_1;

	return output;
}


//m.add
//ACC = r1 + r2
OPERATION(minor_add)
{
	OPERATION_SETUP;


	uint32_t data_1 = ZLX_EMU_get_register__CPU(cpu, paramaters.source_1);

	uint32_t data_2 = ZLX_EMU_get_register__CPU(cpu, paramaters.source_2);

	output.use_destination = true;
	output.use_destination_1 = true;

	output.destination_1 = ZLX_ISA_register_identifier_accumulator;
   	output.destination_1_data =	data_1 + data_2; 


	return output;	
}


//m.adda r1, r2 
//r1 = ACC + r2
OPERATION(minor_adda)
{
	OPERATION_SETUP;

	uint32_t data_1 = ZLX_EMU_get_register__CPU(cpu, paramaters.source_2);

	uint32_t data_2 = ZLX_EMU_get_register__CPU(cpu, ZLX_ISA_register_identifier_accumulator);

	output.use_destination = true;
	output.use_destination_1 = true;

	output.destination_1 = paramaters.source_1;
	output.destination_1_data = data_1 + data_2;
	
	return output;

}


//m.lda.s 
//acc = mem_s[r1 + imm3 * s]
OPERATION(minor_load_accumulator)
{
	
	OPERATION_SETUP;

	uint32_t data_1 = ZLX_EMU_get_register__CPU(cpu, paramaters.source_1);
	
	uint32_t size = paramaters.imm2;

	//yes this is wasting compute
	uint32_t size_in_bytes = imm2_size_to_bytes(size);
	
	uint32_t offset = paramaters.imm3; 

	uint32_t temp = imm3_size_solution(offset, size);	
	

	
	uint32_t address = data_1 + temp;

	output.memory_operation = true;
	output.memory_load = true;
	output.memory_address = address;
	
	uint64_t load = load_variable_size_data(memory, address, size_in_bytes);

	output.memory_value = load;

	output.use_destination = true;
	output.use_destination_1 = true;
	output.destination_1 = ZLX_ISA_register_identifier_accumulator;
	output.destination_1_data = load;


	return output;
}
	
//m.sta.s r1
//mem_s[r1 + imm3 * s] = acc
OPERATION(minor_store_accumulator)
{
	OPERATION_SETUP;

	uint32_t data_1 = ZLX_EMU_get_register__CPU(cpu, paramaters.source_1);
	uint32_t data_2 = ZLX_EMU_get_register__CPU(cpu, ZLX_ISA_register_identifier_accumulator);
	
	uint32_t size = paramaters.imm2;

	//yes this is wasting compute
	uint32_t size_in_bytes = imm2_size_to_bytes(size);
	
	uint32_t offset = paramaters.imm3; 

	uint32_t temp = imm3_size_solution(offset, size);	
	

	
	uint32_t address = data_1 + temp;

	output.memory_operation = true;
	output.memory_store = true;
	output.memory_address = address;
	output.memory_value = data_2;

	
	store_variable_size_data(memory, address, data_2 ,size_in_bytes);




	return output;

}

//m.ald.s 
//r1 = mem_s[acc + imm3 * s]
OPERATION(minor_accumulator_load)
{
	
	OPERATION_SETUP;

	uint32_t data_1 = ZLX_EMU_get_register__CPU(cpu, ZLX_ISA_register_identifier_accumulator);
	
	uint32_t size = paramaters.imm2;

	//yes this is wasting compute
	uint32_t size_in_bytes = imm2_size_to_bytes(size);
	
	uint32_t offset = paramaters.imm3; 

	uint32_t temp = imm3_size_solution(offset, size);	
	

	
	uint32_t address = data_1 + temp;

	output.memory_operation = true;
	output.memory_load = true;
	output.memory_address = address;
	
	uint64_t load = load_variable_size_data(memory, address, size_in_bytes);

	output.memory_value = load;

	output.use_destination = true;
	output.use_destination_1 = true;
	output.destination_1 = paramaters.source_1; 
	output.destination_1_data = load;


	return output;
}


//m.ast.s r1
//mem_s[acc - imm3 * s] = r1
OPERATION(minor_accumulator_store)
{
	OPERATION_SETUP;

	uint32_t data_1 = ZLX_EMU_get_register__CPU(cpu, paramaters.source_1);
	uint32_t data_2 = ZLX_EMU_get_register__CPU(cpu, ZLX_ISA_register_identifier_accumulator);
	
	uint32_t size = paramaters.imm2;

	//yes this is wasting compute
	uint32_t size_in_bytes = imm2_size_to_bytes(size);
	
	uint32_t offset = paramaters.imm3; 

	uint32_t temp = imm3_size_solution(offset, size);	
	
	uint32_t address = data_2 + temp;

	output.memory_operation = true;
	output.memory_store = true;
	output.memory_address = address;
	output.memory_value = data_1;

	store_variable_size_data(memory, address, data_1 ,size_in_bytes);

	return output;


}
//m.ldsa.s imm8
//acc = mem_s[sp - imm8] 
OPERATION(minor_load_stack_accumulator)
{	
	OPERATION_SETUP;

	uint32_t data_1 = ZLX_EMU_get_register__CPU(cpu, ZLX_ISA_register_identifier_stack_pointer);
	
	uint32_t size = paramaters.imm2;
	uint8_t size_in_bytes = imm2_size_to_bytes(size);

	uint8_t offset = paramaters.imm8;	
	
	uint32_t address = data_1 + offset;

	output.memory_operation = true;
	output.memory_load = true;
	output.memory_address = address;
	
	uint64_t load = load_variable_size_data(memory, address, size_in_bytes);

	output.memory_value = load;

	output.use_destination = true;
	output.use_destination_1 = true;
	output.destination_1 = ZLX_ISA_register_identifier_accumulator; 
	output.destination_1_data = load;


	return output;


}

//m.stsa.s imm8
//mem_size[sp - imm8] = acc
OPERATION(minor_store_stack_accumulator)
{
	OPERATION_SETUP;

	uint32_t data_1 = ZLX_EMU_get_register__CPU(cpu, ZLX_ISA_register_identifier_stack_pointer);
	uint32_t data_2 = ZLX_EMU_get_register__CPU(cpu, ZLX_ISA_register_identifier_accumulator);
	
	uint32_t size = paramaters.imm2;

	//yes this is wasting compute
	uint32_t size_in_bytes = imm2_size_to_bytes(size);
	
	uint32_t offset = paramaters.imm8; 

	
	uint32_t address = data_1 + offset;

	output.memory_operation = true;
	output.memory_store = true;
	output.memory_address = address;
	output.memory_value = data_2;

	store_variable_size_data(memory, address, data_2 ,size_in_bytes);

	return output;	
}

OPERATION(minor_geta)
{
	OPERATION_SETUP;

	int32_t imm10 = MY_sign_extend32(paramaters.imm10, 10); 

	output.use_destination = true;
	output.use_destination_1 = true;

	output.destination_1 = ZLX_ISA_register_identifier_accumulator;
   	output.destination_1_data =	imm10; 

	return output;	
}

//m.b imm10
//pc = signed(imm10) < 1
OPERATION(minor_branch)
{
	OPERATION_SETUP;

	int32_t pc = MY_sign_extend32(paramaters.imm10 << 1, 11); 
	//set it to add 
	output.pc_set_or_add = true;	
	output.pc_changed = true;
	output.pc_value = pc;
	return output;
}


OPERATION(minor_branch_accumulator_equals_zero)
{
	OPERATION_SETUP;

	uint32_t data_1 = ZLX_EMU_get_register__CPU(cpu, ZLX_ISA_register_identifier_accumulator);

	int32_t pc = MY_sign_extend32(paramaters.imm10 << 1, 11);
	if(data_1 == 0)
	{
		output.pc_set_or_add = true;	
		output.pc_changed = true;
		output.pc_value = pc;
	
	}
	return output;
}

OPERATION(minor_branch_accumulator_not_equals_zero)
{
	OPERATION_SETUP;

	uint32_t data_1 = ZLX_EMU_get_register__CPU(cpu, ZLX_ISA_register_identifier_accumulator);

	int32_t pc = MY_sign_extend32(paramaters.imm10 << 1, 11);
	if(data_1 != 0)
	{
		output.pc_set_or_add = true;	
		output.pc_changed = true;
		output.pc_value = pc;
	
	}
	return output;
}
//this is fine, it's unsigned for char's
//m.get8 imm8
//t[0-3] = imm8
OPERATION(minor_get8)
{
	OPERATION_SETUP;

	uint8_t imm8 = paramaters.imm8;
	if(paramaters.imm2 > 3)
		MY_FAULT("fails");
	int register_id = paramaters.imm2 + 8;
	
	output.use_destination = true;
	output.use_destination_1 = true;
	output.destination_1 = register_id;
	output.destination_1_data = imm8;
	return output;

}

//m.sub r1, r2
//acc = r1 - r2
OPERATION(minor_sub)
{
	OPERATION_SETUP;

	uint32_t data_1 = ZLX_EMU_get_register__CPU(cpu, paramaters.source_1); 
	uint32_t data_2 = ZLX_EMU_get_register__CPU(cpu, paramaters.source_2); 

	output.use_destination = true;
	output.use_destination_1 = true;
	output.destination_1 = ZLX_ISA_register_identifier_accumulator;
	output.destination_1_data = (data_1) - data_2;
	return output;


}

OPERATION(minor_load_plus)
{
	OPERATION_SETUP; 

	uint32_t data_1 = ZLX_EMU_get_register__CPU(cpu, paramaters.source_1);
	
	uint32_t size = paramaters.imm2;

	uint32_t size_in_bytes = imm2_size_to_bytes(size);

	int32_t offset = MY_sign_extend32(paramaters.imm3, 3);
	if(paramaters.imm3 == 0)
		offset = 4;
	else if(paramaters.imm3 == 5)
		offset = 8;
	output.memory_operation = true;
	output.memory_load = true;
	output.memory_address = data_1;
	
	uint32_t load = load_variable_size_data(memory, data_1, size_in_bytes);


	output.memory_value = load;

	output.use_destination = true;
	output.use_destination_1 = true;
	output.use_destination_2 = true;

	uint32_t iterator_change = data_1 +  offset * size_in_bytes; 

	output.destination_1 = paramaters.source_1; 
	output.destination_1_data = iterator_change;
	
	output.destination_2 = ZLX_ISA_register_identifier_accumulator;
	output.destination_2_data = load;

	return output;	

}

OPERATION(minor_store_plus)
{
	OPERATION_SETUP; 

	uint32_t data_1 = ZLX_EMU_get_register__CPU(cpu, paramaters.source_1);

	uint32_t data_2 = ZLX_EMU_get_register__CPU(cpu, ZLX_ISA_register_identifier_accumulator);
	
	uint32_t size = paramaters.imm2;

	uint32_t size_in_bytes = imm2_size_to_bytes(size);
	

	int32_t offset = MY_sign_extend32(paramaters.imm3, 3);
	if(paramaters.imm3 == 0)
		offset = 4;
	else if(paramaters.imm3 == 5)
		offset = 8;

	output.memory_operation = true;
	output.memory_store = true;
	output.memory_address = data_1;
	
	output.memory_value = data_2;
	store_variable_size_data(memory, data_1, data_2, size_in_bytes);



	output.use_destination = true;
	output.use_destination_1 = true;

	uint32_t iterator_change = data_1 +  offset * size_in_bytes; 

	output.destination_1 = paramaters.source_1; 
	output.destination_1_data = iterator_change;
	
	return output;	
}

//this one is bad, it really should have the option to increment or decrement... cringe
//beqi
OPERATION(minor_branch_compare_iterator)
{	
	OPERATION_SETUP;

	bool operation_type = paramaters.imm2 & 0x1;
	bool pull_type = paramaters.imm2 & 0x2;
	uint32_t iterator;
	uint32_t counter;	
	if(pull_type == false)
	{	
		iterator = ZLX_EMU_get_register__CPU(cpu, ZLX_ISA_register_identifier_iterator_x);
		counter = ZLX_EMU_get_register__CPU(cpu, ZLX_ISA_register_identifier_counter_x);
	}
	else
	{
		iterator = ZLX_EMU_get_register__CPU(cpu, ZLX_ISA_register_identifier_iterator_y);
		counter = ZLX_EMU_get_register__CPU(cpu, ZLX_ISA_register_identifier_counter_y);

	}
	//maybe
	int32_t pc = MY_sign_extend32(paramaters.imm8 << 1, 9);

	if(operation_type == false)
	{
		if(iterator == counter)
		{
			output.pc_set_or_add = true;	
			output.pc_changed = true;
			output.pc_value = pc;
		
		}
	}
	else
	{
		if(iterator != counter)
		{
			output.pc_set_or_add = true;	
			output.pc_changed = true;
			output.pc_value = pc;
		}
	}
	return output;
}


OPERATION(minor_branch_temporary_equal_accumulator)
{

	OPERATION_SETUP;
	uint32_t acc = ZLX_EMU_get_register__CPU(cpu, ZLX_ISA_register_identifier_accumulator);
	if(paramaters.imm2 > 3)
		MY_FAULT("fails");
	int register_id = paramaters.imm2 + 8;
	uint32_t tmp = ZLX_EMU_get_register__CPU(cpu, register_id);

	uint32_t pc = MY_sign_extend32(paramaters.imm8 << 1, 9);;
	
	if(tmp == acc)
	{
		output.pc_set_or_add = true;	
		output.pc_changed = true;
		output.pc_value = pc;
	
	}

	return output;
}
OPERATION(minor_branch_temporary_not_equal_accumulator)
{
	OPERATION_SETUP;
	uint32_t acc = ZLX_EMU_get_register__CPU(cpu, ZLX_ISA_register_identifier_accumulator);
	if(paramaters.imm2 > 3)
		MY_FAULT("fails");
	int register_id = paramaters.imm2 + 8;
	uint32_t tmp = ZLX_EMU_get_register__CPU(cpu, register_id);

	uint32_t pc = MY_sign_extend32(paramaters.imm8 << 1, 9);;
	
	if(tmp != acc)
	{
		output.pc_set_or_add = true;	
		output.pc_changed = true;
		output.pc_value = pc;
	
	}

	return output;
}
// Stage writes here. The common writeback checks destination conflicts.
static void stage_register(ZLX_EMU__CPU_output_t *output, uint8_t reg, uint32_t value)
{
	output->use_destination = true;
	output->use_destination_1 = true;
	output->destination_1 = reg;
	output->destination_1_data = value;
}

static void stage_iterator_branch(ZLX_EMU__CPU_t *cpu, ZLX_EMU__CPU_parameters_t p,
	ZLX_EMU__CPU_output_t *output, bool branch_if_equal)
{
	bool y = p.imm2 & 2;
	uint8_t iter = y ? ZLX_ISA_register_identifier_iterator_y : ZLX_ISA_register_identifier_iterator_x;
	uint8_t counter = y ? ZLX_ISA_register_identifier_counter_y : ZLX_ISA_register_identifier_counter_x;
	uint32_t old = ZLX_EMU_get_register__CPU(cpu, iter);
	bool equal = old == ZLX_EMU_get_register__CPU(cpu, counter);

	stage_register(output, iter, (p.imm2 & 1) ? old - 1 : old + 1);
	output->pc_changed = equal == branch_if_equal;
	output->pc_set_or_add = true;
	output->pc_value = 2 * MY_sign_extend32(p.imm8, 8);
}

OPERATION(minor_bieqc_update)
{
	OPERATION_SETUP;
	stage_iterator_branch(cpu, paramaters, &output, true);
	return output;
}

OPERATION(minor_bineqc_update)
{
	OPERATION_SETUP;
	stage_iterator_branch(cpu, paramaters, &output, false);
	return output;
}

OPERATION(minor_blink10)
{
	OPERATION_SETUP;
	stage_register(&output, ZLX_ISA_register_identifier_return_pointer, ZLX_EMU_get_pc__CPU(cpu) + 2);
	output.pc_changed = true;
	output.pc_set_or_add = true;
	output.pc_value = 2 * MY_sign_extend32(paramaters.imm10, 10);
	return output;
}

OPERATION(minor_bcall10)
{
    OPERATION_SETUP;
    uint32_t sp = ZLX_EMU_get_register__CPU(cpu, ZLX_ISA_register_identifier_stack_pointer) - return_pointer_slot_bytes;
    uint32_t old_rp = ZLX_EMU_get_register__CPU(cpu, ZLX_ISA_register_identifier_return_pointer);
    store_variable_size_data(memory, sp, old_rp, return_pointer_slot_bytes);
    output.memory_operation = true;
    output.memory_store = true;
    output.memory_address = sp;
    output.memory_value = old_rp;
    stage_register(&output, ZLX_ISA_register_identifier_stack_pointer, sp);
    output.use_destination_2 = true;
    output.destination_2 = ZLX_ISA_register_identifier_return_pointer;
    output.destination_2_data = ZLX_EMU_get_pc__CPU(cpu) + 2;
    output.pc_changed = true;
    output.pc_set_or_add = true;
    output.pc_value = 2 * MY_sign_extend32(paramaters.imm10, 10);
    return output;
}

static const ZLX_EMU__CPU_operation_t minor_operations[ZLX_ISA_minor_operation_count] =
{
	[ZLX_ISA_minor_operation_blink10] = minor_blink10,
	[ZLX_ISA_minor_operation_bcall10] = minor_bcall10,
	[ZLX_ISA_minor_operation_bieqc_update] = minor_bieqc_update,
	[ZLX_ISA_minor_operation_bineqc_update] = minor_bineqc_update,
	[ZLX_ISA_minor_operation_move] = minor_move,
	[ZLX_ISA_minor_operation_add] = minor_add,
	[ZLX_ISA_minor_operation_adda] = minor_adda,
	[ZLX_ISA_minor_operation_sub] = minor_sub,
	[ZLX_ISA_minor_operation_geta] = minor_geta, 
	[ZLX_ISA_minor_operation_get8] = minor_get8,
	[ZLX_ISA_minor_operation_b] = minor_branch, 
	[ZLX_ISA_minor_operation_baeqz] = minor_branch_accumulator_equals_zero,
	[ZLX_ISA_minor_operation_baneqz] = minor_branch_accumulator_not_equals_zero,
	[ZLX_ISA_minor_operation_lda] = minor_load_accumulator, 
	[ZLX_ISA_minor_operation_ald] = minor_accumulator_load,
	[ZLX_ISA_minor_operation_sta] = minor_store_accumulator,
	[ZLX_ISA_minor_operation_ast] = minor_accumulator_store,
	[ZLX_ISA_minor_operation_ldsa] = minor_load_stack_accumulator,
	[ZLX_ISA_minor_operation_stsa] = minor_store_stack_accumulator,
	[ZLX_ISA_minor_operation_ld_plus] = minor_load_plus,
	[ZLX_ISA_minor_operation_st_plus] = minor_store_plus,
	[ZLX_ISA_minor_operation_bieqc] = minor_branch_compare_iterator,
	[ZLX_ISA_minor_operation_btneqa] = minor_branch_temporary_not_equal_accumulator,
	[ZLX_ISA_minor_operation_bteqa] = minor_branch_temporary_equal_accumulator,
};

//
//--------------------------------------------------------------------------------
//


//ld.s rd [rs,imm9]
//rd = mem_size[rs + signed(imm9)]
OPERATION(major_load)
{
	OPERATION_SETUP; 
	//rs is in s1 and rd is s2	
	//[opcode][s1][1] [i9{0:4}][size][i9{5:7}[source2][i9{8}]

	uint32_t data_1 = ZLX_EMU_get_register__CPU(cpu, paramaters.source_1);
	
	uint32_t size = paramaters.imm2;

	uint32_t size_in_bytes = imm2_size_to_bytes(size);

	int32_t offset = MY_sign_extend32(paramaters.imm9, 9); 
	
	uint32_t address = data_1 + offset; 

	output.memory_operation = true;
	output.memory_load = true;
	output.memory_address = address;
	
	uint64_t load = load_variable_size_data(memory, address, size_in_bytes);

	output.memory_value = load;

	output.use_destination = true;
	output.use_destination_1 = true;
	output.destination_1 = paramaters.source_2; 
	output.destination_1_data = load;


	return output;

}

//st.s rs2 [rs1,imm9]
//mem_size[rs1 + signed(imm9)] = rs2
OPERATION(major_store)
{
	OPERATION_SETUP; 
	//rs1 is in s1 and rs2 is s2	
	//[opcode][s1][1] [i9{0:4}][size][i9{5:7}[source2][i9{8}]

	uint32_t data_1 = ZLX_EMU_get_register__CPU(cpu, paramaters.source_1);
	uint32_t data_2 = ZLX_EMU_get_register__CPU(cpu, paramaters.source_2);
	
	uint32_t size = paramaters.imm2;

	uint32_t size_in_bytes = imm2_size_to_bytes(size);

	int32_t offset = MY_sign_extend32(paramaters.imm9, 9); 
	
	uint32_t address = data_1 + offset; 

	output.memory_operation = true;
	output.memory_store = true;
	output.memory_address = address;
	output.memory_value = data_2;
	
	store_variable_size_data(memory, address, data_2, size_in_bytes);


	return output;

}
//push rs
//mem_8[--sp] = rs
OPERATION(major_push8)
{
	OPERATION_SETUP; 

	uint32_t data_1 = ZLX_EMU_get_register__CPU(cpu, paramaters.source_1);
	uint32_t data_2 = ZLX_EMU_get_register__CPU(cpu, ZLX_ISA_register_identifier_stack_pointer);
	
	uint32_t stack_change = data_2 - 1;

	output.memory_operation = true;
	output.memory_store = true;
	output.memory_address = stack_change;
	output.memory_value = data_1;
	
	store_variable_size_data(memory, stack_change, data_1, 1);

	output.use_destination = true;
	output.use_destination_1 = true;
	output.destination_1 = ZLX_ISA_register_identifier_stack_pointer; 
	output.destination_1_data = stack_change;

	return output;
}
OPERATION(major_push16)
{
	OPERATION_SETUP; 

	uint32_t data_1 = ZLX_EMU_get_register__CPU(cpu, paramaters.source_1);
	uint32_t data_2 = ZLX_EMU_get_register__CPU(cpu, ZLX_ISA_register_identifier_stack_pointer);
	
	uint32_t stack_change = data_2 - 2;

	output.memory_operation = true;
	output.memory_store = true;
	output.memory_address = stack_change;
	output.memory_value = data_1;
	
	store_variable_size_data(memory, stack_change, data_1, 2);

	output.use_destination = true;
	output.use_destination_1 = true;
	output.destination_1 = ZLX_ISA_register_identifier_stack_pointer; 
	output.destination_1_data = stack_change;

	return output;

}

OPERATION(major_push32)
{
	OPERATION_SETUP; 

	uint32_t data_1 = ZLX_EMU_get_register__CPU(cpu, paramaters.source_1);
	uint32_t data_2 = ZLX_EMU_get_register__CPU(cpu, ZLX_ISA_register_identifier_stack_pointer);
	
	uint32_t stack_change = data_2 - 4;

	output.memory_operation = true;
	output.memory_store = true;
	output.memory_address = stack_change;
	output.memory_value = data_1;
	
	store_variable_size_data(memory, stack_change, data_1, 4);

	output.use_destination = true;
	output.use_destination_1 = true;
	output.destination_1 = ZLX_ISA_register_identifier_stack_pointer; 
	output.destination_1_data = stack_change;

	return output;

}

OPERATION(major_pop8)
{
	OPERATION_SETUP; 

	uint32_t data_1 = ZLX_EMU_get_register__CPU(cpu, ZLX_ISA_register_identifier_stack_pointer);
	

	output.memory_operation = true;
	output.memory_load = true;
	output.memory_address = data_1;
	
	uint32_t load = load_variable_size_data(memory, data_1, 1);

	output.memory_value = load;


	output.use_destination = true;
	output.use_destination_1 = true;
	output.use_destination_2 = true;

	output.destination_1 = ZLX_ISA_register_identifier_stack_pointer; 
	output.destination_1_data = data_1 + 1;
	
	output.destination_2 = paramaters.source_1;
	output.destination_2_data = load;

	return output;
}

OPERATION(major_pop16)
{
	OPERATION_SETUP; 

	uint32_t data_1 = ZLX_EMU_get_register__CPU(cpu, ZLX_ISA_register_identifier_stack_pointer);
	

	output.memory_operation = true;
	output.memory_load = true;
	output.memory_address = data_1;
	
	uint32_t load = load_variable_size_data(memory, data_1, 2);

	output.memory_value = load;


	output.use_destination = true;
	output.use_destination_1 = true;
	output.use_destination_2 = true;

	output.destination_1 = ZLX_ISA_register_identifier_stack_pointer; 
	output.destination_1_data = data_1 + 2;
	
	output.destination_2 = paramaters.source_1;
	output.destination_2_data = load;


	return output;

}

OPERATION(major_pop32)
{
	OPERATION_SETUP; 

	uint32_t data_1 = ZLX_EMU_get_register__CPU(cpu, ZLX_ISA_register_identifier_stack_pointer);
	

	output.memory_operation = true;
	output.memory_load = true;
	output.memory_address = data_1;
	
	uint32_t load = load_variable_size_data(memory, data_1, 4);

	output.memory_value = load;


	output.use_destination = true;
	output.use_destination_1 = true;
	output.use_destination_2 = true;

	output.destination_1 = ZLX_ISA_register_identifier_stack_pointer; 
	output.destination_1_data = data_1 + 4;
	
	output.destination_2 = paramaters.source_1;
	output.destination_2_data = load;


	return output;

}


//addi rd rs1 imm
OPERATION(major_add_immediate)
{
	//[opcode][rs1][1] [i11{0:4][ill{5:9}][rs1][i11{10}]
	OPERATION_SETUP; 
	
	uint32_t data_1 = ZLX_EMU_get_register__CPU(cpu, paramaters.source_1);
	uint32_t change = MY_sign_extend32(paramaters.imm11, 11);


	output.use_destination = true;
	output.use_destination_1 = true;
	output.destination_1 = paramaters.source_2; 
	output.destination_1_data = data_1 + change;
	return output;
}

//r1 = 
OPERATION(major_get16)
{

	OPERATION_SETUP; 

	uint32_t set_to = paramaters.imm16;

	output.use_destination = true;
	output.use_destination_1 = true;
	output.destination_1 = paramaters.source_1; 
	output.destination_1_data = set_to;

	return output;
}

OPERATION(major_get32)
{
	OPERATION_SETUP; 
	uint32_t set_to = paramaters.imm32;
	output.use_destination = true;
	output.use_destination_1 = true;
	output.destination_1 = paramaters.source_1; 
	output.destination_1_data = set_to;

	return output;
}


typedef struct {
	uint32_t left;
	uint32_t right;
} ultra_operands_t;

static ultra_operands_t stage_ultra_move(ZLX_EMU__CPU_t *cpu,
	ZLX_EMU__CPU_parameters_t p, ZLX_EMU__CPU_output_t *output)
{
	const uint8_t ac = ZLX_ISA_register_identifier_accumulator;
	uint8_t src = p.imm1_x ? p.source_4 : ac;
	uint8_t dst = p.imm1_x ? ac : p.source_4;
	uint32_t moved = ZLX_EMU_get_register__CPU(cpu, src);
	stage_register(output, dst, moved);
	output->use_destination_2 = true;
	output->destination_2 = p.source_2;

	// The ALU sees the move first. Writes to z0 are discarded.
	return (ultra_operands_t){
		.left = dst && dst == p.source_1 ? moved : ZLX_EMU_get_register__CPU(cpu, p.source_1),
		.right = dst && dst == p.source_3 ? moved : ZLX_EMU_get_register__CPU(cpu, p.source_3),
	};
}

OPERATION(major_add)
{
	OPERATION_SETUP;
	ultra_operands_t v = stage_ultra_move(cpu, paramaters, &output);
	output.destination_2_data = v.left + v.right;
	return output;
}

OPERATION(major_sub)
{
	OPERATION_SETUP;
	ultra_operands_t v = stage_ultra_move(cpu, paramaters, &output);
	output.destination_2_data = v.left - v.right;
	return output;
}

OPERATION(major_sll)
{
	OPERATION_SETUP;
	ultra_operands_t v = stage_ultra_move(cpu, paramaters, &output);
	output.destination_2_data = v.left << (v.right & 31);
	return output;
}

OPERATION(major_srl)
{
	OPERATION_SETUP;
	ultra_operands_t v = stage_ultra_move(cpu, paramaters, &output);
	output.destination_2_data = v.left >> (v.right & 31);
	return output;
}

OPERATION(major_sra)
{
	OPERATION_SETUP;
	ultra_operands_t v = stage_ultra_move(cpu, paramaters, &output);
	output.destination_2_data = (int32_t)v.left >> (v.right & 31);
	return output;
}

OPERATION(major_xor)
{
	OPERATION_SETUP;
	ultra_operands_t v = stage_ultra_move(cpu, paramaters, &output);
	output.destination_2_data = v.left ^ v.right;
	return output;
}

OPERATION(major_or)
{
	OPERATION_SETUP;
	ultra_operands_t v = stage_ultra_move(cpu, paramaters, &output);
	output.destination_2_data = v.left | v.right;
	return output;
}

OPERATION(major_and)
{
	OPERATION_SETUP;
	ultra_operands_t v = stage_ultra_move(cpu, paramaters, &output);
	output.destination_2_data = v.left & v.right;
	return output;
}

OPERATION(major_andn)
{
	OPERATION_SETUP;
	ultra_operands_t v = stage_ultra_move(cpu, paramaters, &output);
	output.destination_2_data = v.left & ~v.right;
	return output;
}

OPERATION(major_clt)
{
	OPERATION_SETUP;
	ultra_operands_t v = stage_ultra_move(cpu, paramaters, &output);
	output.destination_2_data = (int32_t)v.left < (int32_t)v.right;
	return output;
}

OPERATION(major_cge)
{
	OPERATION_SETUP;
	ultra_operands_t v = stage_ultra_move(cpu, paramaters, &output);
	output.destination_2_data = (int32_t)v.left >= (int32_t)v.right;
	return output;
}

OPERATION(major_cltu)
{
	OPERATION_SETUP;
	ultra_operands_t v = stage_ultra_move(cpu, paramaters, &output);
	output.destination_2_data = v.left < v.right;
	return output;
}

OPERATION(major_cgeu)
{
	OPERATION_SETUP;
	ultra_operands_t v = stage_ultra_move(cpu, paramaters, &output);
	output.destination_2_data = v.left >= v.right;
	return output;
}

OPERATION(major_ceq)
{
	OPERATION_SETUP;
	ultra_operands_t v = stage_ultra_move(cpu, paramaters, &output);
	output.destination_2_data = v.left == v.right;
	return output;
}

OPERATION(major_cneq)
{
	OPERATION_SETUP;
	ultra_operands_t v = stage_ultra_move(cpu, paramaters, &output);
	output.destination_2_data = v.left != v.right;
	return output;
}

OPERATION(major_slli)
{
	OPERATION_SETUP;
	uint32_t value = ZLX_EMU_get_register__CPU(cpu, paramaters.source_1);
	stage_register(&output, paramaters.source_2, value << (paramaters.imm11 & 31));
	return output;
}

OPERATION(major_srli)
{
	OPERATION_SETUP;
	uint32_t value = ZLX_EMU_get_register__CPU(cpu, paramaters.source_1);
	stage_register(&output, paramaters.source_2, value >> (paramaters.imm11 & 31));
	return output;
}

OPERATION(major_srai)
{
	OPERATION_SETUP;
	uint32_t value = ZLX_EMU_get_register__CPU(cpu, paramaters.source_1);
	stage_register(&output, paramaters.source_2, (int32_t)value >> (paramaters.imm11 & 31));
	return output;
}

OPERATION(major_andi)
{
	OPERATION_SETUP;
	uint32_t value = ZLX_EMU_get_register__CPU(cpu, paramaters.source_1);
	stage_register(&output, paramaters.source_2, value & MY_sign_extend32(paramaters.imm11, 11));
	return output;
}

OPERATION(major_ori)
{
	OPERATION_SETUP;
	uint32_t value = ZLX_EMU_get_register__CPU(cpu, paramaters.source_1);
	stage_register(&output, paramaters.source_2, value | MY_sign_extend32(paramaters.imm11, 11));
	return output;
}

OPERATION(major_xori)
{
	OPERATION_SETUP;
	uint32_t value = ZLX_EMU_get_register__CPU(cpu, paramaters.source_1);
	stage_register(&output, paramaters.source_2, value ^ MY_sign_extend32(paramaters.imm11, 11));
	return output;
}

OPERATION(major_extb)
{
	OPERATION_SETUP;
	unsigned index = paramaters.imm11;
	if (index >= 32) MY_FAULT("extb index outside 0..31");
	uint32_t value = ZLX_EMU_get_register__CPU(cpu, paramaters.source_1);
	stage_register(&output, paramaters.source_2, (value >> index) & 1u);
	return output;
}

OPERATION(major_sext)
{
	OPERATION_SETUP;
	unsigned width = paramaters.imm11;
	if (width == 0 || width > 32) MY_FAULT("sext width outside 1..32");
	uint32_t value = ZLX_EMU_get_register__CPU(cpu, paramaters.source_1);
	if (width < 32)
		value = MY_sign_extend32(value & ((UINT32_C(1) << width) - 1), width);
	stage_register(&output, paramaters.source_2, value);
	return output;
}

OPERATION(major_anda)
{
	OPERATION_SETUP;
	uint32_t value = ZLX_EMU_get_register__CPU(cpu, paramaters.source_1);
	uint32_t ac = ZLX_EMU_get_register__CPU(cpu, ZLX_ISA_register_identifier_accumulator);
	stage_register(&output, ZLX_ISA_register_identifier_accumulator, value & ac);
	return output;
}

OPERATION(major_aand)
{
	OPERATION_SETUP;
	uint32_t value = ZLX_EMU_get_register__CPU(cpu, paramaters.source_1);
	uint32_t ac = ZLX_EMU_get_register__CPU(cpu, ZLX_ISA_register_identifier_accumulator);
	stage_register(&output, paramaters.source_1, value & ac);
	return output;
}

OPERATION(major_ora)
{
	OPERATION_SETUP;
	uint32_t value = ZLX_EMU_get_register__CPU(cpu, paramaters.source_1);
	uint32_t ac = ZLX_EMU_get_register__CPU(cpu, ZLX_ISA_register_identifier_accumulator);
	stage_register(&output, ZLX_ISA_register_identifier_accumulator, value | ac);
	return output;
}

OPERATION(major_aor)
{
	OPERATION_SETUP;
	uint32_t value = ZLX_EMU_get_register__CPU(cpu, paramaters.source_1);
	uint32_t ac = ZLX_EMU_get_register__CPU(cpu, ZLX_ISA_register_identifier_accumulator);
	stage_register(&output, paramaters.source_1, value | ac);
	return output;
}

OPERATION(major_xora)
{
	OPERATION_SETUP;
	uint32_t value = ZLX_EMU_get_register__CPU(cpu, paramaters.source_1);
	uint32_t ac = ZLX_EMU_get_register__CPU(cpu, ZLX_ISA_register_identifier_accumulator);
	stage_register(&output, ZLX_ISA_register_identifier_accumulator, value ^ ac);
	return output;
}

OPERATION(major_axor)
{
	OPERATION_SETUP;
	uint32_t value = ZLX_EMU_get_register__CPU(cpu, paramaters.source_1);
	uint32_t ac = ZLX_EMU_get_register__CPU(cpu, ZLX_ISA_register_identifier_accumulator);
	stage_register(&output, paramaters.source_1, value ^ ac);
	return output;
}

OPERATION(major_branch_equals)
{
	OPERATION_SETUP;


	uint32_t rs1 = ZLX_EMU_get_register__CPU(cpu, paramaters.source_1);	
	uint32_t rs2 = ZLX_EMU_get_register__CPU(cpu, paramaters.source_2);	

	uint32_t pc = MY_sign_extend32(paramaters.imm11 << 1, 12); 	


	if(rs1 == rs2)
	{
		output.pc_set_or_add = true;	
		output.pc_changed = true;
		output.pc_value = pc;

	}
	return output;
}


OPERATION(major_branch_not_equals)
{
	OPERATION_SETUP;
	uint32_t rs1 = ZLX_EMU_get_register__CPU(cpu, paramaters.source_1);	
	uint32_t rs2 = ZLX_EMU_get_register__CPU(cpu, paramaters.source_2);	

	uint32_t pc = MY_sign_extend32(paramaters.imm11 << 1, 12); 	


	if(rs1 != rs2)
	{
		output.pc_set_or_add = true;	
		output.pc_changed = true;
		output.pc_value = pc;

	}

	return output;
}

OPERATION(major_branch_less_than)
{
	OPERATION_SETUP;
	int32_t rs1 = ZLX_EMU_get_register__CPU(cpu, paramaters.source_1);	
	int32_t rs2 = ZLX_EMU_get_register__CPU(cpu, paramaters.source_2);	

	uint32_t pc = MY_sign_extend32(paramaters.imm11 << 1, 12); 	


	if(rs1 < rs2)
	{
		output.pc_set_or_add = true;	
		output.pc_changed = true;
		output.pc_value = pc;

	}

	return output;
}

OPERATION(major_branch_greater_than_equals)
{
	OPERATION_SETUP;
	int32_t rs1 = ZLX_EMU_get_register__CPU(cpu, paramaters.source_1);	
	int32_t rs2 = ZLX_EMU_get_register__CPU(cpu, paramaters.source_2);	

	uint32_t pc = MY_sign_extend32(paramaters.imm11 << 1, 12); 	


	if(rs1 >= rs2)
	{
		output.pc_set_or_add = true;	
		output.pc_changed = true;
		output.pc_value = pc;

	}

	return output;
}


OPERATION(major_branch_less_than_unsigned)
{
	OPERATION_SETUP;
	uint32_t rs1 = ZLX_EMU_get_register__CPU(cpu, paramaters.source_1);	
	uint32_t rs2 = ZLX_EMU_get_register__CPU(cpu, paramaters.source_2);	

	uint32_t pc = MY_sign_extend32(paramaters.imm11 << 1, 12); 	


	if(rs1 < rs2)
	{
		output.pc_set_or_add = true;	
		output.pc_changed = true;
		output.pc_value = pc;

	}

	return output;
}

OPERATION(major_branch_greater_than_equals_unsigned)
{
	OPERATION_SETUP;
	uint32_t rs1 = ZLX_EMU_get_register__CPU(cpu, paramaters.source_1);	
	uint32_t rs2 = ZLX_EMU_get_register__CPU(cpu, paramaters.source_2);	

	uint32_t pc = MY_sign_extend32(paramaters.imm11 << 1, 12); 	


	if(rs1 >= rs2)
	{
		output.pc_set_or_add = true;	
		output.pc_changed = true;
		output.pc_value = pc;

	}
	return output;

}

OPERATION(major_jump)
{
	OPERATION_SETUP;
	uint32_t target = ZLX_EMU_get_register__CPU(cpu, paramaters.source_1);
	if (target & 1)
		MY_FAULT("jump target must be parcel aligned");
	output.pc_set_or_add = false;
	output.pc_changed = true;
	output.pc_value = target;
	return output;
}

OPERATION(major_call)
{
	
	OPERATION_SETUP;

	uint32_t dest = ZLX_EMU_get_register__CPU(cpu, paramaters.source_1);
	if (dest & 1) MY_FAULT("jump target must be parcel aligned");
	uint32_t pc = ZLX_EMU_get_pc__CPU(cpu);	

	
	output.pc_set_or_add = false;	
	output.pc_changed = true;
	output.pc_value = dest;

	output.use_destination = true;
	output.use_destination_1 = true;
	output.destination_1 = ZLX_ISA_register_identifier_return_pointer ;
	output.destination_1_data = pc +  paramaters.instruction_size * 2;



	return output;
}

OPERATION(major_b21)
{
    OPERATION_SETUP;
    output.pc_changed = true;
    output.pc_set_or_add = true;
    output.pc_value = 2 * MY_sign_extend32(paramaters.imm32, 21);
    return output;
}

OPERATION(major_addpc)
{
    OPERATION_SETUP;
    uint32_t ac = ZLX_EMU_get_register__CPU(cpu, ZLX_ISA_register_identifier_accumulator);
    stage_register(&output, paramaters.source_1, ZLX_EMU_get_pc__CPU(cpu) + ac);
    return output;
}

OPERATION(major_blink21)
{
    OPERATION_SETUP;
    stage_register(&output, ZLX_ISA_register_identifier_return_pointer, ZLX_EMU_get_pc__CPU(cpu) + 4);
    output.pc_changed = true;
    output.pc_set_or_add = true;
    output.pc_value = 2 * MY_sign_extend32(paramaters.imm32, 21);
    return output;
}

OPERATION(major_bcall21)
{
    OPERATION_SETUP;
    uint32_t sp = ZLX_EMU_get_register__CPU(cpu, ZLX_ISA_register_identifier_stack_pointer) - return_pointer_slot_bytes;
    uint32_t old_rp = ZLX_EMU_get_register__CPU(cpu, ZLX_ISA_register_identifier_return_pointer);
    store_variable_size_data(memory, sp, old_rp, return_pointer_slot_bytes);
    output.memory_operation = true;
    output.memory_store = true;
    output.memory_address = sp;
    output.memory_value = old_rp;
    stage_register(&output, ZLX_ISA_register_identifier_stack_pointer, sp);
    output.use_destination_2 = true;
    output.destination_2 = ZLX_ISA_register_identifier_return_pointer;
    output.destination_2_data = ZLX_EMU_get_pc__CPU(cpu) + 4;
    output.pc_changed = true;
    output.pc_set_or_add = true;
    output.pc_value = 2 * MY_sign_extend32(paramaters.imm32, 21);
    return output;
}

OPERATION(major_callp)
{
	
	OPERATION_SETUP;

	uint32_t dest = ZLX_EMU_get_register__CPU(cpu, paramaters.source_1);
	if (dest & 1) MY_FAULT("jump target must be parcel aligned");
	uint32_t data_2 = ZLX_EMU_get_register__CPU(cpu, ZLX_ISA_register_identifier_stack_pointer);
	uint32_t data_1 = ZLX_EMU_get_register__CPU(cpu, ZLX_ISA_register_identifier_return_pointer);
	
	uint32_t pc = ZLX_EMU_get_pc__CPU(cpu);	


	output.pc_set_or_add = false;	
	output.pc_changed = true;
	output.pc_value = dest;

	output.use_destination = true;
	output.use_destination_2 = true;
	output.destination_2 = ZLX_ISA_register_identifier_return_pointer ;
	output.destination_2_data = pc +  paramaters.instruction_size * 2;

	
	uint32_t stack_change = data_2 - return_pointer_slot_bytes;

	output.memory_operation = true;
	output.memory_store = true;
	output.memory_address = stack_change;
	output.memory_value = data_1;
	
	store_variable_size_data(memory, stack_change, data_1, return_pointer_slot_bytes);

	output.use_destination = true;
	output.use_destination_1 = true;
	output.destination_1 = ZLX_ISA_register_identifier_stack_pointer; 
	output.destination_1_data = stack_change;

	return output;

}



OPERATION(major_returnp)
{
	OPERATION_SETUP;

	uint32_t rp = ZLX_EMU_get_register__CPU(cpu, ZLX_ISA_register_identifier_return_pointer);	
	
	output.pc_set_or_add = false;	
	output.pc_changed = true;
	output.pc_value = rp;

	uint32_t data_1 = ZLX_EMU_get_register__CPU(cpu, ZLX_ISA_register_identifier_stack_pointer);
	
	output.memory_operation = true;
	output.memory_load = true;
	output.memory_address = data_1;
	
	uint32_t load = load_variable_size_data(memory, data_1, return_pointer_slot_bytes);

	output.memory_value = load;

	output.use_destination = true;
	output.use_destination_1 = true;
	output.use_destination_2 = true;

	output.destination_1 = ZLX_ISA_register_identifier_stack_pointer; 
	output.destination_1_data = data_1 + return_pointer_slot_bytes;
	

	uint32_t data = ZLX_EMU_get_register__CPU(cpu, ZLX_ISA_register_identifier_accumulator);
	output.use_destination_3 = true;
	output.destination_3 = paramaters.source_1;
	output.destination_3_data = data;



	output.destination_2 = ZLX_ISA_register_identifier_return_pointer; 
	output.destination_2_data = load;

	return output;

	


}


static void ZLX_ISA_get_stack_triplet_sizes(uint8_t size_codes, uint8_t sizes[3])
{
	for (unsigned i = 0; i < 3; ++i)
		sizes[i] = imm2_size_to_bytes((size_codes >> (2 * i)) & 3);
}

OPERATION(ZLX_ISA_execute_push3)
{
	OPERATION_SETUP;
	uint8_t sizes[3];
	ZLX_ISA_get_stack_triplet_sizes(paramaters.imm6, sizes);
	const uint8_t registers[] = {paramaters.source_1, paramaters.source_2, paramaters.source_3};
	const uint8_t total = sizes[0] + sizes[1] + sizes[2];
	uint32_t sp = ZLX_EMU_get_register__CPU(cpu, ZLX_ISA_register_identifier_stack_pointer);
	if (sp < total || sp > memory->size)
		MY_FAULT("push3 stack range is outside memory");

	uint32_t value = 0;
	for (unsigned i = 0; i < 3; ++i)
	{
		value = ZLX_EMU_get_register__CPU(cpu, registers[i]);
		sp -= sizes[i];
		store_variable_size_data(memory, sp, value, sizes[i]);
	}
	stage_register(&output, ZLX_ISA_register_identifier_stack_pointer, sp);
	output.memory_operation = true;
	output.memory_store = true;
	output.memory_address = sp;
	output.memory_value = MY_cap_to_bytes(value, sizes[2]);
	return output;
}

OPERATION(ZLX_ISA_execute_pop3)
{
	OPERATION_SETUP;
	uint8_t sizes[3];
	ZLX_ISA_get_stack_triplet_sizes(paramaters.imm6, sizes);
	uint32_t sp = ZLX_EMU_get_register__CPU(cpu, ZLX_ISA_register_identifier_stack_pointer);
	const uint64_t next_sp = (uint64_t)sp + sizes[0] + sizes[1] + sizes[2];
	if (next_sp > memory->size || next_sp > UINT32_MAX)
		MY_FAULT("pop3 stack range is outside memory");

	uint32_t value_3 = load_variable_size_data(memory, sp, sizes[2]);
	uint32_t value_2 = load_variable_size_data(memory, sp + sizes[2], sizes[1]);
	uint32_t value_1 = load_variable_size_data(memory, sp + sizes[2] + sizes[1], sizes[0]);

	stage_register(&output, ZLX_ISA_register_identifier_stack_pointer, next_sp);
	output.use_destination_2 = true;
	output.destination_2 = paramaters.source_1;
	output.destination_2_data = value_1;
	output.use_destination_3 = true;
	output.destination_3 = paramaters.source_2;
	output.destination_3_data = value_2;
	output.use_destination_4 = true;
	output.destination_4 = paramaters.source_3;
	output.destination_4_data = value_3;
	output.memory_operation = true;
	output.memory_load = true;
	output.memory_address = sp;
	output.memory_value = value_3;
	return output;
}

OPERATION(ZLX_ISA_execute_retired_pointer_stack_operation)
{
	(void)machine;
	(void)paramaters;
	MY_TODO;
	return (ZLX_EMU__CPU_output_t){0};
}

static const ZLX_EMU__CPU_operation_t major_operations[ZLX_ISA_major_operation_count] =
{
	[ZLX_ISA_major_operation_inc] = major_add_immediate,
	[ZLX_ISA_major_operation_dec] = major_add_immediate,
	[ZLX_ISA_major_operation_shift_left] = major_slli,
	[ZLX_ISA_major_operation_shift_right] = major_srli,
	[ZLX_ISA_major_operation_shift_arith] = major_srai,
	[ZLX_ISA_major_operation_get_pos] = major_add_immediate,
	[ZLX_ISA_major_operation_get_neg] = major_add_immediate,
	[ZLX_ISA_major_operation_add] = major_add,
	[ZLX_ISA_major_operation_sub] = major_sub,
	[ZLX_ISA_major_operation_sll] = major_sll,
	[ZLX_ISA_major_operation_srl] = major_srl,
	[ZLX_ISA_major_operation_sra] = major_sra,
	[ZLX_ISA_major_operation_xor] = major_xor,
	[ZLX_ISA_major_operation_or] = major_or,
	[ZLX_ISA_major_operation_and] = major_and,
	[ZLX_ISA_major_operation_andn] = major_andn,
	[ZLX_ISA_major_operation_clt] = major_clt,
	[ZLX_ISA_major_operation_cge] = major_cge,
	[ZLX_ISA_major_operation_cltu] = major_cltu,
	[ZLX_ISA_major_operation_cgeu] = major_cgeu,
	[ZLX_ISA_major_operation_ceq] = major_ceq,
	[ZLX_ISA_major_operation_cneq] = major_cneq,
	[ZLX_ISA_major_operation_slli] = major_slli,
	[ZLX_ISA_major_operation_srli] = major_srli,
	[ZLX_ISA_major_operation_srai] = major_srai,
	[ZLX_ISA_major_operation_xori] = major_xori,
	[ZLX_ISA_major_operation_andi] = major_andi,
	[ZLX_ISA_major_operation_ori] = major_ori,
	[ZLX_ISA_major_operation_extb] = major_extb,
	[ZLX_ISA_major_operation_sext] = major_sext,
	[ZLX_ISA_major_operation_anda] = major_anda,
	[ZLX_ISA_major_operation_xora] = major_xora,
	[ZLX_ISA_major_operation_ora] = major_ora,
	[ZLX_ISA_major_operation_aand] = major_aand,
	[ZLX_ISA_major_operation_axor] = major_axor,
	[ZLX_ISA_major_operation_aor] = major_aor,
	[ZLX_ISA_major_operation_pop8] = major_pop8,
	[ZLX_ISA_major_operation_pop16] = major_pop16,
	[ZLX_ISA_major_operation_pop32] = major_pop32,
	[ZLX_ISA_major_operation_popp] = ZLX_ISA_execute_retired_pointer_stack_operation,
	[ZLX_ISA_major_operation_push8] = major_push8,
	[ZLX_ISA_major_operation_push16] = major_push16,
	[ZLX_ISA_major_operation_push32] = major_push32,
	[ZLX_ISA_major_operation_push3] = ZLX_ISA_execute_push3,
	[ZLX_ISA_major_operation_pop3] = ZLX_ISA_execute_pop3,
	[ZLX_ISA_major_operation_pushp] = ZLX_ISA_execute_retired_pointer_stack_operation,
	[ZLX_ISA_major_operation_addi] = major_add_immediate,


	[ZLX_ISA_major_operation_ld] = major_load,
	[ZLX_ISA_major_operation_st] = major_store,
	[ZLX_ISA_major_operation_beq] = major_branch_equals,
	[ZLX_ISA_major_operation_bneq] = major_branch_not_equals,
	[ZLX_ISA_major_operation_blt] = major_branch_less_than,
	[ZLX_ISA_major_operation_bge] = major_branch_greater_than_equals,
	[ZLX_ISA_major_operation_bltu] = major_branch_less_than_unsigned,
	[ZLX_ISA_major_operation_bgeu] = major_branch_greater_than_equals_unsigned,
	[ZLX_ISA_major_operation_j] = major_jump,
	[ZLX_ISA_major_operation_jlink] = major_call,
	[ZLX_ISA_major_operation_jcall] = major_callp,
		[ZLX_ISA_major_operation_jret] = major_returnp,
	[ZLX_ISA_major_operation_bcall21] = major_bcall21,
	[ZLX_ISA_major_operation_blink21] = major_blink21,
	[ZLX_ISA_major_operation_b21] = major_b21,
	[ZLX_ISA_major_operation_addpc] = major_addpc,
	[ZLX_ISA_major_operation_get16] = major_get16,
	[ZLX_ISA_major_operation_get32] = major_get32


};

void ZLX_EMU_solve__CPU(ZLX_EMU__CPU_t *cpu, ZLX_EMU__CPU_output_t output, int instruction_size)
{
	
	if(output.use_destination == true)
	{
		if(output.use_destination_1 && output.use_destination_2 &&
		   output.destination_1 == output.destination_2 && output.destination_1 != ZLX_ISA_register_identifier_zero)
			MY_FAULT("cannot have w1 == w2");
		if(output.use_destination_1 && output.use_destination_3 &&
		   output.destination_1 == output.destination_3 && output.destination_1 != ZLX_ISA_register_identifier_zero)
			MY_FAULT("cannot have w3 == w1");
		if(output.use_destination_2 && output.use_destination_3 &&
		   output.destination_2 == output.destination_3 && output.destination_2 != ZLX_ISA_register_identifier_zero)
			MY_FAULT("cannot have w3 == w2");
		if (output.use_destination_4 && output.destination_4 != ZLX_ISA_register_identifier_zero &&
		    ((output.use_destination_1 && output.destination_4 == output.destination_1) ||
		     (output.use_destination_2 && output.destination_4 == output.destination_2) ||
		     (output.use_destination_3 && output.destination_4 == output.destination_3)))
			MY_FAULT("cannot have overlapping w4");

		if(output.use_destination_1 == true)
		{
			ZLX_EMU_set_regiser__CPU(cpu, output.destination_1, output.destination_1_data);
		}

		if(output.use_destination_2 == true)
		{
			ZLX_EMU_set_regiser__CPU(cpu, output.destination_2, output.destination_2_data);
		}
		if(output.use_destination_3 == true)
		{
			ZLX_EMU_set_regiser__CPU(cpu, output.destination_3, output.destination_3_data);
		}
		if (output.use_destination_4)
			ZLX_EMU_set_regiser__CPU(cpu, output.destination_4, output.destination_4_data);
	}

	if(output.pc_changed == true)
	{
		if(output.pc_set_or_add == false)
		{
			cpu->program_counter = output.pc_value; 
		}
		else
		{
			cpu->program_counter += output.pc_value;
		}
	}
	else
	{
		cpu->program_counter += instruction_size * 2; 
	}

}

/*
 *	[ZLX_ISA_minor_operation_move] = minor_move,
	[ZLX_ISA_Minor_operation_swap] = minor_swap,
	[ZLX_ISA_minor_operation_add] = minor_add,
	[ZLX_ISA_minor_operation_adda] = minor_adda,
	[ZLX_ISA_minor_operation_sub] = minor_sub,
	[ZLX_ISA_Minor_operation_addi] = minor_addi,
	[ZLX_ISA_minor_operation_geta] = minor_geta, 
	[ZLX_ISA_minor_operation_get8] = minor_get8,
	[ZLX_ISA_minor_operation_b] = minor_branch, 
	[ZLX_ISA_minor_operation_baeqz] = minor_branch_accumulator_equals_zero,
	[ZLX_ISA_minor_operation_baneqz] = minor_branch_accumulator_not_equals_zero,
	[ZLX_ISA_minor_operation_lda] = minor_load_accumulator, 
	[ZLX_ISA_minor_operation_ald] = minor_accumulator_load,
	[ZLX_ISA_minor_operation_sta] = minor_store_accumulator,
	[ZLX_ISA_minor_operation_ast] = minor_accumulator_store,
	[ZLX_ISA_minor_operation_ldsa] = minor_load_stack_accumulator,
	[ZLX_ISA_minor_operation_stsa] = minor_store_stack_accumulator,

 */


//exact decoder postions to very
ZLX_EMU__CPU_output_t ZLX_EMU_major_execution(ZLX_EMU_Machine_t *machine, ZLX_ISA_Major_t major)
{
	ZLX_EMU__CPU_parameters_t parameters = {0};
	if ((unsigned)major.operation >= ZLX_ISA_major_operation_count ||
	    !major_operations[major.operation] ||
	    major.type != ZLX_ISA_Major_operation_formats[major.operation])
		MY_FAULT("invalid major opcode or operand format");

	parameters.instruction_size = ZLX_ISA_Major_operation_size[major.type];
	switch (major.type)
	{
		case ZLX_ISA_major_format_type_55:
		{
			/* Present compact forms to the existing immediate ALU handlers. */
			unsigned amount = major.u__format_55.a5 ? major.u__format_55.a5 : 32;
			parameters.source_2 = major.u__format_55.b5;
			parameters.source_1 = parameters.source_2;
			if (major.operation == ZLX_ISA_major_operation_get_pos ||
			    major.operation == ZLX_ISA_major_operation_get_neg)
				parameters.source_1 = ZLX_ISA_register_identifier_zero;
			if (major.operation == ZLX_ISA_major_operation_dec ||
			    major.operation == ZLX_ISA_major_operation_get_neg)
				amount = -amount;
			parameters.imm11 = amount & 0x7ff;
			break;
		}
		case ZLX_ISA_major_format_type_5:
			parameters.source_1 = major.u__foramt_5.a5;
			break;
		case ZLX_ISA_major_format_type_5_5551:
			parameters.source_1 = major.u__format_5_5551.a5;
			parameters.source_2 = major.u__format_5_5551.b5;
			parameters.source_3 = major.u__format_5_5551.c5;
			parameters.source_4 = major.u__format_5_5551.d5;
			parameters.imm1_x = major.u__format_5_5551.e1;
			break;
		case ZLX_ISA_major_format_type_5_655:
			parameters.source_1 = major.u__format_5_655.a5;
			parameters.imm6 = major.u__format_5_655.b6;
			parameters.source_2 = major.u__format_5_655.c5;
			parameters.source_3 = major.u__format_5_655.d5;
			break;
		case ZLX_ISA_major_format_type_5_952:
			parameters.source_1 = major.u__format_5_952.a5;
			parameters.source_2 = major.u__format_5_952.c5;
			parameters.imm2 = major.u__format_5_952.d2;
			parameters.imm9 = major.u__format_5_952.b9;
			break;
		case ZLX_ISA_major_format_type_5_b5:
			parameters.source_1 = major.u__format_5_b5.a5;
			parameters.source_2 = major.u__format_5_b5.c5;
			parameters.imm11 = major.u__format_5_b5.b11;
			break;
		case ZLX_ISA_major_format_type_5_f:
			parameters.source_1 = major.u__format_5_f.a5;
			parameters.imm16 = major.u__format_5_f.bf;
			parameters.imm32 = ((uint32_t)major.u__format_5_f.a5 << 16) | major.u__format_5_f.bf;
			break;
		case ZLX_ISA_major_format_type_5_f_f:
			parameters.source_1 = major.u__format_5_f_f.a5;
			parameters.imm32 = ((uint32_t)major.u__format_5_f_f.bf << 16) | major.u__format_5_f_f.cf;
			break;
		default:
			MY_FAULT("unsupported major operand format");
	}
	return major_operations[major.operation](machine, parameters);
}

ZLX_EMU__CPU_output_t ZLX_EMU_minor_execution(ZLX_EMU_Machine_t *machine, ZLX_ISA_Minor_t minor)
{
	ZLX_EMU__CPU_parameters_t parameters = {0};
	if ((unsigned)minor.operation >= ZLX_ISA_minor_operation_count ||
	    !minor_operations[minor.operation] ||
	    minor.type != ZLX_ISA_Minor_operation_formats[minor.operation])
		MY_FAULT("invalid minor opcode or operand format");

	switch (minor.type)
	{
		case ZLX_ISA_minor_format_type_55:
			parameters.imm5 = minor.u__format_55.a5;
			parameters.source_1 = minor.u__format_55.a5;
			parameters.source_2 = minor.u__format_55.b5;
			break;
		case ZLX_ISA_minor_format_type_235:
			parameters.imm2 = minor.u__format_235.a2;
			parameters.imm3 = minor.u__format_235.b3;
			parameters.source_1 = minor.u__format_235.c5;
			break;
		case ZLX_ISA_minor_format_type_28:
			parameters.imm2 = minor.u__format_28.a2;
			parameters.imm8 = minor.u__format_28.b8;
			break;
		case ZLX_ISA_minor_format_type_a:
			parameters.imm10 = minor.u__format_a.aa;
			break;
		default:
			MY_FAULT("unsupported minor operand format");
	}
	return minor_operations[minor.operation](machine, parameters);
}

ZLX_ISA_Instruction_t ZLX_EMU_decode_instruction(ZLX_EMU_Machine_t *machine)
{

	ZLX_EMU__CPU_t *cpu = machine->p__cpu;
	ZLX_ISA_Instruction_t instruction;
	ZLX_ISA_Parcel_t parcels[8];
	

	uint16_t first_parcel = ZLX_EMU_read_16_Memory(machine->p__memory, cpu->program_counter); 
	parcels[0].parcel = first_parcel;	
	int count = 0;
	uint32_t temporary_pc = cpu->program_counter + 2;

	if (ZLX_ISA_decode_instruction(&instruction, parcels, &count, 0))
		return instruction;
	for (int i = 1; i < count && i < 8; ++i)
	{
		parcels[i].parcel = ZLX_EMU_read_16_Memory(machine->p__memory, temporary_pc);
		temporary_pc += 2;
		if (ZLX_ISA_decode_instruction(&instruction, parcels, &count, i))
			return instruction;
	}
	MY_FAULT("could not decode instruction");
	return (ZLX_ISA_Instruction_t){0};

}

ZLX_EMU__CPU_t *ZLX_EMU_init__CPU(void)
{
	ZLX_EMU__CPU_t *cpu = calloc(1, sizeof(ZLX_EMU__CPU_t));
	if(!cpu)
		MY_FAULT("cpu failed to malloc");	
	cpu->program_counter = 0;
	for(int i = 0; i < 32; ++i)
	{
		cpu->a__registers[i] = 0;
	}
	return cpu;
}


void ZLX_EMU_free__CPU(ZLX_EMU__CPU_t *cpu)
{
	free(cpu);
}


uint32_t ZLX_EMU_get_pc__CPU(ZLX_EMU__CPU_t *cpu)
{
	return cpu->program_counter;
}


uint32_t ZLX_EMU_get_register__CPU(ZLX_EMU__CPU_t *cpu, uint8_t id)
{
	if(id >= 32)
	{
		MY_FAULT("id must be less than 32");
	}
	if(id == 0)
	{
		return 0;
	}
	else
	{
		uint32_t value = cpu->a__registers[id];
		return value;
	}
}



void ZLX_EMU_set_regiser__CPU(ZLX_EMU__CPU_t *cpu, uint8_t id, uint32_t value)
{
	if(id >= 32)
	{
		MY_FAULT("id must be less than 32");
	}
	if(id == 0)
	{
		cpu->a__registers[0] = 0;
	}
	else
	{
		cpu->a__registers[id] = value;	
	}
	
}


