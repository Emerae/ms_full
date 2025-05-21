#include "parser_new.h"
#include "libftfull.h"

void	*cy_memset(void *s, int c, size_t n)
{
	size_t			i;
	unsigned char	*tmp;

	tmp = (unsigned char *)s;
	i = 0;
	while (i < n)
	{
		tmp[i] = c;
		i = i + 1;
	}
	return (s);
}
