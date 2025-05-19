#include "../prser.h"

int	cy3_scan_dollar_syntax_1_1_1(t_input *current, int i)
{
	if (!current->input[i + 2] || !((current->input[i + 2] >= 'A'
				&& current->input[i + 2] <= 'Z')
			|| (current->input[i + 2] >= 'a'
				&& current->input[i + 2] <= 'z') || current->input[i + 2] == '_'
			|| current->input[i + 2] == '}'))
		return (1);
	return (0);
}

int	cy3_scan_dollar_syntax_1_1_2(t_input *current, int i, int j)
{
	while (current->input[j] && current->input[j] != '}')
	{
		if (current->input_num[j] != current->input_num[i])
			return (1);
		if (!((current->input[j] >= 'a' && current->input[j] <= 'z')
				|| (current->input[j] >= 'A' && current->input[j] <= 'Z')
				|| (current->input[j] >= '0' && current->input[j] <= '9')
				|| (current->input[j] == '_')))
			return (1);
		j = j + 1;
	}
	if (!current->input[j] || current->input_num[j] != current->input_num[i])
		return (1);
	return (j);
}

int	cy3_scan_dollar_syntax_1_1(t_input *current, int i, char **env)
{
	int	j;

	if (cy3_scan_dollar_syntax_1_1_1(current, i))
		return (1);
	j = i + 2;
	j = cy3_scan_dollar_syntax_1_1_2(current, i, j);
	if (j == 1)
		return (1);
	i = cy3_handle_dollar_brace(current, i, j, env);
	if (i == -1)
		return (1);
	return (0);
}

int	cy3_scan_dollar_syntax_2_1_1(t_input *current, int i, int j)
{
	while (current->input[j])
	{
		if (current->input_num[j] != current->input_num[i])
			break ;
		if (!((current->input[j] >= 'a' && current->input[j] <= 'z')
				|| (current->input[j] >= 'A' && current->input[j] <= 'Z')
				|| (current->input[j] >= '0' && current->input[j] <= '9')
				|| (current->input[j] == '_')))
			break ;
		j = j + 1;
	}
	return (j);
}

int	cy3_scan_dollar_syntax_2_1(t_input *current, int i, char **env)
{
	int	j;

	j = i + 1;
	j = cy3_scan_dollar_syntax_2_1_1(current, i, j);
	printf("DEBUG i = %d j = %d\n", i, j);
	i = cy3_handle_dollar_word(current, i, j - 1, env);
	if (i == -1)
		return (1);
	return (0);
}
