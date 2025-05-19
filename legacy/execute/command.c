
#include "minishell.h"
/**
 * @brief Checks if a string is a built-in command
 *
 * This function determines if the provided string matches any of the
 * shell's built-in commands (echo, cd, pwd, export, unset, env, exit).
 *
 * @param str The command string to check
 * @return Returns the corresponding command enum value if the string is a builtin,
 *         or -1 if it's not a builtin command
 */
static int	is_builtin(char *str);

/**
 * @brief Determines the type of a command
 *
 * This function analyzes the command and classifies it as one of:
 * - BUILTIN: a shell built-in command
 * - DECLARATION: a variable declaration (contains '=')
 * - EXECUTABLE: a command with a path
 * - EXECBIN: a command to be found in PATH
 *
 * @param cmd The command information structure
 * @param i Index of the argument to analyze
 * @return The command type or -1 if no command found
 */
int			cmd_type(t_info *cmd, int i);

/**
 * @brief Handles and prints I/O redirection errors
 *
 * Prints appropriate error messages for redirection errors, either 
 * using the stored error information or a generic syntax error.
 *
 * @param cmd The command information structure containing error details
 * @return Always returns ERROR
 */
static int	error_in_out(t_info *cmd);

/**
 * @brief Executes a command based on its type
 *
 * This is the main command execution function that:
 * 1. Sets up function pointers for different command types
 * 2. Handles signals and updates the return value
 * 3. Expands variables and wildcards
 * 4. Sets up input/output redirections
 * 5. Creates argument tables
 * 6. Executes the command by calling the appropriate handler
 *
 * @param cmd The command information structure
 * @param split The split structure containing tokenized input
 * @param envl Double pointer to the environment list
 * @return SUCCESS on successful execution, ERROR otherwise
 */
int			execute_cmd(t_info *cmd, t_split *split, t_list **envl);

static int	is_builtin(char *str)
{
	if (ft_strcmp(str, "echo") == 0)
		return (ECHO);
	if (ft_strcmp(str, "cd") == 0)
		return (CD);
	if (ft_strcmp(str, "pwd") == 0)
		return (PWD);
	if (ft_strcmp(str, "export") == 0)
		return (EXPORT);
	if (ft_strcmp(str, "unset") == 0)
		return (UNSET);
	if (ft_strcmp(str, "env") == 0)
		return (ENV);
	if (ft_strcmp(str, "exit") == 0)
		return (EXIT);
	return (-1);
}

int			cmd_type(t_info *cmd, int i)
{
	if (!cmd->argv[i])
		return (-1);
	cmd->builtin = is_builtin(cmd->argv[i]);
	if (cmd->builtin > -1)
		return (BUILTIN);
	if (ft_strchr(cmd->argv[i], '='))
		return (DECLARATION);
	if (is_path(cmd->argv[i]))
		return (EXECUTABLE);
	else
		return (EXECBIN);
}

static int	error_in_out(t_info *cmd)
{
	if (cmd->err)
		print_error(NULL, cmd->file_error, cmd->err, NULL);
	else
		print_error(NULL, NULL, 0,\
		"syntax error near unexpected token `newline'\n");
	return (ERROR);
}

int			execute_cmd(t_info *cmd, t_split *split, t_list **envl)
{
	t_exec	exec_func[NB_TYPES];

	exec_func[BUILTIN] = &exec_builtin;
	exec_func[EXECUTABLE] = &exec_executable;
	exec_func[DECLARATION] = &exec_declaration;
	exec_func[EXECBIN] = &exec_execbin;
	if (g_signal)
		update_return(envl, 128 + g_signal);
	cmd->split = split;
	if (cmd->number == 0)
		return (SUCCESS);
	if (expand(cmd, *envl, split))
		return (ERROR);
	if (cmd->nb_args_tmp == 0)
		return (SUCCESS);
	if (update_in_out(cmd) < 0)
		return (error_in_out(cmd));
	if (create_tab_args(cmd))
		return (ERROR);
	if (cmd_type(cmd, 0) >= 0)
		return (exec_func[cmd_type(cmd, 0)])(cmd, envl);
	return (SUCCESS);
}
