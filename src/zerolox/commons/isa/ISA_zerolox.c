#include "ISA_zerolox.h"
#include <stdlib.h>
#include <stdio.h>


ZLX_ISA_Minor_format_type_t ZLX_ISA_Minor_operation_formats[] = 
{
	[ZLX_ISA_Minor_operation_move] = ZLX_ISA_Minor_format_type_55,
	[ZLX_ISA_Minor_operation_swap] = ZLX_ISA_Minor_format_type_55,
	[ZLX_ISA_Minor_operation_add] = ZLX_ISA_Minor_format_type_55,
	[ZLX_ISA_Minor_operation_adda] = ZLX_ISA_Minor_format_type_55,
	[ZLX_ISA_Minor_operation_sub] = ZLX_ISA_Minor_format_type_55,
	[ZLX_ISA_Minor_operation_addi] = ZLX_ISA_Minor_format_type_55,
	[ZLX_ISA_Minor_operation_geta] = ZLX_ISA_Minor_format_type_a,
	[ZLX_ISA_Minor_operation_get8] = ZLX_ISA_Minor_format_type_28, 
	[ZLX_ISA_Minor_operation_b] = ZLX_ISA_Minor_format_type_a, 
	[ZLX_ISA_Minor_operation_baeqz] = ZLX_ISA_Minor_format_type_a,
	[ZLX_ISA_Minor_operation_baneqz] = ZLX_ISA_Minor_format_type_a,
	[ZLX_ISA_Minor_operation_lda] = ZLX_ISA_Minor_format_type_235, 
	[ZLX_ISA_Minor_operation_ald] = ZLX_ISA_Minor_format_type_235,
	[ZLX_ISA_Minor_operation_sta] = ZLX_ISA_Minor_format_type_235,
	[ZLX_ISA_Minor_operation_ast] = ZLX_ISA_Minor_format_type_235,
	[ZLX_ISA_Minor_operation_ldsa] = ZLX_ISA_Minor_format_type_28,
	[ZLX_ISA_Minor_operation_stsa] = ZLX_ISA_Minor_format_type_28,
	[ZLX_ISA_Minor_operation_bieqc] = ZLX_ISA_Minor_format_type_28,
	[ZLX_ISA_Minor_operation_ld_plus] = ZLX_ISA_Minor_format_type_235,
	[ZLX_ISA_Minor_operation_st_plus] = ZLX_ISA_Minor_format_type_235,
};


ZLX_ISA_Major_format_type_t ZLX_ISA_Major_operation_formats[] = 
{
	[ZLX_ISA_Major_operation_ld] = ZLX_ISA_Major_format_type_5_952 ,
	[ZLX_ISA_Major_operation_st] = ZLX_ISA_Major_format_type_5_952 ,
	[ZLX_ISA_Major_operation_push8] = ZLX_ISA_Major_format_type_5,
	[ZLX_ISA_Major_operation_push16] = ZLX_ISA_Major_format_type_5,
	[ZLX_ISA_Major_operation_push32] = ZLX_ISA_Major_format_type_5,
	[ZLX_ISA_Major_operation_pushp] = ZLX_ISA_Major_format_type_5,
	[ZLX_ISA_Major_operation_pop8] = ZLX_ISA_Major_format_type_5,
	[ZLX_ISA_Major_operation_pop16] = ZLX_ISA_Major_format_type_5,
	[ZLX_ISA_Major_operation_pop32] = ZLX_ISA_Major_format_type_5,
	[ZLX_ISA_Major_operation_popp] = ZLX_ISA_Major_format_type_5,
	[ZLX_ISA_Major_operation_get16] = ZLX_ISA_Major_format_type_5_f,
	[ZLX_ISA_Major_operation_get32] = ZLX_ISA_Major_format_type_5_f_f,
	[ZLX_ISA_Major_operation_3form] = ZLX_ISA_Major_format_type_5_655,
	[ZLX_ISA_Major_operation_3form_immediate] = ZLX_ISA_Major_format_type_5_655,
	[ZLX_ISA_Major_operation_inc] = ZLX_ISA_Major_format_type_5,
	[ZLX_ISA_Major_operation_dec] = ZLX_ISA_Major_format_type_5,
	[ZLX_ISA_Major_operation_addi] = ZLX_ISA_Major_format_type_5_b5,
	[ZLX_ISA_Major_operation_bneq] = ZLX_ISA_Major_format_type_5_b5,
	[ZLX_ISA_Major_operation_beq] = ZLX_ISA_Major_format_type_5_b5,
	[ZLX_ISA_Major_operation_blt] = ZLX_ISA_Major_format_type_5_b5,
	[ZLX_ISA_Major_operation_bge] = ZLX_ISA_Major_format_type_5_b5,
	[ZLX_ISA_Major_operation_bltu] = ZLX_ISA_Major_format_type_5_b5,
	[ZLX_ISA_Major_operation_bgeu] = ZLX_ISA_Major_format_type_5_b5,
	[ZLX_ISA_Major_operation_call] = ZLX_ISA_Major_format_type_5,
	[ZLX_ISA_Major_operation_ret] = ZLX_ISA_Major_format_type_5,
};

