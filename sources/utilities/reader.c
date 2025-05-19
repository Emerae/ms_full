
#include "minishell.h"

/**
 * @brief Joins a character to a string, reallocating memory as needed.
 *
 * @param line The original string.
 * @param c The character to be added.
 * @param reset Flag to indicate if the line should be reset.
 * @return The new string with the character added, or NULL if allocation fails.
 */
static char *join_realloc(char *line, char c, int reset);

/**
 * @brief Allocates memory for an empty string.
 *
 * @return A pointer to the newly allocated empty string, or NULL if allocation fails.
 */
static char *malloc_empty_string(void);

/**
 * @brief Checks if a newline character should be ignored due to being escaped.
 *
 * @param c The character to check.
 * @param line The current line being read.
 * @param reset Pointer to the reset flag.
 * @return 1 if the newline should be ignored, 0 otherwise.
 */
static int ignore_escaped(char c, char **line, int *reset);

/**
 * @brief Handles initial read and error checking for the reader function.
 *
 * @param line Pointer to the line being read.
 * @param c Pointer to the character being read.
 * @return The size of the read, or -1 if an error occurs.
 */
static int error_and_first_read(char **line, char *c);

/**
 * @brief Reads input from the standard input and stores it in the provided line.
 *
 * @param line Pointer to the line being read.
 * @return 0 on success, 1 if a newline is encountered, or -1 if an error occurs.
 */
int reader(char **line);
static char	*join_realloc(char *line, char c, int reset)
{
	char	*new_line;
	int		l;
	int		i;

	if (c == '\n')
		return (line);
	if (reset || g_signal == 2)
	{
		g_signal = 4;
		l = 0;
	}
	else
		l = ft_strlen(line);
	new_line = malloc((l + 2) * sizeof(char));
	if (!new_line)
		return (NULL);
	i = -1;
	while (++i < l)
		new_line[i] = line[i];
	free(line);
	new_line[l] = c;
	new_line[l + 1] = '\0';
	return (new_line);
}

static char	*malloc_empty_string(void)
{
	char	*empty;

	empty = malloc(sizeof(char));
	if (!empty)
		return (empty);
	empty[0] = '\0';
	return (empty);
}

static int	ignore_escaped(char c, char **line, int *reset)
{
	if (c == '\n')
	{
		if (ft_strlen(*line) > 0 && (*line)[ft_strlen(*line) - 1] == '\\')
			(*line)[ft_strlen(*line) - 1] = '\0';
		else
		{
			*reset = 1;
			return (1);
		}
	}
	return (0);
}

static int	error_and_first_read(char **line, char *c)
{
	int	size;

	if (!line)
		return (-1);
	*line = malloc_empty_string();
	if (!(*line))
		return (-1);
	size = read(0, c, 1);
	return (size);
}

int			reader(char **line)
{
	static char	c;
	static int	size;
	static int	reset;

	size = error_and_first_read(line, &c);
	if (size < 0)
		return (-1);
	while (size >= 0)
	{
		if (should_quit(size, *line))
			break ;
		else if (size == 0)
			size = read(0, &c, 1);
		else
		{
			if (ignore_escaped(c, line, &reset))
				return (1);
			reset = 0;
			*line = join_realloc(*line, c, reset);
			if (!(*line))
				return (-1);
			size = read(0, &c, 1);
		}
	}
	return (0);
}
