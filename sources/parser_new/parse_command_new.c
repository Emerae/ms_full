#include "parser_new.h"
#include "libftfull.h"
#include "minishell.h"

/*
 * Simple wrapper: produce t_cmd* or NULL, set *status (0 or 258).
 * On syntax error prints message and returns NULL.
 */
static void    syntax_error(char *token)
{
    ft_printf_fd(2, "minishell: syntax error near unexpected token `%s'\n", token);
}

/* look for illegal tokens ';' and '\\' (backslash) outside quotes */
static int has_illegal_token(t_input *head, char **bad_tok)
{
    while (head)
    {
        if (head->type == 2 && head->input && (head->input[0] == ';' || head->input[0] == '\\') && head->input[1] == '\0')
        {
            *bad_tok = head->input;
            return (1);
        }
        head = head->next;
    }
    return (0);
}

t_cmd *parse_command_new(char *line, t_list *env, int *status)
{
    t_input *head = cy1_make_list(line);
    if (!head)
        return (NULL);
    /* basic illegal token check */
    char *bad = NULL;
    if (has_illegal_token(head, &bad))
    {
        syntax_error(bad);
        cy0_free_input_list(head);
        if (status)
            *status = 258;
        return (NULL);
    }
    if (cy3_substi_check(&head, env))
    {
        /* cy3 already prints error */
        cy0_free_input_list(head);
        if (status)
            *status = 1;
        return (NULL);
    }
    if (cy4_1wrong_char(head))
    {
        cy0_free_input_list(head);
        if (status)
            *status = 258;
        return (NULL);
    }
    t_cmd *cmds = cy2_convert_cmd(&head);
    cy0_free_input_list(head);
    if (!cmds)
    {
        if (status)
            *status = 258;
    }
    else if (status)
        *status = 0;
    return (cmds);
}
