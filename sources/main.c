#include "minishell.h"

int			g_signal;


/**
 * @brief Signal handler function.
 * 
 * This function handles the SIGINT and SIGQUIT signals. It updates the global
 * signal variable and prints appropriate messages based on the signal received.
 * 
 * @param signo The signal number.
 */
/**
 * @brief Signal handler function to manage SIGINT and SIGQUIT signals.
 *
 * This function handles the SIGINT and SIGQUIT signals and performs specific
 * actions based on the signal received and the value of the global variable `g_signal`.
 *
 * @param signo The signal number received (e.g., SIGINT or SIGQUIT).
 *
 * Behavior:
 * - If `signo` is SIGINT and `g_signal` is not 6:
 *   - Prints a newline character.
 *   - Calls the `prompt()` function unless `g_signal` is 5.
 *   - Sets `g_signal` to 2.
 * - If `signo` is SIGQUIT and `g_signal` is not 6:
 *   - If `g_signal` is 5, prints "Quit (core dumped)" followed by a newline.
 *   - Sets `g_signal` to 3.
 * - For both signals, if `g_signal` is not 6, it clears the last two characters
 *   from the terminal using backspace and space characters.
 *
 * Note:
 * - The global variable `g_signal` is used to determine the behavior of the handler.
 * - The function assumes the existence of `ft_putstr()` for printing strings
 *   and `prompt()` for displaying the shell prompt.
 */
void		handler(int signo)
{
	if (g_signal != 6)
		ft_putstr("\b\b  \b\b");
	if (signo == SIGINT && (g_signal != 6))
	{
		ft_putstr("\n");
		if (g_signal != 5)
			prompt();
		g_signal = 2;
	}
	else if (signo == SIGQUIT && (g_signal != 6))
	{
		if (g_signal == 5)
			ft_putstr("\b\b  \b\bQuit (core dumped)\n");
		g_signal = 3;
	}
}

/**
 * @brief Parse and execute a command line.
 * 
 * This function parses the given command line, executes the command, updates
 * the environment list, and frees allocated resources. It also prints any
 * parsing errors and updates the return status.
 * 
 * @param envl Pointer to the environment list.
 * @param line The command line to parse and execute
 */

int array_len(char **arr)
{
    int i = 0;
    if (!arr)
        return 0;
    while (arr[i])
        i++;
    return i;
}

void parse_and_exec(t_list **envl, char *line)
{
    int status = 0;
    t_cmd *cmds = parse_command_new(line, *envl, &status);
    if (!cmds)
    {
        update_return(envl, status);
    }
    else
    {
        execute_cmds(cmds, envl, &status);
        update_return(envl, status);
        t_cmd *tmp = cmds;
        while (tmp && tmp->next)
            tmp = tmp->next;
            
        // Vérifier que la commande a des arguments non-vides
        if (tmp && tmp->args && tmp->args[0])
        {
            int len = array_len(tmp->args);
            if (len > 0)  // Vérification supplémentaire que le tableau n'est pas vide
                update_last_arg(envl, NULL, tmp->args[len - 1]);
        }
        
        cy0_free_cmd_list(cmds);
    }
    update_env(envl);
    free(line);
    prompt();
}


/**
 * @brief Wait for and process commands.
 * 
 * This function sets up signal handlers, waits for user input, and processes
 * commands in a loop. It updates the global signal variable and return status
 * based on the command execution results.
 * 
 * @param envl Pointer to the environment list.
 * @return int The exit status of the shell.
 */
static int	waiting_command(t_list **envl)
{
	char	*line;

	signal(SIGINT, &handler);
	signal(SIGQUIT, &handler);
	line = NULL;
	g_signal = 0;
	update_return(envl, 0);
	prompt();
	while (reader(&line) > 0)
	{
		if (g_signal == 4)
			g_signal = 2;
		parse_and_exec(envl, line);
		g_signal = 0;
	}
	if (line)
		free(line);
	if (g_signal)
		return (128 + g_signal);
	return (SUCCESS);
}

/**
 * @brief Entry point of the minishell program.
 *
 * This function initializes the environment list, displays the header,
 * parses the environment variables, waits for user commands, and handles
 * the program exit.
 *
 * @param argc The number of command-line arguments.
 * @param argv The array of command-line arguments.
 * @param env The array of environment variables.
 * @return The exit value of the program.
 */
int			main(int argc, char **argv, char **env)
{
	t_list	*envl;
	int		exit_value;

	(void)argv;
	(void)argc;
	//header();
	parse_env(&envl, env);
	exit_value = waiting_command(&envl);
	if (!exit_value)
		exit_value = get_return(envl);
	ft_lstclear(&envl, &free_entry);
	ft_putstr("exit\n");
	return (exit_value);
}
