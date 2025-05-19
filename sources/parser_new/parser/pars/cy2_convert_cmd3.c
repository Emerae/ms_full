#include "parser_new.h"

void	append_cmd3(t_cmd *new_cmd, t_cmd **current_cmd)
{
	t_cmd	*last;

	if (!*current_cmd)
		*current_cmd = new_cmd;
	else
	{
		last = *current_cmd;
		while (last->next)
			last = last->next;
		last->next = new_cmd;
	}
	printf("append_cmd3: new_cmd->args[0] = %s\n", new_cmd->args[0]);
}

int	append_cmd2(t_cmd *new_cmd, int n_delimiter, t_input **input_node)
{
	int	i;

	i = 0;
	while (i < n_delimiter && *input_node)
	{
		new_cmd->args[i] = cy_true_strdup((*input_node)->input);
		if (!new_cmd->args[i])
			return (1);
		*input_node = (*input_node)->next;
		i = i + 1;
	}
	new_cmd->args[i] = NULL;
	new_cmd->redirs = NULL;
	new_cmd->builtin_id = -1;
	new_cmd->next = NULL;
	return (0);
}

int	append_cmd1(t_cmd **new_cmd, int n_delimiter)
{
	*new_cmd = malloc(sizeof(t_cmd));
	if (!*new_cmd)
		return (1);
	(*new_cmd)->args = malloc(sizeof(char *) * (n_delimiter + 1));
	if (!(*new_cmd)->args)
	{
		free(*new_cmd);
		return (1);
	}
	return (0);
}

int	append_cmd(t_cmd **current_cmd, int n_delimiter, t_input **head_input)
{
	t_cmd	*new_cmd;
	t_input	*input_node;

	input_node = *head_input;
	if (append_cmd1(&new_cmd, n_delimiter))
		return (1);
	if (append_cmd2(new_cmd, n_delimiter, &input_node))
	{
		free(new_cmd->args);
		free(new_cmd);
		return (1);
	}
	append_cmd3(new_cmd, current_cmd);
	*head_input = input_node;
	return (0);
}

int	cy2_convert_cmd2(t_cmdconvert *c)
{
	if (c->nature_delimiter != 1)
		return (1);
	c->skip_nb = cy2_fill_redir(&c->current_cmd,
			&c->current_input, &c->nature_delimiter);
	if (c->skip_nb == 0)
	{
		cy0_free_cmd_list(c->head_cmd);
		return (0);
	}
	while (c->skip_nb > 0)
	{
		c->head_input = c->head_input->next;
		c->skip_nb = c->skip_nb - 1;
	}
	return (1);
}
