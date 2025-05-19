
#include "minishell.h"
/**
 * @brief Converts separators to a standard format for parsing.
 * 
 * This function takes a character and determines if it's a special separator in the shell.
 * For '>', it checks if the next character is also '>' to detect the '>>' operator
 * and returns 'd' to represent it. For other separators, it returns the character itself.
 * For non-separators, it returns a space character.
 *
 * @param c The character to check
 * @param command The full command string being processed
 * @param k The current position in the command string
 * @return The converted separator character
 */

/**
 * @brief Determines if a character is an operator and its type.
 * 
 * This function categorizes operators by returning:
 * - 1 for '<' or '|'
 * - 2 for '=' or ';'
 * - 3 for '>'
 * - 0 if not an operator
 *
 * @param c The character to check
 * @return The operator type code or 0 if not an operator
 */

/**
 * @brief Checks if a character is a space or a command delimiter.
 * 
 * This function returns true (1) if the character is a whitespace, an operator,
 * or a quote character, indicating the end of a token.
 *
 * @param c The character to check
 * @return 1 if the character is a delimiter, 0 otherwise
 */

/**
 * @brief Validates if the current parsing state allows a command to proceed.
 * 
 * Checks if the current state is not an operator, or if the operator is
 * non-problematic (= or ;). Returns the current command count if valid,
 * otherwise returns -1 to indicate an error.
 *
 * @param p Pointer to the parsing state structure
 * @return The command count if state is valid, -1 if not
 */

/**
 * @brief Sets space flags between tokens in the split structure.
 * 
 * Marks both the current token and the previous token (if it exists)
 * as having a space between them.
 *
 * @param split Array of split tokens
 * @param i Current index in the split array
 */

char	sep_converter(char c, char *command, int k)
{
	if (c == '<' || c == '|' || c == '=' || c == ';' || c == '\'' || c == '\"')
		return (c);
	if (c == '>')
	{
		if (command[k + 1] == '>')
			return ('d');
		return (c);
	}
	return (' ');
}

int		is_operator(char c)
{
	if (c == '<' || c == '|')
		return (1);
	if (c == '=' || c == ';')
		return (2);
	if (c == '>')
		return (3);
	return (0);
}

int		is_spaceend(char c)
{
	if (ft_isspace(c) || is_operator(c) || c == '\'' || c == '\"')
		return (1);
	return (0);
}

int		is_state_ok(t_parse *p)
{
	if (p->state != OPERATOR || p->sep == '=' || p->sep == ';')
		return (p->nb);
	return (-1);
}

void	add_space(t_split *split, int i)
{
	if (i > 0)
		split[i - 1].space = 1;
	split[i].space = 1;
}
