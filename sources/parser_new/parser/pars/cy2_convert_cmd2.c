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
    
    // Si find_delim renvoie une erreur
    if (c->n_delimiter == -1)
    {
        cy0_free_cmd_list(c->head_cmd);
        return (0);
    }
    
    // Traiter les redirections
    if (c->nature_delimiter == 1 && c->current_input && 
        c->current_input->input)
    {
        // Vérification sécurisée du symbole de redirection
        int is_redir = c->current_input->input[0] == '<' || 
                      c->current_input->input[0] == '>';
                      
        if (is_redir)
        {
            printf("DEBUG: Traitement de redirection pour '%s'\n", 
                   c->current_input->input);
            
            // Stocker le pointeur actuel pour plus de sécurité
            t_input *redir_node = c->current_input;
            
            // Utiliser cy2_fill_redir pour traiter la redirection
            int skip = cy2_fill_redir(&c->current_cmd, &c->current_input, 
                                     &c->nature_delimiter);
                                     
            if (skip == 0)
            {
                printf("DEBUG: Échec du traitement de redirection\n");
                if (!c->current_input) {
                    // Réinitialiser pour éviter NULL
                    c->current_input = redir_node;
                }
                
                // Continuer malgré l'échec
                if (redir_node && redir_node->next)
                    c->current_input = redir_node->next;
            }
            
            return (-1);  // Continuer le traitement
        }
    }
    
    // Traiter les pipes
    if (c->nature_delimiter == 2 && c->current_input && 
        c->current_input->input && c->current_input->input[0] == '|')
    {
        printf("DEBUG: Pipe détecté: '%s'\n", c->current_input->input);
        
        // Ne pas considérer le pipe comme une commande
        c->current_input = c->current_input->next;
        
        return (-1);  // Continuer le traitement
    }
    
    // Traitement normal des commandes
    if (c->n_delimiter > 0)
    {
        if (append_cmd(&c->current_cmd, c->n_delimiter, &c->head_input))
        {
            cy0_free_cmd_list(c->head_cmd);
            return (0);
        }
    }
    
    return (-1);
}

int cy2_convert_cmd1a(t_cmdconvert *c)
{
    int ret;
    
    // Compte les tentatives pour éviter les boucles infinies
    int attempts = 0;
    int max_attempts = 50;  // Limite raisonnable

    while (attempts < max_attempts)
    {
        attempts = attempts + 1;
        
        ret = cy2_convert_cmd1b(c);
        
        // Si la fonction retourne une erreur, propager l'erreur
        if (ret != -1)
        {
            if (ret == 0)
            {
                printf("DEBUG: cy2_convert_cmd1b a signalé une erreur\n");
                return (0);  // Erreur
            }
            printf("DEBUG: cy2_convert_cmd1b a terminé normalement\n");
            return (1);  // Succès
        }
        
        // Traitement du pipe
        if (c->nature_delimiter == 2)
        {
            printf("DEBUG: Traitement du pipe, nature_delimiter = 2\n");
            
            // Créer une nouvelle commande après le pipe
            if (c->current_cmd->next == NULL)
            {
                t_cmd *new_cmd = init_cmd();
                if (!new_cmd)
                {
                    printf("DEBUG: Échec d'allocation de la nouvelle commande après pipe\n");
                    return (0);
                }
                
                c->current_cmd->next = new_cmd;
                printf("DEBUG: Nouvelle commande créée après pipe, current_cmd->next = %p\n", 
                       c->current_cmd->next);
            }
            
            // Avancer à la commande suivante
            printf("DEBUG: Avancement à la commande suivante après pipe\n");
            c->current_cmd = c->current_cmd->next;
            
            // Si current_input pointe vers le pipe, avancer au-delà
            if (c->current_input && c->current_input->input)
            {
                if (c->current_input->input[0] == '|')
                {
                    printf("DEBUG: Avancer au-delà du pipe: '%s'\n", c->current_input->input);
                    c->current_input = c->current_input->next;
                }
            }
        }
        
        // Si fin de commande, sortir de la boucle
        if (c->nature_delimiter == 3)
        {
            printf("DEBUG: Fin de la commande détectée (nature_delimiter = 3)\n");
            break;
        }
    }
    
    if (attempts >= max_attempts)
    {
        printf("DEBUG: Nombre maximal de tentatives atteint, possible boucle infinie\n");
        return (0);  // Erreur, boucle infinie probable
    }
    
    return (1);  // Succès
}

t_cmd *cy2_convert_cmd(t_input *head_input)
{
    t_cmdconvert c;

    printf("DEBUG: cy2_convert_cmd début avec head_input=%p\n", head_input);
    
    // Initialisation critique: head_cmd doit être créé mais pas ajouté à la liste encore
    c.head_cmd = init_cmd();  // Crée un noeud vide pour commencer
    if (!c.head_cmd)
    {
        printf("Failed alloc for head_cmd\n");
        return (NULL);
    }
    printf("DEBUG: cy2_convert_cmd initialisé avec head_cmd=%p\n", c.head_cmd);
    
    c.current_cmd = c.head_cmd;  // Commencer à cette position
    printf("DEBUG: cy2_convert_cmd initialisé avec current_cmd=%p\n", c.current_cmd);
    c.head_input = head_input;
    printf("DEBUG: cy2_convert_cmd initialisé avec head_input=%p\n", c.head_input);
    c.current_input = head_input;
    printf("DEBUG: cy2_convert_cmd initialisé avec current_input=%p\n", c.current_input);
    c.n_delimiter = 0;
    printf("DEBUG: cy2_convert_cmd initialisé avec n_delimiter=%d\n", c.n_delimiter);
    c.nature_delimiter = 0;
    printf("DEBUG: cy2_convert_cmd initialisé avec nature_delimiter=%d\n", c.nature_delimiter);
    
    // *** IMPORTANT: Stocker le pointeur head_cmd original ***
    t_cmd *original_head = c.head_cmd;
    
    if (!cy2_convert_cmd1a(&c))
    {
        printf("DEBUG: cy2_convert_cmd1a a échoué\n");
        return (NULL);
    }
    
    // Si head_cmd est différent de original_head, utilisez cy2_free_first_cmd_node
    // sinon, conservez le nœud vide initial
    if (c.head_cmd == original_head)
    {
        printf("DEBUG: Le premier nœud est vide, on le supprime\n");
        cy2_free_first_cmd_node(&c.head_cmd);
    }
    else
    {
        printf("DEBUG: Structure de commande créée avec succès\n");
    }
    
    printf("DEBUG: cy2_convert_cmd fin avec head_cmd=%p\n", c.head_cmd);
    return (c.head_cmd);
}
// nature_delimiter : numero ou NULL ou > ou |;
// 0 = Problem 1 = Redir , 2 = Pipe , 3 = NULL;
// nature 1 : redir , nature 2 : pipe , nature 3 : NULL