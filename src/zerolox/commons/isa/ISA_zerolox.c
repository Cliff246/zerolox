#include "ISA_zerolox.h"
#include "MY_commons.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>



char *ZLX_ISA_Register_string[] = 
{
	[ZLX_ISA_register_identifier_zero]= "z0",
	[ZLX_ISA_register_identifier_accumulator] = "ac",
	[ZLX_ISA_register_identifier_stack_pointer] = "sp",
	[ZLX_ISA_register_identifier_return_pointer] = "rp",
	[ZLX_ISA_register_identifier_iterator_x] = "ix",
	[ZLX_ISA_register_identifier_iterator_y] = "iy",
	[ZLX_ISA_register_identifier_counter_x] = "cx",
	[ZLX_ISA_register_identifier_counter_y] = "cy",
	[ZLX_ISA_register_identifier_temporary_0] = "t0",
	[ZLX_ISA_register_identifier_temporary_1] = "t1",
	[ZLX_ISA_register_identifier_temporary_2] = "t2",
	[ZLX_ISA_register_identifier_temporary_3] = "t3",
	[ZLX_ISA_register_identifier_temporary_4] = "t4",
	[ZLX_ISA_register_identifier_temporary_5] = "t5",
	[ZLX_ISA_register_identifier_saved_0] = "s0",
	[ZLX_ISA_register_identifier_saved_1] = "s1",
	[ZLX_ISA_register_identifier_saved_2] = "s2",
	[ZLX_ISA_register_identifier_saved_3] = "s3",
	[ZLX_ISA_register_identifier_saved_4] = "s4",
	[ZLX_ISA_register_identifier_saved_5] = "s5",
	[ZLX_ISA_register_identifier_saved_6] = "s6",
	[ZLX_ISA_register_identifier_saved_7] = "s7",
	[ZLX_ISA_register_identifier_saved_8] = "s8",
	[ZLX_ISA_register_identifier_saved_9] = "s9",
	[ZLX_ISA_register_identifier_argument_0] = "a0",
	[ZLX_ISA_register_identifier_argument_1] = "a1",
	[ZLX_ISA_register_identifier_argument_2] = "a2",
	[ZLX_ISA_register_identifier_argument_3] = "a3",
	[ZLX_ISA_register_identifier_argument_4] = "a4",
	[ZLX_ISA_register_identifier_argument_5] = "a5",
	[ZLX_ISA_register_identifier_argument_6] = "a6",
	[ZLX_ISA_register_identifier_argument_7] = "a7",

};

ZLX_ISA_register_identifier_t ZLX_ISA_get_register_number(char *string)
{
	int found = -1;
	for(int i = 0; i < 32; ++i)
	{
		if(strcmp(string, ZLX_ISA_Register_string[i]) == 0)
		{
			found = i;
			break;
		}
	}
	if(found == -1)
		MY_FAULT("string is not a register");
	return (ZLX_ISA_register_identifier_t) found;
}

