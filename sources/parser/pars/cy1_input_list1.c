#include "parser_new.h"

int	cy1_append_input(t_input **head, int start, int end, char *input)
{
	t_input	*new_node;
	t_input	*tmp;
	int		i;

	new_node = cy1_create_node(start, end, input);
	if (!new_node)
		return (1);
	if (*head == NULL)
	{
		new_node->number = 1;
		*head = new_node;
		return (0);
	}
	tmp = *head;
	i = 2;
	while (tmp->next != NULL)
	{
		tmp = tmp->next;
		i = i + 1;
	}
	tmp->next = new_node;
	new_node->prev = tmp;
	new_node->number = i;
	return (0);
}

int	cy1_identify_end(char *input, int *start)
{
	int	id;
	int	end;

	id = cy0_analyse_char(input[*start]);
	if (id == -2 || id == -3)
	{
		end = *start + 1;
		while (input[*start] != input[end])
			end = end + 1;
		if (id == -3)
			*start = *start - INT_MAX - 1;
		return (-end);
	}
	end = *start;
	if (id == -1)
	{
		while (cy0_analyse_char(input[end]) == -1)
			end = end + 1;
		*start = *start - INT_MAX - 1;
		return (end - 1);
	}
	while (cy0_analyse_char(input[end]) <= -4)
		end = end + 1;
	return (end - 1);
}
