#include "parser_new.h"

static t_redir *clone_redir(t_redir *redir)
{
    t_redir *new_redir = NULL;
    t_redir *current = NULL;
    t_redir *prev = NULL;

    while (redir)
    {
        new_redir = malloc(sizeof(t_redir));
        if (!new_redir)
        {
            // En cas d'échec, libérer ce qui a déjà été alloué
            while (current)
            {
                prev = current->next;
                free(current->file);
                free(current);
                current = prev;
            }
            return (NULL);
        }

        new_redir->type = redir->type;
        new_redir->file = cy_true_strdup(redir->file);
        if (!new_redir->file)
        {
            free(new_redir);
            while (current)
            {
                prev = current->next;
                free(current->file);
                free(current);
                current = prev;
            }
            return (NULL);
        }

        new_redir->next = NULL;
        if (!current)
            current = new_redir;
        else
            prev->next = new_redir;
        prev = new_redir;
        redir = redir->next;
    }

    return (current);
}
