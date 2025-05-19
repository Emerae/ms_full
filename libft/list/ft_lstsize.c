
#include "libftfull.h"

int	ft_lstsize(t_list *lst)
{
	int		s;

	s = 0;
	while (lst)
	{
		lst = lst->next;
		s++;
	}
	return (s);
}
