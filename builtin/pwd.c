
#include "minishell.h"
/**
 * @brief Prints the current working directory
 *
 * Implements the pwd (print working directory) command by creating a child
 * process that outputs the current working directory. Uses getcwd() to retrieve
 * the current path, then forks a child process to handle output (allowing for
 * potential redirection via the cmd parameter).
 *
 * @param cmd Structure containing command information, used for stdin/stdout redirection
 * @param envl Environment variable list (unused in this function)
 * @return int SUCCESS on successful execution, or error code on failure
 */

int	ft_pwd(t_info *cmd, t_list **envl)
{
	char	cwd[SIZE_PATH];
	int		pid;

	(void)envl;
	getcwd(cwd, SIZE_PATH);
	pid = fork();
	if (pid == -1)
		return (error_msg(FORK_FAIL));
	else if (pid == 0)
	{
		change_stdin_stdout(cmd);
		ft_putstr(cwd);
		ft_putstr("\n");
		exit(SUCCESS);
	}
	return (SUCCESS);
}