ZLX_ISA_minor_format_type_t ZLX_ISA_Minor_operation_formats[ZLX_ISA_minor_operation_count] =
{
	[ZLX_ISA_minor_operation_bcall10] = ZLX_ISA_minor_format_type_a,
	[ZLX_ISA_minor_operation_blink10] = ZLX_ISA_minor_format_type_a,
	[ZLX_ISA_minor_operation_move] = ZLX_ISA_minor_format_type_55,
	[ZLX_ISA_minor_operation_add] = ZLX_ISA_minor_format_type_55,
	[ZLX_ISA_minor_operation_adda] = ZLX_ISA_minor_format_type_55,
	[ZLX_ISA_minor_operation_sexta] = ZLX_ISA_minor_format_type_55,
	[ZLX_ISA_minor_operation_sub] = ZLX_ISA_minor_format_type_55,
	[ZLX_ISA_minor_operation_geta] = ZLX_ISA_minor_format_type_a,
	[ZLX_ISA_minor_operation_bieqc_update] = ZLX_ISA_minor_format_type_28,
	[ZLX_ISA_minor_operation_bineqc_update] = ZLX_ISA_minor_format_type_28,
	[ZLX_ISA_minor_operation_get8] = ZLX_ISA_minor_format_type_28, 
	[ZLX_ISA_minor_operation_b] = ZLX_ISA_minor_format_type_a, 
	[ZLX_ISA_minor_operation_baeqz] = ZLX_ISA_minor_format_type_a,
	[ZLX_ISA_minor_operation_baneqz] = ZLX_ISA_minor_format_type_a,
	[ZLX_ISA_minor_operation_lda] = ZLX_ISA_minor_format_type_235, 
	[ZLX_ISA_minor_operation_ald] = ZLX_ISA_minor_format_type_235,
	[ZLX_ISA_minor_operation_sta] = ZLX_ISA_minor_format_type_235,
	[ZLX_ISA_minor_operation_ast] = ZLX_ISA_minor_format_type_235,
	[ZLX_ISA_minor_operation_ldsa] = ZLX_ISA_minor_format_type_28,
	[ZLX_ISA_minor_operation_stsa] = ZLX_ISA_minor_format_type_28,
	[ZLX_ISA_minor_operation_bieqc] = ZLX_ISA_minor_format_type_28,
	[ZLX_ISA_minor_operation_ld_plus] = ZLX_ISA_minor_format_type_235,
	[ZLX_ISA_minor_operation_st_plus] = ZLX_ISA_minor_format_type_235,
	[ZLX_ISA_minor_operation_bteqa] = ZLX_ISA_minor_format_type_28,
	[ZLX_ISA_minor_operation_btneqa] = ZLX_ISA_minor_format_type_28,

};


