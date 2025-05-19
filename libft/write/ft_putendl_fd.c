
#include "libftfull.h"
#include <unistd.h>

void	ft_putendl_fd(char *s, int fd)
{
	size_t	l;

	if (!s)
		return ;
	l = ft_strlen(s);
	write(fd, s, l);
	write(fd, "\n", 1);
}
