
#include "libftfull.h"

size_t	ft_strlcpy(char *dst, const char *src, size_t size)
{
	size_t	i;
	char	*str;

	i = 0;
	str = (char *)src;
	if (!dst)
		return (0);
	while (str[i] && i + 1 < size)
	{
		dst[i] = str[i];
		i++;
	}
	if (size > 0)
		dst[i] = '\0';
	while (str[i])
		i++;
	return (i);
}
