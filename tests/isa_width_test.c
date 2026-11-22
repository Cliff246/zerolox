#include <assert.h>
#include <stdint.h>
#include "isa/ISA_zerolox.h"

int main(void)
{
	for (unsigned word = 0; word <= UINT16_MAX; ++word)
	{
		unsigned group = word >> 11;
		unsigned opcode = word >> 6;
		unsigned field = opcode & 31;
		unsigned width = 0;
		bool legal = false;
		if (word & 1)
		{
			width = 1;
			legal = group <= ZLX_ISA_minor_operation_bineqc_update ||
			        group == ZLX_ISA_minor_operation_calli;
		}
		else if (opcode <= ZLX_ISA_major_operation_extb)
		{
			width = ZLX_ISA_Major_operation_size[ZLX_ISA_Major_operation_formats[opcode]];
			legal = true;
		}
		else if (group >= ZLX_ISA_major_class_inc && group <= ZLX_ISA_major_class_get_neg)
		{
			width = 1;
			bool shift = group >= ZLX_ISA_major_class_shift_left &&
			             group <= ZLX_ISA_major_class_shift_arith;
			legal = !shift || field != 0;
		}

		ZLX_ISA_Parcel_t parcels[8] = {{(uint16_t)word}};
		ZLX_ISA_Instruction_t instruction;
		int count = 0;
		assert(ZLX_ISA_parcel_count(word) == width);
		bool complete = ZLX_ISA_decode_instruction(&instruction, parcels, &count, 0);
		assert(count == (legal ? (int)width : 0));
		assert(complete == (legal && width == 1));
		for (int position = 1; position < count; ++position)
		{
			complete = ZLX_ISA_decode_instruction(&instruction, parcels, &count, position);
			assert(complete == (position + 1 == (int)width));
		}
		if (legal)
			assert(instruction.size == width);
	}
	return 0;
}
