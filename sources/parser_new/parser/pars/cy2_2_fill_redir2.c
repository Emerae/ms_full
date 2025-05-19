#include "parser_new.h"

int cy2_fill_redir_loop_body(t_fill_redir *s, int *nature)
{
    int processed = 0;
    
    while (s->node)
    {
        // Vérifier si c'est une redirection
        if (s->node->input && (redir_type_from_str(s->node->input) >= 0))
        {
            printf("DEBUG: Processing redirection node with input '%s'\n", s->node->input);
            
            // Trouver le nœud de fichier (sauter les espaces)
            t_input *file_node = s->node->next;
            int nodes_to_skip = 1;  // Compter le nœud de redirection
            
            while (file_node && file_node->type == 1)
            {
                file_node = file_node->next;
                nodes_to_skip++;
            }
            
            if (file_node)
            {
                // Ajouter la redirection
                t_redir *new_redir = malloc(sizeof(t_redir));
                if (!new_redir)
                    return 0;
                    
                new_redir->type = redir_type_from_str(s->node->input);
                new_redir->file = strdup(file_node->input);
                new_redir->next = NULL;
                
                // Ajouter à la liste
                if (!s->head)
                    s->head = new_redir;
                else
                    s->last->next = new_redir;
                s->last = new_redir;
                
                printf("DEBUG-REDIR: Adding redirection type %d for file '%s'\n", 
                       new_redir->type, new_redir->file);
                       
                processed = 1;
                
                // Avancer au nœud après le fichier
                s->node = file_node;
                s->nb_skip_head = nodes_to_skip;
                
                // Vérifier s'il y a une autre redirection
                t_input *next = s->node->next;
                if (next)
                {
                    // Sauter les espaces
                    while (next && next->type == 1)
                        next = next->next;
                        
                    // Si le prochain nœud est une redirection, continuer la boucle
                    if (next && next->input && redir_type_from_str(next->input) >= 0)
                    {
                        s->node = next;
                        continue;
                    }
                }
            }
            else
            {
                printf("DEBUG: Failed to process redirection\n");
                return 0;
            }
        }
        
        // Vérifier pour les cas de fin
        if (!s->node->next)
        {
            printf("DEBUG: End of command reached\n");
            *nature = 3;  // Fin de commande
            break;
        }
        else if (s->node->next->input && s->node->next->input[0] == '|')
        {
            printf("DEBUG: Pipe found\n");
            *nature = 2;  // Pipe
            break;
        }
        
        // Si on arrive ici, il n'y a plus de redirections à traiter
        break;
    }
    
    return processed;
}
