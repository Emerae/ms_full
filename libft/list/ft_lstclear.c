
#include "libftfull.h"

void	ft_lstclear(t_list **lst, void (*del)(void *))
{
	t_list	*beg;
	t_list	*save;

	if (!lst)
		return ;
	beg = *lst;
	while (beg)
	{
		save = beg->next;
		ft_lstdelone(beg, del);
		beg = save;
	}
	*lst = NULL;
}
