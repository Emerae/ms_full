
#include "minishell.h"
/**
 * @file cd.c
 * @brief Implementation of the cd (change directory) command for minishell
 * 
 * This file contains functions that handle directory navigation within the shell.
 * It supports changing to home directory, previous directory, and specified paths.
 * 
 * @static cd_home - Changes current directory to the user's home directory
 * @param envl      List of environment variables to find HOME
 * @return          SUCCESS (0) if successful, ERROR (1) if failed
 * 
 * @static cd_old - Changes current directory to the previous directory (OLDPWD)
 * @param envl      List of environment variables to find OLDPWD
 * @return          SUCCESS (0) if successful, ERROR (1) if failed
 * 
 * @function ft_cd - Main cd command implementation
 * @param cmd       Structure containing command information and arguments
 * @param envl      Pointer to the list of environment variables
 * @return          SUCCESS (0) if successful, ERROR (1) if failed,
 *                  MISUSE (2) if too many arguments provided
 * 
 * Error handling:
 * - HOME/OLDPWD not set
 * - Too many arguments
 * - Invalid/inaccessible path
 */

static int	cd_home(t_list *envl)
{
	char	*path_home;

	path_home = search_in_env(envl, "HOME");
	if (!path_home)
	{
		print_error("minishell: cd", NULL, 0, "HOME not set");
		return (ERROR);
	}
	errno = 0;
	if (chdir(path_home))
	{
		print_error("cd", path_home, errno, NULL);
		return (ERROR);
	}
	return (SUCCESS);
}

static int	cd_old(t_list *envl)
{
	char	*path_old;

	path_old = search_in_env(envl, "OLDPWD");
	if (!path_old)
	{
		print_error("minishell: cd", NULL, 0, "OLDPWD not set");
		return (ERROR);
	}
	errno = 0;
	ft_putstr_fd(path_old, STDERR);
	ft_putstr_fd("\n", STDERR);
	if (chdir(path_old))
	{
		print_error("cd", path_old, errno, NULL);
		return (ERROR);
	}
	return (SUCCESS);
}

int			ft_cd(t_info *cmd, t_list **envl)
{
	int		err;
	char	*path;

	if (number_of_args(cmd->argv + cmd->offset) > 2)
	{
		print_error("minishell: cd", NULL, 0, "too many arguments");
		return (MISUSE);
	}
	if (cmd->args[cmd->offset + 1])
	{
		path = cmd->argv[cmd->offset + 1];
		errno = 0;
		if (ft_strcmp("-", path) == 0)
			return (cd_old(*envl));
		err = chdir(path);
		if (err)
		{
			print_error("cd", cmd->argv[cmd->offset + 1], errno, NULL);
			return (ERROR);
		}
		return (SUCCESS);
	}
	return (cd_home(*envl));
}
