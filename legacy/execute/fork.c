
#include "minishell.h"
/**
 * @brief Fork the current process and execute a command
 *
 * This function creates a child process using fork() and executes the specified
 * command with execve(). It handles redirection of stdin/stdout in the child process
 * before execution.
 *
 * @param cmd    Structure containing command information including arguments and environment
 * @param envl   Linked list of environment variables
 * @param file   Optional executable path that overrides cmd->args[cmd->offset] if provided
 *
 * @return SUCCESS on successful fork, error code otherwise
 *         The child process doesn't return but exits with:
 *         - NOTFOUND (127) if command not found
 *         - CANTEXEC (126) for permission issues or other execution errors
 */

int	fork_and_exec(t_info *cmd, t_list *envl, char *file)
{
	int		pid;
	char	**argv;
	char	*to_exec;

	cmd->env = create_env_tab(envl, 0);
	pid = fork();
	if (pid == -1)
		return (error_msg(FORK_FAIL));
	else if (pid == 0)
	{
		change_stdin_stdout(cmd);
		errno = 0;
		argv = cmd->argv + cmd->offset;
		to_exec = cmd->args[cmd->offset];
		if (file)
			to_exec = file;
		execve(to_exec, argv, cmd->env);
		print_error(to_exec, NULL, errno, NULL);
		if (errno == 2)
			exit(NOTFOUND);
		else
			exit(CANTEXEC);
	}
	return (SUCCESS);
}
