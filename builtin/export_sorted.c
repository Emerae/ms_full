#include "minishell.h"
/**
 * @file export_sorted.c
 * @brief Functions for displaying environment variables in sorted order
 *
 * This file contains functions to handle the printing of environment
 * variables in alphabetically sorted order, typically for the 'export'
 * builtin command with no arguments.
 *
 * @function copy_entry
 * @brief Creates a deep copy of an environment variable entry
 * @param entry The environment entry to copy (t_env *)
 * @return A newly allocated copy of the entry, or NULL if allocation fails
 *
 * @function cmp_entry
 * @brief Comparison function for sorting environment entries
 * @param data1 First environment entry (t_env *)
 * @param data2 Second environment entry (t_env *)
 * @return Integer less than, equal to, or greater than 0 if data1 is found
 *         to be less than, equal to, or greater than data2
 *
 * @function print_sorted
 * @brief Prints the environment variables in alphabetically sorted order
 * @param envl The environment variable linked list
 * @param cmd Command information structure containing I/O redirection details
 * @return SUCCESS on successful execution, or appropriate error code
 *
 * The print_sorted function forks a child process to handle sorting and printing.
 * This ensures that all memory allocated during the sort operation is properly
 * cleaned up when the child process exits.
 */

static void	*copy_entry(void *entry)
{
	t_env	*ventry;
	t_env	*res;

	ventry = (t_env *)entry;
	res = malloc(sizeof(t_env));
	if (!res)
		return (NULL);
	res->var = ft_strdup(ventry->var);
	res->value = NULL;
	if (ventry->value)
		res->value = ft_strdup(ventry->value);
	res->exported = ventry->exported;
	return (res);
}

static int	cmp_entry(void *data1, void *data2)
{
	char	*str1;
	char	*str2;

	str1 = ((t_env *)data1)->var;
	str2 = ((t_env *)data2)->var;
	if (str1 && str2)
		return (ft_strcmp(str1, str2));
	return (0);
}

int			print_sorted(t_info *cmd, t_list *envl)
{
	t_list	*sorted;
	int		pid;

	pid = fork();
	if (pid == -1)
		return (error_msg(FORK_FAIL));
	else if (pid == 0)
	{
		change_stdin_stdout(cmd);
		sorted = ft_lstmap(envl, &copy_entry, &free_entry);
		ft_lstsort(&sorted, &cmp_entry);
		print_envl(sorted, 1);
		ft_lstclear(&sorted, &free_entry);
		exit(SUCCESS);
	}
	return (SUCCESS);
}