ZLX_ISA_major_format_type_t ZLX_ISA_Major_operation_formats[ZLX_ISA_major_operation_count] =
{
	[ZLX_ISA_major_operation_inc] = ZLX_ISA_major_format_type_55,
	[ZLX_ISA_major_operation_dec] = ZLX_ISA_major_format_type_55,
	[ZLX_ISA_major_operation_shift_left] = ZLX_ISA_major_format_type_55,
	[ZLX_ISA_major_operation_shift_right] = ZLX_ISA_major_format_type_55,
	[ZLX_ISA_major_operation_shift_arith] = ZLX_ISA_major_format_type_55,
	[ZLX_ISA_major_operation_get_pos] = ZLX_ISA_major_format_type_55,
	[ZLX_ISA_major_operation_get_neg] = ZLX_ISA_major_format_type_55,
	[ZLX_ISA_major_operation_ld] = ZLX_ISA_major_format_type_5_952 ,
	[ZLX_ISA_major_operation_st] = ZLX_ISA_major_format_type_5_952 ,
	[ZLX_ISA_major_operation_push8] = ZLX_ISA_major_format_type_5,
	[ZLX_ISA_major_operation_push16] = ZLX_ISA_major_format_type_5,
	[ZLX_ISA_major_operation_push32] = ZLX_ISA_major_format_type_5,
	[ZLX_ISA_major_operation_push3] = ZLX_ISA_major_format_type_5_655,
	[ZLX_ISA_major_operation_pop3] = ZLX_ISA_major_format_type_5_655,
	[ZLX_ISA_major_operation_pushp] = ZLX_ISA_major_format_type_5,
	[ZLX_ISA_major_operation_pop8] = ZLX_ISA_major_format_type_5,
	[ZLX_ISA_major_operation_pop16] = ZLX_ISA_major_format_type_5,
	[ZLX_ISA_major_operation_pop32] = ZLX_ISA_major_format_type_5,
	[ZLX_ISA_major_operation_popp] = ZLX_ISA_major_format_type_5,
	[ZLX_ISA_major_operation_bcall21] = ZLX_ISA_major_format_type_5_f,
	[ZLX_ISA_major_operation_blink21] = ZLX_ISA_major_format_type_5_f,
	[ZLX_ISA_major_operation_b21] = ZLX_ISA_major_format_type_5_f,
	[ZLX_ISA_major_operation_addpc] = ZLX_ISA_major_format_type_5,
	[ZLX_ISA_major_operation_get16] = ZLX_ISA_major_format_type_5_f,
	[ZLX_ISA_major_operation_get32] = ZLX_ISA_major_format_type_5_f_f,


	[ZLX_ISA_major_operation_addi] = ZLX_ISA_major_format_type_5_b5,
	[ZLX_ISA_major_operation_bneq] = ZLX_ISA_major_format_type_5_b5,
	[ZLX_ISA_major_operation_beq] = ZLX_ISA_major_format_type_5_b5,
	[ZLX_ISA_major_operation_blt] = ZLX_ISA_major_format_type_5_b5,
	[ZLX_ISA_major_operation_bge] = ZLX_ISA_major_format_type_5_b5,
	[ZLX_ISA_major_operation_bltu] = ZLX_ISA_major_format_type_5_b5,
	[ZLX_ISA_major_operation_bgeu] = ZLX_ISA_major_format_type_5_b5,
	[ZLX_ISA_major_operation_j] = ZLX_ISA_major_format_type_5,
	[ZLX_ISA_major_operation_jlink] = ZLX_ISA_major_format_type_5,
	[ZLX_ISA_major_operation_jcall] = ZLX_ISA_major_format_type_5,
	[ZLX_ISA_major_operation_jret] = ZLX_ISA_major_format_type_5,
	[ZLX_ISA_major_operation_add] = ZLX_ISA_major_format_type_5_5551,
	[ZLX_ISA_major_operation_sub] = ZLX_ISA_major_format_type_5_5551,
	[ZLX_ISA_major_operation_sll] = ZLX_ISA_major_format_type_5_5551,
	[ZLX_ISA_major_operation_srl] = ZLX_ISA_major_format_type_5_5551,
	[ZLX_ISA_major_operation_sra] = ZLX_ISA_major_format_type_5_5551,
	[ZLX_ISA_major_operation_xor] = ZLX_ISA_major_format_type_5_5551,
	[ZLX_ISA_major_operation_or] = ZLX_ISA_major_format_type_5_5551,
	[ZLX_ISA_major_operation_and] = ZLX_ISA_major_format_type_5_5551,
	[ZLX_ISA_major_operation_andn] = ZLX_ISA_major_format_type_5_5551,
	[ZLX_ISA_major_operation_clt] = ZLX_ISA_major_format_type_5_5551,
	[ZLX_ISA_major_operation_cge] = ZLX_ISA_major_format_type_5_5551,
	[ZLX_ISA_major_operation_cltu] = ZLX_ISA_major_format_type_5_5551,
	[ZLX_ISA_major_operation_cgeu] = ZLX_ISA_major_format_type_5_5551,
	[ZLX_ISA_major_operation_ceq] = ZLX_ISA_major_format_type_5_5551,
	[ZLX_ISA_major_operation_cneq] = ZLX_ISA_major_format_type_5_5551,
	[ZLX_ISA_major_operation_slli] = ZLX_ISA_major_format_type_5_b5,
	[ZLX_ISA_major_operation_srli] = ZLX_ISA_major_format_type_5_b5,
	[ZLX_ISA_major_operation_srai] = ZLX_ISA_major_format_type_5_b5,
	[ZLX_ISA_major_operation_xori] = ZLX_ISA_major_format_type_5_b5,
	[ZLX_ISA_major_operation_andi] = ZLX_ISA_major_format_type_5_b5,
	[ZLX_ISA_major_operation_ori] = ZLX_ISA_major_format_type_5_b5,
	[ZLX_ISA_major_operation_extb] = ZLX_ISA_major_format_type_5_b5,
	[ZLX_ISA_major_operation_sext] = ZLX_ISA_major_format_type_5_b5,
	[ZLX_ISA_major_operation_anda] = ZLX_ISA_major_format_type_5,
	[ZLX_ISA_major_operation_xora] = ZLX_ISA_major_format_type_5,
	[ZLX_ISA_major_operation_ora] = ZLX_ISA_major_format_type_5,
	[ZLX_ISA_major_operation_aand] = ZLX_ISA_major_format_type_5,
	[ZLX_ISA_major_operation_axor] = ZLX_ISA_major_format_type_5,
	[ZLX_ISA_major_operation_aor] = ZLX_ISA_major_format_type_5,
};

