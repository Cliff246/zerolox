#include "EMU__CPU.h"
#include <MY_commons.h>
#include "EMU_Machine.h"
#include "EMU_Memory.h"
#include "isa/ISA_zerolox.h"




typedef ZLX_EMU__CPU_output_t (*ZLX_EMU__CPU_operation_t)(ZLX_EMU_Machine_t *machine, ZLX_EMU__CPU_parameters_t paramaters);


#define OPERATION(name) static ZLX_EMU__CPU_output_t name(ZLX_EMU_Machine_t *machine, ZLX_EMU__CPU_parameters_t paramaters) 



#define OPERATION_SETUP \
	ZLX_EMU__CPU_t *cpu = machine->p__cpu;\
	ZLX_EMU__CPU_output_t output = {0};\
	ZLX_EMU_Memory_t *memory = machine->p__memory;\



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
		//size is variable 
		//this is size = p and size = 24 and size = 64
		//this is based off p size
		case 3:
			size = 3;
			break;
		default:
			MY_FAULT("there are 4 options for a imm2 get size");
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
			//load 24 
		case 3:
			data = ZLX_EMU_read_24_Memory(memory, address);
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
			ZLX_EMU_write_8_Memory(memory, data, address);
			break;
		case 2: 
			ZLX_EMU_write_16_Memory(memory, data, address);
			break;
			//load 24 
		case 3:
			ZLX_EMU_write_24_Memory(memory, data, address);
			break;
		case 4:
			ZLX_EMU_write_32_Memory(memory, data, address);
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
//3 = 24/64 in different ptr sizes
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

	output.destination_1 = ZLX_ISA_Register_identifier_accumulator;
   	output.destination_1_data =	data_1 + data_2; 


	return output;	
}


//m.adda r1, r2 
//r1 = ACC + r2
OPERATION(minor_adda)
{
	OPERATION_SETUP;

	uint32_t data_1 = ZLX_EMU_get_register__CPU(cpu, paramaters.source_2);

	uint32_t data_2 = ZLX_EMU_get_register__CPU(cpu, ZLX_ISA_Register_identifier_accumulator);

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
	output.destination_1 = ZLX_ISA_Register_identifier_accumulator;
	output.destination_1_data = load;


	return output;
}
	
//m.sta.s r1
//mem_s[r1 + imm3 * s] = acc
OPERATION(minor_store_accumulator)
{
	OPERATION_SETUP;

	uint32_t data_1 = ZLX_EMU_get_register__CPU(cpu, paramaters.source_1);
	uint32_t data_2 = ZLX_EMU_get_register__CPU(cpu, ZLX_ISA_Register_identifier_accumulator);
	
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

	uint32_t data_1 = ZLX_EMU_get_register__CPU(cpu, ZLX_ISA_Register_identifier_accumulator);
	
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
	uint32_t data_2 = ZLX_EMU_get_register__CPU(cpu, ZLX_ISA_Register_identifier_accumulator);
	
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

	uint32_t data_1 = ZLX_EMU_get_register__CPU(cpu, ZLX_ISA_Register_identifier_stack_pointer);
	
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
	output.destination_1 = ZLX_ISA_Register_identifier_accumulator; 
	output.destination_1_data = load;


	return output;


}

