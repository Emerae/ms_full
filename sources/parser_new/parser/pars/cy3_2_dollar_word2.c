#include "parser_new.h"

void	cy3_handle_dollar_word_key(t_input *current, t_dollar_word *s)
{
	s->keylen = 0;
	s->k = s->i + 1;
	while (s->k <= s->j && s->keylen < 255)
	{
		s->key[s->keylen] = current->input[s->k];
		s->k = s->k + 1;
		s->keylen = s->keylen + 1;
	}
	s->key[s->keylen] = '\0';
}

void cy3_handle_dollar_word_findenv(t_dollar_word *s, char **env)
{
    char *equal;

    printf("DEBUG-F1: Entrée dans cy3_handle_dollar_word_findenv, s=%p, env=%p\n", s, env);
    printf("DEBUG-F1: Recherche de clé '%s', longueur=%d\n", s->key, s->keylen);
    
    s->e = 0;
    if (!env)
    {
        printf("DEBUG-F1: env est NULL!\n");
        return;
    }
    
    printf("DEBUG-F1: Début de boucle avec env[%d]=%p\n", s->e, env[s->e]);
    while (env[s->e])
    {
        printf("DEBUG-F1: Examen de env[%d]='%s'\n", s->e, env[s->e]);
        equal = cy_strchr(env[s->e], '=');
        if (!equal)
        {
            printf("DEBUG-F1: Pas de symbole '=' trouvé\n");
            s->e = s->e + 1;
            continue;
        }
        
        int var_len = (int)(equal - env[s->e]);
        printf("DEBUG-F1: Variable trouvée, longueur=%d, à comparer avec keylen=%d\n", var_len, s->keylen);
        printf("DEBUG-F1: Comparaison de '%.*s' avec '%s'\n", var_len, env[s->e], s->key);
        
        if (var_len == s->keylen && cy_strncmp(env[s->e], s->key, s->keylen) == 0)
        {
            printf("DEBUG-F1: Match trouvé à l'index %d\n", s->e);
            break;
        }
        s->e = s->e + 1;
    }
    
    printf("DEBUG-F1: Sortie de boucle, env[%d]=%p\n", s->e, env[s->e]);
}

int	cy3_handle_dollar_word_2a(t_input *current, t_dollar_word *s)
{
	s->k = 0;
	while (s->k < s->vlen)
	{
		s->new_input[s->p] = s->value[s->k];
		s->new_type[s->p] = '5';
		s->new_num[s->p] = current->input_num[s->i];
		s->p = s->p + 1;
		s->k = s->k + 1;
	}
	return (0);
}

int	cy3_handle_dollar_word_2b(t_input *current, t_dollar_word *s)
{
	s->k = s->j + 1;
	while (current->input[s->k])
	{
		s->new_input[s->p] = current->input[s->k];
		s->new_type[s->p] = current->input_type[s->k];
		s->new_num[s->p] = current->input_num[s->k];
		s->p = s->p + 1;
		s->k = s->k + 1;
	}
	return (0);
}

int cy3_handle_dollar_word_2(t_input *current, t_dollar_word *s)
{
    printf("DEBUG-F3: Entrée dans cy3_handle_dollar_word_2, current=%p, s=%p\n", current, s);
    
    // La mémoire est déjà allouée à ce stade
    // Il suffit d'utiliser cy3_handle_dollar_word_2a et cy3_handle_dollar_word_2b    
    cy3_handle_dollar_word_2a(current, s);
    cy3_handle_dollar_word_2b(current, s);
    s->new_input[s->p] = '\0';
    s->new_type[s->p] = '\0';
    s->new_num[s->p] = '\0';
    free(current->input);
    free(current->input_type);
    free(current->input_num);
    current->input = s->new_input;
    current->input_type = s->new_type;
    current->input_num = s->new_num;
    printf("DEBUG-F3: Sortie de cy3_handle_dollar_word_2\n");
    return (s->i + s->vlen - 1);
}
