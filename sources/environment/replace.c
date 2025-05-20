
#include "minishell.h"
/**
 * @brief Calculates the size of an environment variable's value
 * 
 * Extracts a variable name from the input string and searches for its value
 * in the environment list. Handles special case for "$?" (exit status).
 * 
 * @param str Input string containing the variable name (without the '$')
 * @param envl Linked list containing environment variables
 * @param value Pointer where the variable's value will be stored
 * @return Size of the variable's value (0 if not found)
 */

/**
 * @brief Replaces an environment variable with its value
 * 
 * Helper function that handles the replacement of a single variable reference.
 * Updates the index to skip over the variable name in the original string.
 * 
 * @param i Pointer to the current position in the string (after '$')
 * @param old The original string containing variables to expand
 * @param envl Linked list containing environment variables
 * @param dest Destination string where the variable value will be copied
 * @return Size of the replaced variable value
 */

/**
 * @brief Expands environment variables in a string
 * 
 * Processes the input string and expands all environment variables ($ prefixed).
 * Handles escape sequences and preserves characters as needed.
 * Supports special handling of escaped characters (", \, $).
 * 
 * @param fill Destination buffer where expanded string will be stored
 * @param old Original string with potential environment variables
 * @param envl Linked list containing environment variables
 */

int			size_var(char *str, t_list *envl, char **value)
{
	char	*var;
	int		size;
	int		i;

	if (str[0] == '?')
		var = ft_strdup("?exitcode");
	else
	{
		size = ft_strlen(str);
		var = malloc((size + 1) * sizeof(char));
		if (!var)
			return (0);
		i = -1;
		while (++i < size && (ft_isalpha(str[i]) || str[i] == '_'))
			var[i] = str[i];
		var[i] = '\0';
	}
	*value = search_in_env(envl, var);
	free(var);
	if (!(*value))
		return (0);
	return (ft_strlen(*value));
}

static int	replace_var(int *i, char *old, t_list *envl, char *dest)
{
	int		size;
	int		l;
	char	*current_var;

	l = ft_strlen(old);
	size = size_var(old + ++(*i), envl, &current_var);
	if (current_var)
		ft_strlcpy(dest, current_var, size + 1);
	if (old[*i] == '?')
		(*i)++;
	else
	{
		while (*i < l && (ft_isalpha(old[*i]) || old[*i] == '_'))
			(*i)++;
	}
	return (size);
}

void		fill_expanded(char *fill, char *old, t_list *envl)
{
	int		l;
	int		res;
	int		i;

	l = ft_strlen(old);
	res = 0;
	i = 0;
	while (i < l)
	{
		if ((old[i] == '\"' || old[i] == '\\' || old[i] == '$')
				&& i > 0 && old[i - 1] == '\\')
		{
			fill[res - 1] = old[i];
			i++;
		}
		else if (old[i] == '$')
		{
			if (i == 0 || old[i - 1] != '\\')
				res += replace_var(&i, old, envl, fill + res);
		}
		fill[res] = old[i];
		res++;
		i++;
	}
	fill[res] = '\0';
}
