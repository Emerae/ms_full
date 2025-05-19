
#include "libftfull.h"

char	*ft_substr(const char *s, unsigned int start, size_t len)
{
	size_t	i;
	size_t	j;
	char	*sub;

	i = 0;
	if (!s)
		return (NULL);
	while (s[i] && i < start)
		i++;
	j = 0;
	while (s[i] && i - start < len)
	{
		j++;
		i++;
	}
	sub = ft_calloc(j + 1, sizeof(char));
	if (!sub)
		return (NULL);
	i = start;
	while (i - start < j)
	{
		sub[i - start] = s[i];
		i++;
	}
	return (sub);
}
