#include "parser_new.h"
#include "libftfull.h"

static int	cy1_make_list_1(
	t_input **head,
	int *start,
	int *end,
	char *input
)
{
	int	error;

	*end = cy1_identify_end(input, start);
	error = cy1_append_input(head, *start, *end, input);
	if (error == 1)
	{
		cy0_free_input_list(*head);
		return (1);
	}
	if (*start < 0)
		*start = *start + INT_MAX + 1;
	if (*end < 0)
		*end = -*end;
	*start = *end + 1;
	return (0);
}

t_input	*cy1_make_list(char *input)
{
	t_input	*head;
	int		start;
	int		end;

	head = NULL;
	start = 0;
	while (cy0_analyse_char(input[start]) == -1)
		start = start + 1;
	while (input[start])
	{
		if (cy1_make_list_1(&head, &start, &end, input) == 1)
			return (NULL);
	}
	cy1_remove_space_nodes(&head);
	return (head);
}
