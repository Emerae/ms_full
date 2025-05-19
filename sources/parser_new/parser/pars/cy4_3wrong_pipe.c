#include "parser_new.h"

int	cy4_3contains_pipe_ch(char *input)
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

int	cy4_3wrong_pipea(t_input *node)
{
	if (node->input[0] != '|')
		return (1);
	if (node->input[1] == '\0')
		return (0);
	return (1);
}

int	cy4_3wrong_pipe(t_input *head)
{
	t_input	*node;
	int		r;

	r = 0;
	node = head;
	while (node)
	{
		if (node->input)
		{
			if (node->input && r > 0)
			{
				if (r == 1 && node->type == 2)
					return (1);
			}
		}
		node = node->next;
	}
	return (0);
}
