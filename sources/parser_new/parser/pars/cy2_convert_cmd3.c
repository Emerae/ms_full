#include "parser_new.h"

void append_cmd3(t_cmd *new_cmd, t_cmd **current_cmd)
{
    t_cmd *last;
    char *arg0_text = "(null)";

    // Vérifier les pointeurs
    if (!new_cmd || !current_cmd)
    {
        printf("ERROR: Pointeurs NULL dans append_cmd3\n");
        return;
    }
    
    // Vérifier que new_cmd a au moins un argument
    if (!new_cmd->args || !new_cmd->args[0])
    {
        printf("WARNING: Tentative d'ajouter une commande vide dans append_cmd3\n");
        // Ne pas ajouter de commande vide
        free(new_cmd->args);
        free(new_cmd);
        return;
    }
    
    // Définir le texte pour l'affichage
    if (new_cmd->args[0])
    {
        arg0_text = new_cmd->args[0];
    }

    if (!*current_cmd)
    {
        printf("DEBUG: Premier nœud de commande (head) initialisé avec '%s'\n", arg0_text);
        *current_cmd = new_cmd;
    }
    else
    {
        // Trouver le dernier nœud
        last = *current_cmd;
        while (last->next)
        {
            last = last->next;
        }
            
        printf("DEBUG: Ajout de la commande '%s' à la fin de la liste\n", arg0_text);
        last->next = new_cmd;
    }
    
    printf("append_cmd3: new_cmd->args[0] = %s\n", arg0_text);
}

int append_cmd2(t_cmd *new_cmd, int n_delimiter, t_input **input_node)
{
    int i;

    i = 0;
    while (i < n_delimiter && *input_node)
    {
        // Ignorer les nœuds d'espace
        if ((*input_node)->type == 1) {
            *input_node = (*input_node)->next;
            continue;
        }
        
        new_cmd->args[i] = cy_true_strdup((*input_node)->input);
        if (!new_cmd->args[i])
            return (1);
        *input_node = (*input_node)->next;
        i = i + 1;
    }
    new_cmd->args[i] = NULL;
    new_cmd->redirs = NULL;
    new_cmd->builtin_id = -1;
    new_cmd->next = NULL;
    return (0);
}

int	append_cmd1(t_cmd **new_cmd, int n_delimiter)
{
	*new_cmd = malloc(sizeof(t_cmd));
	if (!*new_cmd)
		return (1);
	(*new_cmd)->args = malloc(sizeof(char *) * (n_delimiter + 1));
	if (!(*new_cmd)->args)
	{
		free(*new_cmd);
		return (1);
	}
	return (0);
}

int append_cmd(t_cmd **current_cmd, int n_delimiter, t_input **head_input)
{
    t_cmd   *new_cmd;
    t_input *input_node;

    input_node = *head_input;
    if (append_cmd1(&new_cmd, n_delimiter))
        return (1);
    if (append_cmd2(new_cmd, n_delimiter, &input_node))
    {
        free(new_cmd->args);
        free(new_cmd);
        return (1);
    }
    
    // Filtrer les redirections des arguments
    filter_cmd_redirections(new_cmd);
    
    append_cmd3(new_cmd, current_cmd);
    *head_input = input_node;
    return (0);
}

int cy2_convert_cmd2(t_cmdconvert *c)
{
    if (c->nature_delimiter != 1)
        return (1);
    
    printf("DEBUG-CONV: Traitement de redirection pour input '%s'\n", 
           c->current_input ? c->current_input->input : "(null)");
    
    // Tester si c'est bien une redirection
    if (!c->current_input || !c->current_input->input || 
        redir_type_from_str(c->current_input->input) < 0)
    {
        printf("DEBUG-CONV: Ce n'est pas une redirection\n");
        return (1);
    }
    
    // Traiter la redirection
    c->skip_nb = cy2_fill_redir(&c->current_cmd, &c->current_input, &c->nature_delimiter);
    if (c->skip_nb == 0)
    {
        printf("DEBUG-CONV: Échec du traitement de la redirection\n");
        cy0_free_cmd_list(c->head_cmd);
        return (0);
    }
    
    // Déboguer l'état de la commande
    printf("DEBUG-CONV: Après traitement, commande a %d redirections\n", 
           c->current_cmd && c->current_cmd->redirs ? 1 : 0);
    
    // La fonction cy2_fill_redir a déjà avancé current_input
    // Mettre à jour head_input en fonction de skip_nb
    for (int i = 0; i < c->skip_nb && c->head_input; i++)
        c->head_input = c->head_input->next;
    
    return (1);
}
