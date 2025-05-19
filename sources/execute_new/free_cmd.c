#include "parser_new.h"
#include <stdlib.h>

static void free_redir_list(t_redir *redir)
{
    t_redir *next;
    while (redir)
    {
        next = redir->next;
        free(redir->file);
        free(redir);
        redir = next;
    }
}

void free_cmd_list(t_cmd *cmd)
{
    t_cmd *next;
    while (cmd)
    {
        next = cmd->next;
        if (cmd->args)
        {
            for (int i = 0; cmd->args[i]; i++)
                free(cmd->args[i]);
            free(cmd->args);
        }
        free_redir_list(cmd->redirs);
        free(cmd);
        cmd = next;
    }
} 
