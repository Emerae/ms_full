
#include "libftfull.h"

size_t	ft_strlcat(char *dst, const char *src, size_t size)
{
	size_t	d;
	size_t	s;
	size_t	i;

	d = ft_strlen(dst);
	s = ft_strlen(src);
	if (size <= d)
		return (size + s);
	i = d;
	while (i + 1 < size && src[i - d])
	{
		dst[i] = (char)(src[i - d]);
		i++;
	}
	dst[i] = '\0';
	return (s + d);
}
