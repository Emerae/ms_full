#include "../prser.h"

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

void	cy3_handle_dollar_word_findenv(t_dollar_word *s, char **env)
{
	char	*equal;

	s->e = 0;
	while (env[s->e + 1])
	{
		equal = cy_strchr(env[s->e], '=');
		if (!equal)
		{
			s->e = s->e + 1;
			continue ;
		}
		if ((int)(equal - env[s->e]) == s->keylen &&
			cy_strncmp(env[s->e], s->key, s->keylen) == 0)
			break ;
		s->e = s->e + 1;
	}
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

int	cy3_handle_dollar_word_2(t_input *current, t_dollar_word *s)
{
	s->vlen = cy_strlen(s->value);
	s->lold = cy_strlen(current->input);
	s->new_input = malloc(s->lold - (s->j - s->i + 1) + s->vlen + 1);
	s->new_type = malloc(s->lold - (s->j - s->i + 1) + s->vlen + 1);
	s->new_num = malloc(s->lold - (s->j - s->i + 1) + s->vlen + 1);
	if (!s->new_input || !s->new_type || !s->new_num)
		return (-1);
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
	return (s->i + s->vlen - 1);
}
