
#include "minishell.h"
/**
 * @brief Recursively sets file descriptors and subprocess flag for a tree node and its left children
 *
 * This function traverses the tree starting from the given node and recursively sets
 * either the input or output file descriptor based on the 'out' parameter, and also
 * sets the subprocess flag.
 *
 * @param tree The tree node to modify
 * @param out Flag to determine whether to set output (1) or input (0) fd
 * @param fd The file descriptor to set
 * @param is_sub The subprocess flag value to set
 */
static void	fill_subtree_fd(t_tree *tree, int out, int fd, int is_sub);

/**
 * @brief Sets up pipes between commands in a command tree
 *
 * This function recursively traverses the command tree and creates pipes between
 * commands as needed. For semicolon operators (SEMIC), it resets the file descriptors
 * for the right subtree. For pipe operators, it creates a pipe and connects the
 * output of the left command to the input of the right command.
 *
 * @param tree The command tree to process
 * @param split Split structure (unused in this function)
 * @param envl Environment list (unused in this function)
 * @return SUCCESS if pipe setup was successful, PIPE_FAIL otherwise
 */
int			pipe_recursive(t_tree *tree, t_split *split, t_list **envl);

static void	fill_subtree_fd(t_tree *tree, int out, int fd, int is_sub)
{
	if (!tree)
		return ;
	if (out)
		tree->info->output = fd;
	else
		tree->info->input = fd;
	tree->info->is_subprocess = is_sub;
	fill_subtree_fd(tree->left, out, fd, is_sub);
}

int			pipe_recursive(t_tree *tree, t_split *split, t_list **envl)
{
	int	pfd[2];
	int	type;

	if (!tree)
		return (SUCCESS);
	type = tree->info->type;
	if (type == CMD)
		return (SUCCESS);
	if (type == SEMIC)
	{
		fill_subtree_fd(tree->right, 0, 0, 0);
		fill_subtree_fd(tree->right, 1, 1, 0);
	}
	else
	{
		if (pipe(pfd))
			return (PIPE_FAIL);
		tree->left->info->output = pfd[1];
		tree->left->info->is_subprocess = 1;
		fill_subtree_fd(tree->right, 0, pfd[0], 1);
	}
	if (pipe_recursive(tree->left, split, envl)
		|| pipe_recursive(tree->right, split, envl))
		return (PIPE_FAIL);
	return (SUCCESS);
}
