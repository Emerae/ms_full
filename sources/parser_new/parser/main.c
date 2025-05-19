#include "prser.h"
// #include "../parser/parser.h"

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
				printf("[%d]num _%s_\n", current->number, current->input_num);
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

void	print_envv(char **env)
{
	int	i = 0;

	while (env[i])
	{
		printf("l%d:%s\n",i,env[i]);
		i = i + 1;
	}
	printf("----\n");
}

void	print_cmd_list(t_cmd *head_cmd)
{
	t_cmd *cmd = head_cmd;
	int arg_index;
	t_redir *redir;

	while (cmd)
	{
		printf("Command:\n");

		// Print args
		if (cmd->args)
		{
			arg_index = 0;
			while (cmd->args[arg_index])
			{
				printf("  arg %d : _%s_\n", arg_index + 1, cmd->args[arg_index]);
				arg_index = arg_index + 1;
			}
		}
		else
			printf("  No arguments\n");

		// Print builtin ID
		if (cmd->builtin_id == -1)
			printf("  builtin : not a builtin\n");
		else if (cmd->builtin_id == 1)
			printf("  builtin : echo -n\n");
		else if (cmd->builtin_id == 2)
			printf("  builtin : echo\n");
		else if (cmd->builtin_id == 3)
			printf("  builtin : cd\n");
		else if (cmd->builtin_id == 4)
			printf("  builtin : pwd\n");
		else if (cmd->builtin_id == 5)
			printf("  builtin : export\n");
		else if (cmd->builtin_id == 6)
			printf("  builtin : unset\n");
		else if (cmd->builtin_id == 7)
			printf("  builtin : env\n");
		else if (cmd->builtin_id == 8)
			printf("  builtin : exit\n");
		else
			printf("  builtin : bug %d\n", cmd->builtin_id);

		// Print redirections
		if (cmd->redirs)
		{
			printf("  redirections:\n");
			redir = cmd->redirs;
			while (redir)
			{
				if (redir->type == 0)
					printf("    Type: <  File: %s\n", redir->file);
				else if (redir->type == 1)
					printf("    Type: >  File: %s\n", redir->file);
				else if (redir->type == 2)
					printf("    Type: >> File: %s\n", redir->file);
				else if (redir->type == 3)
					printf("    Type: << File: %s\n", redir->file);
				else
				printf("Problem Type: %d  File: %s\n", redir->type, redir->file);
				redir = redir->next;
			}
		}
		else
			printf("  No redirections\n");
		// Move to the next command in the pipeline
		cmd = cmd->next;
		if (cmd)
			printf("---- next command ----\n");
	}
}

void	free_list(t_input **head)
{
	t_input	*tmp;
	while (*head)
	{
		tmp = *head;
		*head = (*head)->next;
		if (tmp -> input)
			free(tmp->input);
		free(tmp);
	}
	*head = NULL;
}

int	main(void)
{
	char		*input;
	t_input		*head_input;
	char		**env;

	env = cy0_copy_env_from_proc();
	if (!env)
	{
		printf("failed loading env\n");
		// GOOD ONE
		// return (1);
		// Bad one
		{
			env = malloc(sizeof(char *) * 4);
			if (!env)
				return (1);
			env[0] = strdup("ab=鮴");
			env[1] = strdup("a=b");
			env[2] = strdup("HOME=/home/Cyril/42/minishell/250518");
			env[3] = NULL;
			printf("env[0]: %s\n", env[0]);
			printf("env[1]: %s\n", env[1]);
			printf("env[2]: %s\n", env[2]);
		}
	}
	if (cy00_modify_env(&env, 0))
	{
		printf("env problem\n");
		return (1);
	}

	printf("minishell starts\n");
	while (1)
	{
		input = readline("minishell% ");
		if (!input)
			break;
		if (*input)
			add_history(input);
		printf("You entered: _%s_\n", input);
		head_input = cy1_make_list(input);
		if (!head_input)
		{
			printf("head_input went wrong\n");
			free(input);
			if (cy00_modify_env(&env, -1))
				break;
			continue ;
		}
		if (head_input->type == 1 && !head_input->next)
		{
			printf("empty command, just space\n");
			cy0_free_input_list(head_input);
			free(input);
			if (cy00_modify_env(&env, -1))
				break;
			continue ;
		}
		printf("\nsubsti check\n");
		if (cy3_substi_check(&head_input, env))
		{
			printf("substi check failed\n");
			free(input);
			cy0_free_input_list(head_input);
			if (cy00_modify_env(&env, -1))
				break;
			continue ;
		}
		printf("scanning dollar syntax\n");
		print_input_list(head_input);
		cy3_scan_dollar_syntax(head_input, env);
		if (cy3_scan_dollar_syntax(head_input, env))
		{
			printf("dollar syntax failed\n");
			free(input);
			cy0_free_input_list(head_input);
			if (cy00_modify_env(&env, -1))
				break;
			continue ;
		}
		print_input_list(head_input);
		printf("removing space nodes\n");
		cy1_remove_space_nodes(&head_input);
		print_input_list(head_input);

		printf("4-1 wrong char\n");
		if (cy4_1wrong_char(head_input))
		{
			printf("wrong char\n");
			free(input);
			cy0_free_input_list(head_input);
			continue ;
		}
		printf("4-2 wrong redir\n");
		if (cy4_2wrong_redir(head_input))
		{
			printf("wrong redir\n");
			free(input);
			cy0_free_input_list(head_input);
			continue ;
		}
		printf("4-3 wrong pipe\n");
		if (cy4_3wrong_pipe(head_input))
		{
			printf("wrong pipe\n");
			free(input);
			cy0_free_input_list(head_input);
			continue ;
		}
		printf("4-4 wrong redir log\n");
		if (cy4_4wrong_redir_log(head_input))
		{
			printf("wrong redir log\n");
			free(input);
			cy0_free_input_list(head_input);
			continue ;
		}
		printf("4-5 wrong pipe log\n");
		if (cy4_5wrong_pipe_log(head_input))
		{
			printf("wrong pipe log\n");
			free(input);
			cy0_free_input_list(head_input);
			continue ;
		}
		t_cmd		*head_cmd;
		head_cmd = cy2_convert_cmd(head_input);
		if (!head_cmd)
		{
			printf("\nhead cmd went wrong\n");
			free(input);
			cy0_free_input_list(head_input);
			continue ;
		}
		printf("\nconverting to cmd went well\n");
		print_input_list(head_input);
		print_cmd_list(head_cmd);
		cy0_free_input_list(head_input);
		free(input);
	}
	printf("closing\n");
	rl_clear_history();
	return (0);
}

// print_input_list(head_input);

	// // Current Directory builder
		// static char	current_location[PATH_MAX];
		// char *home;
	// home = getenv("HOME");
	// if (home)
	// {
	// 	cy_strlcpy(current_location, home, PATH_MAX);
	// 	printf("HOME = %s\n", home);
	// 	printf("Current location = %s\n", current_location);
	// }
	// 	else
	// {
	// 	printf("No HOME found\n");
	// 	return (2);
	// }