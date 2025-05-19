#include "parser_new.h"
#include <stdlib.h>

static void	free_node(t_node *n)
{
	if (!n)
		return ;
	free(n->content);
	free(n);
}

t_node	*cy1_remove_space_nodes(t_node *head)
{
	t_node	*prev;
	t_node	*curr;
	t_node	*next;

	prev = NULL;
	curr = head;
	while (curr)
	{
		next = curr->next;
		if (curr->id == -1)                /* blank-space token ─ drop it  */
		{
			if (prev)
				prev->next = next;
			else
				head = next;              /* head node was a blank        */
			free_node(curr);
		}
		else
			prev = curr;                  /* keep this node, move prev    */
		curr = next;                       /* move forward using *next*,   */
	}                                       /* not through a freed pointer  */
	return (head);
}