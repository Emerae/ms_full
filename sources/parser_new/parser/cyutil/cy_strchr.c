#include "parser_new.h"
#include "libftfull.h"

char	*cy_strchr(const char *s, int c)
{
	size_t	i;
	char	*tmp;

	i = 0;
	tmp = (char *)s;
	while (1)
	{
		if (tmp[i] == c)
			return (&tmp[i]);
		if (tmp[i] == '\0')
			break ;
		i = i + 1;
	}
	return (0);
}
