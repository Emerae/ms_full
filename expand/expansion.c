
#include "minishell.h"
/**
 * @brief Advances the index past a variable name in a string
 *
 * If the variable is '?' (exit status), advances one character.
 * Otherwise, advances until a non-alphanumeric and non-underscore character is found.
 *
 * @param str String containing the variable
 * @param i Starting index of the variable name (after '$')
 * @param l Length of the string
 * @return Index position after the variable name
 */
static int	skip_var(char *str, int i, int l);

/**
 * @brief Calculates the size needed for a string after variable expansion
 * 
 * Iterates through the string and calculates the total size needed after
 * all environment variables (marked with '$') have been expanded to their values.
 * Handles escaped '$' characters by skipping them.
 *
 * @param str String to be expanded
 * @param envl Environment variable list
 * @return Total size needed for the expanded string (including null terminator)
 */
static int	expanded_size(char *str, t_list *envl);

/**
 * @brief Performs simple variable expansion on a string
 * 
 * Expands environment variables in the string and adds the result
 * to the expansion list, preserving separator and space information.
 *
 * @param expansion Pointer to the expansion list
 * @param curr Structure containing the string and its metadata
 * @param envl Environment variable list
 */
void		expand_simple(t_list **expansion, t_split curr, t_list *envl);

/**
 * @brief Performs complex variable expansion with word splitting
 * 
 * Expands environment variables in the string, then splits the result
 * on spaces. Each resulting word is added to the expansion list with
 * appropriate separator information. If multiple words result from expansion
 * in a redirection context (r=1), returns an ambiguous redirection error.
 *
 * @param expansion Pointer to the expansion list
 * @param curr Structure containing the string and its metadata
 * @param envl Environment variable list
 * @param r Flag indicating if this is a redirection context
 * @return SUCCESS or error code (ALLOCATION_FAIL or AMBIGUOUS_REDIR)
 */
int			expand_hard(t_list **expansion, t_split curr, t_list *envl, int r);

static int	skip_var(char *str, int i, int l)
{
	if (str[i] == '?')
		i++;
	else
	{
		while (i < l && (ft_isalpha(str[i]) || str[i] == '_'))
			i++;
	}
	return (i);
}

static int	expanded_size(char *str, t_list *envl)
{
	int		i;
	int		res;
	int		l;
	char	*value;

	l = ft_strlen(str);
	res = 0;
	i = 0;
	while (i < l)
	{
		if (str[i] == '$')
		{
			if (i > 0 && str[i - 1] == '\\')
				i++;
			else
			{
				res += size_var(str + ++i, envl, &value);
				i = skip_var(str, i, l);
			}
		}
		res++;
		i++;
	}
	return (res + 1);
}

void		expand_simple(t_list **expansion, t_split curr, t_list *envl)
{
	int		size_tot;
	char	*new;

	size_tot = expanded_size(curr.str, envl);
	new = malloc(size_tot * sizeof(char));
	if (!new)
		return ;
	fill_expanded(new, curr.str, envl);
	ft_lstadd_back(expansion, new_entry(new, curr.sep, curr.space));
	free(new);
}

int			expand_hard(t_list **expansion, t_split curr, t_list *envl, int r)
{
	int		size_tot;
	int		i;
	char	**split;
	char	*new;

	size_tot = expanded_size(curr.str, envl);
	new = malloc(size_tot * sizeof(char));
	if (!new)
		return (ALLOCATION_FAIL);
	fill_expanded(new, curr.str, envl);
	split = ft_splitchar(new, ' ');
	free(new);
	i = 0;
	while (split[i])
	{
		if (split[i + 1])
			ft_lstadd_back(expansion, new_entry(split[i], ' ', 1));
		else
			ft_lstadd_back(expansion, new_entry(split[i], ' ', curr.space));
		i++;
	}
	free_tab(split);
	if (i > 1 && r)
		return (error_msg_info(AMBIGUOUS_REDIR, curr.str));
	return (SUCCESS);
}
