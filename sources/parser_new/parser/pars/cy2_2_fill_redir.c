#include "../prser.h"

int	redir_type_from_str(const char *s)
{
	if (!s)
		return (-1);
	if (cy_strcmp(s, "<") == 0)
		return (0);
	if (cy_strcmp(s, ">") == 0)
		return (1);
	if (cy_strcmp(s, ">>") == 0)
		return (2);
	if (cy_strcmp(s, "<<") == 0)
		return (3);
	return (-2);
}

int	cy2_fill_redir_1(t_input *node, t_redir **head, t_redir **last)
{
	t_redir	*new_redir;
	int		type;

	if (!node || !node->next)
		return (0);
	type = redir_type_from_str(node->input);
	if (type < 0)
		return (0);
	new_redir = malloc(sizeof(t_redir));
	if (!new_redir)
		return (0);
	new_redir->type = type;
	new_redir->file = cy_true_strdup(node->next->input);
	if (!new_redir->file)
	{
		free(new_redir);
		return (0);
	}
	new_redir->next = NULL;
	if (!*head)
		*head = new_redir;
	else
		(*last)->next = new_redir;
	*last = new_redir;
	return (1);
}

int	cy2_fill_redir_2(t_input *node, int *nature, int *flag)
{
	if (!node->next)
	{
		*nature = 3;
		return (1);
	}
	if (cy_strcmp(node->next->input, "|") == 0)
	{
		*nature = 2;
		*flag = 2;
		return (1);
	}
	return (0);
}

static void	cy2_fill_redir_3(t_cmd **current_cmd, t_input **current_input,
	t_redir *head_redir, int flag)
{
	if (!head_redir)
		*current_input = NULL;
	(*current_cmd)->redirs = head_redir;
	if (flag == 2 && *current_input)
		*current_input = (*current_input)->next;
}

int	cy2_fill_redir(t_cmd **current_cmd, t_input **current_input, int *nature)
{
	t_fill_redir	s;
	int				ok;

	s.node = *current_input;
	s.head = NULL;
	s.last = NULL;
	s.nb_skip_head = 1;
	s.flag = 0;
	ok = cy2_fill_redir_loop_body(&s, nature);
	if (ok == 0)
		return (0);
	*current_input = s.node;
	cy2_fill_redir_3(current_cmd, current_input, s.head, s.flag);
	return (s.nb_skip_head);
}