uint8_t ZLX_ISA_Major_operation_size[] = 
{
	[ZLX_ISA_major_format_type_5] = 1,
	[ZLX_ISA_major_format_type_55] = 1,
	[ZLX_ISA_major_format_type_5_5551] = 2,
	[ZLX_ISA_major_format_type_5_952] = 2,
	[ZLX_ISA_major_format_type_32] = 1,
	[ZLX_ISA_major_format_type_5_b5] = 2,
	[ZLX_ISA_major_format_type_5_f] = 2,
	[ZLX_ISA_major_format_type_5_f_f] = 3,
	[ZLX_ISA_major_format_type_5_655] = 2,
	[ZLX_ISA_major_format_type_5_53251] = 2
};



char *ZLX_ISA_opcodes[] = 
{
	[0] = ""
};




/* A class may declare its width or defer to its assigned suboperations.
 * subops == 0 means the low five bits are an operand for a declared class. */
typedef struct {
	ZLX_ISA_major_operation_t base;
	uint8_t subops;
	uint8_t parcels;
} MajorClass;

/* The ordinary operations preceding inc retain their 10-bit wire values. */
_Static_assert(ZLX_ISA_major_operation_inc >= 32 && ZLX_ISA_major_operation_inc <= 64,
               "adjust the suboperation pages when extending ordinary opcodes");
static const MajorClass major_classes[32] = {
	[0] = {0, 32, 0},
	[1] = {32, ZLX_ISA_major_operation_inc - 32, 0},
	[ZLX_ISA_major_class_inc] = {ZLX_ISA_major_operation_inc, 0, 1},
	[ZLX_ISA_major_class_dec] = {ZLX_ISA_major_operation_dec, 0, 1},
	[ZLX_ISA_major_class_shift_left] = {ZLX_ISA_major_operation_shift_left, 0, 1},
	[ZLX_ISA_major_class_shift_right] = {ZLX_ISA_major_operation_shift_right, 0, 1},
	[ZLX_ISA_major_class_shift_arith] = {ZLX_ISA_major_operation_shift_arith, 0, 1},
	[ZLX_ISA_major_class_get_pos] = {ZLX_ISA_major_operation_get_pos, 0, 1},
	[ZLX_ISA_major_class_get_neg] = {ZLX_ISA_major_operation_get_neg, 0, 1},
};

static ZLX_ISA_major_operation_t major_operation(MajorClass group, unsigned field)
{
	if (group.subops == 0 && group.parcels != 0)
		return group.base;
	if (field < group.subops)
		return group.base + field;
	return ZLX_ISA_major_operation_count;
}

