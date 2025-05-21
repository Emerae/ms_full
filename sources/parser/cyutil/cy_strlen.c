#include "parser_new.h"
#include "libftfull.h"

int	cy_strlen(char *s)
{
	int	i;

	i = 0;
	if (s == NULL)
		return (0);
	while (s[i])
		i = i + 1;
	return (i);
}

int	cy_strlen2(const char *s)
{
	int	i;

	i = 0;
	if (s == NULL)
		return (0);
	while (s[i])
		i = i + 1;
	return (i);
}

// size t is an unsigned integer type
//used to represent the size of any obj in bytes
// max size_t depends on the machine