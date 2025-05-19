
#include "minishell.h"

/**
 * @brief Removes trailing whitespace characters from a string
 *
 * This function iterates backward through the string and replaces
 * any trailing whitespace characters with null terminators.
 *
 * @param s The null-terminated string to trim
 */

/**
 * @brief Removes surrounding quote characters from a string
 *
 * If the separator in the t_split structure is a single or double quote,
 * this function removes these quotes by shifting the content left and
 * truncating the string appropriately.
 *
 * @param split A t_split structure containing the string and separator information
 */
void	trim_spaces(char *s)
{
	int	l;
	int	i;

	l = ft_strlen(s);
	i = l - 1;
	while (i >= 0 && ft_isspace(s[i]))
	{
		s[i] = '\0';
		i--;
	}
}

void	trim_seps(t_split split)
{
	int	i;
	int	l;

	if (split.sep == '\'' || split.sep == '\"')
	{
		i = 0;
		l = ft_strlen(split.str);
		while (i + 2 < l)
		{
			split.str[i] = split.str[i + 1];
			i++;
		}
		split.str[i] = '\0';
		split.str[i + 1] = '\0';
	}
}
