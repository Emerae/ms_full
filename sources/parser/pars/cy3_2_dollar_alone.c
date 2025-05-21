#include "parser_new.h"

void	cy3_handle_dollar_alone3(t_input *current, t_dollar_alone *sda)
{
	sda->new_input[sda->p] = '\0';
	sda->new_type[sda->p] = '\0';
	sda->new_num[sda->p] = '\0';
	free(current->input);
	free(current->input_type);
	free(current->input_num);
	current->input = sda->new_input;
	current->input_type = sda->new_type;
	current->input_num = sda->new_num;
}

void	cy3_handle_dollar_alone2(t_input *current, int i, t_dollar_alone *sda)
{
	sda->k = i + 1;
	while (current->input[sda->k])
	{
		sda->new_input[sda->p] = current->input[sda->k];
		sda->new_type[sda->p] = current->input_type[sda->k];
		sda->new_num[sda->p] = current->input_num[sda->k];
		sda->k = sda->k + 1;
		sda->p = sda->p + 1;
	}
}

void	cy3_handle_dollar_alone1(t_input *current, int i, t_dollar_alone *sda)
{
	sda->k = 0;
	while (sda->k < i)
	{
		sda->new_input[sda->p] = current->input[sda->k];
		sda->new_type[sda->p] = current->input_type[sda->k];
		sda->new_num[sda->p] = current->input_num[sda->k];
		sda->p = sda->p + 1;
		sda->k = sda->k + 1;
	}
}

int	cy3_handle_dollar_alone(t_input *current, int i)
{
	t_dollar_alone	sda;

	sda.p = 0;
	sda.lold = cy_strlen(current->input);
	sda.new_input = malloc(sda.lold);
	sda.new_type = malloc(sda.lold);
	sda.new_num = malloc(sda.lold);
	if (!sda.new_input || !sda.new_type || !sda.new_num)
		return (-1);
	cy3_handle_dollar_alone1(current, i, &sda);
	cy3_handle_dollar_alone2(current, i, &sda);
	cy3_handle_dollar_alone3(current, &sda);
	return (i);
}
