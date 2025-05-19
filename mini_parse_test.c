
#include <stdio.h>
#include "parser_new.h"
#include "libft.h"

static void print_cmd(t_cmd *cmd)
{
    int i = 0;
    printf("Command:\n");
    if (cmd->args)
    {
        while (cmd->args[i])
        {
            printf("  arg[%d] = "%s"\n", i, cmd->args[i]);
            i++;
        }
    }
    t_redir *r = cmd->redirs;
    while (r)
    {
        printf("  redir type %d -> %s\n", r->type, r->file);
        r = r->next;
    }
    printf("  builtin_id = %d\n", cmd->builtin_id);
}

int main(int ac, char **av, char **envp)
{
    if (ac != 2)
    {
        fprintf(stderr, "usage: %s "command line"\n", av[0]);
        return 1;
    }
    int status = 0;
    t_cmd *cmds = parse_command_new(av[1], NULL, &status);
    if (!cmds)
    {
        printf("Parse error, status=%d\n", status);
        return 1;
    }
    t_cmd *tmp = cmds;
    while (tmp)
    {
        print_cmd(tmp);
        tmp = tmp->next;
        if (tmp)
            printf("---- next cmd ----\n");
    }
    return 0;
}
