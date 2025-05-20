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
    // Vérifier les arguments
    if (!current_cmd || !*current_cmd || !current_input || !*current_input)
    {
        printf("DEBUG-REDIR: Arguments invalides\n");
        return (0);
    }
    
    if (!(*current_cmd)->args)
    {
        (*current_cmd)->args = malloc(sizeof(char *) * 2);
        if (!(*current_cmd)->args)
            return (0);
        (*current_cmd)->args[0] = ft_strdup("_redir_placeholder_");
        (*current_cmd)->args[1] = NULL;
    }
    
    t_input *redir_node = *current_input;
    
    // Vérifier si c'est bien une redirection
    int redir_type = redir_type_from_str(redir_node->input);
    if (redir_type < 0)
    {
        printf("DEBUG-REDIR: '%s' n'est pas une redirection\n", 
               redir_node->input);
        return (0);
    }
    
    // Trouver le fichier associé (sauter les espaces)
    t_input *file_node = redir_node->next;
    while (file_node && file_node->type == 1)
        file_node = file_node->next;
    
    // Vérifier que le fichier existe
    if (!file_node || !file_node->input)
    {
        printf("DEBUG-REDIR: Pas de fichier après redirection\n");
        return (0);
    }
    
    // Créer la structure de redirection
    t_redir *new_redir = malloc(sizeof(t_redir));
    if (!new_redir)
        return (0);
    
    new_redir->type = redir_type;
    new_redir->file = cy_true_strdup(file_node->input);
    new_redir->next = NULL;
    
    if (!new_redir->file)
    {
        free(new_redir);
        return (0);
    }
    
    printf("DEBUG-REDIR: Redirection type %d vers '%s' créée\n",
           redir_type, new_redir->file);
    
    // Ajouter à la liste des redirections
    if (!(*current_cmd)->redirs)
    {
        (*current_cmd)->redirs = new_redir;
    }
    else
    {
        t_redir *last = (*current_cmd)->redirs;
        while (last->next)
            last = last->next;
        last->next = new_redir;
    }
    
    // Avancer au-delà du fichier
    *current_input = file_node->next;
    
    // Mettre à jour le type de délimiteur
    if (*current_input)
    {
        if ((*current_input)->input && (*current_input)->input[0] == '|')
            *nature = 2;  // Pipe
        else
            *nature = 0;  // Autre
    }
    else
    {
        *nature = 3;  // Fin
    }
    
    return (1);
}
