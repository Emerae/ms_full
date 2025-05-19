#include "../prser.h"

int	cy4_2contains_redir_ch(char *input)
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

int	cy4_2wrong_redira(t_input *node)
{
	if (node->input[0] != '<' && node->input[0] != '>')
		return (1);
	if (node->input[1] == '\0')
		return (0);
	if (node->input[0] == node->input[1])
	{
		if (node->input[2] == '\0')
			return (0);
	}
	return (1);
}

int	cy4_2wrong_redir(t_input *head)
{
	t_input	*node;
	int		r;

	r = 0;
	node = head;
	while (node)
	{
		if (node->input)
		{
			r = cy4_2contains_redir_ch(node->input);
			if (node->input && r > 0)
			{
				r = cy4_2wrong_redira(node);
				if (r == 1 && node->type == 2)
					return (1);
			}
		}
		node = node->next;
	}
	return (0);
}
