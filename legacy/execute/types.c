
#include "minishell.h"
/**
 * @brief Executes built-in shell commands
 * 
 * Sets up an array of function pointers for each built-in command
 * (echo, cd, pwd, export, unset, env, exit) and calls the appropriate one 
 * based on cmd->builtin index.
 * 
 * @param cmd Structure containing command information and arguments
 * @param envl Double pointer to the environment list to allow modification
 * @return Return value of the executed built-in command
 */

/**
 * @brief Executes an external executable file
 * 
 * Handles execution of executable files by forking and executing the command.
 * Sets a special signal value (6) if the command is "./minishell" to handle
 * recursive shell execution.
 * 
 * @param cmd Structure containing command information and arguments
 * @param envl Double pointer to the environment list
 * @return Return value from fork_and_exec function
 */

/**
 * @brief Handles command declarations with environment variables
 * 
 * Processes variable assignments (commands with '=' character) and then
 * delegates to the appropriate execution function for any command that follows.
 * If no command follows the declarations and not in a subprocess, exports
 * all the variables to the environment.
 * 
 * @param cmd Structure containing command information and arguments
 * @param envl Double pointer to the environment list to allow modification
 * @return Status code from the executed command or SUCCESS
 */

/**
 * @brief Executes binary files from system paths
 * 
 * Locates and executes binary files found in system paths.
 * Prints an error if the command is not found.
 * Sets a special signal value (6) for terminal interaction commands
 * like "man", "vi", and "vim".
 * 
 * @param cmd Structure containing command information and arguments
 * @param envl Double pointer to the environment list
 * @return Status code of execution or NOTFOUND if command not found
 */

int	exec_builtin(t_info *cmd, t_list **envl)
{
	t_exec	builtin[NB_BUILTIN];

	builtin[ECHO] = &ft_echo;
	builtin[CD] = &ft_cd;
	builtin[PWD] = &ft_pwd;
	builtin[EXPORT] = &ft_export;
	builtin[UNSET] = &ft_unset;
	builtin[ENV] = &ft_env;
	builtin[EXIT] = &ft_exit;
	return (builtin[cmd->builtin](cmd, envl));
}

int	exec_executable(t_info *cmd, t_list **envl)
{
	if (ft_strcmp(cmd->argv[cmd->offset], "./minishell") == 0)
		g_signal = 6;
	return (fork_and_exec(cmd, *envl, NULL));
}

int	exec_declaration(t_info *cmd, t_list **envl)
{
	int		i;
	t_exec	exec_func[NB_TYPES];

	exec_func[BUILTIN] = &exec_builtin;
	exec_func[EXECUTABLE] = &exec_executable;
	exec_func[DECLARATION] = &exec_declaration;
	exec_func[EXECBIN] = &exec_execbin;
	i = 0;
	while (cmd->argv[i] && ft_strchr(cmd->argv[i], '='))
		i++;
	if (cmd->argv[i])
	{
		cmd->offset = i;
		return (exec_func[cmd_type(cmd, i)](cmd, envl));
	}
	else if (!cmd->is_subprocess)
		return (export_all(cmd->argv, envl, 0));
	return (SUCCESS);
}

int	exec_execbin(t_info *cmd, t_list **envl)
{
	char	*file;
	int		err;

	if (open_executable(cmd, *envl, &file))
	{
		if (file)
			free(file);
		print_error(cmd->argv[cmd->offset], NULL, 0, "command not found");
		return (NOTFOUND);
	}
	if (ft_strcmp(cmd->argv[cmd->offset], "man") == 0 
		|| ft_strcmp(cmd->argv[cmd->offset], "vi") == 0
		|| ft_strcmp(cmd->argv[cmd->offset], "vim") == 0)
		g_signal = 6;
	err = fork_and_exec(cmd, *envl, file);
	free(file);
	return (err);
}
