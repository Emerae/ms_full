#include "parser_new.h"

int	cy3_scan_dollar_syntax_2(t_input *current, int i, char **env)
{
	int	j;

	if (current->input[i + 1] == '?')
	{
		if (current->input_num[i + 1] != current->input_num[i])
			return (1);
		j = i + 1;
		printf("i = %d\n j = %d\n", i, j);
		i = cy3_handle_dollar_bang(current, i, j, env);
		printf("ii = %d\njj = %d\n", i, j);
		if (i == -1)
			return (1);
		return (0);
	}
	return (cy3_scan_dollar_syntax_2_1(current, i, env));
}

int	cy3_scan_dollar_syntax_dollar_1(t_input *current,
		int *i, char **env)
{
	if (current->input[*i + 2] && current->input[*i + 3]
		&& current->input[*i + 3] == '}' && current->input[*i + 2] == '?'
		&& current->input_num[*i] == current->input_num[*i + 1]
		&& current->input_num[*i + 1] == current->input_num[*i + 2]
		&& current->input_num[*i + 2] == current->input_num[*i + 3])
	{
		printf("found ${?}\n");
		printf("i = %d\n", *i);
		*i = cy3_handle_dollar_bang(current, *i, *i + 3, env);
		printf("ii = %d\n", *i);
		if (*i == -1)
			return (1);
		*i = *i + 1;
		return (0);
	}
	if (cy3_scan_dollar_syntax_1_1(current, *i, env))
		return (1);
	return (0);
}

int	cy3_scan_dollar_syntax_dollar_2(t_input *current,
		int *i, char **env)
{
	if ((current->input[*i + 1] >= 'A' && current->input[*i + 1] <= 'Z')
		|| (current->input[*i + 1] >= 'a' && current->input[*i + 1] <= 'z')
		|| current->input[*i + 1] == '_'
		|| current->input[*i + 1] == '?')
	{
		if (cy3_scan_dollar_syntax_2(current, *i, env))
			return (1);
	}
	else
	{
		*i = cy3_handle_dollar_alone(current, *i);
		if (*i == -1)
			return (1);
	}
	return (0);
}

int	cy3_scan_dollar_syntax_dollar(t_input *current, int *i, char **env)
{
	if (!current || !current->input || !i || *i < 0 || 
        *i >= cy_strlen(current->input) || !env)
        return (1);
	if (current->input[*i + 1] == '\0'
		|| current->input_num[*i + 1] != current->input_num[*i])
	{
		*i = *i + 1;
		return (0);
	}
	else if (current->input[*i + 1] == '{')
		return (cy3_scan_dollar_syntax_dollar_1(current, i, env));
	else
		return (cy3_scan_dollar_syntax_dollar_2(current, i, env));
	return (0);
}

int	cy3_scan_dollar_syntax(t_input *head, char **env)
{
	t_input	*current;
	int		i;

	current = head;
	while (current)
	{
		i = 0;
		while (current->input && current->input[i])
		{
			if (current->input[i] == '$' && current->type != 3)
			{
				if (cy3_scan_dollar_syntax_dollar(current, &i, env))
					return (1);
				continue ;
			}
			i = i + 1;
		}
		current = current->next;
	}
	return (0);
}
