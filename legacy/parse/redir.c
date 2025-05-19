
#include "minishell.h"
/**
 * @file redir.c
 * @brief Functions to handle input/output redirection in a shell
 */

/**
 * @brief Handles input redirection (<)
 * 
 * Opens a file for reading and assigns it to the command's input file descriptor.
 * Closes any previously assigned input file descriptor.
 *
 * @param cmd Pointer to the command info structure
 * @param i Index of the redirection operator in args array
 * @return 0 on success, -1 if no filename provided, -2 if file open failed
 */

/**
 * @brief Handles output redirection (>)
 * 
 * Opens a file for writing and assigns it to the command's output file descriptor.
 * Creates the file if it doesn't exist, truncates it if it does.
 * Closes any previously assigned output file descriptor.
 *
 * @param cmd Pointer to the command info structure
 * @param i Index of the redirection operator in args array
 * @return 0 on success, -1 if no filename provided, -2 if file open failed
 */

/**
 * @brief Handles append redirection (>>)
 * 
 * Opens a file for appending and assigns it to the command's output file descriptor.
 * Creates the file if it doesn't exist, appends to the end if it does.
 * Closes any previously assigned output file descriptor.
 *
 * @param cmd Pointer to the command info structure
 * @param i Index of the redirection operator in args array
 * @return 0 on success, -1 if no filename provided, -2 if file open failed
 */

/**
 * @brief Processes all redirection operators in a command
 *
 * Iterates through the command arguments and handles any redirection operators
 * by calling the appropriate handler function (fill_input, fill_out, fill_app).
 * Updates the command's input and output file descriptors accordingly.
 *
 * @param cmd Pointer to the command info structure
 * @return 0 on success, -1 on error (missing filename or file open failure)
 */

static int	fill_input(t_info *cmd, int i)
{
	if (++i >= cmd->nb_args)
		return (-1);
	if (cmd->input != 0)
		close(cmd->input);
	errno = 0;
	cmd->input = open(cmd->args[i], O_RDONLY);
	cmd->err = errno;
	cmd->file_error = cmd->args[i];
	if (cmd->input < 0)
		return (-2);
	return (0);
}

static int	fill_out(t_info *cmd, int i)
{
	if (++i >= cmd->nb_args)
		return (-1);
	if (cmd->output != 1)
		close(cmd->output);
	errno = 0;
	cmd->output = open(cmd->args[i], O_WRONLY | O_CREAT | O_TRUNC, 0666);
	cmd->err = errno;
	cmd->file_error = cmd->args[i];
	if (cmd->output < 0)
		return (-2);
	return (0);
}

static int	fill_app(t_info *cmd, int i)
{
	if (++i >= cmd->nb_args)
		return (-1);
	if (cmd->output != 1)
		close(cmd->output);
	errno = 0;
	cmd->output = open(cmd->args[i], O_WRONLY | O_CREAT | O_APPEND, 0666);
	cmd->err = errno;
	cmd->file_error = cmd->args[i];
	if (cmd->output < 0)
		return (-2);
	return (0);
}

int			update_in_out(t_info *cmd)
{
	int		i;

	i = -1;
	while (++i < cmd->nb_args)
	{
		if (cmd->seps[i] == '<' && fill_input(cmd, i))
			return (-1);
		else if (cmd->seps[i] == '>' && fill_out(cmd, i))
			return (-1);
		else if (cmd->seps[i] == 'd' && fill_app(cmd, i))
			return (-1);
	}
	return (0);
}
