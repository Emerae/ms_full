#include "parser_new.h"

int	cy3_handle_dollar_brace(t_input *current, int i, int j, char **env)
{
	t_dollar_braces	sdbr;

	sdbr.len = j - i + 1;
	if (j == i + 2)
		return (cy3_handle_dollar_brace1(current, i, j, &sdbr));
	cy3_handle_dollar_brace2(&sdbr, current, i, j);
	cy3_handle_dollar_brace3(&sdbr, env);
	if (env[sdbr.e + 1])
		return (cy3_handle_dollar_brace4(current, i, j, &sdbr));
	return (cy3_handle_dollar_brace5(current, i, j, &sdbr));
}

int	cy3_handle_dollar_brace1(t_input *current, int i, int j, t_dollar_braces *s)
{
	if (cy3_handle_dollar_brace1a(current, i, s) == -1)
		return (-1);
	cy3_handle_dollar_brace1b(current, j, s);
	return (i);
}

int	cy3_handle_dollar_brace1a(t_input *current, int i, t_dollar_braces *s)
{
	int	k;

	k = 0;
	s->lold = cy_strlen(current->input);
	s->new_input = malloc(s->lold - s->len + 1);
	s->new_type = malloc(s->lold - s->len + 1);
	s->new_num = malloc(s->lold - s->len + 1);
	if (!s->new_input || !s->new_type || !s->new_num)
		return (-1);
	while (k < i)
	{
		s->new_input[k] = current->input[k];
		s->new_type[k] = current->input_type[k];
		s->new_num[k] = current->input_num[k];
		k = k + 1;
	}
	s->k = k;
	return (0);
}

void	cy3_handle_dollar_brace1b(t_input *current, int j, t_dollar_braces *s)
{
	j = j + 1;
	while (current->input[j])
	{
		s->new_input[s->k] = current->input[j];
		s->new_type[s->k] = current->input_type[j];
		s->new_num[s->k] = current->input_num[j];
		s->k = s->k + 1;
		j = j + 1;
	}
	s->p = s->k;
	cy3_handle_dollar_brace6(current, s);
}
