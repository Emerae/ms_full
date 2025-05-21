#include "minishell.h"

char	*get_env_value(t_list *envl, const char *var)
{
	t_list	*cur;
	t_env	*e;

	ft_putstr_fd("DEBUG: get_env_value appelé pour '", 2);
	ft_putstr_fd((char *)var, 2);
	ft_putstr_fd("'\n", 2);
	if (!envl || !var)
	{
		ft_putstr_fd("DEBUG: paramètres invalides\n", 2);
		return (NULL);
	}
	cur = envl;
	while (cur)
	{
		e = (t_env *)cur->content;
		if (!e || !e->var)
		{
			ft_putstr_fd("DEBUG: entrée d'environnement invalide\n", 2);
			cur = cur->next;
			continue ;
		}
		ft_putstr_fd("DEBUG: comparaison avec '", 2);
		ft_putstr_fd(e->var, 2);
		ft_putstr_fd("'\n", 2);
		if (ft_strcmp(e->var, var) == 0)
			return (e->value);
		cur = cur->next;
	}
	ft_putstr_fd("DEBUG: variable '", 2);
	ft_putstr_fd((char *)var, 2);
	ft_putstr_fd("' non trouvée\n", 2);
	return (NULL);
}

/* Suppression de la fonction add_env car elle est déjà définie dans declare.c */ 
