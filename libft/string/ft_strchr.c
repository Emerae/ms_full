
#include "libftfull.h"

char	*ft_strchr(const char *s, int c)
{
	size_t	n;
	size_t	i;
	char	*str;

	n = ft_strlen(s);
	i = 0;
	str = (char *)s;
	while (i < n)
	{
		if (s[i] == (unsigned char)c)
			return (str + i);
		i++;
	}
	if (c == 0)
		return (str + n);
	return (NULL);
}
