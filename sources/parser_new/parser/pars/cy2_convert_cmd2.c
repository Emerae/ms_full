#include "../prser.h"

t_cmd	*init_cmd(void)
{
	t_cmd	*cmd;

	cmd = malloc(sizeof(t_cmd));
	if (!cmd)
		return (NULL);
	cmd->args = NULL;
	cmd->redirs = NULL;
	cmd->builtin_id = 0;
	cmd->next = NULL;
	return (cmd);
}

int	cy_add_empty_cmd_node(t_cmd *head_cmd)
{
	t_cmd	*new_node;
	t_cmd	*current;

	new_node = malloc(sizeof(t_cmd));
	if (!new_node)
		return (1);
	new_node->args = NULL;
	new_node->redirs = NULL;
	new_node->builtin_id = -1;
	new_node->next = NULL;
	current = head_cmd;
	while (current->next)
		current = current->next;
	current->next = new_node;
	return (0);
}

int	cy2_convert_cmd1b(t_cmdconvert *c)
{
	c->n_delimiter = find_delim(&c->current_input, &c->nature_delimiter);
	if (c->n_delimiter == -1)
	{
		if (cy_add_empty_cmd_node(c->head_cmd))
		{
			cy0_free_cmd_list(c->head_cmd);
			return (0);
		}
		return (1);
	}
	if (append_cmd(&c->current_cmd, c->n_delimiter, &c->head_input))
	{
		cy0_free_cmd_list(c->head_cmd);
		return (0);
	}
	return (-1);
}

int	cy2_convert_cmd1a(t_cmdconvert *c)
{
	int	ret;

	while (1)
	{
		ret = cy2_convert_cmd1b(c);
		if (ret != -1)
			return (ret);
		cy2_fill_builtin_id(&c->current_cmd);
		if (!cy2_convert_cmd2(c))
			return (0);
		if (c->nature_delimiter == 3)
			break ;
		if (c->nature_delimiter == 2)
		{
			c->head_input = c->head_input->next;
			c->current_input = c->current_input->next;
		}
	}
	return (1);
}

t_cmd	*cy2_convert_cmd(t_input *head_input)
{
	t_cmdconvert	c;

	c.head_cmd = init_cmd();
	if (!c.head_cmd)
	{
		printf("Failed alloc for head_cmd\n");
		return (NULL);
	}
	c.current_cmd = c.head_cmd;
	c.head_input = head_input;
	c.current_input = head_input;
	c.n_delimiter = 0;
	c.nature_delimiter = 0;
	if (!cy2_convert_cmd1a(&c))
		return (NULL);
	cy2_free_first_cmd_node(&c.head_cmd);
	return (c.head_cmd);
}
// nature_delimiter : numero ou NULL ou > ou |;
// 0 = Problem 1 = Redir , 2 = Pipe , 3 = NULL;
// nature 1 : redir , nature 2 : pipe , nature 3 : NULL