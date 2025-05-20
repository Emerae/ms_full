#include "parser_new.h"

void	free_redirs(t_redir *redir)
{
	t_redir	*tmp;

	while (redir)
	{
		tmp = redir->next;
		free(redir->file);
		free(redir);
		redir = tmp;
	}
}

void cy2_free_first_cmd_node(t_cmd **head)
{
    t_cmd *to_free;
    int i;

    printf("DEBUG: Entrée dans cy2_free_first_cmd_node avec head=%p\n", *head);
    if (!head || !*head)
        return;
    
    to_free = *head;
    
    // IMPORTANT: Vérifier si ce nœud a des redirections et un nœud suivant
    if (to_free->redirs && to_free->next)
    {
        printf("DEBUG: Transfert des redirections du premier nœud au suivant\n");
        
        // Si le nœud suivant a déjà des redirections, trouver la fin de sa liste
        if (to_free->next->redirs)
        {
            t_redir *last = to_free->next->redirs;
            while (last->next)
                last = last->next;
            
            // Ajouter les redirections du premier nœud à la fin
            last->next = to_free->redirs;
        }
        else
        {
            // Simplement transférer le pointeur de redirections
            to_free->next->redirs = to_free->redirs;
        }
        
        // Important: empêcher free_redirs de libérer ces redirections
        to_free->redirs = NULL;
    }
    
    // Le reste de la fonction inchangé
    if (to_free->args)
    {
        i = 0;
        while (to_free->args[i])
        {
            free(to_free->args[i]);
            i = i + 1;
        }
        free(to_free->args);
    }
    if (to_free->redirs)
        free_redirs(to_free->redirs);
    *head = to_free->next;
    free(to_free);
    
    printf("DEBUG: Sortie de cy2_free_first_cmd_node, nouveau head=%p\n", *head);
}
