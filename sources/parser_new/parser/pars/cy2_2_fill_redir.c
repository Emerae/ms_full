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

/* Dans sources/parser_new/parser/pars/cy2_2_fill_redir.c */

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
    
    // Ajouter le message de débogage
    printf("DEBUG-REDIR: Adding redirection type %d for file '%s'\n", type, file_node->input);
    
    if (!*head)
        *head = new_redir;
    else
        (*last)->next = new_redir;
    *last = new_redir;
    
    // Retourner le nombre total de nœuds traités
    return (nodes_skipped + 1);  // +1 pour inclure le nœud du fichier
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

/*
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
}*/

int cy2_fill_redir(t_cmd **current_cmd, t_input **current_input, int *nature)
{
    // Vérifier si le nœud actuel existe et si c'est une redirection
    if (!current_input || !*current_input || !(*current_input)->input)
    {
        printf("DEBUG-REDIR: Aucun nœud d'entrée valide pour la redirection\n");
        return (0);
    }
    
    printf("DEBUG-REDIR: Vérification de '%s' pour redirection\n", (*current_input)->input);
    
    // Vérifier si c'est une redirection connue
    int redir_type = redir_type_from_str((*current_input)->input);
    if (redir_type < 0)
    {
        printf("DEBUG-REDIR: Ce n'est pas une redirection reconnue\n");
        return (0);
    }
    
    // On a trouvé une redirection! Trouver le fichier associé
    t_input *file_node = (*current_input)->next;
    int nodes_skipped = 1;  // On a déjà traité le symbole de redirection
    
    // Sauter les espaces
    while (file_node && file_node->type == 1)
    {
        file_node = file_node->next;
        nodes_skipped++;
    }
    
    // Vérifier si on a bien un nom de fichier
    if (!file_node || !file_node->input)
    {
        printf("DEBUG-REDIR: Pas de fichier trouvé après la redirection\n");
        return (0);
    }
    
    printf("DEBUG-REDIR: Création d'une redirection de type %d pour le fichier '%s'\n", 
           redir_type, file_node->input);
    
    // Créer une nouvelle structure de redirection
    t_redir *new_redir = malloc(sizeof(t_redir));
    if (!new_redir)
    {
        printf("DEBUG-REDIR: Échec d'allocation pour la redirection\n");
        return (0);
    }
    
    // Initialiser la structure
    new_redir->type = redir_type;
    new_redir->file = cy_true_strdup(file_node->input);
    new_redir->next = NULL;
    
    if (!new_redir->file)
    {
        printf("DEBUG-REDIR: Échec d'allocation pour le nom de fichier\n");
        free(new_redir);
        return (0);
    }
    
    // Ajouter la redirection à la commande
    if (!(*current_cmd)->redirs)
    {
        // Première redirection pour cette commande
        (*current_cmd)->redirs = new_redir;
        printf("DEBUG-REDIR: Première redirection ajoutée\n");
    }
    else
    {
        // Ajouter à la fin de la liste
        t_redir *last = (*current_cmd)->redirs;
        while (last->next)
            last = last->next;
        last->next = new_redir;
        printf("DEBUG-REDIR: Redirection ajoutée à la fin de la liste\n");
    }
    
    // Avancer au-delà du fichier
    *current_input = file_node->next;
    nodes_skipped++; // +1 pour le fichier
    
    // Mettre à jour le type de délimiteur si nécessaire
    if (*current_input && (*current_input)->input && (*current_input)->input[0] == '|')
        *nature = 2;  // Pipe
    else if (!*current_input)
        *nature = 3;  // Fin de commande
    
    printf("DEBUG-REDIR: Retour de %d nœuds traités, nature=%d\n", 
           nodes_skipped, *nature);
    
    return (nodes_skipped);
}