uint8_t ZLX_ISA_parcel_count(uint16_t first)
{
	if (first & 1)
		return 1;
	MajorClass group = major_classes[first >> 11];
	if (group.parcels)
		return group.parcels;
	ZLX_ISA_major_operation_t op = major_operation(group, (first >> 6) & 31);
	if (op == ZLX_ISA_major_operation_count)
		return 0;
	return ZLX_ISA_Major_operation_size[ZLX_ISA_Major_operation_formats[op]];
}

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

			instruction->type = ZLX_ISA_instruction_type_minor;
			instruction->u__minor.operation = (ZLX_ISA_minor_operation_t)opcode;
			ZLX_ISA_minor_format_type_t type = ZLX_ISA_Minor_operation_formats[opcode];
			instruction->u__minor.type = type;
			*count = 1;

			switch(type)
			{
				case ZLX_ISA_minor_format_type_a:
					instruction->u__minor.u__format_a.aa = (first >> 1) & 0x3FF;
					break;
				case ZLX_ISA_minor_format_type_55:
					instruction->u__minor.u__format_55.a5 = (first >> 6) & 0x1F;
					instruction->u__minor.u__format_55.b5 = (first >> 1) & 0x1F;
					break;
				case ZLX_ISA_minor_format_type_28:
					instruction->u__minor.u__format_28.a2 = (first >> 9) & 0x3;
					instruction->u__minor.u__format_28.b8 = (first >> 1) & 0xFF;
					break;
				case ZLX_ISA_minor_format_type_235:
					instruction->u__minor.u__format_235.a2 = (first >> 9) & 0x3;
					instruction->u__minor.u__format_235.b3 = (first >> 6) & 0x7;
					instruction->u__minor.u__format_235.c5 = (first >> 1) & 0x1F;
					break;
				default:
					*count = 0;
					return false;
			}
			instruction->size = *count;
			return true;
		}

		unsigned field = (first >> 6) & 31;
		ZLX_ISA_major_operation_t op = major_operation(major_classes[first >> 11], field);
		*count = ZLX_ISA_parcel_count(first);
		if (*count == 0 || op == ZLX_ISA_major_operation_count ||
		    (field == 0 && (op == ZLX_ISA_major_operation_shift_left ||
		                    op == ZLX_ISA_major_operation_shift_right ||
		                    op == ZLX_ISA_major_operation_shift_arith)))
		{
			*count = 0;
			return false;
		}
		instruction->type = ZLX_ISA_instruction_type_major;
		instruction->u__major.operation = op;
		instruction->u__major.type = ZLX_ISA_Major_operation_formats[op];
		if(*count > 1)
		{
			return false;
		}
	}
	else
	{
		// The caller appends parcels in order. Decode when the final one arrives.
		if(instruction->type != ZLX_ISA_instruction_type_major || position + 1 != *count)
		{
			return false;
		}
	}

	uint16_t second = *count > 1 ? parcels[1].parcel : 0;
	uint16_t third = *count > 2 ? parcels[2].parcel : 0;
	uint8_t a5 = (first >> 1) & 0x1F;

	switch(instruction->u__major.type)
	{
		case ZLX_ISA_major_format_type_55:
			instruction->u__major.u__format_55.a5 = (first >> 6) & 31;
			instruction->u__major.u__format_55.b5 = a5;
			break;
		case ZLX_ISA_major_format_type_5:
			instruction->u__major.u__foramt_5.a5 = a5;
			break;
		case ZLX_ISA_major_format_type_32:
			instruction->u__major.u__foramt_32.a3 = (first >> 3) & 0x7;
			instruction->u__major.u__foramt_32.b2 = (first >> 1) & 0x3;
			break;
		case ZLX_ISA_major_format_type_5_5551:
			instruction->u__major.u__format_5_5551.a5 = a5;
			instruction->u__major.u__format_5_5551.b5 = (second >> 11) & 0x1F;
			instruction->u__major.u__format_5_5551.c5 = (second >> 6) & 0x1F;
			instruction->u__major.u__format_5_5551.d5 = (second >> 1) & 0x1F;
			instruction->u__major.u__format_5_5551.e1 = second & 1;
			break;
		case ZLX_ISA_major_format_type_5_952:
			instruction->u__major.u__format_5_952.a5 = a5;
			instruction->u__major.u__format_5_952.b9 =
				((second >> 11) & 0x1F) | (((second >> 6) & 0x7) << 5) | ((second & 1) << 8);
			instruction->u__major.u__format_5_952.c5 = (second >> 1) & 0x1F;
			instruction->u__major.u__format_5_952.d2 = (second >> 9) & 0x3;
			break;
		case ZLX_ISA_major_format_type_5_b5:
			instruction->u__major.u__format_5_b5.a5 = a5;
			instruction->u__major.u__format_5_b5.b11 =
				((second >> 11) & 0x1F) | (((second >> 6) & 0x1F) << 5) | ((second & 1) << 10);
			instruction->u__major.u__format_5_b5.c5 = (second >> 1) & 0x1F;
			break;
		case ZLX_ISA_major_format_type_5_f:
			instruction->u__major.u__format_5_f.a5 = a5;
			instruction->u__major.u__format_5_f.bf = second;
			break;
		case ZLX_ISA_major_format_type_5_f_f:
			instruction->u__major.u__format_5_f_f.a5 = a5;
			instruction->u__major.u__format_5_f_f.bf = second;
			instruction->u__major.u__format_5_f_f.cf = third;
			break;
		case ZLX_ISA_major_format_type_5_655:
			instruction->u__major.u__format_5_655.a5 = a5;
			instruction->u__major.u__format_5_655.b6 = ((second >> 11) & 0x1F) | ((second & 1) << 5);
			instruction->u__major.u__format_5_655.c5 = (second >> 6) & 0x1F;
			instruction->u__major.u__format_5_655.d5 = (second >> 1) & 0x1F;
			break;
		case ZLX_ISA_major_format_type_5_53251:
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
