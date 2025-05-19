
#include "minishell.h"
/**
 * @brief Searches for a variable in the environment list
 *
 * This function iterates through the environment list (envl) and searches for
 * a variable matching the provided name (var).
 *
 * @param envl Pointer to the environment list
 * @param var Variable name to search for
 * @return The value of the variable if found, NULL otherwise
 */
char		*search_in_env(t_list *envl, char *var);

/**
 * @brief Counts the number of elements in a linked list
 *
 * @param begin_list Pointer to the first element of the list
 * @return The number of elements in the list
 */
int			list_size(t_list *begin_list);

/**
 * @brief Sorts a linked list using bubble sort algorithm
 *
 * This function sorts the linked list in place by swapping the content pointers
 * based on the provided comparison function.
 *
 * @param begin_list Double pointer to the first element of the list
 * @param cmp Comparison function that returns >0 if first arg is greater than second
 */
void		ft_lstsort(t_list **begin_list, int (*cmp)());

/**
 * @brief Extracts a substring from the beginning of a string up to a delimiter
 *
 * Allocates memory for a new string containing characters from the input string
 * up to but not including the first occurrence of the delimiter character.
 *
 * @param line Input string to extract from
 * @param c Delimiter character
 * @return Newly allocated string containing the substring
 */
static char	*ft_strcut(char *line, char c);

/**
 * @brief Checks if a variable name matches an entry in the environment list
 *
 * Compares a variable name with an entry in the environment list.
 * If cut is non-zero, the function extracts everything before '=' in var.
 *
 * @param envl Pointer to an element in the environment list
 * @param var Variable name to check (may include '=value' if cut is non-zero)
 * @param cut Flag to determine if var should be cut at '='
 * @return 1 if the variable matches, 0 otherwise
 */
int			variable_match(t_list *envl, char *var, int cut);

char		*search_in_env(t_list *envl, char *var)
{
	while (envl)
	{
		if (ft_strcmp(((t_env *)envl->content)->var, var) == 0)
			return (((t_env *)envl->content)->value);
		envl = envl->next;
	}
	return (NULL);
}

int			list_size(t_list *begin_list)
{
	int		i;

	i = 0;
	while (begin_list)
	{
		begin_list = begin_list->next;
		i++;
	}
	return (i);
}

void		ft_lstsort(t_list **begin_list, int (*cmp)())
{
	t_list	*current;
	void	*save;
	int		size;
	int		i;
	int		j;

	i = 0;
	size = list_size(*begin_list);
	while (i < size)
	{
		j = 0;
		current = *begin_list;
		while (j < size - 1 - i)
		{
			if ((*cmp)(current->content, current->next->content) > 0)
			{
				save = current->content;
				current->content = current->next->content;
				current->next->content = save;
			}
			current = current->next;
			j++;
		}
		i++;
	}
}

static char	*ft_strcut(char *line, char c)
{
	int		i;
	char	*res;

	i = 0;
	while (line[i] && line[i] != c)
		i++;
	res = malloc((i + 1) * sizeof(char));
	i = 0;
	while (line[i] && line[i] != c)
	{
		res[i] = line[i];
		i++;
	}
	res[i] = '\0';
	return (res);
}

int			variable_match(t_list *envl, char *var, int cut)
{
	char	*env_var;
	char	*to_find;

	if (cut)
		to_find = ft_strcut(var, '=');
	else
		to_find = ft_strdup(var);
	env_var = ((t_env *)envl->content)->var;
	if (ft_strcmp(env_var, to_find) == 0)
	{
		free(to_find);
		return (1);
	}
	free(to_find);
	return (0);
}