//m.stsa.s imm8
//mem_size[sp - imm8] = acc
OPERATION(minor_store_stack_accumulator)
{
	OPERATION_SETUP;

	uint32_t data_1 = ZLX_EMU_get_register__CPU(cpu, ZLX_ISA_Register_identifier_stack_pointer);
	uint32_t data_2 = ZLX_EMU_get_register__CPU(cpu, ZLX_ISA_Register_identifier_accumulator);
	
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

OPERATION(minor_swap)
{

	OPERATION_SETUP;
	uint32_t data_1 = ZLX_EMU_get_register__CPU(cpu, paramaters.source_1);
	uint32_t data_2 = ZLX_EMU_get_register__CPU(cpu, paramaters.source_2);


	output.use_destination = true;
	output.use_destination_1 = true;
	output.use_destination_2 = true;

	output.destination_1 = paramaters.source_1;
	output.destination_1_data = data_2;

	output.destination_2 = paramaters.source_2;
	output.destination_2_data = data_1;
	return output;
}

//m.addi r1, imm5
//acc = r1 + imm5
OPERATION(minor_addi)
{
	OPERATION_SETUP;


	uint32_t data_1 = ZLX_EMU_get_register__CPU(cpu, paramaters.source_1);

	uint32_t imm5 = paramaters.imm5; 

	output.use_destination = true;
	output.use_destination_1 = true;

	output.destination_1 = ZLX_ISA_Register_identifier_accumulator;
   	output.destination_1_data =	data_1 + imm5; 


	return output;	

}

OPERATION(minor_geta)
{
	OPERATION_SETUP;

	int32_t imm10 = MY_sign_extend32(paramaters.imm10, 10); 

	output.use_destination = true;
	output.use_destination_1 = true;

	output.destination_1 = ZLX_ISA_Register_identifier_accumulator;
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

	uint32_t data_1 = ZLX_EMU_get_register__CPU(cpu, ZLX_ISA_Register_identifier_accumulator);

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

	uint32_t data_1 = ZLX_EMU_get_register__CPU(cpu, ZLX_ISA_Register_identifier_accumulator);

	int32_t pc = MY_sign_extend32(paramaters.imm10 << 1, 11);
	if(data_1 != 0)
	{
		output.pc_set_or_add = true;	
		output.pc_changed = true;
		output.pc_value = pc;
	
	}
	return output;
}
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
	output.destination_1 = ZLX_ISA_Register_identifier_accumulator;
	output.destination_1_data = (data_2) - data_1;
	return output;


}

static ZLX_EMU__CPU_operation_t minor_operations[] = 
{
	[ZLX_ISA_Minor_operation_move] = minor_move,
	[ZLX_ISA_Minor_operation_swap] = minor_swap,
	[ZLX_ISA_Minor_operation_add] = minor_add,
	[ZLX_ISA_Minor_operation_adda] = minor_adda,
	[ZLX_ISA_Minor_operation_sub] = minor_sub,
	[ZLX_ISA_Minor_operation_addi] = minor_addi,
	[ZLX_ISA_Minor_operation_geta] = minor_geta, 
	[ZLX_ISA_Minor_operation_get8] = minor_get8,
	[ZLX_ISA_Minor_operation_b] = minor_branch, 
	[ZLX_ISA_Minor_operation_baeqz] = minor_branch_accumulator_equals_zero,
	[ZLX_ISA_Minor_operation_baneqz] = minor_branch_accumulator_not_equals_zero,
	[ZLX_ISA_Minor_operation_lda] = minor_load_accumulator, 
	[ZLX_ISA_Minor_operation_ald] = minor_accumulator_load,
	[ZLX_ISA_Minor_operation_sta] = minor_store_accumulator,
	[ZLX_ISA_Minor_operation_ast] = minor_accumulator_store,
	[ZLX_ISA_Minor_operation_ldsa] = minor_load_stack_accumulator,
	[ZLX_ISA_Minor_operation_stsa] = minor_store_stack_accumulator,

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
	uint32_t data_2 = ZLX_EMU_get_register__CPU(cpu, ZLX_ISA_Register_identifier_stack_pointer);
	
	uint32_t stack_change = data_2 - 1;

	output.memory_operation = true;
	output.memory_store = true;
	output.memory_address = stack_change;
	output.memory_value = data_1;
	
	store_variable_size_data(memory, stack_change, data_1, 1);

	output.use_destination = true;
	output.use_destination_1 = true;
	output.destination_1 = ZLX_ISA_Register_identifier_stack_pointer; 
	output.destination_1_data = stack_change;

	return output;
}
OPERATION(major_push16)
{
	OPERATION_SETUP; 

	uint32_t data_1 = ZLX_EMU_get_register__CPU(cpu, paramaters.source_1);
	uint32_t data_2 = ZLX_EMU_get_register__CPU(cpu, ZLX_ISA_Register_identifier_stack_pointer);
	
	uint32_t stack_change = data_2 - 2;

	output.memory_operation = true;
	output.memory_store = true;
	output.memory_address = stack_change;
	output.memory_value = data_1;
	
	store_variable_size_data(memory, stack_change, data_1, 2);

	output.use_destination = true;
	output.use_destination_1 = true;
	output.destination_1 = ZLX_ISA_Register_identifier_stack_pointer; 
	output.destination_1_data = stack_change;

	return output;

}

