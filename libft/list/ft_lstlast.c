
#include "libftfull.h"

t_list	*ft_lstlast(t_list *lst)
{
	t_list	*beg;

	beg = lst;
	while (beg && beg->next)
		beg = beg->next;
	return (beg);
}
