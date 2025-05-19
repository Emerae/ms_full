
#include "libftfull.h"

static void	rec_putnbr(unsigned int nb, int sg)
{
	if (nb == 0)
	{
		if (sg == -1)
			ft_putchar('-');
		return ;
	}
	rec_putnbr(nb / 10, sg);
	ft_putchar(nb % 10 + '0');
}

void		ft_putnbr(int n)
{
	short int		sg;

	sg = 1 - 2 * (n < 0);
	if (n == 0)
		ft_putchar('0');
	else
		rec_putnbr(sg * n, sg);
}
