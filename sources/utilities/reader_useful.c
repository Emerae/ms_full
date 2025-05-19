
#include "minishell.h"

/**
 * should_quit - Determines if the program should quit based on input size and signal state.
 * @size: The size of the input line.
 * @line: The input line string.
 *
 * This function checks if the program should quit based on the following conditions:
 * - If the size of the input line is 0.
 * - If the global signal variable `g_signal` is not equal to 3.
 * - If the input line is NULL or its length is 0.
 * - If the global signal variable `g_signal` is equal to 2.
 *
 * Returns 1 if the program should quit, otherwise returns 0.
 */
int	should_quit(int size, char *line)
{
	if (size == 0 && g_signal != 3
		&& (!line || ft_strlen(line) == 0 || g_signal == 2))
		return (1);
	return (0);
}
