#include "../prser.h"

int	cy4_4contains_redir_ch(char *input)
{
	int	i;

	i = 0;
	while (input && input[i])
	{
		if (input[i] == '>')
			return (1);
		if (input[i] == '<')
			return (2);
		i = i + 1;
	}
	return (0);
}

int	cy4_4contains_pipe_ch(char *input)
{
	int	i;

	i = 0;
	while (input && input[i])
	{
		if (input[i] == '|')
			return (1);
		i = i + 1;
	}
	return (0);
}

int	cy4_4wrong_redir_log2(t_input *next_node)
{
	if (!next_node || !next_node->input)
		return (1);
	if (cy4_4contains_pipe_ch(next_node->input)
		|| cy4_4contains_redir_ch(next_node->input))
	{
		return (1);
	}
	next_node = next_node->next;
	if (!next_node || !next_node->input)
		return (0);
	if (!cy4_4contains_pipe_ch(next_node->input)
		&& !cy4_4contains_redir_ch(next_node->input))
	{
		return (1);
	}
	return (0);
}

int	cy4_4wrong_redir_log(t_input *head)
{
	t_input	*current;

	current = head;
	while (current)
	{
		if (current->input && cy4_4contains_redir_ch(current->input)
			&& current->type == 2)
		{
			if (cy4_4wrong_redir_log2(current->next))
				return (1);
		}
		current = current->next;
	}
	return (0);
}
