#include "../prser.h"

int	cy4_5contains_pipe_ch(char *input)
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

int	cy4_5wrong_pipe_log(t_input *head)
{
	t_input	*current;

	current = head;
	while (current)
	{
		if (cy4_5contains_pipe_ch(current->input))
		{
			if (!current->prev || cy4_5contains_pipe_ch(current->prev->input))
				return (1);
			if (current->next && cy4_5contains_pipe_ch(current->next->input)
				&& current->next->type == 2)
			{
				return (1);
			}
		}
		current = current->next;
		printf("de\n");
	}
	return (0);
}
