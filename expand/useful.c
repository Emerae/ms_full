
#include "minishell.h"
/**
 * @brief Checks if a character is a redirection character
 * 
 * Determines if the given character is one of the redirection operators 
 * used in shell commands ('>', 'd', '<').
 * 
 * @param c The character to check
 * @return 1 if the character is a redirection character, 0 otherwise
 */

/**
 * @brief Creates a new list entry with split information
 * 
 * Allocates memory for a new t_split structure, initializes it with the provided
 * string (duplicated), separator character, and space flag, then creates a new
 * list node containing this structure.
 * 
 * @param str The string to store in the new entry
 * @param sep The separator character associated with this entry
 * @param space Flag indicating space presence/behavior
 * @return A new t_list node containing the initialized t_split structure
 */

/**
 * @brief Frees memory allocated for a t_split structure
 * 
 * Properly deallocates all memory associated with a t_split structure,
 * including the string it contains and the structure itself.
 * 
 * @param expd Pointer to the t_split structure to free (cast as void*)
 */

int		is_redir(char c)
{
	if (c == '>' || c == 'd' || c == '<')
		return (1);
	return (0);
}

t_list	*new_entry(char *str, char sep, int space)
{
	t_split	*content;

	content = malloc(sizeof(t_split));
	content->str = ft_strdup(str);
	content->sep = sep;
	content->space = space;
	return (ft_lstnew(content));
}

void	free_expd(void *expd)
{
	t_split	*to_free;

	to_free = (t_split *)expd;
	if (to_free)
	{
		free(to_free->str);
		free(to_free);
	}
}
