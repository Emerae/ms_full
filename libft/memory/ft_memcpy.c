
#include "libftfull.h"

void	*ft_memcpy(void *dest, const void *src, size_t n)
{
	unsigned char	*mem;
	unsigned char	*str;
	size_t			i;

	if (dest == 0 && src == 0)
		return (0);
	mem = (unsigned char *)dest;
	str = (unsigned char *)src;
	i = -1;
	while (++i < n)
		mem[i] = str[i];
	return (dest);
}
