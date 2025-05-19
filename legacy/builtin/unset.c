
#include "minishell.h"
/**
 * @file unset.c
 * @brief Implementation of the unset builtin command
 *
 * @function unset_one
 * @brief Removes a specific environment variable from the environment list
 * @param to_unset The name of the environment variable to remove
 * @param envl The linked list of environment variables
 * @note The function assumes that the first node of envl is a header node and
 *       starts searching from the second node
 *
 * @function ft_unset
 * @brief Implementation of the unset builtin command
 * @param cmd Structure containing command information and arguments
 * @param envl Pointer to the linked list of environment variables
 * @return SUCCESS (0) if all variables were unset successfully,
 *         ERROR (1) if any identifier was invalid
 * @note The function validates each argument to ensure it contains only
 *       authorized characters and doesn't contain an '=' character
 */

static void	unset_one(char *to_unset, t_list *envl)
{
	t_list	*previous;

	previous = envl;
	envl = envl->next;
	while (envl)
	{
		if (variable_match(envl, to_unset, 0))
		{
			previous->next = envl->next;
			free_entry(envl->content);
			free(envl);
			return ;
		}
		previous = envl;
		envl = envl->next;
	}
}

int			ft_unset(t_info *cmd, t_list **envl)
{
	int		i;
	int		ret;

	if (cmd->number <= 1)
		return (0);
	i = cmd->offset + 1;
	ret = SUCCESS;
	while (cmd->argv[i])
	{
		if (authorized_char(cmd->argv[i]) && !ft_strchr(cmd->argv[i], '='))
			unset_one(cmd->argv[i], *envl);
		else
		{
			invalid_identifier(cmd->argv[i], "unset", 1);
			ret = ERROR;
		}
		i++;
	}
	return (ret);
}
