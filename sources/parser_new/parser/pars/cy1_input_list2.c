#include "parser_new.h"
#include <limits.h>
#include <stdio.h>


static void	print_node_debug(int start, int end, char *input)
{
	int	offset;
	int	len;

	if (start < 0)
		offset = start + INT_MAX + 1;
	else
		offset = start;
	len = end - start + 1;
	if (len < 0)
		len = end - offset + 1;
	if (len < 0)
		len = 0;

	printf("create_node: start=%d, end=%d, input='", start, end);
	printf("%.*s'\n", len, &input[offset]);
}

/*
static void	print_node_debug(int start, int end, char *input)
{
	int	offset;

	if (start < 0)
		offset = start + INT_MAX + 1;
	else
		offset = start;
	printf("create_node: start=%d, end=%d, input='", start, end);
	printf("%.*s'\n", end - start + 1, &input[offset]);
}
*/

static int	set_node_type(t_input *node, int start, int end)
{
	if (start < 0 && end < 0)
		node->type = 4;
	else if (start >= 0 && end < 0)
		node->type = 3;
	else if (start < 0 && end > 0)
		node->type = 1;
	else
		node->type = 2;
	return (0);
}

static int create_node_1(t_input *new_node, int start, int end, char *input)
{
    // Initialiser TOUS les champs à des valeurs sûres
    new_node->input = NULL;
    new_node->input_type = NULL;
    new_node->input_num = NULL;
    new_node->prev = NULL;
    new_node->next = NULL;
    new_node->number = -1;
    new_node->type = 0;  // Valeur par défaut sûre
    
    // Puis allouer et initialiser input
    new_node->input = cy_strdup(input, start, end);
    if (!new_node->input)
    {
        printf("Failed alloc for text");
        free(new_node);
        return (1);
    }
    
    // Définir correctement le type
    set_node_type(new_node, start, end);
    
    return (0);
}

t_input	*cy1_create_node(int start, int end, char *input)
{
	t_input	*new_node;

	print_node_debug(start, end, input);
	new_node = (t_input *)malloc(sizeof(t_input));
	if (!new_node)
	{
		printf("Failed alloc for node");
		return (NULL);
	}
	if (create_node_1(new_node, start, end, input))
		return (NULL);
	return (new_node);
}
