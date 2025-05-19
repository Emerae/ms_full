#include "minishell.h"
/**
 * @file update.c
 * @brief Functions for updating environment variables in minishell
 */

/**
 * @brief Updates the return code environment variable
 *
 * Updates the "?begin" environment variable with the given error code or
 * signal value. When g_signal is 2 (SIGINT), it sets the return code to 130.
 *
 * @param envl Pointer to the environment variable list
 * @param err Error code to set if not interrupted by signal
 */

/**
 * @brief Retrieves the current return code
 *
 * Gets the value of the "?begin" environment variable and converts it to integer.
 *
 * @param envl The environment variable list
 * @return The integer value of the stored return code
 */

/**
 * @brief Updates PWD and OLDPWD environment variables
 *
 * Checks if the current working directory differs from the stored PWD value.
 * If it does, updates OLDPWD to the previous PWD value and PWD to the current directory.
 *
 * @param envl Pointer to the environment variable list
 */

/**
 * @brief Updates the underscore (_) environment variable with the last argument
 *
 * Sets the "_" environment variable to the value of the last processed argument.
 * This mimics bash behavior of tracking the last argument.
 *
 * @param envl Pointer to the environment variable list
 * @param cmd Command information structure or NULL
 * @param arg Last argument or NULL
 */

void	update_return(t_list **envl, int err)
{
	if (g_signal == 2)
		add_env("?begin", ft_itoa(130), envl, -1);
	else
		add_env("?begin", ft_itoa(err), envl, -1);
}

int		get_return(t_list *envl)
{
	return (ft_atoi(search_in_env(envl, "?begin")));
}

void	update_env(t_list **envl)
{
	char	*pwd;
	char	cwd[SIZE_PATH];

	pwd = search_in_env(*envl, "PWD");
	if (!pwd)
		return ;
	getcwd(cwd, SIZE_PATH);
	if (ft_strcmp(pwd, cwd) != 0)
	{
		add_env("OLDPWD", ft_strdup(pwd), envl, 1);
		add_env("PWD", ft_strdup(cwd), envl, 1);
	}
}

void	update_last_arg(t_list **envl, t_info *cmd, void *arg)
{
	int	i;
	char *last_arg;

	if (cmd)
	{
		i = cmd->start - 1;
		if (i >= 0)
			last_arg = ft_strdup(((t_split *)arg)[i].str);
		else
			return;
	}
	else
	{
		last_arg = ft_strdup((char *)arg);
	}
	if (last_arg)
		add_env("_", last_arg, envl, 1);
}
