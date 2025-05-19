#include "parser_new.h"

static void	cy2_fill_redir_skip_update(t_fill_redir *s)
{
	if (s->flag == 1 && s->node->next)
	{
		if (s->nb_skip_head == 1)
			s->nb_skip_head = 2;
		else
			s->nb_skip_head = s->nb_skip_head + 2;
	}
	else if (s->flag == 1 && !s->node->next)
	{
		if (s->nb_skip_head != 1)
			s->nb_skip_head = s->nb_skip_head + 1;
	}
}

int cy2_fill_redir_loop_body(t_fill_redir *s, int *nature)
{
    int nodes_skipped;
    int continue_loop;
    int redir_count;
    
    redir_count = 0;
    continue_loop = 1;
    
    while (continue_loop)
    {
        // Mise à jour du compteur de saut
        cy2_fill_redir_skip_update(s);
        
        // Vérifier si on atteint la fin de la commande ou un pipe
        if (cy2_fill_redir_2(s->node, nature, &s->flag))
        {
            // Si on a trouvé au moins une redirection, considérer comme succès
            if (redir_count > 0)
                return (1);
            break; // Sinon sortir et échouer
        }
        
        // Avancer au nœud suivant si le flag est activé
        if (s->flag == 1 && s->node)
            s->node = s->node->next;
        
        // Si le nœud actuel n'est pas une redirection, sortir
        if (!s->node || !s->node->input || !redir_type_from_str(s->node->input))
        {
            // Si on a déjà trouvé des redirections, c'est un succès
            if (redir_count > 0)
                return (1);
            break; // Sinon sortir et échouer
        }
        
        // Traiter la redirection courante
        printf("DEBUG: Processing redirection node with input '%s'\n", s->node->input);
        nodes_skipped = cy2_fill_redir_1(s->node, &s->head, &s->last);
        
        if (nodes_skipped <= 0)
        {
            printf("DEBUG: Failed to process redirection\n");
            if (redir_count == 0)
                return (0); // Échec total si aucune redirection n'a été traitée
            break; // Sinon sortir avec succès
        }
        
        // Incrémente le compteur de redirections traitées
        redir_count++;
        printf("DEBUG: Successfully processed redirection, count=%d\n", redir_count);
        
        // Avancer au nœud suivant (sauter la redirection et son fichier)
        while (nodes_skipped > 0 && s->node)
        {
            s->node = s->node->next;
            nodes_skipped--;
        }
        
        // Préparer à traiter la prochaine redirection
        s->flag = 1;
    }
    
    // Retourner succès si au moins une redirection a été traitée
    return (redir_count > 0);
}
