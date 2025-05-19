
#include "libftfull.h"
#include <unistd.h>

void	ft_putendl(char *s)
{
	size_t	l;

	if (!s)
		return ;
	l = ft_strlen(s);
	write(0, s, l);
	write(0, "\n", 1);
}