OPERATION(major_push32)
{
	OPERATION_SETUP; 

	uint32_t data_1 = ZLX_EMU_get_register__CPU(cpu, paramaters.source_1);
	uint32_t data_2 = ZLX_EMU_get_register__CPU(cpu, ZLX_ISA_Register_identifier_stack_pointer);
	
	uint32_t stack_change = data_2 - 4;

	output.memory_operation = true;
	output.memory_store = true;
	output.memory_address = stack_change;
	output.memory_value = data_1;
	
	store_variable_size_data(memory, stack_change, data_1, 4);

	output.use_destination = true;
	output.use_destination_1 = true;
	output.destination_1 = ZLX_ISA_Register_identifier_stack_pointer; 
	output.destination_1_data = stack_change;

	return output;

}

OPERATION(major_pushp)
{
	OPERATION_SETUP; 

	uint32_t data_1 = ZLX_EMU_get_register__CPU(cpu, paramaters.source_1);
	uint32_t data_2 = ZLX_EMU_get_register__CPU(cpu, ZLX_ISA_Register_identifier_stack_pointer);
	
	uint32_t stack_change = data_2 - 3;

	output.memory_operation = true;
	output.memory_store = true;
	output.memory_address = stack_change;
	output.memory_value = data_1;
	
	store_variable_size_data(memory, stack_change, data_1, 3);

	output.use_destination = true;
	output.use_destination_1 = true;
	output.destination_1 = ZLX_ISA_Register_identifier_stack_pointer; 
	output.destination_1_data = stack_change;

	return output;

}

OPERATION(major_pop8)
{
	OPERATION_SETUP; 

	uint32_t data_1 = ZLX_EMU_get_register__CPU(cpu, ZLX_ISA_Register_identifier_stack_pointer);
	

	output.memory_operation = true;
	output.memory_load = true;
	output.memory_address = data_1;
	
	uint32_t load = load_variable_size_data(memory, data_1, 1);

	output.memory_value = load;


	output.use_destination = true;
	output.use_destination_1 = true;
	output.use_destination_2 = true;

	output.destination_1 = ZLX_ISA_Register_identifier_stack_pointer; 
	output.destination_1_data = data_1 + 1;
	
	output.destination_2 = paramaters.source_1;
	output.destination_2_data = load;

	return output;
}

OPERATION(major_pop16)
{
	OPERATION_SETUP; 

	uint32_t data_1 = ZLX_EMU_get_register__CPU(cpu, ZLX_ISA_Register_identifier_stack_pointer);
	

	output.memory_operation = true;
	output.memory_load = true;
	output.memory_address = data_1;
	
	uint32_t load = load_variable_size_data(memory, data_1, 2);

	output.memory_value = load;


	output.use_destination = true;
	output.use_destination_1 = true;
	output.use_destination_2 = true;

	output.destination_1 = ZLX_ISA_Register_identifier_stack_pointer; 
	output.destination_1_data = data_1 + 2;
	
	output.destination_2 = paramaters.source_1;
	output.destination_2_data = load;


	return output;

}

OPERATION(major_pop32)
{
	OPERATION_SETUP; 

	uint32_t data_1 = ZLX_EMU_get_register__CPU(cpu, ZLX_ISA_Register_identifier_stack_pointer);
	

	output.memory_operation = true;
	output.memory_load = true;
	output.memory_address = data_1;
	
	uint32_t load = load_variable_size_data(memory, data_1, 4);

	output.memory_value = load;


	output.use_destination = true;
	output.use_destination_1 = true;
	output.use_destination_2 = true;

	output.destination_1 = ZLX_ISA_Register_identifier_stack_pointer; 
	output.destination_1_data = data_1 + 4;
	
	output.destination_2 = paramaters.source_1;
	output.destination_2_data = load;


	return output;

}

