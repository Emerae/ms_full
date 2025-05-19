#include "parser_new.h"

int	cy3_handle_dollar_word_3a(t_input *current, int i, t_dollar_word *s)
{
	s->p = 0;
	s->k = 0;
	while (s->k < i)
	{
		s->new_input[s->p] = current->input[s->k];
		s->new_type[s->p] = current->input_type[s->k];
		s->new_num[s->p] = current->input_num[s->k];
		s->p = s->p + 1;
		s->k = s->k + 1;
	}
	return (0);
}

int	cy3_handle_dollar_word_3b(t_input *current, int j, t_dollar_word *s)
{
	s->k = j + 1;
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

int	cy3_handle_dollar_word_3(t_input *current, int i, int j)
{
	t_dollar_word	s;

	s.lold = cy_strlen(current->input);
	s.new_input = malloc(s.lold - (j - i + 1) + 1);
	s.new_type = malloc(s.lold - (j - i + 1) + 1);
	s.new_num = malloc(s.lold - (j - i + 1) + 1);
	if (!s.new_input || !s.new_type || !s.new_num)
		return (-1);
	cy3_handle_dollar_word_3a(current, i, &s);
	cy3_handle_dollar_word_3b(current, j, &s);
	s.new_input[s.p] = '\0';
	s.new_type[s.p] = '\0';
	s.new_num[s.p] = '\0';
	free(current->input);
	free(current->input_type);
	free(current->input_num);
	current->input = s.new_input;
	current->input_type = s.new_type;
	current->input_num = s.new_num;
	return (i);
}

int cy3_handle_dollar_word_1(t_input *current, char **env, t_dollar_word *s)
{
    printf("DEBUG-F2: Entrée dans cy3_handle_dollar_word_1, current=%p, env=%p, s=%p\n", current, env, s);
    printf("DEBUG-F2: s->e=%d\n", s->e);
    
    if (!env)
    {
        printf("DEBUG-F2: env est NULL!\n");
        return cy3_handle_dollar_word_3(current, s->i, s->j);
    }
    
    if (env[s->e])
    {
        printf("DEBUG-F2: Variable trouvée: env[%d]='%s'\n", s->e, env[s->e]);
        s->value = cy_strchr(env[s->e], '=');
        if (!s->value)
        {
            printf("DEBUG-F2: Format invalide (pas de '=')\n");
            return cy3_handle_dollar_word_3(current, s->i, s->j);
        }
        
        s->value = s->value + 1; // Passer le caractère '='
        printf("DEBUG-F2: Valeur trouvée: '%s'\n", s->value);
        
        // Allouer la mémoire ICI, avant d'y accéder
        s->vlen = cy_strlen(s->value);
        s->lold = cy_strlen(current->input);
        
        s->new_input = malloc(s->lold - (s->j - s->i + 1) + s->vlen + 1);
        s->new_type = malloc(s->lold - (s->j - s->i + 1) + s->vlen + 1);
        s->new_num = malloc(s->lold - (s->j - s->i + 1) + s->vlen + 1);
        
        if (!s->new_input || !s->new_type || !s->new_num)
        {
            if (s->new_input) free(s->new_input);
            if (s->new_type) free(s->new_type);
            if (s->new_num) free(s->new_num);
            return (-1);
        }
        
        // Maintenant on peut utiliser les pointeurs en toute sécurité
        s->p = 0;
        s->k = 0;
        while (s->k < s->i)
        {
            s->new_input[s->p] = current->input[s->k];
            s->new_type[s->p] = current->input_type[s->k];
            s->new_num[s->p] = current->input_num[s->k];
            s->p = s->p + 1;
            s->k = s->k + 1;
        }
        
        printf("DEBUG-F2: Appel de cy3_handle_dollar_word_2\n");
        return (cy3_handle_dollar_word_2(current, s));
    }
    
    printf("DEBUG-F2: Variable non trouvée, appel de cy3_handle_dollar_word_3\n");
    return (cy3_handle_dollar_word_3(current, s->i, s->j));
}

int cy3_handle_dollar_word(t_input *current, int i, int j, char **env)
{
    // Vérifications de sécurité d'abord
    if (!current || !current->input || !env)
    {
        printf("DEBUG: Pointeurs NULL dans cy3_handle_dollar_word\n");
        return (-1);
    }
    
    // Initialisation complète de la structure
    t_dollar_word s;
    memset(&s, 0, sizeof(t_dollar_word));
    s.i = i;
    s.j = j;
    
    // Vérification des limites des indices
    int input_len = cy_strlen(current->input);
    if (i < 0 || j < i || i >= input_len || j >= input_len)
    {
        printf("DEBUG: Indices hors limites: i=%d, j=%d, len=%d\n", i, j, input_len);
        return (-1);
    }
    
    // Extraction du nom de la variable
    cy3_handle_dollar_word_key(current, &s);
    
    // Ajouter un log pour voir la clé extraite
    printf("DEBUG: Extraction de la variable d'env '%s'\n", s.key);
    
    // Recherche dans l'environnement
    cy3_handle_dollar_word_findenv(&s, env);
    
    // Remplacement
    return (cy3_handle_dollar_word_1(current, env, &s));
}
