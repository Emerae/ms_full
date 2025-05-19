
#include "minishell.h"
/**
 * @file parse.c
 * @brief Functions for parsing command strings into tokenized structures
 */

/**
 * @brief Frees memory allocated for a split structure array up to a specific index
 *
 * @param split Array of t_split structures to free
 * @param i Index up to which the array elements should be freed
 * @return -1 Always returns -1 as an error indicator
 */
static int	free_split(t_split *split, int i);

/**
 * @brief Fills a t_split structure with a word extracted from command
 *
 * Copies the word into the structure, ensures proper null-termination,
 * trims separators, and initializes space flag to 0.
 *
 * @param word Pointer to t_split structure to fill
 * @param word_len Length of the word to be copied
 * @param cmd Pointer to the command string from which to extract the word
 */
static void	fill_one_word(t_split *word, int word_len, const char *cmd);

/**
 * @brief Parses command string and fills an array of t_split structures
 *
 * Identifies and extracts individual words from the command string,
 * allocates memory for each word, and handles separator and space information.
 *
 * @param split Array of t_split structures to fill
 * @param words Number of words to extract
 * @param command The input command string to parse
 * @return 0 on success, -1 on memory allocation failure
 */
static int	fill_words(t_split *split, int words, char *command);

/**
 * @brief Adds virtual spaces around operators and checks for syntax errors
 *
 * Identifies operators such as >, <, |, ; and adds virtual spaces around them.
 * Also performs syntax validation for these operators and sets the error token
 * if a syntax error is detected.
 *
 * @param split Array of t_split structures
 * @param words Number of words in the split array
 * @param token Pointer to store the error token if syntax error is found
 * @return 0 if successful, 1 if a syntax error is detected
 */
static char	add_virtual_spaces(t_split *split, int words, char *token);

/**
 * @brief Parses a command string into a tokenized structure
 *
 * This is the main parsing function that handles:
 * - Trimming leading/trailing spaces
 * - Counting words and validating quote syntax
 * - Allocating memory for the token array
 * - Filling the token array with words and metadata
 * - Adding virtual spaces around operators
 * - Setting appropriate error codes if problems arise
 *
 * @param command The input command string to parse
 * @param error Pointer to t_error structure to store error information
 * @return Pointer to array of t_split structures, or NULL if an error occurred
 */
t_split		*parse_command(char *command, t_error *error);

static int	free_split(t_split *split, int i)
{
	int	j;

	j = -1;
	while (++j < i)
		free(split[j].str);
	free(split);
	return (-1);
}

static void	fill_one_word(t_split *word, int word_len, const char *cmd)
{
	ft_strlcpy(word->str, cmd, word_len + 1);
	word->str[word_len + 1] = '\0';
	trim_seps(*word);
	word->space = 0;
}

static int	fill_words(t_split *split, int words, char *command)
{
	int		i;
	int		k;
	char	sep;
	int		size;
	int		word_len;

	size = ft_strlen(command);
	i = 0;
	k = 0;
	while (i < words)
	{
		while (ft_isspace(command[k]))
			k++;
		word_len = len_of_word(command, k, &sep);
		split[i].sep = sep_converter(sep, command, k);
		split[i].str = malloc((word_len + 2) * sizeof(char));
		if (!split[i].str)
			return (free_split(split, i));
		fill_one_word(&(split[i]), word_len, command + k);
		if (k + word_len < size && ft_isspace(command[k + word_len]))
			split[i].space = 1;
		k += word_len;
		i++;
	}
	return (0);
}

static char	add_virtual_spaces(t_split *split, int words, char *token)
{
	int		i;
	char	c;
	int		redir;

	i = -1;
	redir = 0;
	while (++i < words)
	{
		c = split[i].sep;
		if ((c == '>' || c == '<' || c == 'd' || c == ';' || c == '|'))
		{
			if (redir || (i == 0 && (c == ';' || c == '|')))
			{
				*token = c;
				return (1);
			}
			redir = 1;
			add_space(split, i);
		}
		else
			redir = 0;
	}
	split[i - 1].space = 1;
	return (0);
}

t_split		*parse_command(char *command, t_error *error)
{
	int		words;
	t_split	*split;

	error->num = SUCCESS;
	trim_spaces(command);
	if (ft_strlen(command) == 0)
		return (NULL);
	words = nb_words(command);
	error->num = SYNTAX_QUOTES;
	if (words < 0)
		return (NULL);
	split = malloc((words + 1) * sizeof(t_split));
	error->num = ALLOCATION_FAIL;
	if (!split || fill_words(split, words, command) < 0)
		return (NULL);
	split[words].str = NULL;
	split[words].sep = '\0';
	error->num = SYNTAX_REDIR;
	if (add_virtual_spaces(split, words, &(error->token))
		&& free_split(split, words))
		return (NULL);
	return (split);
}
