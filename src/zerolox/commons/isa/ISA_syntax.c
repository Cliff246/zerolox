#include "MY_commons.h"
#include "ISA_zerolox.h"
#include "ISA_syntax.h"

#include <stdlib.h>
#include <sys/_types/_sigaltstack.h>

ZLX_ISA_Statement_t *ZLX_ISA_init_Statement(void)
{
	ZLX_ISA_Statement_t *statement = malloc(sizeof(ZLX_ISA_Statement_t));
	if(!statement)
		MY_FAULT("malloc failed");





	return statement;
}

ZLX_ISA_Mnemonic_t ZLX_ISA_major_mnemonic[] = 
{

};


ZLX_ISA_Mnemonic_t ZLX_ISA_minor_mnemonic[] = 
{
	[ZLX_ISA_Minor_operation_move] = {.form = ZLX_ISA_Pattern_form_2_register, .a__mnemonic = "mv"},
	[ZLX_ISA_Minor_operation_add] = {.a__mnemonic = "add"},
	[ZLX_ISA_Minor_operation_adda] = {.a__mnemonic = "adda"},
	[ZLX_ISA_Minor_operation_sub] = {.a__mnemonic = "sub"},
	[ZLX_ISA_Minor_operation_geta] = {.a__mnemonic = "geta"}, 
	[ZLX_ISA_Minor_operation_get8] = {.a__mnemonic = "get8"},
	[ZLX_ISA_Minor_operation_b] = {.a__mnemonic = "b"}, 
	[ZLX_ISA_Minor_operation_baeqz] = {.a__mnemonic = "baeqz"},
	[ZLX_ISA_Minor_operation_baneqz] = {.a__mnemonic = "baneqz"},
	[ZLX_ISA_Minor_operation_lda] = {.a__mnemonic = "lda"},  
	[ZLX_ISA_Minor_operation_ald] = {.a__mnemonic = "ald"},
	[ZLX_ISA_Minor_operation_sta] = {.a__mnemonic = "sta"},
	[ZLX_ISA_Minor_operation_ast] = {.a__mnemonic = "ast"},	
	[ZLX_ISA_Minor_operation_stsa] = {.a__mnemonic = "stsa"},
	[ZLX_ISA_Minor_operation_ldsa] = {.a__mnemonic = "ldsa"},
	[ZLX_ISA_Minor_operation_ld_plus] = {.a__mnemonic = "ldpa"}, 
	[ZLX_ISA_Minor_operation_st_plus] = {.a__mnemonic = "stpa"},  
	[ZLX_ISA_Minor_operation_bieqc] = {.a__mnemonic = "bieqc"},
	[ZLX_ISA_Minor_operation_bteqa] = {.a__mnemonic = "bteqa"},
	[ZLX_ISA_Minor_operation_btneqa] = {.a__mnemonic = "btneqa"},

};





