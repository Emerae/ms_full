#include "parser_new.h"

int	cy4_1wrong_char(t_input *head)
{
	t_input	*current;
	int		i;

	current = head;
	while (current)
	{
		if (current->input && current->type == 2) /* Vérifier uniquement les nœuds de type texte normal */
		{
			i = 0;
			while (current->input[i])
			{
				if (cy0_analyse_char2(current->input[i]) == 1)
					return (1);
				if (cy0_analyse_char2(current->input[i]) == -14)
					return (2);
				i = i + 1;
			}
		}
		current = current->next;
	}
	return (0);
}
