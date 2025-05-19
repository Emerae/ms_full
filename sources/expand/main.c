
#include "minishell.h"
/**
 * @brief Expands a string segment and adds it to the expansion list
 *
 * Based on the separator type, determines and performs the appropriate 
 * type of expansion. If the separator is a double quote ("), it performs
 * a simple expansion. Otherwise, it performs a hard expansion.
 * 
 * @param expansion Double pointer to the list where expanded elements are stored
 * @param curr The current split structure containing string and metadata
 * @param envl Linked list of environment variables
 * @param r Flag indicating redirection context
 * @return int SUCCESS on successful expansion, ERROR otherwise
 */

/**
 * @brief Converts a linked list of expanded elements into arrays
 *
 * Takes a linked list of expanded elements and converts it into parallel arrays
 * stored in the cmd structure: args_tmp (strings), seps_tmp (separators), and
 * spaces_tmp (space flags). Each array is NULL/'\0'/-1 terminated respectively.
 *
 * @param expansion Linked list containing expanded elements
 * @param cmd Pointer to command info structure where arrays will be stored
 * @return int SUCCESS on successful conversion, ERROR on allocation failure
 */

/**
 * @brief Processes and expands a single split element
 *
 * Handles expansion of a single element based on its separator type.
 * If the separator is a quote or space, performs expansion.
 * Otherwise, adds the element directly to the expansion list.
 * Updates the redirection flag based on the current element.
 *
 * @param expansion Double pointer to the expansion list
 * @param envl Linked list of environment variables
 * @param curr The current split element to process
 * @param redir Pointer to redirection flag to update
 * @return int SUCCESS if processing succeeded, ERROR otherwise
 */

/**
 * @brief Main expansion function for command arguments
 *
 * Processes all split elements for a command, performing expansions
 * as needed. Converts the resulting expansion list to arrays in the
 * cmd structure and joins arguments when appropriate.
 *
 * @param cmd Pointer to command info structure
 * @param envl Linked list of environment variables
 * @param split Array of split structures containing the parsed input
 * @return int SUCCESS on successful expansion, ERROR otherwise
 */

int			expand_then_add(t_list **expansion, t_split curr, t_list *envl,
				int r)
{
	if (curr.sep == '\"')
	{
		expand_simple(expansion, curr, envl);
		return (SUCCESS);
	}
	return (expand_hard(expansion, curr, envl, r));
}

static int	list_to_tab(t_list *expansion, t_info *cmd)
{
	t_split	*curr;
	int		i;

	cmd->nb_args_tmp = list_size(expansion);
	cmd->args_tmp = malloc((cmd->nb_args_tmp + 1) * sizeof(char *));
	cmd->seps_tmp = malloc((cmd->nb_args_tmp + 1) * sizeof(char));
	cmd->spaces_tmp = malloc((cmd->nb_args_tmp + 1) * sizeof(int));
	if (!cmd->args_tmp || !cmd->seps_tmp || !cmd->spaces_tmp)
		return (error_msg(ALLOCATION_FAIL));
	i = -1;
	while (++i < cmd->nb_args_tmp)
	{
		curr = expansion->content;
		cmd->args_tmp[i] = ft_strdup(curr->str);
		cmd->seps_tmp[i] = curr->sep;
		cmd->spaces_tmp[i] = curr->space;
		expansion = expansion->next;
	}
	cmd->args_tmp[i] = NULL;
	cmd->seps_tmp[i] = '\0';
	cmd->spaces_tmp[i] = -1;
	return (SUCCESS);
}

int			expand_one(t_list **expansion, t_list *envl, t_split curr,
				int *redir)
{
	int	err;

	err = SUCCESS;
	if (curr.sep == '\"' || curr.sep == ' ')
		err = expand_then_add(expansion, curr, envl, *redir);
	else
		ft_lstadd_back(expansion, new_entry(curr.str, curr.sep, curr.space));
	if (err)
		return (ERROR);
	*redir = 0;
	if (is_redir(curr.sep))
		*redir = 1;
	return (SUCCESS);
}

int			expand(t_info *cmd, t_list *envl, t_split *split)
{
	t_list	*expansion;
	int		i;
	int		redir;
	int		err;

	i = -1;
	expansion = NULL;
	err = SUCCESS;
	redir = 0;
	while (++i < cmd->number && !err)
		err = expand_one(&expansion, envl, split[cmd->start + i], &redir);
	if (err || list_to_tab(expansion, cmd))
	{
		ft_lstclear(&expansion, &free_expd);
		return (ERROR);
	}
	ft_lstclear(&expansion, &free_expd);
	return (join_args(cmd));
}
