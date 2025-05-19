
#include "minishell.h"
/**
 * @file errors.c
 * @brief Contains utility functions for error handling and error messages in minishell.
 * 
 * This file implements various functions to print formatted error messages to the standard
 * error output (stderr). It handles different types of errors including syntax errors,
 * allocation failures, and invalid identifiers.
 */

/**
 * @brief Prints parsing-related error messages.
 * 
 * Formats and prints error messages related to parsing issues such as unclosed quotes,
 * redirection syntax errors, and memory allocation failures.
 * 
 * @param error A t_error structure containing error information (error number and token).
 */

/**
 * @brief Prints a general error message with optional executable and file information.
 * 
 * Formats and prints an error message to stderr with the specified components.
 * 
 * @param exe Optional executable name causing the error.
 * @param file Optional filename related to the error.
 * @param err Error number to be translated via strerror() if error parameter is NULL.
 * @param error Optional custom error message; if provided, err is ignored.
 */

/**
 * @brief Prints an "invalid identifier" error message.
 * 
 * Used for reporting issues with variable names or other identifiers that don't
 * conform to the expected format.
 * 
 * @param str The invalid identifier string.
 * @param func Optional function name where the error occurred.
 * @param exported Flag indicating if the error occurred during an export operation.
 */

/**
 * @brief Prints a generic error message based on the error code.
 * 
 * @param error Error code indicating the type of error.
 * @return Always returns ERROR macro value.
 */

/**
 * @brief Prints an error message with additional information.
 * 
 * @param error Error code indicating the type of error.
 * @param info Additional information to include in the error message.
 * @return Always returns ERROR macro value.
 */

void	print_error_parsing(t_error error)
{
	if (error.num == SUCCESS)
		return ;
	ft_putstr_fd("minishell: ", STDERR);
	if (error.num == SYNTAX_QUOTES)
		ft_putstr_fd("syntax error near unexpected token `newline'\n", STDERR);
	else if (error.num == SYNTAX_REDIR)
	{
		ft_putstr_fd("syntax error near unexpected token `", STDERR);
		if (error.token == 'd')
			ft_putstr_fd(">>", STDERR);
		else
			ft_putchar_fd(error.token, STDERR);
		ft_putstr_fd("'\n", STDERR);
	}
	else if (error.num == ALLOCATION_FAIL)
		ft_putstr_fd("allocation failed\n", STDERR);
}

void	print_error(char *exe, char *file, int err, char *error)
{
	if (!error)
		ft_putstr_fd("minishell: ", STDERR);
	if (exe)
	{
		ft_putstr_fd(exe, STDERR);
		ft_putstr_fd(": ", STDERR);
	}
	if (file)
	{
		ft_putstr_fd(file, STDERR);
		ft_putstr_fd(": ", STDERR);
	}
	if (error)
		ft_putstr_fd(error, STDERR);
	else
		ft_putstr_fd(strerror(err), STDERR);
	ft_putstr_fd("\n", STDERR);
}

void	invalid_identifier(char *str, char *func, int exported)
{
	ft_putstr_fd("minishell: ", STDERR);
	if (func && exported)
	{
		ft_putstr_fd(func, STDERR);
		ft_putstr_fd(": ", STDERR);
	}
	write(STDERR, "`", 1);
	ft_putstr_fd(str, STDERR);
	ft_putstr_fd("': not a valid identifier\n", STDERR);
}

int		error_msg(int error)
{
	ft_putstr_fd("minishell: ", STDERR);
	if (error == ALLOCATION_FAIL)
		ft_putstr_fd("allocation failed\n", STDERR);
	else if (error == FORK_FAIL)
		ft_putstr_fd("fork failed\n", STDERR);
	return (ERROR);
}

int		error_msg_info(int error, char *info)
{
	ft_putstr_fd("minishell: ", STDERR);
	if (error == AMBIGUOUS_REDIR)
	{
		ft_putstr_fd(info, STDERR);
		ft_putstr_fd(": ambiguous redirect\n", STDERR);
	}
	return (ERROR);
}
