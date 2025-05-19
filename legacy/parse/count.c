
#include "minishell.h"
/**
 * @brief Determine the next parsing state based on the current character
 * 
 * @param command The input command string to analyze
 * @param i The current index in the command string
 * @return int The new state: DB_QUOTE for double quotes, QUOTE for single quotes,
 *             OPERATOR for operators, or SPACE for other characters
 */

/**
 * @brief Count the number of words in a command string
 * 
 * This function initializes a parsing structure and uses state_reset to count
 * the number of words in the provided command string.
 * 
 * @param command The command string to analyze
 * @return int The number of words found in the command
 */

/**
 * @brief Determine the length of a word starting at a specific index
 * 
 * This function calculates the length of a word in the command string
 * starting from index i. It also identifies the separator character
 * following the word.
 * 
 * @param command The command string to analyze
 * @param i The starting index of the word
 * @param sep Pointer to store the separator character following the word
 * @return int The length of the word
 */

int	new_state(char *command, int i)
{
	if (command[i] == '\"')
		return (DB_QUOTE);
	if (command[i] == '\'')
		return (QUOTE);
	if (is_operator(command[i]))
		return (OPERATOR);
	return (SPACE);
}

int	nb_words(char *command)
{
	t_parse	p;

	p.len_word = -1;
	p.sep = ' ';
	p.state = 0;
	p.nb = 0;
	return (state_reset(command, 0, &p));
}

int	len_of_word(char *command, int i, char *sep)
{
	t_parse	p;

	p.len_word = 0;
	p.sep = ' ';
	p.state = 0;
	p.nb = 0;
	(void)state_reset(command, i, &p);
	*sep = p.sep;
	return (p.len_word);
}
