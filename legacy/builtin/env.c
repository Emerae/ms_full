
#include "minishell.h"
/**
 * @file env.c
 * @brief Functions for handling environment variables and env command
 */

/**
 * @brief Prints an environment entry in the format used by 'declare -x'
 *
 * Outputs the environment entry in the format "declare -x VAR="VALUE"" to stdout.
 * If the entry has no value, it will just output "declare -x VAR".
 *
 * @param entry The environment entry to print
 */
static void	print_quoted(t_env entry);

/**
 * @brief Prints an environment entry in the standard format
 *
 * Outputs the environment entry in the format "VAR=VALUE" to stdout.
 *
 * @param entry The environment entry to print
 */
static void	print_unquoted(t_env entry);

/**
 * @brief Prints a list of environment variables
 *
 * Iterates through the environment list and prints each entry.
 * If 'declare' is set, entries are printed in the 'declare -x' format with quotes.
 * Otherwise, entries are printed in the standard VAR=VALUE format.
 * Only prints variables with authorized characters and based on their exported status.
 *
 * @param envl A linked list of environment variables
 * @param declare Flag to determine if output should be in 'declare -x' format
 */
void		print_envl(t_list *envl, int declare);

/**
 * @brief Implements the 'env' shell command
 *
 * Forks a process and prints all environment variables that have been exported.
 * Reports an error if any arguments are provided, as the 'env' command
 * should be used without options or arguments.
 * Updates the "_" environment variable to "env".
 *
 * @param cmd Command information structure containing arguments and offset
 * @param envl Pointer to the list of environment variables
 * @return SUCCESS always for the parent process
 */
int			ft_env(t_info *cmd, t_list **envl);

static void	print_quoted(t_env entry)
{
	ft_putstr("declare -x ");
	ft_putstr(entry.var);
	if (entry.value)
	{
		ft_putstr("=\"");
		ft_putstr(entry.value);
		ft_putstr("\"");
	}
	ft_putstr("\n");
}

static void	print_unquoted(t_env entry)
{
	ft_putstr(entry.var);
	ft_putstr("=");
	ft_putstr(entry.value);
	ft_putstr("\n");
}

void		print_envl(t_list *envl, int declare)
{
	t_env	*entry;

	while (envl)
	{
		entry = (t_env *)envl->content;
		if (authorized_char(entry->var))
		{
			if (declare && entry->exported >= 1
				&& ft_strcmp(entry->var, "_") != 0)
				print_quoted(*entry);
			else if (!declare && entry->exported >= 2)
				print_unquoted(*entry);
		}
		envl = envl->next;
	}
}

int			ft_env(t_info *cmd, t_list **envl)
{
	int		pid;

	add_env("_", ft_strdup("env"), envl, 1);
	pid = fork();
	if (pid == 0)
	{
		if (number_of_args(cmd->argv + cmd->offset) > 1)
		{
			print_error("env", NULL, 0,\
			"should be used without option and argument");
			exit(MISUSE);
		}
		change_stdin_stdout(cmd);
		print_envl(*envl, 0);
		exit(SUCCESS);
	}
	return (SUCCESS);
}