uint8_t ZLX_ISA_Major_operation_size[] = 
{
	[ZLX_ISA_Major_format_type_5] = 1,
	[ZLX_ISA_Major_format_type_5_5551] = 2,
	[ZLX_ISA_Major_format_type_5_952] = 2,
	[ZLX_ISA_Major_format_type_32] = 1,
	[ZLX_ISA_Major_format_type_5_b5] = 2,
	[ZLX_ISA_Major_format_type_5_f] = 2,
	[ZLX_ISA_Major_format_type_5_f_f] = 3,
	[ZLX_ISA_Major_format_type_5_655] = 2,
	[ZLX_ISA_Major_format_type_5_53251] = 2
};



char *ZLX_ISA_opcodes[] = 
{
	[0] = ""
};




bool ZLX_ISA_decode_instruction(ZLX_ISA_Instruction_t *instruction, ZLX_ISA_Parcel_t parcels[8], int *count, int position)
{
	if(instruction == NULL || parcels == NULL || count == NULL || position < 0 || position >= 8)
	{
		return false;
	}

	uint16_t first = parcels[0].parcel;

	if(position == 0)
	{
		if(first & 1)
		{
			// Minor: [opcode5][operands10][1]
			uint32_t opcode = (first >> 11) & 0x1F;
			if(opcode >= sizeof(ZLX_ISA_Minor_operation_formats) / sizeof(ZLX_ISA_Minor_operation_formats[0]))
			{
				*count = 0;
				return false;
			}

			instruction->type = ZLX_ISA_Minor_type;
			instruction->u__minor.operation = (ZLX_ISA_Minor_operation_t)opcode;
			ZLX_ISA_Minor_format_type_t type = ZLX_ISA_Minor_operation_formats[opcode];
			instruction->u__minor.type = type;
			*count = 1;

			switch(type)
			{
				case ZLX_ISA_Minor_format_type_a:
					instruction->u__minor.u__format_a.aa = (first >> 1) & 0x3FF;
					break;
				case ZLX_ISA_Minor_format_type_55:
					instruction->u__minor.u__format_55.a5 = (first >> 6) & 0x1F;
					instruction->u__minor.u__format_55.b5 = (first >> 1) & 0x1F;
					break;
				case ZLX_ISA_Minor_format_type_28:
					instruction->u__minor.u__format_28.a2 = (first >> 9) & 0x3;
					instruction->u__minor.u__format_28.b8 = (first >> 1) & 0xFF;
					break;
				case ZLX_ISA_Minor_format_type_235:
					instruction->u__minor.u__format_235.a2 = (first >> 9) & 0x3;
					instruction->u__minor.u__format_235.b3 = (first >> 6) & 0x7;
					instruction->u__minor.u__format_235.c5 = (first >> 1) & 0x1F;
					break;
				default:
					*count = 0;
					return false;
			}

			return true;
		}

		// Major: [opcode10][a5][0], possibly followed by more parcels.
		uint32_t opcode = (first >> 6) & 0x3FF;
		if(opcode >= sizeof(ZLX_ISA_Major_operation_formats) / sizeof(ZLX_ISA_Major_operation_formats[0]))
		{
			*count = 0;
			return false;
		}

		instruction->type = ZLX_ISA_Major_type;
		instruction->u__major.operation = (ZLX_ISA_Major_operation_t)opcode;
		ZLX_ISA_Major_format_type_t type = ZLX_ISA_Major_operation_formats[opcode];
		instruction->u__major.type = type;

		if((uint32_t)type >= sizeof(ZLX_ISA_Major_operation_size) / sizeof(ZLX_ISA_Major_operation_size[0]))
		{
			*count = 0;
			return false;
		}
		*count = ZLX_ISA_Major_operation_size[type];
		if(*count < 1 || *count > 8)
		{
			*count = 0;
			return false;
		}
		if(*count > 1)
		{
			return false;
		}
	}
	else
	{
		// The caller appends parcels in order. Decode when the final one arrives.
		if(instruction->type != ZLX_ISA_Major_type || position + 1 != *count)
		{
			return false;
		}
	}

	uint16_t second = *count > 1 ? parcels[1].parcel : 0;
	uint16_t third = *count > 2 ? parcels[2].parcel : 0;
	uint8_t a5 = (first >> 1) & 0x1F;

	switch(instruction->u__major.type)
	{
		case ZLX_ISA_Major_format_type_5:
			instruction->u__major.u__foramt_5.a5 = a5;
			break;
		case ZLX_ISA_Major_format_type_32:
			instruction->u__major.u__foramt_32.a3 = (first >> 3) & 0x7;
			instruction->u__major.u__foramt_32.b2 = (first >> 1) & 0x3;
			break;
		case ZLX_ISA_Major_format_type_5_5551:
			instruction->u__major.u__format_5_5551.a5 = a5;
			instruction->u__major.u__format_5_5551.b5 = (second >> 11) & 0x1F;
			instruction->u__major.u__format_5_5551.c5 = (second >> 6) & 0x1F;
			instruction->u__major.u__format_5_5551.d5 = (second >> 1) & 0x1F;
			instruction->u__major.u__format_5_5551.e1 = second & 1;
			break;
		case ZLX_ISA_Major_format_type_5_952:
			instruction->u__major.u__format_5_952.a5 = a5;
			instruction->u__major.u__format_5_952.b9 =
				((second >> 11) & 0x1F) | (((second >> 6) & 0x7) << 5) | ((second & 1) << 8);
			instruction->u__major.u__format_5_952.c5 = (second >> 1) & 0x1F;
			instruction->u__major.u__format_5_952.d2 = (second >> 9) & 0x3;
			break;
		case ZLX_ISA_Major_format_type_5_b5:
			instruction->u__major.u__format_5_b5.a5 = a5;
			instruction->u__major.u__format_5_b5.b11 =
				((second >> 11) & 0x1F) | (((second >> 6) & 0x1F) << 5) | ((second & 1) << 10);
			instruction->u__major.u__format_5_b5.c5 = (second >> 1) & 0x1F;
			break;
		case ZLX_ISA_Major_format_type_5_f:
			instruction->u__major.u__format_5_f.a5 = a5;
			instruction->u__major.u__format_5_f.bf = second;
			break;
		case ZLX_ISA_Major_format_type_5_f_f:
			instruction->u__major.u__format_5_f_f.a5 = a5;
			instruction->u__major.u__format_5_f_f.bf = second;
			instruction->u__major.u__format_5_f_f.cf = third;
			break;
		case ZLX_ISA_Major_format_type_5_655:
			instruction->u__major.u__format_5_655.a5 = a5;
			instruction->u__major.u__format_5_655.b6 = ((second >> 11) & 0x1F) | ((second & 1) << 5);
			instruction->u__major.u__format_5_655.c5 = (second >> 6) & 0x1F;
			instruction->u__major.u__format_5_655.d5 = (second >> 1) & 0x1F;
			break;
		case ZLX_ISA_Major_format_type_5_53251:
			instruction->u__major.u__format_5_53251.a5 = a5;
			instruction->u__major.u__format_5_53251.b5 = (second >> 11) & 0x1F;
			instruction->u__major.u__format_5_53251.c3 = (second >> 8) & 0x7;
			instruction->u__major.u__format_5_53251.d2 = (second >> 6) & 0x3;
			instruction->u__major.u__format_5_53251.e5 = (second >> 1) & 0x1F;
			instruction->u__major.u__format_5_53251.f1 = second & 1;
			break;
		default:
			*count = 0;
			return false;
	}
	instruction->size = *count;
	return true;
}
