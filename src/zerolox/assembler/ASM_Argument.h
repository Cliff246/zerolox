#ifndef ZLX_ASM_ARGUMENT_HEADER
#define ZLX_ASM_ARGUMENT_HEADER

#include <assert.h>
#include <limits.h>
#include <stdint.h>
#include <stdbool.h>

#include <stdint.h>
#define ZLX_ASM_MAX_ARGUMENTS 8
#define ZLX_ASM_ARGUMENT_NEXT_NEIGHBOUR 0
#define ZLX_ASM_ARGUMENT_FIRST_CHILD 1

static_assert(ZLX_ASM_MAX_ARGUMENTS < sizeof(int) * CHAR_BIT, "max arguments determined by bit flag" );

typedef enum ZLX_ASM_argument_type
{
	ZLX_ASM_argument_type_number,
	ZLX_ASM_argument_type_register,
	ZLX_ASM_argument_type_symbol,
}ZLX_ASM_argument_type_t;

//this could be actually so bad
typedef struct ZLX_ASM_Argument
{
	ZLX_ASM_argument_type_t type;
	
	
	char contents[16];
	
	union 
	{
		int u__number;
		int u__register;
		int u__symbol;
	};

}ZLX_ASM_Argument_t;





//we need a tree 
//
//
//	[entity]
//	[next_neighbour][first_child]
//




typedef struct ZLX_ASM_Argument_group
{
	
	//tree of positions
	char first_argument;
	char arguments_count;
	int enabled;
	char a__tree[ZLX_ASM_MAX_ARGUMENTS][2];	

	ZLX_ASM_Argument_t a__arguments[ZLX_ASM_MAX_ARGUMENTS]; 

}ZLX_ASM_Argument_group_t;



int ZLX_ASM_set_first_argument_Argument_group(ZLX_ASM_Argument_group_t *_group, ZLX_ASM_Argument_t _argument); 
int ZLX_ASM_add_neigbour_Argument_group(ZLX_ASM_Argument_group_t *_group, int head, ZLX_ASM_Argument_t _argument);
int ZLX_ASM_add_child_Argument_group(ZLX_ASM_Argument_group_t *_group, int head, ZLX_ASM_Argument_t _argument);


ZLX_ASM_Argument_group_t *ZLX_ASM_init_Argument_group();

void ZLX_ASM_free_Argument_group(ZLX_ASM_Argument_group_t *_group);

#endif
