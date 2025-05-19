
#include "libftfull.h"
#include <stdlib.h>

char	*ft_strmapi(const char *s, char (*f)(unsigned int, char))
{
	size_t	i;
	size_t	l;
	char	*map;

	i = -1;
	if (!s)
		return (NULL);
	l = ft_strlen(s);
	map = malloc((l + 1) * sizeof(char));
	if (!map)
		return (NULL);
	while (++i < l)
		map[i] = (*f)(i, s[i]);
	map[l] = '\0';
	return (map);
}
