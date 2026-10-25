#include "ISA_zerolox.h"
#include "ISA_syntax.h"


char *ZLX_ISA_major_mnemonic[] = 
{

};
char *ZLX_ISA_minor_mnemonic[] = 
{
	[ZLX_ISA_Minor_operation_move] = "mv",
	[ZLX_ISA_Minor_operation_add] = "add",
	[ZLX_ISA_Minor_operation_adda] = "adda",
	[ZLX_ISA_Minor_operation_sub] = "sub",
	[ZLX_ISA_Minor_operation_geta] = "geta", 
	[ZLX_ISA_Minor_operation_get8] = "get8",
	[ZLX_ISA_Minor_operation_b] = "b", 
	[ZLX_ISA_Minor_operation_baeqz] = "baeqz",
	[ZLX_ISA_Minor_operation_baneqz] = "baneqz",
	[ZLX_ISA_Minor_operation_lda] = "lda",  
	[ZLX_ISA_Minor_operation_ald] = "ald",
	[ZLX_ISA_Minor_operation_sta] = "sta",
	[ZLX_ISA_Minor_operation_ast] = "ast",	
	[ZLX_ISA_Minor_operation_stsa] = "stsa",
	[ZLX_ISA_Minor_operation_ldsa] = "ldsa",
	[ZLX_ISA_Minor_operation_ld_plus] = "ld+a", 
	[ZLX_ISA_Minor_operation_st_plus] = "st+a",  
	[ZLX_ISA_Minor_operation_bieqc] = "bieqc",
	[ZLX_ISA_Minor_operation_bteqa] = "bteqa",
	[ZLX_ISA_Minor_operation_btneqa] = "btneqa",

};



