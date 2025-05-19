
#include "minishell.h"
/**
 * @file echo.c
 * @brief Implementation of the echo built-in command
 * 
 * This file contains functions to implement the echo built-in command
 * for the minishell project. It supports the '-n' option to suppress
 * the trailing newline.
 */

/**
 * @brief Prints arguments with spaces between them
 * 
 * Prints all arguments from the array starting at index i, with a space
 * between each argument. If option is 0, appends a newline character
 * at the end of the output.
 * 
 * @param args Array of strings to print
 * @param option Flag to control newline output (1: no newline, 0: with newline)
 * @param i Starting index in args array
 */

/**
 * @brief Processes echo command options
 * 
 * Checks for the presence of the '-n' option in the echo command arguments.
 * Returns the index of the first non-option argument.
 * 
 * @param cmd Structure containing command information
 * @return Index of the first non-option argument in the args array
 */

/**
 * @brief Implements the echo built-in command
 * 
 * Executes the echo command with support for the '-n' option, which suppresses
 * the trailing newline. Creates a child process to handle the command execution
 * and properly redirects input/output as needed.
 * 
 * @param cmd Structure containing command information including arguments
 * @param envl Pointer to the environment variables list (unused in this function)
 * @return SUCCESS (0) if the command executed successfully, or an error code
 */

static int	is_option_n(char *s)
{
	if (!s || *s != '-')             /* NULL or doesn’t start with '-'       */
		return (0);
	++s;
	if (!*s)                         /* a lone '-' is not a valid option     */
		return (0);
	while (*s == 'n')
		++s;
	return (*s == '\0');             /* accept only pure n’s                 */
}

int	builtin_echo(t_cmd *cmd, int fd_out)
{
	int	i;
	int	print_nl;

	if (!cmd || !cmd->args || !cmd->args[0])
		return (1);                       /* internal misuse – bail out    */

	i = 1;
	print_nl = 1;

	/* ---------- option parsing (-n / -nnn / …) -------------------------- */
	while (is_option_n(cmd->args[i]))
	{
		print_nl = 0;
		++i;
	}

	/* ---------- body ----------------------------------------------------- */
	while (cmd->args[i])
	{
		ft_putstr_fd(cmd->args[i], fd_out);
		if (cmd->args[i + 1])
			ft_putchar_fd(' ', fd_out);
		++i;
	}
	if (print_nl)
		ft_putchar_fd('\n', fd_out);

	return (0);
}
