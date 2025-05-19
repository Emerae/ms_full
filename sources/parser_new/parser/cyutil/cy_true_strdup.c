#include "parser_new.h"
#include "libftfull.h"

char	*cy_true_strdup(char *s)
{
	char	*ret;
	size_t	ls;
	size_t	i;

	if (!s)
		return (NULL);
	ls = cy_strlen(s);
	ret = malloc(ls + 1);
	if (!ret)
		return (NULL);
	i = 0;
	while (s[i])
	{
		ret[i] = s[i];
		i = i + 1;
	}
	ret[i] = '\0';
	return (ret);
}
