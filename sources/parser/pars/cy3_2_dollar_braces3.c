#include "parser_new.h"

void	cy3_handle_dollar_brace6(t_input *current, t_dollar_braces *s)
{
	s->new_input[s->p] = '\0';
	s->new_type[s->p] = '\0';
	s->new_num[s->p] = '\0';
	free(current->input);
	free(current->input_type);
	free(current->input_num);
	current->input = s->new_input;
	current->input_type = s->new_type;
	current->input_num = s->new_num;
}

void	cy3_handle_dollar_brace7(t_input *current, int i, t_dollar_braces *s)
{
	s->k = 0;
	while (s->k < i)
	{
		s->new_input[s->p] = current->input[s->k];
		s->new_type[s->p] = current->input_type[s->k];
		s->new_num[s->p] = current->input_num[s->k];
		s->p = s->p + 1;
		s->k = s->k + 1;
	}
}

void	cy3_handle_dollar_brace8(t_input *current, int i, t_dollar_braces *s)
{
	s->k = 0;
	while (s->k < s->vlen)
	{
		s->new_input[s->p] = s->value[s->k];
		s->new_type[s->p] = '5';
		s->new_num[s->p] = current->input_num[i];
		s->p = s->p + 1;
		s->k = s->k + 1;
	}
}

void	cy3_handle_dollar_brace9(t_input *current, int j, t_dollar_braces *s)
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
}
