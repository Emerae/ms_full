
#include "libftfull.h"

int	ft_memcmp(const void *s1, const void *s2, size_t n)
{
	size_t			i;
	unsigned char	*mem1;
	unsigned char	*mem2;

	mem1 = (unsigned char *)s1;
	mem2 = (unsigned char *)s2;
	i = 0;
	if (n == 0)
		return (0);
	while (i < n && mem1[i] == mem2[i])
		i++;
	if (i != n)
		return (mem1[i] - mem2[i]);
	return (0);
}