OPERATION(major_pop24)
{
	OPERATION_SETUP; 

	uint32_t data_1 = ZLX_EMU_get_register__CPU(cpu, ZLX_ISA_Register_identifier_stack_pointer);
	
	output.memory_operation = true;
	output.memory_load = true;
	output.memory_address = data_1;
	
	uint32_t load = load_variable_size_data(memory, data_1, 3);

	output.memory_value = load;

	output.use_destination = true;
	output.use_destination_1 = true;
	output.use_destination_2 = true;

	output.destination_1 = ZLX_ISA_Register_identifier_stack_pointer; 
	output.destination_1_data = data_1 + 3;
	
	output.destination_2 = paramaters.source_1;
	output.destination_2_data = load;

	return output;
}

//ldxa.s imm3
//acc=mem_s[ix]; ix+=imm3*s
OPERATION(major_load_iterator_x_accumualtor)
{

	OPERATION_SETUP; 

	uint32_t data_1 = ZLX_EMU_get_register__CPU(cpu, ZLX_ISA_Register_identifier_iterator_x);
	
	uint32_t size = paramaters.imm2;

	uint32_t size_in_bytes = imm2_size_to_bytes(size);


	int32_t offset = MY_sign_extend32(paramaters.imm3, 3);

	output.memory_operation = true;
	output.memory_load = true;
	output.memory_address = data_1;
	
	uint32_t load = load_variable_size_data(memory, data_1, size_in_bytes);


	output.memory_value = load;

	output.use_destination = true;
	output.use_destination_1 = true;
	output.use_destination_2 = true;

	uint32_t iterator_change = data_1 +  offset * size_in_bytes; 

	output.destination_1 = ZLX_ISA_Register_identifier_iterator_x; 
	output.destination_1_data = iterator_change;
	
	output.destination_2 = ZLX_ISA_Register_identifier_accumulator;
	output.destination_2_data = load;

	return output;	
}
//ldya
OPERATION(major_load_iterator_y_accumualtor)
{

	OPERATION_SETUP; 

	uint32_t data_1 = ZLX_EMU_get_register__CPU(cpu, ZLX_ISA_Register_identifier_iterator_y);
	
	uint32_t size = paramaters.imm2;

	uint32_t size_in_bytes = imm2_size_to_bytes(size);


	int32_t offset = MY_sign_extend32(paramaters.imm3, 3);

	output.memory_operation = true;
	output.memory_load = true;
	output.memory_address = data_1;
	
	uint32_t load = load_variable_size_data(memory, data_1, size_in_bytes);


	output.memory_value = load;

	output.use_destination = true;
	output.use_destination_1 = true;
	output.use_destination_2 = true;

	uint32_t iterator_change = data_1 +  offset * size_in_bytes; 

	output.destination_1 = ZLX_ISA_Register_identifier_iterator_y; 
	output.destination_1_data = iterator_change;
	
	output.destination_2 = ZLX_ISA_Register_identifier_accumulator;
	output.destination_2_data = load;

	return output;	
}
//stxa.s
OPERATION(major_store_iterator_x_accumualtor)
{

	OPERATION_SETUP; 

	uint32_t data_1 = ZLX_EMU_get_register__CPU(cpu, ZLX_ISA_Register_identifier_iterator_x);

	uint32_t data_2 = ZLX_EMU_get_register__CPU(cpu, ZLX_ISA_Register_identifier_accumulator);
	
	uint32_t size = paramaters.imm2;

	uint32_t size_in_bytes = imm2_size_to_bytes(size);


	int32_t offset = MY_sign_extend32(paramaters.imm3, 3);

	output.memory_operation = true;
	output.memory_store = true;
	output.memory_address = data_1;
	
	output.memory_value = data_2;
	store_variable_size_data(memory, data_1, data_2, size_in_bytes);



	output.use_destination = true;
	output.use_destination_1 = true;

	uint32_t iterator_change = data_1 +  offset * size_in_bytes; 

	output.destination_1 = ZLX_ISA_Register_identifier_iterator_x; 
	output.destination_1_data = iterator_change;
	
	return output;	
}

