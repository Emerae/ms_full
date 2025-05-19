#include "parser_new.h"

static void	cy2_fill_builtin_id_1(t_cmd **current_cmd)
{
	if (cy_strcmp((*current_cmd)->args[0], "cd") == 0)
		(*current_cmd)->builtin_id = 3;
	else if (cy_strcmp((*current_cmd)->args[0], "pwd") == 0)
		(*current_cmd)->builtin_id = 4;
	else if (cy_strcmp((*current_cmd)->args[0], "export") == 0)
		(*current_cmd)->builtin_id = 5;
	else if (cy_strcmp((*current_cmd)->args[0], "unset") == 0)
		(*current_cmd)->builtin_id = 6;
	else if (cy_strcmp((*current_cmd)->args[0], "env") == 0)
		(*current_cmd)->builtin_id = 7;
	else if (cy_strcmp((*current_cmd)->args[0], "exit") == 0)
		(*current_cmd)->builtin_id = 8;
}

void	cy2_fill_builtin_id(t_cmd **current_cmd)
{
	if (current_cmd && *current_cmd)
		*current_cmd = (*current_cmd)->next;
	if (!(*current_cmd)->args || !(*current_cmd)->args[0])
		return ;
	if (cy_strcmp((*current_cmd)->args[0], "echo") == 0)
	{
		if ((*current_cmd)->args[1]
			&& cy_strcmp((*current_cmd)->args[1], "-n") == 0)
			(*current_cmd)->builtin_id = 1;
		else
			(*current_cmd)->builtin_id = 2;
	}
	else
		cy2_fill_builtin_id_1(current_cmd);
}
