
#include "minishell.h"
/**
 * @file tree.c
 * @brief Functions for creating and manipulating a syntax tree for shell command parsing
 */

/**
 * @brief Determines if a character represents a node type in the syntax tree
 *
 * @param c Character to check
 * @return int Returns the node type (PIPE or SEMIC) if the character is a node, 0 otherwise
 */

/**
 * @brief Recursively assigns the root pointer to each node in the tree
 *
 * This function traverses the tree and sets the root pointer in each node's info structure
 * to point to the provided root node, enabling nodes to access the tree's root.
 *
 * @param tree The current node in the tree
 * @param root The root node of the tree
 */

/**
 * @brief Checks if the previous string in a split array ends with a backslash
 *
 * This function is used to determine if a separator should be escaped (ignored).
 *
 * @param split Array of split strings
 * @param i Current position in the split array
 * @return int 1 if previous string ends with backslash, 0 otherwise
 */

/**
 * @brief Finalizes the tree construction
 *
 * Creates a leaf node with the remaining part of the input if necessary,
 * and sets the root pointer for all nodes in the tree.
 *
 * @param tree Pointer to the tree to be finalized
 * @param last_node The last processed node
 * @param i Array containing current position [0] and start position of current segment [1]
 * @param line Original input string
 */

/**
 * @brief Creates a syntax tree from a split string input
 *
 * This function builds a binary syntax tree representing the command structure.
 * It iterates through the split array, creating nodes for pipe (|) and semicolon (;)
 * operators, unless they are escaped with a backslash.
 *
 * @param split Array of split strings and their separators
 * @param line Original input string
 * @return t_tree* Root of the constructed syntax tree
 */

static int	is_node(char c)
{
	if (c == '|')
		return (PIPE);
	if (c == ';')
		return (SEMIC);
	return (0);
}

void		fill_root(t_tree *tree, t_tree *root)
{
	if (!tree)
		return ;
	tree->info->root = root;
	fill_root(tree->left, root);
	fill_root(tree->right, root);
}

int			previous_end_with_slash(t_split *split, int i)
{
	char	*last;

	if (i > 0)
	{
		last = split[i - 1].str;
		return (last[ft_strlen(last) - 1] == '\\');
	}
	return (0);
}

void		tree_final(t_tree **tree, t_tree *last_node, int i[2], char *line)
{
	if (last_node)
		last_node->right = create_leave(i[1], i[0] - i[1], line);
	if (!(*tree))
		*tree = create_leave(0, i[0], line);
	fill_root(*tree, *tree);
}

t_tree		*create_tree(t_split *split, char *line)
{
	t_tree	*tree;
	t_tree	*last_node;
	t_tree	*new_node;
	int		i[2];

	i[0] = 0;
	i[1] = 0;
	new_node = NULL;
	tree = NULL;
	while (split[i[0]].str)
	{
		last_node = new_node;
		if (is_node(split[i[0]].sep) && !previous_end_with_slash(split, i[0]))
		{
			new_node = create_node(i[0], is_node(split[i[0]].sep), i[1], line);
			if (!tree)
				tree = new_node;
			if (last_node)
				last_node->right = new_node;
			i[1] = i[0] + 1;
		}
		i[0]++;
	}
	tree_final(&tree, last_node, i, line);
	return (tree);
}
