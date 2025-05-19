
#include "minishell.h"
/**
 * @brief Initializes auxiliary fields of the t_info structure
 * 
 * This function sets default values for various fields in the given t_info structure.
 * It's used as a helper function for init_node to reduce complexity.
 * 
 * @param info Pointer to the t_info structure to initialize
 */
static void	init_aux(t_info *info);

/**
 * @brief Initializes a new t_info structure for a node
 * 
 * Allocates and initializes a new t_info structure, setting both provided parameters
 * and default values. This structure contains parsing information for commands in a shell.
 * 
 * @param i Starting position of the node in the command line
 * @param type Type of node (0 for leaf/command, other values for operators)
 * @param number Length or token number
 * @param line Original command line string
 * @return t_info* Pointer to initialized structure, or NULL if allocation fails
 */
t_info		*init_node(int i, int type, int number, char *line);

/**
 * @brief Creates a leaf node in the parsing tree
 * 
 * Allocates and initializes a tree node with no children (leaf) that represents
 * a command or argument in the parsing tree.
 * 
 * @param start Starting position in the command line
 * @param number Length of the token
 * @param line Original command line string
 * @return t_tree* Pointer to the new leaf node, or NULL if allocation fails
 */
t_tree		*create_leave(int start, int number, char *line);

/**
 * @brief Creates an operator node in the parsing tree
 * 
 * Allocates and initializes a tree node that represents an operator (like pipe or redirect)
 * in the parsing tree. The left child is automatically created as a leaf.
 * 
 * @param i Position of the operator in the command line
 * @param type Type of the operator node
 * @param start Starting position of the left operand
 * @param line Original command line string
 * @return t_tree* Pointer to the new node, or NULL if allocation fails
 */
t_tree		*create_node(int i, int type, int start, char *line);

static void	init_aux(t_info *info)
{
	info->nb_args = 1;
	info->nb_args_tmp = 0;
	info->env = NULL;
	info->spaces = NULL;
	info->file_error = NULL;
	info->err = 0;
	info->first_word = NULL;
	info->start_args = 0;
	info->offset = 0;
	info->args_tmp = NULL;
	info->seps_tmp = NULL;
	info->spaces_tmp = NULL;
	info->all = NULL;
	info->split = NULL;
}

t_info		*init_node(int i, int type, int number, char *line)
{
	t_info	*info;

	info = malloc(sizeof(t_info));
	if (!info)
		return (NULL);
	info->type = type;
	info->input = 0;
	info->output = 1;
	info->start = i;
	info->number = number;
	info->builtin = 0;
	info->line = line;
	info->root = NULL;
	info->args = NULL;
	info->argv = NULL;
	info->seps = NULL;
	info->is_subprocess = 0;
	init_aux(info);
	return (info);
}

t_tree		*create_leave(int start, int number, char *line)
{
	t_tree	*new;
	t_info	*new_info;

	new = malloc(sizeof(t_tree));
	if (!new)
		return (NULL);
	new_info = init_node(start, 0, number, line);
	new->left = NULL;
	new->right = NULL;
	new->info = new_info;
	return (new);
}

t_tree		*create_node(int i, int type, int start, char *line)
{
	t_tree		*new;
	t_info		*new_info;

	new = malloc(sizeof(t_tree));
	if (!new)
		return (NULL);
	new_info = init_node(i, type, 1, line);
	new->left = create_leave(start, i - start, line);
	new->right = NULL;
	new->info = new_info;
	return (new);
}
