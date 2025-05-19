#include "parser_new.h"

void	cy1_remove_space_nodes_free(t_input *to_free)
{
	if (to_free->input)
		free(to_free->input);
	if (to_free->input_type)
		free(to_free->input_type);
	if (to_free->input_num)
		free(to_free->input_num);
	free(to_free);
}

void	cy1_remove_space_nodes_1(t_input **head_input, t_input **current)
{
	t_input	*to_free;

	to_free = *current;
	*current = (*current)->next;
	if (to_free->prev)
		to_free->prev->next = to_free->next;
	else
		*head_input = to_free->next;
	if (to_free->next)
		to_free->next->prev = to_free->prev;
	cy1_remove_space_nodes_free(to_free);
}

void	cy1_remove_space_nodes(t_input **head_input)
{
	t_input	*current;

	if (!head_input || !*head_input)
		return ;
	current = *head_input;
	while (current)
	{
		if (current->type == 1)
			cy1_remove_space_nodes_1(head_input, &current);
		else
			current = current->next;
	}
}
