#include "ISA_zerolox.h"


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

char *ZLX_ISA_opcodes[] = 
{
	[0] = ""
};

bool ZLX_ISA_decode_instruction(ZLX_ISA_Instruction_t *instruction, ZLX_ISA_Parcel_t parcels[8], int *count, int position)
{
	if(*count)
	{

	}	



	return false;;
}

