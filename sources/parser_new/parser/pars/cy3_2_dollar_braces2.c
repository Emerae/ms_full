#include "parser_new.h"

void	cy3_handle_dollar_brace2(t_dollar_braces *s,
								t_input *current, int i, int j)
{
	s->keylen = 0;
	s->k = i + 2;
	while (s->k < j && s->keylen < 255)
	{
		s->key[s->keylen] = current->input[s->k];
		s->keylen = s->keylen + 1;
		s->k = s->k + 1;
	}
	s->key[s->keylen] = '\0';
}

void	cy3_handle_dollar_brace3(t_dollar_braces *s, char **env)
{
	s->e = 0;
	// CORRECTION: vérifie env[s->e] au lieu de env[s->e + 1]
	while (env[s->e])
	{
		s->equal = cy_strchr(env[s->e], '=');
		if (!s->equal)
		{
			s->e = s->e + 1;
			continue ;
		}
		if ((int)(s->equal - env[s->e]) == s->keylen &&
			cy_strncmp(env[s->e], s->key, s->keylen) == 0)
			break ;
		s->e = s->e + 1;
	}
	// CORRECTION: vérifie env[s->e] au lieu de env[s->e + 1]
	if (env[s->e])
		s->value = cy_strchr(env[s->e], '=') + 1;
}

int	cy3_handle_dollar_brace4(t_input *current, int i, int j, t_dollar_braces *s)
{
	s->vlen = cy_strlen(s->value);
	s->lold = cy_strlen(current->input);
	s->new_input = malloc(s->lold - (j - i + 1) + s->vlen + 1);
	s->new_type = malloc(s->lold - (j - i + 1) + s->vlen + 1);
	s->new_num = malloc(s->lold - (j - i + 1) + s->vlen + 1);
	if (!s->new_input || !s->new_type || !s->new_num)
		return (-1);
	s->p = 0;
	cy3_handle_dollar_brace7(current, i, s);
	cy3_handle_dollar_brace8(current, i, s);
	cy3_handle_dollar_brace9(current, j, s);
	cy3_handle_dollar_brace6(current, s);
	return (i + s->vlen - 1);
}

int	cy3_handle_dollar_brace5(t_input *current, int i, int j, t_dollar_braces *s)
{
	s->lold = cy_strlen(current->input);
	s->new_input = malloc(s->lold - (j - i + 1) + 1);
	s->new_type = malloc(s->lold - (j - i + 1) + 1);
	s->new_num = malloc(s->lold - (j - i + 1) + 1);
	if (!s->new_input || !s->new_type || !s->new_num)
		return (-1);
	s->p = 0;
	cy3_handle_dollar_brace7(current, i, s);
	cy3_handle_dollar_brace9(current, j, s);
	cy3_handle_dollar_brace6(current, s);
	return (i);
}
