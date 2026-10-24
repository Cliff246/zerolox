#ifndef ZLX_ISA_ZEROLOX_HEADER
#define ZLX_ISA_ZEROLOX_HEADER

#include <stdint.h>
#include "stdbool.h"




		
typedef enum ZLX_ISA_Register_identifier
{
	ZLX_ISA_Register_identifier_zero = 0,
	ZLX_ISA_Register_identifier_accumulator = 1,
	ZLX_ISA_Register_identifier_stack_pointer = 2,
	ZLX_ISA_Register_identifier_return_pointer = 3,
	ZLX_ISA_Register_identifier_iterator_x = 4,
	ZLX_ISA_Register_identifier_iterator_y = 5,
	ZLX_ISA_Register_identifier_counter_x = 6,
	ZLX_ISA_Register_identifier_counter_y = 7,
}ZLX_ISA_Register_identifier_t;

//
//---------------------------------------------------------------------------
//

typedef enum ZLX_ISA_Minor_operation
{
	ZLX_ISA_Minor_operation_move,
	ZLX_ISA_Minor_operation_add,
	ZLX_ISA_Minor_operation_adda,
	ZLX_ISA_Minor_operation_sub,
	ZLX_ISA_Minor_operation_geta, 
	ZLX_ISA_Minor_operation_get8,
	ZLX_ISA_Minor_operation_b, 
	ZLX_ISA_Minor_operation_baeqz,
	ZLX_ISA_Minor_operation_baneqz,
	ZLX_ISA_Minor_operation_lda, 
	ZLX_ISA_Minor_operation_ald,
	ZLX_ISA_Minor_operation_sta,
	ZLX_ISA_Minor_operation_ast,	
	ZLX_ISA_Minor_operation_stsa,
	ZLX_ISA_Minor_operation_ldsa,
	ZLX_ISA_Minor_operation_ld_plus, //ld+
	ZLX_ISA_Minor_operation_st_plus,  //st+
	ZLX_ISA_Minor_operation_bieqc,
	ZLX_ISA_Minor_operation_bteqa,
	ZLX_ISA_Minor_operation_btneqa,
}ZLX_ISA_Minor_operation_t;


//minor extraction formats

typedef enum ZLX_ISA_Minor_format_type
{
	ZLX_ISA_Minor_format_type_55,
	ZLX_ISA_Minor_format_type_235,
	ZLX_ISA_Minor_format_type_28,
	ZLX_ISA_Minor_format_type_a,
}ZLX_ISA_Minor_format_type_t;



typedef struct ZLX_ISA_Minor_format_55
{
	uint8_t a5;
	uint8_t b5;
}ZLX_ISA_Minor_format_55_t;

typedef struct ZLX_ISA_Minor_format_235
{
	uint8_t a2;
	uint8_t b3;
	uint8_t c5;
}ZLX_ISA_Minor_format_235_t;

typedef struct ZLX_ISA_Minor_format_28
{
	uint8_t a2;
	uint8_t b8;
}ZLX_ISA_Minor_format_28_t;

typedef struct ZLX_ISA_Minor_format_a
{
	uint16_t aa;
}ZLX_ISA_Minor_format_a_t;


//minor operation class





typedef struct ZLX_ISA_Minor
{
	ZLX_ISA_Minor_format_type_t type;
	ZLX_ISA_Minor_operation_t operation;
	union 
	{
		ZLX_ISA_Minor_format_55_t u__format_55;
		ZLX_ISA_Minor_format_235_t u__format_235;
		ZLX_ISA_Minor_format_28_t u__format_28;
		ZLX_ISA_Minor_format_a_t u__format_a;
	};	
}ZLX_ISA_Minor_t;


extern ZLX_ISA_Minor_format_type_t ZLX_ISA_Minor_operation_formats[];




//
//
//----------------------------------------------------------------------------------------------
//MAJORS
//
//
//

