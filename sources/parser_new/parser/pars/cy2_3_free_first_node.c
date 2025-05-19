#include "../prser.h"

void	free_redirs(t_redir *redir)
{
	t_redir	*tmp;

	while (redir)
	{
		tmp = redir->next;
		free(redir->file);
		free(redir);
		redir = tmp;
	}
}

void	cy2_free_first_cmd_node(t_cmd **head)
{
	t_cmd	*to_free;
	int		i;

	if (!head || !*head)
		return ;
	to_free = *head;
	if (to_free->args)
	{
		i = 0;
		while (to_free->args[i])
		{
			free(to_free->args[i]);
			i = i + 1;
		}
		free(to_free->args);
	}
	if (to_free->redirs)
		free_redirs(to_free->redirs);
	*head = to_free->next;
	free(to_free);
}
