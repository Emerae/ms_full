
#include "libftfull.h"
#include <unistd.h>

void	ft_putstr(char *s)
{
	size_t	l;

	if (!s)
		return ;
	l = ft_strlen(s);
	write(1, s, l);
}
