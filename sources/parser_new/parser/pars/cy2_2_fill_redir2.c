#include "../prser.h"

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

int	cy2_fill_redir_loop_body(t_fill_redir *s, int *nature)
{
	while (1)
	{
		cy2_fill_redir_skip_update(s);
		if (cy2_fill_redir_2(s->node, nature, &s->flag))
			break ;
		if (s->flag == 1)
			s->node = s->node->next;
		if (!cy2_fill_redir_1(s->node, &s->head, &s->last))
			return (0);
		s->node = s->node->next;
		s->flag = 1;
	}
	return (1);
}
