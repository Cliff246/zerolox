#include "ASM_Argument.h"
#include "MY_commons.h"

#include <stdlib.h>


static char get_first_child(ZLX_ASM_Argument_group_t *group, int index)
{
	if(!group || index < 0)
		MY_FAULT("bad paramaters");
	if(index > group->arguments_count)
		MY_FAULT("index > arguments count");	


	char next_neigbour = group->a__tree[index][ZLX_ASM_ARGUMENT_NEXT_NEIGHBOUR];	
	char first_child = group->a__tree[index][ZLX_ASM_ARGUMENT_FIRST_CHILD];

	return first_child;	
}

static char get_next_argument(ZLX_ASM_Argument_group_t *group, int index)
{
	if(!group || index < 0)
		MY_FAULT("bad paramaters");
	if(index > group->arguments_count)
		MY_FAULT("index > arguments count");	


	char next_neigbour = group->a__tree[index][ZLX_ASM_ARGUMENT_NEXT_NEIGHBOUR];	
	char first_child = group->a__tree[index][ZLX_ASM_ARGUMENT_FIRST_CHILD];

	return next_neigbour;
}

static bool get_argument_enabled(ZLX_ASM_Argument_group_t *group, int index)
{
	if(index > ZLX_ASM_MAX_ARGUMENTS || index < 0)
		MY_FAULT("index out of bounds");
	bool result = MY_GET_BIT(group->enabled, index);
	return result;
}

static void set_argument_enabled(ZLX_ASM_Argument_group_t *group, int index)
{
	if(index > ZLX_ASM_MAX_ARGUMENTS || index < 0)
		MY_FAULT("index out of bounds");
	group->enabled = MY_SET_BIT(group->enabled, index);

}

int ZLX_ASM_set_first_argument_Argument_group(ZLX_ASM_Argument_group_t *_group, ZLX_ASM_Argument_t _argument)
{
	if(_group->first_argument != -1)
		MY_FAULT("must be init once");
	_group->arguments_count = 1;
	_group->first_argument = 0;
	_group->a__arguments[0] = _argument;
	set_argument_enabled(_group, 0);
	


	return 0;
}	

int ZLX_ASM_add_neigbour_Argument_group(ZLX_ASM_Argument_group_t *_group, int head, ZLX_ASM_Argument_t _argument)
{
	if(!get_argument_enabled(_group, head))
		MY_FAULT("head is not enabled");
	int first_free = -1;
	//i need to study binary tree's fuck
	for(int i = 0; i < ZLX_ASM_MAX_ARGUMENTS; ++i)
	{
		if(get_argument_enabled(_group, i))
		{
			first_free = i;
			break;	
		}
	}
	if(first_free == -1)
	{
		MY_FAULT("must have open space");
	}	
	

	set_argument_enabled(_group, first_free);
	_group->a__tree[head][ZLX_ASM_ARGUMENT_NEXT_NEIGHBOUR] = first_free;
	_group->a__arguments[first_free] = _argument;
	


	return first_free;
}
int ZLX_ASM_add_child_Argument_group(ZLX_ASM_Argument_group_t *_group, int head, ZLX_ASM_Argument_t _argument)
{
	if(!get_argument_enabled(_group, head))
		MY_FAULT("head is not enabled");
	int first_free = -1;
	//i need to study binary tree's fuck
	for(int i = 0; i < ZLX_ASM_MAX_ARGUMENTS; ++i)
	{
		if(get_argument_enabled(_group, i))
		{
			first_free = i;
			break;	
		}
	}
	if(first_free == -1)
	{
		MY_FAULT("must have open space");
	}	
	

	set_argument_enabled(_group, first_free);
	_group->a__tree[head][ZLX_ASM_ARGUMENT_FIRST_CHILD] = first_free;
	_group->a__arguments[first_free] = _argument;
	


	return first_free;
}


ZLX_ASM_Argument_group_t *ZLX_ASM_init_Argument_group()
{
	

	ZLX_ASM_Argument_group_t *group = calloc(1, sizeof(ZLX_ASM_Argument_group_t));
	if(!group)
	   MY_FAULT("calloc failed");	

	for(int a = 0; a < ZLX_ASM_MAX_ARGUMENTS; ++a)
	{
		group->a__tree[a][0] = -1;
		group->a__tree[a][1] = -1;

	}
	group->arguments_count = 0;
	group->enabled = 0x00000000;
	group->first_argument = -1;
	return group;
}

void ZLX_ASM_free_Argument(ZLX_ASM_Argument_t *argument)
{
	if(!argument)
		return;

	// Text payload ownership is not defined yet; do not free up__string here.
	free(argument);
}
