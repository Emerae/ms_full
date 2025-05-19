#include "minishell.h"
/**
 * @brief Implements the 'export' builtin command functionality
 *
 * This function handles the shell's export command which is used to set
 * environment variables. When called without arguments, it displays all
 * environment variables in alphabetical order. When called with arguments,
 * it sets each argument as an environment variable.
 *
 * @param cmd Structure containing command information including arguments
 * @param envl Double pointer to the environment variable list
 *
 * @return SUCCESS (0) if executed in a subprocess,
 *         return value of print_sorted() if no arguments are provided,
 *         return value of export_all() if arguments are provided
 */

int		ft_export(t_info *cmd, t_list **envl)
{
	if (number_of_args(cmd->argv + cmd->offset) <= 1)
		return (print_sorted(cmd, *envl));
	if (!cmd->is_subprocess)
		return (export_all(cmd->argv + cmd->offset + 1, envl, 2));
	else
		return (SUCCESS);
}
