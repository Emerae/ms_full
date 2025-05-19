
#include "minishell.h"
/**
 * @file join.c
 * @brief Functions to join and process command arguments during expansion.
 */

/**
 * @brief Counts the number of arguments after expansion
 *
 * Determines the number of arguments by counting elements in the temporary
 * arguments array that have a space associated with them.
 *
 * @param cmd The command info structure
 * @return The number of arguments after joining
 */
static int	count_arg(t_info cmd);

/**
 * @brief Fills the final argument array by joining temporary arguments
 *
 * Populates cmd.args by concatenating strings from cmd.args_tmp until
 * a space delimiter is encountered. Each space in cmd.spaces_tmp marks
 * the end of a joined argument. Also copies separator information from
 * cmd.seps_tmp to cmd.seps.
 *
 * @param cmd The command info structure
 */
static void	fill_args(t_info cmd);

/**
 * @brief Joins the temporary arguments into final arguments
 *
 * Processes the expanded arguments (cmd->args_tmp) to create the final
 * argument array (cmd->args) by allocating memory and calling fill_args.
 * Arguments are joined based on space information in cmd->spaces_tmp.
 *
 * @param cmd Pointer to the command info structure
 * @return SUCCESS on successful operation, error code otherwise
 */
int			join_args(t_info *cmd);

static int	count_arg(t_info cmd)
{
	int	nb_arg;
	int	i;

	nb_arg = 0;
	i = -1;
	while (++i < cmd.nb_args_tmp)
	{
		if (cmd.spaces_tmp[i])
			nb_arg++;
	}
	return (nb_arg);
}

static void	fill_args(t_info cmd)
{
	int		i;
	int		j;
	int		end;
	char	*current;

	i = -1;
	j = 0;
	while (++i < cmd.nb_args)
	{
		end = 0;
		current = ft_strdup("");
		while (j < cmd.nb_args_tmp && !end)
		{
			cmd.args[i] = current;
			current = ft_strjoin(cmd.args[i], cmd.args_tmp[j]);
			free(cmd.args[i]);
			if (cmd.spaces_tmp[j])
				end = 1;
			j++;
		}
		cmd.args[i] = ft_strdup(current);
		cmd.seps[i] = cmd.seps_tmp[j - 1];
		free(current);
	}
	cmd.args[i] = NULL;
}

int			join_args(t_info *cmd)
{
	cmd->nb_args = count_arg(*cmd);
	cmd->args = malloc((cmd->nb_args + 1) * sizeof(char *));
	cmd->seps = malloc((cmd->nb_args + 1) * sizeof(char));
	if (!cmd->args || !cmd->seps)
		return (error_msg(ALLOCATION_FAIL));
	fill_args(*cmd);
	return (SUCCESS);
}
