
#include "libftfull.h"

int	print_8pct(t_print param, va_list args)
{
	int		size;
	char	*to_print;
	int		i;

	(void)args;
	size = 1;
	if (param.field > 0)
		size = param.field;
	to_print = malloc(size * sizeof(char));
	if (!to_print)
		return (0);
	i = -1;
	while (++i < size)
	{
		to_print[i] = ' ';
		if (param.zero && !param.align)
			to_print[i] = '0';
	}
	if (param.align)
		to_print[0] = '%';
	else
		to_print[size - 1] = '%';
	write(param.fd, to_print, size);
	free(to_print);
	return (size);
}
