#ifndef ZLX_ISA_ZEROLOX_HEADER
#define ZLX_ISA_ZEROLOX_HEADER

#include <stdint.h>




		
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
	ZLX_ISA_Minor_operation_swap,
	ZLX_ISA_Minor_operation_add,
	ZLX_ISA_Minor_operation_adda,
	ZLX_ISA_Minor_operation_sub,
	ZLX_ISA_Minor_operation_addi,
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
	ZLX_ISA_Minor_operation_ldsa

}ZLX_ISA_Minor_operation_t;


//minor extraction formats

typedef enum ZLX_ISA_Minor_format_type
{
	ZLX_ISA_Minor_format_type_55,
	ZLX_ISA_Minor_format_type_235,
	ZLX_ISA_Minor_format_type_28,
	ZLX_ISA_Minor_format_type_10,
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

//
//
//----------------------------------------------------------------------------------------------
//MAJORS
//
typedef enum ZLX_ISA_Major_operation
{
	ZLX_ISA_Major_operation_ld,
	ZLX_ISA_Major_operation_st,

}ZLX_ISA_Major_operation_t;



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

}ZLX_ISA_Major_format_5_5321_t;

typedef struct ZLX_ISA_Major_format_5_b5
{
	//[opcode][a5][1] [b11{0:4}][b11{5:9}[c5]{b{10}]
	uint8_t a5;
	uint16_t b11;
	uint8_t c5;	
}ZLX_ISA_Major_format_5_b5_t;


typedef enum ZLX_ISA_Zerolox_version
{
	ZLX_ISA_ZEROLOX1,
}ZLX_ISA_Zerolox_version_t;






typedef struct ZLX_ISA_Parcel
{
	uint16_t parcel;
}ZLX_ISA_Parcel_t;

typedef struct ZLX_ISA_Instruction
{
	ZLX_ISA_Parcel_t a__parcels[8];
}ZLX_ISA_Instruction_t;


extern char *ZLX_ISA_opcodes[];




#endif

