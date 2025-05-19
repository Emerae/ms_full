
#include "minishell.h"
/**
 * @file env.c
 * @brief Functions for environment variable management
 *
 * This file contains functions for working with environment variables,
 * specifically for converting environment lists to string arrays.
 */

/**
 * @brief Counts the number of environment variables with required export status
 *
 * @param list The linked list of environment variables (starts with sentinel node)
 * @param exported The minimum export status to count (0 = not exported, 1 = exported)
 * @return int The number of environment variables meeting the export criteria
 */

/**
 * @brief Creates a NULL-terminated array of environment strings in "VAR=VALUE" format
 *
 * This function creates an array of strings from the environment list, including
 * only variables that meet the specified export status. Each variable is formatted
 * as "VAR=VALUE". The array is NULL-terminated.
 *
 * @param envl The linked list of environment variables (starts with sentinel node)
 * @param exported The minimum export status to include (0 = all vars, 1 = only exported)
 * @return char** A newly allocated array of environment strings, or NULL on error
 *                The caller is responsible for freeing this memory
 */
/*
static int	size_of_list(t_list *list, int exported)
{
	int	i;

	i = 0;
	list = list->next;
	while (list)
	{
		if (((t_env *)list->content)->exported >= exported)
			i++;
		list = list->next;
	}
	return (i);
}
*/

char **create_env_tab(t_list *envl, int exported)
{
    int size = 0;
    t_list *tmp = envl;
    
    // Compter les variables à inclure
    while (tmp)
    {
        t_env *env = (t_env *)tmp->content;
        if (env->exported >= exported)
            size++;
        tmp = tmp->next;
    }
    
    // Allouer le tableau
    char **env_tab = malloc(sizeof(char *) * (size + 1));
    if (!env_tab)
        return NULL;
    
    // Remplir le tableau
    int i = 0;
    tmp = envl;
    while (tmp)
    {
        t_env *env = (t_env *)tmp->content;
        if (env->exported >= exported)
        {
            char *name = env->var;
            char *value = env->value ? env->value : "";
            
            // Calculer la taille nécessaire
            int len = ft_strlen(name) + ft_strlen(value) + 2; // +2 pour '=' et '\0'
            
            // Allouer la chaîne
            env_tab[i] = malloc(len);
            if (!env_tab[i])
            {
                // Nettoyer en cas d'erreur
                while (--i >= 0)
                    free(env_tab[i]);
                free(env_tab);
                return NULL;
            }
            
            // Construire la chaîne "NAME=VALUE"
            ft_strlcpy(env_tab[i], name, len);
            ft_strlcat(env_tab[i], "=", len);
            ft_strlcat(env_tab[i], value, len);
            
            i++;
        }
        tmp = tmp->next;
    }
    
    env_tab[i] = NULL;
    return env_tab;
}
