
#include "libftfull.h"

void	*ft_memmove(void *dest, const void *src, size_t n)
{
	unsigned char	*mem;
	unsigned char	*str;
	size_t			i;

	mem = (unsigned char *)dest;
	str = (unsigned char *)src;
	i = -1;
	if (!dest && !src)
		return (0);
	if (mem < str)
	{
		while (++i < n)
			mem[i] = str[i];
	}
	else
	{
		while (++i < n)
			mem[n - i - 1] = str[n - i - 1];
	}
	return (dest);
}
