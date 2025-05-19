
#include "libftfull.h"
#include <unistd.h>

void	ft_putstr_fd(char *s, int fd)
{
	size_t	l;

	if (!s)
		return ;
	l = ft_strlen(s);
	write(fd, s, l);
}
