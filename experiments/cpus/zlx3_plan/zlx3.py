
from enum import Enum

def fits5bits(val: int):
	if(val < 0 or val > 31):
		raise Exception("fits5bits failed %d" % val)
	return val

class ParamaterType(Enum):
	INVALID = 0
	REGISTER = 1
	IMMEDIATE = 2
	LATCH = 3
#[6][5] [7*3]
#[opcode][extra][paramater1][paramater2][paramater2]


class MicroOp(Enum):
	INVALID = 0 #don't use
	LD = 1 		#load (extra field)
	ST = 2		#store (extra fields)
	ADD = 3		#add
	SUB = 4		#sub
	SLL = 5		#shift left logical
	SRL = 6		#shift right logical
	SRA = 7		#shift right arithmetic
	CEQ = 8		#compare equal
	CNE = 9		#compare not equal
	CLT = 10	#compare less than
	CGE = 11	#compare greater equal
	CLTU = 12	#compare less than unsigned
	CGEU = 13	#compare greater equal unsigned
	AND = 14	#and
	XOR = 15	#xor
	OR = 16		#or
	JMP = 17	#jmp
	JMPC = 18	#jmpc


#convert zerolox to this
class MicroInstruction:
	def __init__(self, op: MicroOp, extra: int, dst_type: ParamaterType, dst_val: int, src1_type: ParamaterType, src1_val: int, src2_type: ParamaterType, src2_val):
		if(extra < 0 or extra > 31):
			raise Exception("invalid extra range")
		if(dst_val < 0 or dst_val > 31):
			raise Exception("invalid dst value")
		if(src1_val < 0 or src1_val > 31):
			raise Exception("invalid src1 value")
		if(src2_val < 0 or src2_val > 31):
			raise Exception("invalid src2 value")

		self.op = op
		self.extra = extra
		self.dst_type = dst_type
		self.dst_val = dst_val
		self.src1_type = src1_type
		self.src1_val = src1_val
		self.src2_type = src2_type
		self.src2_val = src2_val


class CPU:


	def __init__(self):
		self.immediates = [0] * 32
		self.registers: list[int] = [0] * 32
		self.latches = [0] * 32
		self.instructions :list[MicroInstruction] = [] * 32
		self.pc = 0

	def get_paramater(self, ptype: ParamaterType, pval):
		if(ptype == ParamaterType.REGISTER):
			return self.registers[fits5bits(pval)]
		elif(ptype == ParamaterType.IMMEDIATE):
			return self.immediates[fits5bits(pval)]
		elif(ptype == ParamaterType.LATCH):
			return self.latches[fits5bits(pval)]
		else:
			raise Exception("invalid paramater")

	def step(self):

		op = self.instructions[self.pc]





