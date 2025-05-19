
#include "minishell.h"

/**
 * fill_entry - Fills a t_env structure with variable and value from a line.
 * @cont: Pointer to the t_env structure to be filled.
 * @line: The input string containing the environment variable and its value.
 *
 * This function parses the input string to separate the environment variable
 * name and its value. If the input string contains an '=' character, the part
 * before '=' is considered the variable name and the part after '=' is the value.
 * If there is no '=' character, the entire input string is considered as the
 * variable name with no value. The function allocates memory for the variable
 * name and value, and assigns them to the respective fields in the t_env structure.
 */
/**
 * @file env.c
 * @brief Environment variable handling for minishell
 *
 * This file contains functions to parse, manipulate and manage
 * environment variables stored in a linked list structure.
 */

/**
 * @brief Parse an environment variable string into a t_env structure
 *
 * Parses a string in the format "NAME=VALUE" or just "NAME" and fills the
 * provided t_env container with the appropriate values. If the string contains
 * an equals sign, the value is extracted and duplicated.
 *
 * @param cont Pointer to the t_env structure to be filled
 * @param line String containing the environment variable (format: NAME=VALUE)
 */
void	fill_entry(t_env *cont, char *line);

/**
 * @brief Create a new environment variable list entry
 *
 * Allocates memory for a new list node and its t_env content. Fills the
 * content by parsing the provided environment variable string and sets
 * the export status.
 *
 * @param line String containing the environment variable (format: NAME=VALUE)
 * @param exported Flag indicating if the variable is exported (1) or not (0)
 *                 or has special status (-1)
 * @return t_list* Newly created list node or NULL if memory allocation fails
 */
t_list	*init_entry(char *line, int exported);

/**
 * @brief Free memory associated with an environment variable entry
 *
 * Frees all memory allocated for a t_env structure, including the
 * variable name string and value string if they exist.
 *
 * @param ventry Void pointer to t_env structure to be freed
 */
void	free_entry(void *ventry);

/**
 * @brief Parse the environment array into a linked list
 *
 * Creates a linked list of environment variables from the provided
 * environment array. Special handling includes:
 * - Adding a "?begin" sentinel entry
 * - Incrementing the SHLVL (shell level) value
 * - Adding a "_" environment variable pointing to "./minishell"
 *
 * @param envl Pointer to the list head pointer to store the environment list
 * @param env Array of environment variable strings (format: NAME=VALUE)
 */
void	parse_env(t_list **envl, char **env);
void	fill_entry(t_env *cont, char *line)
{
	char	*val;
	int		end;

	cont->value = NULL;
	end = ft_strlen(line);
	if (ft_strrchr(line, '=') != NULL)
	{
		val = ft_strchr(line, '=') + 1;
		end = ft_strlen(line) - ft_strlen(val) - 1;
		cont->value = ft_strdup(val);
	}
	cont->var = ft_strdup(line);
	(cont->var)[end] = '\0';
}

t_list	*init_entry(char *line, int exported)
{
	t_list	*new;
	t_env	*cont;

	new = malloc(sizeof(t_list));
	if (!new)
		return (NULL);
	cont = malloc(sizeof(t_env));
	if (!cont)
	{
		free(new);
		return (NULL);
	}
	fill_entry(cont, line);
	cont->exported = exported;
	new->content = cont;
	new->next = NULL;
	return (new);
}

void	free_entry(void *ventry)
{
	t_env	*entry;

	entry = (t_env *)ventry;
	if (entry->var)
		free(entry->var);
	if (entry->value)
		free(entry->value);
	free(entry);
}

void	parse_env(t_list **envl, char **env)
{
	t_list	*new;
	int		shlvl;
	int		i;

	i = 0;
	*envl = NULL;
	new = init_entry("?begin", -1);
	ft_lstadd_back(envl, new);
	while (env[i])
	{
		new = init_entry(env[i], 2);
		if (ft_strcmp(((t_env *)new->content)->var, "SHLVL") == 0)
		{
			shlvl = ft_atoi(((t_env *)new->content)->value);
			shlvl++;
			free(((t_env *)new->content)->value);
			((t_env *)new->content)->value = ft_itoa(shlvl);
		}
		ft_lstadd_back(envl, new);
		i++;
	}
	add_env("_", ft_strdup("./minishell"), envl, 1);
}
