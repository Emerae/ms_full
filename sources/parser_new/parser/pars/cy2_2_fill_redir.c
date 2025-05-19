#include "parser_new.h"

int	redir_type_from_str(const char *s)
{
	if (!s)
		return (-1);
	if (cy_strcmp(s, "<") == 0)
		return (0);
	if (cy_strcmp(s, ">") == 0)
		return (1);
	if (cy_strcmp(s, ">>") == 0)
		return (2);
	if (cy_strcmp(s, "<<") == 0)
		return (3);
	return (-2);
}

int cy2_fill_redir_1(t_input *node, t_redir **head, t_redir **last)
{
    t_redir *new_redir;
    int     type;
    t_input *file_node;
    int     nodes_skipped = 0;

    if (!node || !node->next)
        return (0);
    type = redir_type_from_str(node->input);
    if (type < 0)
        return (0);
    
    // Sauter les nœuds d'espace pour trouver le nom de fichier
    file_node = node->next;
    nodes_skipped = 1;  // Compter le premier saut
    
    while (file_node && file_node->type == 1) { // Type 1 = espace
        file_node = file_node->next;
        nodes_skipped++;
    }
        
    if (!file_node) // S'assurer qu'il y a bien un nom de fichier
        return (0);
        
    new_redir = malloc(sizeof(t_redir));
    if (!new_redir)
        return (0);
    new_redir->type = type;
    new_redir->file = cy_true_strdup(file_node->input);
    if (!new_redir->file)
    {
        free(new_redir);
        return (0);
    }
    new_redir->next = NULL;
    
    // Ajouter ici le message de débogage
    printf("DEBUG-REDIR: Adding redirection type %d for file '%s'\n", type, file_node->input);
    
    if (!*head)
        *head = new_redir;
    else
        (*last)->next = new_redir;
    *last = new_redir;
    
    // Retourner le nombre de nœuds parcourus au total
    return (nodes_skipped);
}

int	cy2_fill_redir_2(t_input *node, int *nature, int *flag)
{
	if (!node->next)
	{
		*nature = 3;
		return (1);
	}
	if (cy_strcmp(node->next->input, "|") == 0)
	{
		*nature = 2;
		*flag = 2;
		return (1);
	}
	return (0);
}

static void cy2_fill_redir_3(t_cmd **current_cmd, t_input **current_input,
    t_redir *head_redir, int flag)
{
    if (!head_redir)
    {
        *current_input = NULL;
        return;
    }
        
    // Trouver la fin de la liste actuelle de redirections
    t_redir *existing_last = NULL;
    if ((*current_cmd)->redirs)
    {
        existing_last = (*current_cmd)->redirs;
        while (existing_last->next)
            existing_last = existing_last->next;
            
        // Ajouter la nouvelle redirection à la fin
        existing_last->next = head_redir;
    }
    else
    {
        // Première redirection pour cette commande
        (*current_cmd)->redirs = head_redir;
    }
    
    if (flag == 2 && *current_input)
        *current_input = (*current_input)->next;
}

int cy2_fill_redir(t_cmd **current_cmd, t_input **current_input, int *nature)
{
    t_fill_redir    s;
    int             ok;

    s.node = *current_input;
    s.head = NULL;
    s.last = NULL;
    s.nb_skip_head = 1;
    s.flag = 0;
    ok = cy2_fill_redir_loop_body(&s, nature);
    if (ok == 0)
        return (0);
        
    // Ajouter ici l'affichage des redirections à la fin
    if (s.head) {
        printf("DEBUG-REDIR: Redirections list after filling:\n");
        t_redir *debug_r = s.head;
        int debug_count = 0;
        while (debug_r) {
            printf("DEBUG-REDIR:   [%d] type=%d, file='%s'\n", 
                   debug_count++, debug_r->type, debug_r->file);
            debug_r = debug_r->next;
        }
    }
    
    *current_input = s.node;
    cy2_fill_redir_3(current_cmd, current_input, s.head, s.flag);
    return (s.nb_skip_head);
}
