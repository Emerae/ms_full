#include "parser_new.h"

t_cmd	*init_cmd(void)
{
	t_cmd	*cmd;

	cmd = malloc(sizeof(t_cmd));
	if (!cmd)
		return (NULL);
	cmd->args = NULL;
	cmd->redirs = NULL;
	cmd->builtin_id = 0;
	cmd->next = NULL;
	return (cmd);
}

int	cy_add_empty_cmd_node(t_cmd *head_cmd)
{
	t_cmd	*new_node;
	t_cmd	*current;

	new_node = malloc(sizeof(t_cmd));
	if (!new_node)
		return (1);
	new_node->args = NULL;
	new_node->redirs = NULL;
	new_node->builtin_id = -1;
	new_node->next = NULL;
	current = head_cmd;
	while (current->next)
		current = current->next;
	current->next = new_node;
	return (0);
}

int cy2_convert_cmd1b(t_cmdconvert *c)
{
    // Obtenir le nombre de délimiteurs et le type
    c->n_delimiter = find_delim(&c->current_input, &c->nature_delimiter);
    
    printf("DEBUG: find_delim a trouvé %d arguments (nature=%d)\n", 
           c->n_delimiter, c->nature_delimiter);
    
    // Si find_delim renvoie une erreur
    if (c->n_delimiter == -1)
    {
        cy0_free_cmd_list(c->head_cmd);
        return (0);
    }
    
    // IMPORTANT: Créer d'abord la commande avec ses arguments
    // AVANT de traiter les redirections
    if (c->n_delimiter > 0)
    {
        if (append_cmd(&c->current_cmd, c->n_delimiter, &c->head_input))
        {
            cy0_free_cmd_list(c->head_cmd);
            return (0);
        }
        printf("DEBUG: Commande créée avec %d arguments\n", c->n_delimiter);
    }
    
    // Ensuite seulement, traiter les redirections
    if (c->nature_delimiter == 1 && c->current_input && 
        c->current_input->input)
    {
        int is_redir = c->current_input->input[0] == '<' || 
                      c->current_input->input[0] == '>';
                      
        if (is_redir)
        {
            printf("DEBUG: Traitement de redirection pour '%s'\n", 
                   c->current_input->input);
            
            t_input *redir_node = c->current_input;
            int skip = cy2_fill_redir(&c->current_cmd, &c->current_input, 
                                     &c->nature_delimiter);
                                     
            if (skip == 0)
            {
                printf("DEBUG: Échec du traitement de redirection\n");
                if (!c->current_input) {
                    c->current_input = redir_node;
                }
                
                if (redir_node && redir_node->next)
                    c->current_input = redir_node->next;
            }
        }
    }
    
    // Traiter les pipes
    if (c->nature_delimiter == 2 && c->current_input && 
        c->current_input->input && c->current_input->input[0] == '|')
    {
        printf("DEBUG: Pipe détecté: '%s'\n", c->current_input->input);
        c->current_input = c->current_input->next;
    }
    
    return (-1);
}

int cy2_convert_cmd1a(t_cmdconvert *c)
{
    int ret;

    while (1)
    {
        ret = cy2_convert_cmd1b(c);
        if (ret != -1)
            return (ret);
        cy2_fill_builtin_id(&c->current_cmd);
        if (!cy2_convert_cmd2(c))
            return (0);
        if (c->nature_delimiter == 3)
            break;
        if (c->nature_delimiter == 2) // Cas du pipe
        {
            // Ignorons explicitement le token pipe
            c->current_input = c->current_input->next;
            
            // Créer directement la prochaine commande sans nœud vide
            c->current_cmd->next = init_cmd();
            if (!c->current_cmd->next)
            {
                cy0_free_cmd_list(c->head_cmd);
                return (0);
            }
            
            // Passer à la commande suivante
            c->current_cmd = c->current_cmd->next;
            
            // Afficher un message de débogage
            printf("DEBUG: Création d'une nouvelle commande après pipe\n");
        }
    }
    return (1);
}

t_cmd	*cy2_convert_cmd(t_input *head_input)
{
	t_cmdconvert	c;

	printf("DEBUG: cy2_convert_cmd début avec head_input=%p\n", head_input);
	c.head_cmd = init_cmd();
	if (!c.head_cmd)
	{
		printf("Failed alloc for head_cmd\n");
		return (NULL);
	}
	printf("DEBUG: cy2_convert_cmd initialisé avec head_cmd=%p\n", c.head_cmd);
	c.current_cmd = c.head_cmd;
	printf("DEBUG: cy2_convert_cmd initialisé avec current_cmd=%p\n", c.current_cmd);
	c.head_input = head_input;
	printf("DEBUG: cy2_convert_cmd initialisé avec head_input=%p\n", c.head_input);
	c.current_input = head_input;
	printf("DEBUG: cy2_convert_cmd initialisé avec current_input=%p\n", c.current_input);
	c.n_delimiter = 0;
	printf("DEBUG: cy2_convert_cmd initialisé avec n_delimiter=%d\n", c.n_delimiter);
	c.nature_delimiter = 0;
	printf("DEBUG: cy2_convert_cmd initialisé avec nature_delimiter=%d\n", c.nature_delimiter);
	if (!cy2_convert_cmd1a(&c))
		return (NULL);
    printf("DEBUG: Avant cy2_free_first_cmd_node, head_cmd=%p avec redirs=%p\n", 
       c.head_cmd, c.head_cmd->redirs);
    if (c.head_cmd->redirs) {
        printf("DEBUG:   Premier fichier redir='%s', type=%d\n", 
            c.head_cmd->redirs->file, c.head_cmd->redirs->type);
    }
	cy2_free_first_cmd_node(&c.head_cmd);
	printf("DEBUG: cy2_convert_cmd fin avec head_cmd=%p\n", c.head_cmd);
	return (c.head_cmd);
}
// nature_delimiter : numero ou NULL ou > ou |;
// 0 = Problem 1 = Redir , 2 = Pipe , 3 = NULL;
// nature 1 : redir , nature 2 : pipe , nature 3 : NULL