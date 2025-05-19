#include "parser_new.h"
#include <stdio.h>

void	print_input_list(t_input *head_input)
{
	t_input *current;

	current = head_input;
	printf("type 1 space, 2 text, 3 '', 4 \"\"\n");
	while (current)
	{
		if (current->input)
		{
			printf("[%d]inp _%s_\n", current->number, current->input);
			if (current->input_num)
				printf("[%d]num _%d_\n", current->number, current->input_num);
			if (current->input_type)
				printf("[%d]typ _%s_\n", current->number, current->input_type);
			printf("[%d] type: %d\n", current->number, current->type);
		}
		else
			printf("[%d] (null)\n", current->number);
		printf("\n");
		current = current->next;
	}
} 