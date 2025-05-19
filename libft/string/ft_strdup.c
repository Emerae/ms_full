
#include "libftfull.h"
#include <stdlib.h>

char	*ft_strdup(const char *s)
{
	size_t	l;
	size_t	i;
	char	*dup;

	l = ft_strlen(s);
	dup = malloc(sizeof(char) * (l + 1));
	i = -1;
	if (!dup)
		return (NULL);
	while (++i < l)
		dup[i] = s[i];
	dup[i] = '\0';
	return (dup);
}
