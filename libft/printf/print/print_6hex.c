
#include "libftfull.h"

int	print_6hex(t_print param, va_list args)
{
	unsigned int	nb;
	int				size;
	char			*nb_utox;
	char			*to_print;

	nb = (unsigned int)va_arg(args, int);
	nb_utox = ft_utox(nb);
	if (nb == 0 && param.precision == 0)
		nb_utox[0] = '\0';
	size = size_int(param, nb_utox);
	to_print = malloc(size * sizeof(char));
	if (!to_print)
		return (0);
	fill_int(&to_print, nb_utox, param, size);
	free(nb_utox);
	ft_putstr_fd(to_print, param.fd);
	free(to_print);
	return (size - 1);
}
