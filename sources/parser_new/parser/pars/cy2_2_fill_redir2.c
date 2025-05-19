#include "parser_new.h"

static void	cy2_fill_redir_skip_update(t_fill_redir *s)
{
	if (s->flag == 1 && s->node->next)
	{
		if (s->nb_skip_head == 1)
			s->nb_skip_head = 2;
		else
			s->nb_skip_head = s->nb_skip_head + 2;
	}
	else if (s->flag == 1 && !s->node->next)
	{
		if (s->nb_skip_head != 1)
			s->nb_skip_head = s->nb_skip_head + 1;
	}
}

int cy2_fill_redir_loop_body(t_fill_redir *s, int *nature)
{
    int nodes_skipped;
    
    while (1)
    {
        cy2_fill_redir_skip_update(s);
        if (cy2_fill_redir_2(s->node, nature, &s->flag))
            break;
        if (s->flag == 1)
            s->node = s->node->next;
            
        nodes_skipped = cy2_fill_redir_1(s->node, &s->head, &s->last);
        if (!nodes_skipped)
            return (0);
            
        // Avancer du nombre de nœuds effectivement parcourus
        while (nodes_skipped > 0 && s->node) {
            s->node = s->node->next;
            nodes_skipped--;
        }
        
        s->flag = 1;
    }
    return (1);
}
