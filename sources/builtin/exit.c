
#include "minishell.h"
/**
 * @file exit.c
 * @brief Implementation of the shell's built-in exit command
 *
 * This file contains the implementation of the 'exit' built-in command for the
 * minishell project. The exit command terminates the shell with a status code.
 *
 * @functions
 * - static int ft_isnum(char *str): Checks if a string contains only digits
 *   @param str The string to check
 *   @return 1 if all characters are digits, 0 otherwise
 *
 * - static void free_exit(t_info *cmd, t_list **envl): Prints exit message and
 *   frees all allocated memory before termination
 *   @param cmd Pointer to command info structure
 *   @param envl Pointer to environment variable list
 *
 * - int ft_exit(t_info *cmd, t_list **envl): Main exit command handler with argument parsing
 *   @param cmd Pointer to command info structure
 *   @param envl Pointer to environment variable list
 *   @return ERROR if too many arguments, otherwise does not return (calls exit())
 *
 * @behavior
 * - With no arguments: exits with the last command's status code (stored in ?begin)
 * - With one numeric argument: exits with that value as status code
 * - With non-numeric argument: prints error and exits with MISUSE status
 * - With multiple arguments: prints error and returns ERROR without exiting
 */

static int	ft_isnum(char *str)
{
	int	i;

	i = 0;
	while (str[i])
	{
		if (!ft_isdigit(str[i]))
			return (0);
		i++;
	}
	return (1);
}

static void	free_exit(t_info *cmd, t_list **envl)
{
	ft_putstr("exit\n");
	free_all(cmd->line, cmd->split);
	free_tree(cmd->root);
	ft_lstclear(envl, &free_entry);
}

int			ft_exit(t_info *cmd, t_list **envl)
{
	int	exit_value;

	if (number_of_args(cmd->argv) < 2)
	{
		exit_value = ft_atoi(search_in_env(*envl, "?begin"));
		free_exit(cmd, envl);
		exit(exit_value);
	}
	else if (!ft_isnum(cmd->argv[cmd->offset + 1]))
	{
		free_exit(cmd, envl);
		print_error("minishell: exit", NULL, 0, "numeric argument required");
		exit(MISUSE);
	}
	else if (number_of_args(cmd->argv) > 2)
	{
		ft_putstr("exit\n");
		print_error("minishell: exit", NULL, 0, "too many arguments");
		return (ERROR);
	}
	exit_value = ft_atoi(cmd->argv[cmd->offset + 1]);
	free_exit(cmd, envl);
	exit(exit_value);
}
