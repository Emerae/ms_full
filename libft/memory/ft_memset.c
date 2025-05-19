
#include "libftfull.h"

void	*ft_memset(void *s, int c, size_t n)
{
	unsigned char	*mem;
	size_t			i;

	mem = (unsigned char *)s;
	i = -1;
	while (++i < n)
		mem[i] = (unsigned char)c;
	return (s);
}
