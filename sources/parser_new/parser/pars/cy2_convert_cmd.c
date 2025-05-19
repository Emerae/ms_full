#include "parser_new.h"
#include "libftfull.h"
#include <stdio.h>

int	find_delimiter2(char *input, int *nature, int type)
{
	if (cy_strcmp(input, "<") == 0
		|| cy_strcmp(input, ">") == 0
		|| cy_strcmp(input, ">>") == 0
		|| cy_strcmp(input, "<<") == 0)
	{
		if (type == 2)
		{
			*nature = 1;
			return (1);
		}
	}
	else if (cy_strcmp(input, "|") == 0 && type == 2)
	{
		*nature = 2;
		return (1);
	}
	return (0);
}

int find_delimiter1(t_input **current_input,
                  t_input *node, int *nature, int ret)
{
    while (node)
    {
        // AJOUTEZ CE BLOC: Ignorer les nœuds d'espace
        if (node->type == 1) {  // Type 1 = espace
            node = node->next;
            continue;
        }
        
        if (node->input == NULL)
        {
            printf("Error: node->input is NULL\n");
            break;
        }
        if (find_delimiter2(node->input, nature, node->type))
            break;
        if (node->next == NULL)
        {
            *nature = 3;
            *current_input = node;
            printf("find_delimiter: ret = %d, nature = %d\n", ret, *nature);
            printf("current_input->input = %s\n", (*current_input)->input);
            return (ret);
        }
        ret = ret + 1;
        node = node->next;
    }
    *current_input = node;
    printf("find_delimiter: ret = %d, nature = %d\n", ret - 1, *nature);
    printf("current_input->input = %s\n", (*current_input)->input);
    return (ret - 1);
}

int	find_delim(t_input **current_input, int *nature)
{
	t_input	*node;
	int		ret;

	node = *current_input;
	ret = 1;
	if (!current_input || !*current_input || !nature)
	{
		printf("Error: current_input is NULL\n");
		return (-1);
	}
	return (find_delimiter1(current_input, node, nature, ret));
}
