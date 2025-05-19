
#include "minishell.h"
/**
 * @file free.c
 * @brief Utility functions for freeing allocated memory in the minishell project.
 */

/**
 * @brief Frees all allocated memory for a given line and split structure.
 *
 * @param line The line to be freed.
 * @param split The split structure containing strings to be freed.
 */
void free_all(char *line, t_split *split);

/**
 * @brief Frees all allocated memory for a given array of strings.
 *
 * @param args The array of strings to be freed.
 */
void free_tab(char **args);

/**
 * @brief Frees all allocated memory for given linked lists of words, separators, and spaces.
 *
 * @param words The linked list of words to be freed.
 * @param seps The linked list of separators to be freed.
 * @param spaces The linked list of spaces to be freed.
 */
void free_lists(t_list **words, t_list **seps, t_list **spaces);

/**
 * @brief Frees all allocated memory for a given command structure.
 *
 * @param cmd The command structure to be freed.
 */
static void free_cmd(t_info *cmd);

/**
 * @brief Recursively frees all allocated memory for a given binary tree structure.
 *
 * @param tree The binary tree structure to be freed.
 */
void free_tree(t_tree *tree);

void		free_all(char *line, t_split *split)
{
	int	i;

	(void)line;
	if (line)
		free(line);
	if (!split)
		return ;
	i = 0;
	while (split[i].str)
	{
		free(split[i].str);
		i++;
	}
	free(split);
}

void		free_tab(char **args)
{
	int	i;

	if (!args)
		return ;
	i = 0;
	while (args[i])
	{
		free(args[i]);
		i++;
	}
	free(args);
}

void		free_lists(t_list **words, t_list **seps, t_list **spaces)
{
	ft_lstclear(words, &free);
	ft_lstclear(seps, &free);
	ft_lstclear(spaces, &free);
}

static void	free_cmd(t_info *cmd)
{
	if (cmd->args)
		free_tab(cmd->args);
	if (cmd->argv)
		free(cmd->argv);
	if (cmd->env)
		free_tab(cmd->env);
	if (cmd->seps)
		free(cmd->seps);
	if (cmd->spaces)
		free(cmd->spaces);
	if (cmd->args_tmp)
		free_tab(cmd->args_tmp);
	if (cmd->seps_tmp)
		free(cmd->seps_tmp);
	if (cmd->spaces_tmp)
		free(cmd->spaces_tmp);
}

void		free_tree(t_tree *tree)
{
	if (!tree)
		return ;
	free_tree(tree->left);
	free_tree(tree->right);
	if (tree->info)
	{
		free_cmd(tree->info);
		close_unused_fd(tree->info);
		free(tree->info);
	}
	if (tree)
		free(tree);
}
