#include "parser_new.h"
#include "libftfull.h"
#include <limits.h>

char	*cy_strdup(char *input, int start, int end)
{
	int		len;
	char	*res;

	len = end - start + 1;
	if (len <= 0)                                /* ← new guard */
		return (cy_true_strdup(""));             /*  or NULL if you prefer */

	res = malloc((len + 1) * sizeof(char));
	if (!res)
		return (NULL);
	ft_strlcpy(res, &input[start], len + 1);
	return (res);
}

/*
char	*cy_strdup(char *s, int start, int end)
{
	char	*res;
	int		ls;

	res = NULL;
	if (start < 0 && end < 0)
	{
		start = start + INT_MAX + 1 + 1;
		end = -end - 1;
	}
	else if (start >= 0 && end < 0)
	{
		start = start + 1;
		end = -end - 1;
	}
	else if (start < 0 && end > 0)
		start = start + INT_MAX + 1;
	ls = end - start + 1;
	res = malloc((ls + 1) * sizeof (char));
	if (res == 0)
		return (0);
	cy_strlcpy(res, &s[start], ls + 1);
	return (res);
}
*/
// duplicates a null-terminated string by allocating memory for a new copy
// of the string and returning a pointer to it