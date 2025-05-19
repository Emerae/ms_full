
#include "minishell.h"
/**
 * @brief Redirects standard input and output to the file descriptors specified in cmd.
 *
 * If cmd->output differs from STDOUT, closes STDOUT and duplicates cmd->output.
 * If cmd->input differs from STDIN, closes STDIN and duplicates cmd->input.
 *
 * @param cmd The command info structure containing input/output file descriptors.
 */
void	change_stdin_stdout(t_info *cmd);

/**
 * @brief Closes file descriptors that are no longer needed.
 *
 * Closes cmd->input if it's not STDIN and not -1.
 * Closes cmd->output if it's not STDOUT and not -1.
 *
 * @param cmd The command info structure containing input/output file descriptors.
 */
void	close_unused_fd(t_info *cmd);

/**
 * @brief Checks if a string contains only characters valid for a shell variable name.
 *
 * Valid variable names must:
 * - Start with a letter or underscore.
 * - Contain only alphanumeric characters or underscores before the '=' character.
 *
 * @param s The string to check.
 * @return 1 if the string contains only authorized characters, 0 otherwise.
 */
int		authorized_char(char *s);

/**
 * @brief Determines if a string represents a file path.
 *
 * Checks if the string contains at least one '/' character.
 *
 * @param word The string to check.
 * @return 1 if the string contains a '/', 0 otherwise.
 */
int		is_path(char *word);

void	change_stdin_stdout(t_info *cmd)
{
	if (cmd->output != STDOUT)
	{
		close(STDOUT);
		dup(cmd->output);
	}
	if (cmd->input != STDIN)
	{
		close(STDIN);
		dup(cmd->input);
	}
}

void	close_unused_fd(t_info *cmd)
{
	if (cmd->input != STDIN && cmd->input != -1)
		close(cmd->input);
	if (cmd->output != STDOUT && cmd->output != -1)
		close(cmd->output);
}

int		authorized_char(char *s)
{
	int	i;

	i = 0;
	if (!ft_isalpha(s[0]) && (s[0] != '_'))
		return (0);
	while (s[i] && (s[i] != '='))
	{
		if (!ft_isalnum(s[i]) && (s[i] != '_'))
			return (0);
		i++;
	}
	return (1);
}

int		is_path(char *word)
{
	int	i;

	i = 0;
	while (word[i])
	{
		if (word[i] == '/')
			return (1);
		i++;
	}
	return (0);
}

