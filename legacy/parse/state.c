
#include "minishell.h"
/**
 * @brief Processes operator characters in the command string
 *
 * This function handles operator tokens in the command. It specifically detects
 * redirection operators (e.g., '>>' is treated as a single token) and advances
 * the parsing position accordingly.
 *
 * @param command The command string being parsed
 * @param i Current position in the command string
 * @param p Pointer to the parsing state structure
 * @return Returns the result of state_reset after processing the operator
 */

/**
 * @brief Processes double-quoted strings in the command
 *
 * This function handles text enclosed in double quotes ("). It accounts for
 * escaped characters within the quotes and finds the matching closing quote.
 * If the closing quote is found, updates the word length and resets the state.
 *
 * @param command The command string being parsed
 * @param i Current position in the command string (after the opening quote)
 * @param p Pointer to the parsing state structure
 * @return -1 if no closing quote is found, otherwise returns the result of state_reset
 */

/**
 * @brief Processes single-quoted strings in the command
 *
 * This function handles text enclosed in single quotes ('). It finds the matching
 * closing quote, updates the word length accordingly, and resets the state.
 * Unlike double quotes, no character escaping is processed within single quotes.
 *
 * @param command The command string being parsed
 * @param i Current position in the command string (after the opening quote)
 * @param p Pointer to the parsing state structure
 * @return -1 if no closing quote is found, otherwise returns the result of state_reset
 */

/**
 * @brief Processes a sequence of non-space characters
 *
 * This function handles sequences of characters until it reaches a space or
 * end of the string. It calculates the word length and calls state_reset.
 *
 * @param command The command string being parsed
 * @param i Current position in the command string
 * @param p Pointer to the parsing state structure
 * @return Returns the result of state_reset after processing the sequence
 */

/**
 * @brief Resets the parsing state and determines the next state
 *
 * This function is the core of the state machine parser. It checks if a token has
 * been identified (len_word > 0), or if we've reached the end of input. Otherwise,
 * it skips whitespace, identifies the next token type, and delegates to the 
 * appropriate state handler function.
 *
 * @param command The command string being parsed
 * @param i Current position in the command string
 * @param p Pointer to the parsing state structure
 * @return Result of is_state_ok if a token is complete or input ends,
 *         otherwise returns result of the delegated state handler
 */

int	state_operator(char *command, int i, t_parse *p)
{
	if (is_operator(command[i]) == 3 && command[i + 1] == '>')
	{
		if (p->len_word == 0)
			p->len_word = 2;
		return (state_reset(command, i + 2, p));
	}
	if (p->len_word == 0)
		p->len_word = 1;
	return (state_reset(command, i + 1, p));
}

int	state_db_quote(char *command, int i, t_parse *p)
{
	int	j;

	j = i;
	while (command[i] && command[i] != '\"')
	{
		if (command[i] == '\\')
		{
			i++;
			if (command[i] == '\0')
				return (-1);
		}
		i++;
	}
	if (command[i] == '\"')
	{
		if (p->len_word == 0)
			p->len_word = i - j + 2;
		return (state_reset(command, i + 1, p));
	}
	return (-1);
}

int	state_quote(char *command, int i, t_parse *p)
{
	int	j;

	j = i;
	while (command[i] && command[i] != '\'')
		i++;
	if (command[i] == '\'')
	{
		if (p->len_word == 0)
			p->len_word = i - j + 2;
		return (state_reset(command, i + 1, p));
	}
	return (-1);
}

int	state_space(char *command, int i, t_parse *p)
{
	int	j;

	j = i;
	while (command[i] && !is_spaceend(command[i]))
		i++;
	if (p->len_word == 0)
		p->len_word = i - j + 1;
	return (state_reset(command, i, p));
}

int	state_reset(char *command, int i, t_parse *p)
{
	int			l;

	l = ft_strlen(command);
	if (p->len_word > 0)
		return (is_state_ok(p));
	if (i >= l)
		return (is_state_ok(p));
	while (ft_isspace(command[i]))
		i++;
	if (command[i] == '\0')
		return (is_state_ok(p));
	p->state = new_state(command, i);
	p->sep = command[i];
	(p->nb)++;
	if (p->state == SPACE)
		return (state_space(command, i + 1, p));
	else if (p->state == QUOTE)
		return (state_quote(command, i + 1, p));
	else if (p->state == DB_QUOTE)
		return (state_db_quote(command, i + 1, p));
	else
		return (state_operator(command, i, p));
}
