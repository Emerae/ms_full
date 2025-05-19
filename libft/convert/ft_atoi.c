
#include "libftfull.h"

static int	is_space(char c)
{
	if (c == ' ' || (c > 8 && c < 14))
		return (1);
	return (0);
}

int			ft_atoi(const char *nptr)
{
	unsigned long long int	nb;
	short int				sg;

	nb = 0;
	while (*nptr && is_space(*nptr))
		nptr++;
	sg = 1 - 2 * (*nptr == '-');
	if (*nptr == '-' || *nptr == '+')
		nptr++;
	while (*nptr && ft_isdigit(*nptr))
	{
		nb = 10 * nb + (*nptr - '0');
		if (nb > (unsigned long long)(9223372036854775807 + (sg == -1)))
			return ((sg + 1) / (-2));
		nptr++;
	}
	return ((int)(sg * nb));
}
