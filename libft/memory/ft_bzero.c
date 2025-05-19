
#include "libftfull.h"

void	*ft_bzero(void *s, size_t n)
{
	unsigned char	*mem;
	size_t			i;

	mem = (unsigned char *)s;
	i = -1;
	while (++i < n)
		mem[i] = '\0';
	return (s);
}
