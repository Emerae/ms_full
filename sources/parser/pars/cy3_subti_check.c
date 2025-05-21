#include "parser_new.h"

static int	cy3_fill_input_type_1(t_input *current, int *flag)
{
	int	linput;

	linput = cy_strlen(current->input);
	current->input_type = malloc(linput + 1);
	current->input_num = malloc(linput + 1);
	if (!current->input_type || !current->input_num)
		return (1);
	cy_memset(current->input_type, current->type + '0', linput);
	current->input_type[linput] = '\0';
	if (current->type != 1)
	{
		cy_memset(current->input_num, *flag + '0', linput);
		*flag = 3 - *flag;
	}
	else
	{
		cy_memset(current->input_num, '0', linput);
	}
	current->input_num[linput] = '\0';
	return (0);
}

int	cy3_fill_input_type(t_input *head)
{
	t_input	*current;
	int		flag;

	current = head;
	flag = 1;
	while (current)
	{
		if (current->input)
		{
			if (cy3_fill_input_type_1(current, &flag))
				return (1);
		}
		current = current->next;
	}
	return (0);
}

int cy3_substi_check(t_input **head_input, char **env, t_list *envl)
{
    int ret;

    ret = cy3_fill_input_type(*head_input);
    if (ret)
    {
        cy0_free_input_list(*head_input);
        return (1);
    }
    print_input_list(*head_input);
    
    ret = cy3_fuse_nospace(*head_input);
    if (ret)
    {
        cy0_free_input_list(*head_input);
        return (1);
    }
    
    ret = cy3_scan_dollar_syntax(*head_input, env, envl);
    if (ret)
    {
        printf("ERROR: Échec de l'expansion des variables\n");
        cy0_free_input_list(*head_input);
        return (1);
    }
    
    printf("subtsi check success\n");
    return (0);
}
