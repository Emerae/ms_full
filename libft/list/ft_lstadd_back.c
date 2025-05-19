
#include "libftfull.h"

void	ft_lstadd_back(t_list **alst, t_list *new)
{
	t_list	*begin;

	begin = *alst;
	if (!begin)
	{
		*alst = new;
		return ;
	}
	while (begin && begin->next)
		begin = begin->next;
	begin->next = new;
}