//stya.s
OPERATION(major_store_iterator_y_accumualtor)
{

	OPERATION_SETUP; 

	uint32_t data_1 = ZLX_EMU_get_register__CPU(cpu, ZLX_ISA_Register_identifier_iterator_y);

	uint32_t data_2 = ZLX_EMU_get_register__CPU(cpu, ZLX_ISA_Register_identifier_accumulator);
	
	uint32_t size = paramaters.imm2;

	uint32_t size_in_bytes = imm2_size_to_bytes(size);


	int32_t offset = MY_sign_extend32(paramaters.imm3, 3);

	output.memory_operation = true;
	output.memory_store = true;
	output.memory_address = data_1;
	
	output.memory_value = data_2;
	store_variable_size_data(memory, data_1, data_2, size_in_bytes);



	output.use_destination = true;
	output.use_destination_1 = true;

	uint32_t iterator_change = data_1 +  offset * size_in_bytes; 

	output.destination_1 = ZLX_ISA_Register_identifier_iterator_y; 
	output.destination_1_data = iterator_change;
	
	return output;	
}

//inc r1
//r1 += 1
OPERATION(major_increment)
{
	OPERATION_SETUP; 
	uint32_t data_1 = ZLX_EMU_get_register__CPU(cpu, paramaters.source_1);
	output.use_destination = true;
	output.use_destination_1 = true;
	output.destination_1 = paramaters.source_1; 
	output.destination_1_data = data_1 + 1;
	return output;
}

//dec r1
//r1 -= 1
OPERATION(major_decrement)
{
	OPERATION_SETUP; 
	uint32_t data_1 = ZLX_EMU_get_register__CPU(cpu, paramaters.source_1);
	output.use_destination = true;
	output.use_destination_1 = true;
	output.destination_1 = paramaters.source_1; 
	output.destination_1_data = data_1 - 1;
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



static ZLX_EMU__CPU_operation_t major_operations[] = 
{

};

void ZLX_EMU_solve__CPU(ZLX_EMU__CPU_t *cpu, ZLX_EMU__CPU_output_t output)
{
	
	if(output.use_destination == true)
	{
		bool use_d1 = false, use_d2 = false;
		if(output.use_destination_1 == true)
		{
			use_d1 = true;
			ZLX_EMU_set_regiser__CPU(cpu, output.destination_1, output.destination_1_data);
		}

		if(output.use_destination_2 == true)
		{
			use_d2 = true;

			ZLX_EMU_set_regiser__CPU(cpu, output.destination_2, output.destination_2_data);
		}
		
		if(use_d1 == use_d2)
		{
			if(output.destination_1 == output.destination_2 && output.destination_1 != ZLX_ISA_Register_identifier_zero)
			{
				MY_FAULT("cannot have w1 == w2");
			}
		}
	
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
		MY_FAULT("add instruction increment i think");
		MY_TODO;		
	}

}

void ZLX_EMU_minor_execution(ZLX_EMU_Machine_t *machine, ZLX_ISA_Minor_t minor)
{
	ZLX_EMU__CPU_parameters_t parameters = {};
	ZLX_EMU__CPU_output_t output;
	switch(minor.operation)
	{
		case ZLX_ISA_Minor_operation_move:
		

			parameters.source_1 = minor.u__format_55.a5;
			parameters.source_2 = minor.u__format_55.b5;
			break;
		
		
		default:
			printf("ZLX %d\n",minor.operation); 
			MY_FAULT("not a valid instruction");
			break;
	}


	output = minor_operations[minor.operation](machine, parameters);





}



void ZLX_EMU_decode_instruction(ZLX_EMU_Machine_t *machine)
{

	ZLX_EMU__CPU_t *cpu = machine->p__cpu;
}


void ZLX_EMU_step(ZLX_EMU_Machine_t *machine)
{
	ZLX_EMU__CPU_t *cpu = machine->p__cpu;
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
