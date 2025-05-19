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

int	cy3_handle_dollar_word_1(t_input *current, char **env, t_dollar_word *s)
{
	// CORRECTION: vérifie env[s->e] au lieu de env[s->e + 1]
	if (env[s->e])
	{
		s->value = cy_strchr(env[s->e], '=') + 1;
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
		return (cy3_handle_dollar_word_2(current, s));
	}
	return (cy3_handle_dollar_word_3(current, s->i, s->j));
}

int	cy3_handle_dollar_word(t_input *current, int i, int j, char **env)
{
	t_dollar_word	s;

	s.i = i;
	s.j = j;
	cy3_handle_dollar_word_key(current, &s);
	cy3_handle_dollar_word_findenv(&s, env);
	return (cy3_handle_dollar_word_1(current, env, &s));
}
