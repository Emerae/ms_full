
#include "minishell.h"
/**
 * @file env.c
 * @brief Functions for environment variable management
 *
 * This file contains functions for working with environment variables,
 * specifically for converting environment lists to string arrays.
 */

/**
 * @brief Counts the number of environment variables with required export status
 *
 * @param list The linked list of environment variables (starts with sentinel node)
 * @param exported The minimum export status to count (0 = not exported, 1 = exported)
 * @return int The number of environment variables meeting the export criteria
 */

/**
 * @brief Creates a NULL-terminated array of environment strings in "VAR=VALUE" format
 *
 * This function creates an array of strings from the environment list, including
 * only variables that meet the specified export status. Each variable is formatted
 * as "VAR=VALUE". The array is NULL-terminated.
 *
 * @param envl The linked list of environment variables (starts with sentinel node)
 * @param exported The minimum export status to include (0 = all vars, 1 = only exported)
 * @return char** A newly allocated array of environment strings, or NULL on error
 *                The caller is responsible for freeing this memory
 */

static int	size_of_list(t_list *list, int exported)
{
	int	i;

	i = 0;
	list = list->next;
	while (list)
	{
		if (((t_env *)list->content)->exported >= exported)
			i++;
		list = list->next;
	}
	return (i);
}

char		**create_env_tab(t_list *envl, int exported)
{
	int		size;
	int		i;
	char	**env;
	char	*tmp;

	size = size_of_list(envl, exported);
	env = malloc((size + 1) * sizeof(char *));
	if (!env)
		return (NULL);
	i = 0;
	envl = envl->next;
	while (i < size)
	{
		if (((t_env *)envl->content)->exported >= exported)
		{
			tmp = ft_strjoin(((t_env *)envl->content)->var, "=");
			env[i] = ft_strjoin(tmp, ((t_env *)envl->content)->value);
			free(tmp);
			i++;
		}
		envl = envl->next;
	}
	env[i] = NULL;
	return (env);
}