typedef enum ZLX_ISA_Major_3form
{
	ZLX_ISA_Major_operation_3form_add,
	ZLX_ISA_Major_operation_3form_sub,
	ZLX_ISA_Major_operation_3form_sll,
	ZLX_ISA_Major_operation_3form_srl,
	ZLX_ISA_Major_operation_3form_sra,
	ZLX_ISA_Major_operation_3form_xor,
	ZLX_ISA_Major_operation_3form_or,
	ZLX_ISA_Major_operation_3form_and,

}ZLX_ISA_Major_3form_t;

typedef enum ZLX_ISA_Major_3form_immediate
{
	ZLX_ISA_Major_operation_3form_immediate_slli,
	ZLX_ISA_Major_operation_3form_immediate_srli,
	ZLX_ISA_Major_operation_3form_immediate_srai,
	ZLX_ISA_Major_operation_3form_immediate_xori,
	ZLX_ISA_Major_operation_3form_immediate_andi,
	ZLX_ISA_Major_operation_3form_immediate_ori,
	
}ZLX_ISA_Major_3form_immediate_t;



typedef enum ZLX_ISA_Major_operation
{
	ZLX_ISA_Major_operation_ld,
	ZLX_ISA_Major_operation_st,
	ZLX_ISA_Major_operation_push8,
	ZLX_ISA_Major_operation_push16,
	ZLX_ISA_Major_operation_push32,
	ZLX_ISA_Major_operation_pushp,
	ZLX_ISA_Major_operation_pop8,
	ZLX_ISA_Major_operation_pop16,
	ZLX_ISA_Major_operation_pop32,
	ZLX_ISA_Major_operation_popp,
	ZLX_ISA_Major_operation_get16,
	ZLX_ISA_Major_operation_get32,
	ZLX_ISA_Major_operation_3form,
	ZLX_ISA_Major_operation_3form_immediate,
	ZLX_ISA_Major_operation_inc,
	ZLX_ISA_Major_operation_dec,
	ZLX_ISA_Major_operation_addi,
	ZLX_ISA_Major_operation_bneq,
	ZLX_ISA_Major_operation_beq,
	ZLX_ISA_Major_operation_blt,
	ZLX_ISA_Major_operation_bge,
	ZLX_ISA_Major_operation_bltu,
	ZLX_ISA_Major_operation_bgeu,
	ZLX_ISA_Major_operation_call,
	ZLX_ISA_Major_operation_ret,


}ZLX_ISA_Major_operation_t;
/*
 *add
 *sub
 *sll
 *srl
 *sra
 /i don't want to copy riscv so, i will have to steal from something else
 *xor
 *or
 *and
 *xori
 *ori
 *andi
 *bne
 *beq
 *call
 *ret
 *blt
 *ge
 *blt
 *bgeu
 *
 *	ULTRA OPS
 *
 */

typedef enum ZLX_ISA_Major_format_type
{
	ZLX_ISA_Major_format_type_5,
	ZLX_ISA_Major_format_type_5_5551,
	ZLX_ISA_Major_format_type_5_952,
	ZLX_ISA_Major_format_type_32,
	ZLX_ISA_Major_format_type_5_b5,
	ZLX_ISA_Major_format_type_5_f,
	ZLX_ISA_Major_format_type_5_f_f,
	ZLX_ISA_Major_format_type_5_655,
	ZLX_ISA_Major_format_type_5_53251,
}ZLX_ISA_Major_format_type_t;



typedef struct ZLX_ISA_Major_format_5_5551
{
	//[opcode][a5][1] [b5][c5][d5][e1]
	
	//parcel1
	uint8_t a5;
	//parcel2
	uint8_t b5;
	uint8_t c5;
	uint8_t d5;
	uint8_t e1;
}ZLX_ISA_Major_format_5_5551_t;

typedef struct ZLX_ISA_Major_format_5_952
{
	//[opcode][a5][1] 
	//[b9{0:4}][d2][b9{5:7}][c5][b9{8}
	uint8_t a5;

	//imm9 
	uint16_t b9;
	
	uint8_t c5;
	
	uint8_t d2;
		

}ZLX_ISA_Major_format_5_952_t;

