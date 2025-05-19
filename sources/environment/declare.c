
#include "minishell.h"
/**
 * @file declare.c
 * @brief Functions for managing environment variables in the shell
 */

/**
 * @brief Adds a new environment variable to the environment list
 *
 * Creates a new node in the environment linked list with the given variable
 * name and value. Sets the exported flag according to the provided parameter.
 *
 * @param var Name of the environment variable
 * @param value Value of the environment variable, can be NULL
 * @param envl Pointer to the environment list
 * @param exported Export status for the variable:
 *                 0: not exported
 *                 1: exported without value
 *                 2: fully exported
 * @return SUCCESS on successful addition, ERROR on memory allocation failure
 */
static int	add_new_var(char *var, char *value, t_list **envl, int exported);

/**
 * @brief Adds or updates an environment variable
 * 
 * If the variable exists, updates its value and export status.
 * If the variable doesn't exist, creates a new entry by calling add_new_var().
 *
 * @param var Name of the environment variable
 * @param value Value of the environment variable, can be NULL
 * @param envl Pointer to the environment list
 * @param exported Export status to set (if higher than current)
 * @return SUCCESS on successful operation, ERROR on failure
 */
int			add_env(char *var, char *value, t_list **envl, int exported);

/**
 * @brief Processes a single environment variable for export
 *
 * Parses the input string to separate variable name and value (if present).
 * Checks if the variable name contains only authorized characters.
 *
 * @param var String in format "name=value" or just "name"
 * @param envl Pointer to the environment list
 * @param exported Export status to set
 * @return SUCCESS if variable was added/updated, ERROR on invalid identifier
 */
static int	export_one(char *var, t_list **envl, int exported);

/**
 * @brief Processes multiple environment variables for export
 *
 * Iterates through the array of variable strings and calls export_one()
 * on each of them.
 *
 * @param vars NULL-terminated array of strings to process
 * @param envl Pointer to the environment list
 * @param exported Export status to set for all variables
 * @return SUCCESS if all variables were exported successfully, ERROR otherwise
 */
int			export_all(char **vars, t_list **envl, int exported);

static int	add_new_var(char *var, char *value, t_list **envl, int exported)
{
	t_list	*new;
	t_env	*cont;

	new = malloc(sizeof(t_list));
	if (!new)
		return (ERROR);
	cont = malloc(sizeof(t_env));
	if (!cont)
	{
		free(new);
		return (ERROR);
	}
	cont->var = var;
	cont->value = value;
	cont->exported = 0;
	if (!value && exported == 2)
		cont->exported = 1;
	else if (value && exported == 2)
		cont->exported = 2;
	new->content = cont;
	new->next = NULL;
	ft_lstadd_back(envl, new);
	return (SUCCESS);
}

int			add_env(char *var, char *value, t_list **envl, int exported)
{
	t_list	*env;

	env = *envl;
	while (env)
	{
		if (ft_strcmp(((t_env *)env->content)->var, var) == 0)
		{
			if (value)
			{
				if (((t_env *)env->content)->value)
					free(((t_env *)env->content)->value);
				((t_env *)env->content)->value = value;
			}
			if (exported > ((t_env *)env->content)->exported)
				((t_env *)env->content)->exported = exported;
			return (SUCCESS);
		}
		env = env->next;
	}
	return (add_new_var(ft_strdup(var), value, envl, exported));
}

static int	export_one(char *var, t_list **envl, int exported)
{
	char	*value;
	char	*tmp;

	tmp = ft_strchr(var, '=');
	if (tmp)
	{
		value = ft_strdup(tmp + 1);
		tmp[0] = '\0';
	}
	else
		value = NULL;
	if (authorized_char(var))
		return (add_env(var, value, envl, exported));
	invalid_identifier(var, "export", exported);
	if (value)
		free(value);
	return (ERROR);
}

int			export_all(char **vars, t_list **envl, int exported)
{
	int	i;
	int	err;

	i = 0;
	err = 0;
	while (vars[i])
	{
		err += export_one(vars[i], envl, exported);
		i++;
	}
	if (err)
		return (ERROR);
	return (SUCCESS);
}
