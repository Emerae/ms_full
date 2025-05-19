#include "minishell.h"

char *get_env_value(t_list *envl, const char *var)
{
    t_list *cur = envl;
    while (cur)
    {
        t_env *e = (t_env *)cur->content;
        if (ft_strcmp(e->var, var) == 0)
            return (e->value);
        cur = cur->next;
    }
    return (NULL);
}

/* Suppression de la fonction add_env car elle est déjà définie dans declare.c */ 
