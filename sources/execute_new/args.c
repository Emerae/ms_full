
#include "minishell.h"
/**
 * @brief Counts the number of elements in a NULL-terminated string array
 *
 * @param args NULL-terminated array of strings
 * @return int Number of elements in the array
 */

/**
 * @brief Creates a filtered array of arguments for command execution
 * 
 * This function allocates memory for the argv array and populates it
 * with arguments from cmd->args, excluding redirection symbols.
 *
 * @param cmd Structure containing command information
 * @return int SUCCESS on success, error code on failure
 */

/**
 * @brief Counts the number of arguments in a command based on spaces
 *
 * @param cmd Structure containing command information
 * @param split Array of split tokens
 * @return int Number of arguments
 */

/**
 * @brief Fills the args array by joining words from the split structure
 *
 * This function joins consecutive tokens that aren't separated by spaces
 * into single arguments, and records their separator types.
 *
 * @param cmd Structure containing command information
 * @param split Array of split tokens
 */

/**
 * @brief Joins words from the split structure into the command's args array
 *
 * Allocates memory for args and seps arrays, then populates them by
 * joining words according to spaces in the split structure.
 *
 * @param cmd Pointer to structure containing command information
 * @param split Array of split tokens
 * @return int 0 on success, -1 on allocation failure
 */

int			number_of_args(char **args)
{
	int	i;

	i = 0;
	while (args[i])
		i++;
	return (i);
}

int			create_tab_args(t_info *cmd)
{
	char	c;
	int		i;
	int		j;

	cmd->argv = malloc((cmd->nb_args + 1) * sizeof(char *));
	if (!(cmd->argv))
		return (error_msg(ALLOCATION_FAIL));
	i = 0;
	j = cmd->offset;
	while (j < cmd->nb_args)
	{
		c = cmd->seps[j];
		if (c == 'd' || c == '>' || c == '<')
			j++;
		else
		{
			cmd->argv[i] = cmd->args[j];
			i++;
		}
		j++;
	}
	cmd->argv[i] = NULL;
	return (SUCCESS);
}

static int	count_arg(t_info cmd, t_split *split)
{
	int	nb_arg;
	int	i;

	nb_arg = 0;
	i = -1;
	while (++i < cmd.number)
	{
		if (split[cmd.start + i].space)
			nb_arg++;
	}
	if (!split[cmd.start + i].str)
		nb_arg++;
	return (nb_arg);
}

static void	fill_args(t_info cmd, t_split *split)
{
	int		i;
	int		j;
	int		end;
	char	*current;
	char	*save;

	i = -1;
	j = 0;
	while (++i < cmd.nb_args)
	{
		end = 0;
		current = ft_strdup("");
		while (j < cmd.number && !end)
		{
			save = current;
			current = ft_strjoin(cmd.args[i], split[cmd.start + j].str);
			free(save);
			end = split[cmd.start + j].space;
			j++;
		}
		cmd.args[i] = ft_strdup(current);
		cmd.seps[i] = split[cmd.start + j - 1].sep;
		free(current);
	}
	cmd.args[i] = NULL;
}

int			join_words(t_info *cmd, t_split *split)
{
	cmd->nb_args = count_arg(*cmd, split);
	cmd->args = malloc((cmd->nb_args + 1) * sizeof(char *));
	cmd->seps = malloc((cmd->nb_args + 1) * sizeof(char));
	if (!(cmd->args) || !(cmd->seps))
		return (-1);
	fill_args(*cmd, split);
	return (0);
}
