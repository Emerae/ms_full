#include "minishell.h"

char *get_env_value(t_list *envl, const char *var)
{
    printf("DEBUG: get_env_value appelé pour '%s'\n", var);
    
    if (!envl || !var) {
        printf("DEBUG: paramètres invalides\n");
        return NULL;
    }
    
    t_list *cur = envl;
    while (cur)
    {
        t_env *e = (t_env *)cur->content;
        if (!e || !e->var) {
            printf("DEBUG: entrée d'environnement invalide\n");
            cur = cur->next;
            continue;
        }
        
        printf("DEBUG: comparaison avec '%s'\n", e->var);
        if (ft_strcmp(e->var, var) == 0)
            return e->value;
        cur = cur->next;
    }
    printf("DEBUG: variable '%s' non trouvée\n", var);
    return NULL;
}

/* Suppression de la fonction add_env car elle est déjà définie dans declare.c */ 