typedef struct ZLX_ISA_Major_format_5
{
	//[opcode][a5][1]
	uint8_t a5;
}ZLX_ISA_Major_format_5_t;

typedef struct ZLX_ISA_Major_format_32
{
	//[opcode][a3][b2][1]
	uint8_t a3;
	uint8_t b2;
}ZLX_ISA_Major_format_32_t;

typedef struct ZLX_ISA_Major_format_5_655
{
	//[opcode][a5][1] [b{0:4]][c5][d5][b:{5}]
	uint8_t a5;
	
	uint8_t b6;
   	uint8_t c5;
	uint8_t d5;
}ZLX_ISA_Major_format_5_655_t;

typedef struct ZLX_ISA_Major_format_5_53251
{
	//[opcode][a5][1] [b5][c3][d2][e5][f1]
	uint8_t a5;

	uint8_t b5;
	uint8_t c3;
	uint8_t d2;
	uint8_t e5;
	uint8_t f1;

}ZLX_ISA_Major_format_5_53251_t;

typedef struct ZLX_ISA_Major_format_5_b5
{
	//[opcode][a5][1] [b11{0:4}][b11{5:9}[c5]{b{10}]
	uint8_t a5;
	uint16_t b11;
	uint8_t c5;	
}ZLX_ISA_Major_format_5_b5_t;

typedef struct ZLX_ISA_Major_format_5_f
{
	uint8_t a5;
	uint16_t bf;
}ZLX_ISA_Major_format_5_f_t;

typedef struct ZLX_ISA_Major_format_5_f_f
{
	uint8_t a5;
	uint16_t bf;
	uint16_t cf;
}ZLX_ISA_Major_format_5_f_f_t;



typedef struct ZLX_ISA_Major
{
	ZLX_ISA_Major_operation_t operation;
	ZLX_ISA_Major_format_type_t type;
	union 
	{
		ZLX_ISA_Major_format_5_b5_t u__format_5_b5; 
		ZLX_ISA_Major_format_5_655_t u__format_5_655;
		ZLX_ISA_Major_format_5_952_t u__format_5_952;
		ZLX_ISA_Major_format_5_53251_t u__format_5_53251;
		ZLX_ISA_Major_format_5_5551_t u__format_5_5551;
		ZLX_ISA_Major_format_32_t u__foramt_32;
		ZLX_ISA_Major_format_5_t u__foramt_5;
		ZLX_ISA_Major_format_5_f_t u__format_5_f;
		ZLX_ISA_Major_format_5_f_f_t u__format_5_f_f;
	};

}ZLX_ISA_Major_t;


extern ZLX_ISA_Major_format_type_t ZLX_ISA_Major_operation_formats[];
extern uint8_t ZLX_ISA_Major_operation_size[];

//

typedef enum ZLX_ISA_Zerolox_version
{
	ZLX_ISA_ZEROLOX1,
}ZLX_ISA_Zerolox_version_t;

typedef enum ZLX_ISA_Instruction_type
{
	ZLX_ISA_Major_type,
	ZLX_ISA_Minor_type,
}ZLX_ISA_Instruction_type_t;


typedef struct ZLX_ISA_Instruction
{
	ZLX_ISA_Instruction_type_t type;
	uint8_t size;
	union
	{
		ZLX_ISA_Major_t u__major;
		ZLX_ISA_Minor_t u__minor;
	};
}ZLX_ISA_Instruction_t;





typedef struct ZLX_ISA_Parcel
{
	uint16_t parcel;
}ZLX_ISA_Parcel_t;


//continue to push into the parcel, it fills then return true on completion 
bool ZLX_ISA_decode_instruction(ZLX_ISA_Instruction_t *instruction, ZLX_ISA_Parcel_t parcels[8], int *count, int position);



extern char *ZLX_ISA_opcodes[];







#endif

