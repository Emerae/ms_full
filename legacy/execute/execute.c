
#include "minishell.h"
/**
 * @brief Converts process status to appropriate return value
 *
 * Handles status codes from processes by either using the error code passed in
 * or interpreting the process status value.
 *
 * @param status The status returned from wait() system call
 * @param err Default error value to return if status is not set
 * @return The interpreted return value
 */
static int	return_value(int status, int err);

/**
 * @brief Recursively executes commands in the syntax tree
 *
 * Traverses the syntax tree and executes commands based on their type.
 * Handles command execution and manages semicolon operators by waiting
 * for child processes to complete before continuing.
 *
 * @param tree The syntax tree node being processed
 * @param split Array of split command components
 * @param envl Pointer to environment variable list
 * @return The execution status/error code
 */
static int	execute_recursive(t_tree *tree, t_split *split, t_list **envl);

/**
 * @brief Removes trailing semicolons from the split command array
 *
 * Scans the split command array and removes the last semicolon
 * separator if present to prevent syntax errors.
 *
 * @param split Array of split command components
 */
static void	remove_last_semic(t_split *split);

/**
 * @brief Main execution function that processes and runs commands
 *
 * Sets up the execution environment by:
 * 1. Removing trailing semicolons
 * 2. Creating a syntax tree from the split command
 * 3. Setting up pipes recursively
 * 4. Executing commands recursively
 * 5. Waiting for all processes to complete
 * 6. Cleaning up resources
 *
 * @param split Array of split command components
 * @param envl Pointer to environment variable list
 * @param line Original command line string
 * @return Final execution status or error code
 */
int			execute(t_split *split, t_list **envl, char *line);

static int	return_value(int status, int err)
{
	int	ret;

	ret = err;
	if (status > 0)
	{
		if (status % 256 == 0)
			ret = status / 256;
		else
			ret = status;
	}
	return (ret);
}

static int	execute_recursive(t_tree *tree, t_split *split, t_list **envl)
{
	int	err;
	int	status;

	if (!tree)
		return (0);
	status = SUCCESS;
	err = execute_recursive(tree->left, split, envl);
	if (tree->info->type == CMD)
	{
		err = execute_cmd(tree->info, split, envl);
		close_unused_fd(tree->info);
		return (err);
	}
	if (tree->info->type == SEMIC)
	{
		while (wait(&status) > 0)
			;
		update_last_arg(envl, tree->info, split);
		update_return(envl, return_value(status, err));
	}
	return (execute_recursive(tree->right, split, envl));
}

static void	remove_last_semic(t_split *split)
{
	int	i;

	i = 0;
	while (split[i].str)
		i++;
	if (i > 0 && split[i - 1].sep == ';')
	{
		split[i - 1].str = NULL;
		split[i - 1].sep = '\0';
	}
}

int			execute(t_split *split, t_list **envl, char *line)
{
	t_tree	*tree;
	int		status;
	int		err;

	status = SUCCESS;
	remove_last_semic(split);
	tree = create_tree(split, line);
	if (pipe_recursive(tree, split, envl))
	{
		free_tree(tree);
		print_error(NULL, NULL, TOOMANY, NULL);
		return (TOOMANY);
	}
	err = execute_recursive(tree, split, envl);
	if (g_signal != 6)
		g_signal = 5;
	while (wait(&status) > 0)
		;
	free_tree(tree);
	return (return_value(status, err));
}
