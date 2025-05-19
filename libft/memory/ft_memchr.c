
#include "libftfull.h"

void	*ft_memchr(const void *s, int c, size_t n)
{
	size_t			i;
	unsigned char	*mem;

	mem = (unsigned char *)s;
	i = -1;
	while (++i < n)
	{
		if (mem[i] == (unsigned char)c)
			return (mem + i);
	}
	return (NULL);
}
