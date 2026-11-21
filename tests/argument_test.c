#include <assert.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include "ASM_Argument.h"
#include "MY_commons.h"

int main(int _argc, char **_argv)
{
	unsigned int bits = MY_SET_BIT(0u, 0);
	assert(bits == 1u);
	bits = MY_SET_BIT(bits, sizeof(bits) * CHAR_BIT - 1);
	assert(MY_GET_BIT(bits, 0) == 1u);
	assert(MY_GET_BIT(bits, sizeof(bits) * CHAR_BIT - 1) == 1u);
	bits = MY_CLR_BIT(bits, 0);
	assert(MY_GET_BIT(bits, 0) == 0u);
	bits = MY_CLR_BIT(bits, sizeof(bits) * CHAR_BIT - 1);
	assert(bits == 0u);
	assert(MY_SET_BIT(1u | 2u, 1 + 1) == 7u);
	assert(MY_CLR_BIT(1u | 2u, 0) == 2u);
	assert(MY_GET_BIT(1u | 2u, 0) == 1u);

	ZLX_ASM_Argument_group_t *group = ZLX_ASM_init_Argument_group();
	ZLX_ASM_Argument_t argument = {.u__number = 10};
	assert(group->first_argument == -1);
	assert(ZLX_ASM_set_first_argument_Argument_group(group, argument) == 0);
	assert(group->enabled == 1);
	assert(group->arguments_count == 1);

	if(_argc > 1)
	{
		if(strcmp(_argv[1], "invalid_head") == 0)
			ZLX_ASM_add_child_Argument_group(group, ZLX_ASM_MAX_ARGUMENTS, argument);
		else if(strcmp(_argv[1], "negative_head") == 0)
			ZLX_ASM_add_neigbour_Argument_group(group, -1, argument);
		else if(strcmp(_argv[1], "inactive_head") == 0)
			ZLX_ASM_add_child_Argument_group(group, 1, argument);
		else if(strcmp(_argv[1], "null_group") == 0)
			ZLX_ASM_add_child_Argument_group(NULL, 0, argument);
		else if(strcmp(_argv[1], "duplicate_root") == 0)
			ZLX_ASM_set_first_argument_Argument_group(group, argument);
		else
			assert(strcmp(_argv[1], "full") == 0);
	}

	for(int i = 1; i < ZLX_ASM_MAX_ARGUMENTS; ++i)
	{
		argument.u__number = 10 + i;
		int index;
		if(i % 2)
			index = ZLX_ASM_add_child_Argument_group(group, i - 1, argument);
		else
			index = ZLX_ASM_add_neigbour_Argument_group(group, i - 1, argument);
		assert(index == i);
		assert(group->arguments_count == i + 1);
		assert(group->a__arguments[i - 1].u__number == 10 + i - 1);
		assert(group->a__tree[i][0] == -1 && group->a__tree[i][1] == -1);
		assert(group->a__tree[i - 1][i % 2 ? ZLX_ASM_ARGUMENT_FIRST_CHILD : ZLX_ASM_ARGUMENT_NEXT_NEIGHBOUR] == i);
	}
	assert(group->enabled == (1u << ZLX_ASM_MAX_ARGUMENTS) - 1u);
	if(_argc > 1 && strcmp(_argv[1], "full") == 0)
		ZLX_ASM_add_neigbour_Argument_group(group, ZLX_ASM_MAX_ARGUMENTS - 1, argument);
	free(group);
	return 0;
}
