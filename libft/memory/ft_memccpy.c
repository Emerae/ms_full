
#include "libftfull.h"

void	*ft_memccpy(void *dest, const void *src, int c, size_t n)
{
	unsigned char	*mem;
	unsigned char	*str;
	size_t			i;

	mem = (unsigned char *)dest;
	str = (unsigned char *)src;
	i = -1;
	while (++i < n)
	{
		mem[i] = str[i];
		if (str[i] == (unsigned char)c)
			return (dest + i + 1);
	}
	return (NULL);
}
