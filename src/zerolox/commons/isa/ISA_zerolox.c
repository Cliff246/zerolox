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
	[ZLX_ISA_Major_operation_ld] = ZLX_ISA_Major_format_type_5_b5 ,
	[ZLX_ISA_Major_operation_st] = ZLX_ISA_Major_format_type_5_b5 ,
	[ZLX_ISA_Major_operation_push8] = ZLX_ISA_Major_format_type_5,
	[ZLX_ISA_Major_operation_push16] = ZLX_ISA_Major_format_type_5,
	[ZLX_ISA_Major_operation_push32] = ZLX_ISA_Major_format_type_5,
	[ZLX_ISA_Major_operation_pushp] = ZLX_ISA_Major_format_type_5,
	[ZLX_ISA_Major_operation_pop8] = ZLX_ISA_Major_format_type_5,
	[ZLX_ISA_Major_operation_pop16] = ZLX_ISA_Major_format_type_5,
	[ZLX_ISA_Major_operation_pop32] = ZLX_ISA_Major_format_type_5,
	[ZLX_ISA_Major_operation_popp] = ZLX_ISA_Major_format_type_5,
	[ZLX_ISA_Major_operation_get16] = ZLX_ISA_Major_format_type_5,
	[ZLX_ISA_Major_operation_get32] = ZLX_ISA_Major_format_type_5,
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

	
	if(position == 0)
	{
		ZLX_ISA_Parcel_t parcel = parcels[0];				

		bool is_major_or_minor = parcel.parcel & 1;
		if(is_major_or_minor == true)
		{
			//minor is false
			instruction->type = ZLX_ISA_Minor_type;
			*count = 1;
			ZLX_ISA_Minor_operation_t op = parcel.parcel & 61440;

			instruction->u__minor.operation = op; 

			ZLX_ISA_Minor_format_type_t type = ZLX_ISA_Minor_operation_formats[op]; 
			instruction->u__minor.type = type;	
			switch(type)
			{
				case ZLX_ISA_Minor_format_type_a:
					instruction->u__minor.u__format_a.aa = parcel.parcel & 2046; 
					break;	
				case ZLX_ISA_Minor_format_type_55:

					instruction->u__minor.u__format_55.a5 = parcel.parcel & 1984; 
					instruction->u__minor.u__format_55.b5 = parcel.parcel & 46; 
					break;
				case ZLX_ISA_Minor_format_type_28:
					instruction->u__minor.u__format_28.a2 = parcel.parcel & 1536; 
					instruction->u__minor.u__format_28.b8 = parcel.parcel & 1022; 
					break;
				case ZLX_ISA_Minor_format_type_235:
					instruction->u__minor.u__format_235.a2 = parcel.parcel & 1536; 
					instruction->u__minor.u__format_235.b3 = parcel.parcel & 448; 
					instruction->u__minor.u__format_235.c5 = parcel.parcel & 46; 
					break;


				default:
					printf("fucked %d\n", type);
					exit(1);
			}

			return true;


		}
		else
		{
			instruction->type = ZLX_ISA_Major_type;

		
			ZLX_ISA_Major_operation_t op = parcel.parcel & 65504;
			
			ZLX_ISA_Major_format_type_t type = ZLX_ISA_Major_operation_formats[op];

			instruction->u__major.operation = op;		
			instruction->u__major.type = type;


			*count = 3 ;
		}
	}
	else
	{


	}
	return false;;
}




