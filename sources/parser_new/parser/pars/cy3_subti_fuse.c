#include "parser_new.h"

static int	cy3_fuse_nospace_1(t_input *current,
	t_input *fusing, int lcurrent, int lfusing)
{
	char	*new_input;
	char	*new_type;
	char	*new_num;

	new_input = malloc(lcurrent + lfusing + 1);
	new_type = malloc(lcurrent + lfusing + 1);
	new_num = malloc(lcurrent + lfusing + 1);
	if (!new_input || !new_type || !new_num)
	{
		free(new_input);
		free(new_type);
		free(new_num);
		return (1);
	}
	cy_strlcpy(new_input, current->input, lcurrent + lfusing + 1);
	cy_strlcat(new_input, fusing->input, lcurrent + lfusing + 1);
	cy_strlcpy(new_type, current->input_type, lcurrent + lfusing + 1);
	cy_strlcat(new_type, fusing->input_type, lcurrent + lfusing + 1);
	cy_strlcpy(new_num, current->input_num, lcurrent + lfusing + 1);
	cy_strlcat(new_num, fusing->input_num, lcurrent + lfusing + 1);
	current->input = new_input;
	current->input_type = new_type;
	current->input_num = new_num;
	return (0);
}

static void	cy3_fuse_nospace_2(t_input *current, t_input *fusing)
{
	current->next = fusing->next;
	if (fusing->next)
		fusing->next->prev = current;
	free(fusing->input);
	free(fusing->input_type);
	free(fusing->input_num);
	free(fusing);
}

static int	cy3_fuse_nospace_3(t_input *current, t_input *fusing)
{
	int	lcurrent;
	int	lfusing;

	lcurrent = cy_strlen(current->input);
	lfusing = cy_strlen(fusing->input);
	free(current->input);
	free(current->input_type);
	free(current->input_num);
	if (cy3_fuse_nospace_1(current, fusing, lcurrent, lfusing))
		return (1);
	cy3_fuse_nospace_2(current, fusing);
	return (0);
}

int	cy3_fuse_nospace(t_input *head)
{
	t_input	*anchor;
	t_input	*current;
	t_input	*fusing;

	anchor = head;
	while (anchor)
	{
		if (anchor->type == 1)
		{
			anchor = anchor->next;
			continue ;
		}
		current = anchor;
		while (current->next && current->next->type != 1)
		{
			fusing = current->next;
			if (cy3_fuse_nospace_3(current, fusing))
				return (1);
		}
		anchor = anchor->next;
	}
	return (0);
}
