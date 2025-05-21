#include "parser_new.h"
#include "libftfull.h"
#include "minishell.h"
#include <stdio.h>

/*
 * Simple wrapper: produce t_cmd* or NULL, set *status (0 or 258).
 * On syntax error prints message and returns NULL.
 */
static void    syntax_error(char *token)
{
    ft_printf_fd(2, "minishell: syntax error near unexpected token `%s'\n", token);
}

/* look for illegal tokens ';' and '\\' (backslash) outside quotes */
static int has_illegal_token(t_input *head, char **bad_tok)
{
    if (!head || !bad_tok) 
        return (0);

    while (head)
    {
        // Vérification défensive: s'assurer que tous les champs nécessaires sont valides
        if (!head->input || head->type < 1 || head->type > 4) {
            head = head->next;
            continue;  // Ignorer ce noeud s'il est mal formé
        }

        // Vérification des tokens interdits
        if (head->type == 2 && head->input[0] && 
            (head->input[0] == ';' || head->input[0] == '\\') && 
            head->input[1] == '\0')
        {
            *bad_tok = head->input;
            return (1);
        }
        head = head->next;
    }
    return (0);
}

/**
 * @brief Vérifie si une commande est en réalité un symbole de pipe
 *
 * @param cmd La commande à vérifier
 * @return int 1 si c'est un pipe, 0 sinon
 
static int	is_pipe_command(t_cmd *cmd)
{
	if (!cmd || !cmd->args || !cmd->args[0])
		return (0);
	return (ft_strcmp(cmd->args[0], "|") == 0);
}
**/

/**
 * @brief Libère la mémoire d'une commande pipe
 *
 * @param cmd La commande pipe à libérer
 */
static void free_pipe_command(t_cmd *cmd)
{
    if (!cmd)
        return;
    
    // Libérer les arguments
    if (cmd->args)
    {
        int i = 0;
        while (cmd->args[i])
        {
            free(cmd->args[i]);
            i++;
        }
        free(cmd->args);
    }
    
    // Libérer les redirections
    if (cmd->redirs)
    {
        t_redir *current = cmd->redirs;
        t_redir *next;
        
        while (current)
        {
            next = current->next;
            if (current->file)
                free(current->file);
            free(current);
            current = next;
        }
    }
    
    // Libérer la commande elle-même
    free(cmd);
}

static void update_builtin_id(t_cmd *cmd)
{
    if (!cmd || !cmd->args || !cmd->args[0])
        return;
        
    if (ft_strcmp(cmd->args[0], "echo") == 0)
    {
        if (cmd->args[1] && ft_strcmp(cmd->args[1], "-n") == 0)
            cmd->builtin_id = 1;
        else
            cmd->builtin_id = 2;
    }
    else if (ft_strcmp(cmd->args[0], "cd") == 0)
        cmd->builtin_id = 3;
    else if (ft_strcmp(cmd->args[0], "pwd") == 0)
        cmd->builtin_id = 4;
    else if (ft_strcmp(cmd->args[0], "export") == 0)
        cmd->builtin_id = 5;
    else if (ft_strcmp(cmd->args[0], "unset") == 0)
        cmd->builtin_id = 6;
    else if (ft_strcmp(cmd->args[0], "env") == 0)
        cmd->builtin_id = 7;
    else if (ft_strcmp(cmd->args[0], "exit") == 0)
        cmd->builtin_id = 8;
    else
        cmd->builtin_id = -1;

    printf("DEBUG: Commande '%s' identifiée avec builtin_id=%d\n", 
           cmd->args[0], cmd->builtin_id);
}

t_cmd *fix_pipeline_structure(t_cmd *cmds)
{
    t_cmd *current;
    t_cmd *temp;
    t_cmd *head = cmds;
    t_cmd *prev = NULL;

    printf("DEBUG: Début de fix_pipeline_structure avec head=%p\n", head);
    if (!cmds)
        return NULL;
    
    // Nettoyer toutes les commandes vides en début de liste
    while (head && (!head->args || !head->args[0] || 
           ft_strcmp(head->args[0], "|") == 0))
    {
        printf("DEBUG: Suppression d'une commande inutile en tête de liste\n");
        temp = head;
        head = head->next;
        free_pipe_command(temp);
    }
    
    if (!head)
    {
        printf("DEBUG: Après suppression, aucune commande ne reste\n");
        return NULL;
    }
    
    // Parcourir le reste de la liste et supprimer les commandes inutiles
    current = head;
    prev = head;
    
    while (current)
    {
        if (!current->args || !current->args[0] || 
            ft_strcmp(current->args[0], "|") == 0)
        {
            // Commande vide ou pipe à supprimer
            printf("DEBUG: Suppression d'une commande inutile interne\n");
            
            if (current == head)
            {
                head = current->next;
                prev = head;
                free_pipe_command(current);
                current = head;
            }
            else
            {
                prev->next = current->next;
                free_pipe_command(current);
                current = prev->next;
            }
        }
        else
        {
            // Mettre à jour les informations de builtin
            update_builtin_id(current);
            
            // Passer à la commande suivante
            prev = current;
            current = current->next;
        }
    }
    
    printf("DEBUG: Fin de fix_pipeline_structure avec head=%p\n", head);
    return head;
}


/**
 * @brief Transfère les redirections d'une commande à une autre
 *
 * @param dest Commande destination
 * @param src Commande source
 */
static void	transfer_redirections(t_cmd *dest, t_cmd *src)
{
	t_redir	*last_redir;

	if (!dest || !src || !src->redirs)
		return ;
	if (!dest->redirs)
	{
		dest->redirs = src->redirs;
		src->redirs = NULL;
		return ;
	}
	last_redir = dest->redirs;
	while (last_redir->next)
		last_redir = last_redir->next;
	last_redir->next = src->redirs;
	src->redirs = NULL;
}

/**
 * @brief Libère les arguments d'une commande
 *
 * @param cmd La commande dont les arguments doivent être libérés
 */
static void	free_cmd_args(t_cmd *cmd)
{
	int	i;

	if (!cmd || !cmd->args)
		return ;
	i = 0;
	while (cmd->args[i])
	{
		free(cmd->args[i]);
		i++;
	}
	free(cmd->args);
	cmd->args = NULL;
}

static int is_redirection_target(t_cmd *cmd)
{
    if (!cmd || !cmd->args || !cmd->args[0])
        return (0);
    
    // Cas évident: token pipe
    if (ft_strcmp(cmd->args[0], "|") == 0)
        return (1);
    
    // Cas spécial: placeholder de redirection
    if (ft_strcmp(cmd->args[0], "_redir_placeholder_") == 0)
        return (1);
    
    // Vérifier la validité comme commande
    if (cmd->redirs)
    {
        // Si builtin, ce n'est pas une cible de redirection
        if (cmd->builtin_id > 0 && cmd->builtin_id <= 8)
            return (0);
            
        // Si premier caractère est '/' ou './', c'est un chemin
        if (cmd->args[0][0] == '/' || 
            (cmd->args[0][0] == '.' && cmd->args[0][1] == '/'))
            return (access(cmd->args[0], F_OK) != 0);
            
        // Sinon, considérer comme cible de redirection uniquement 
        // si aucun exécutable avec ce nom n'existe
        return (1);
    }
    
    return (0);
}

/**
 * @brief Fusionne les commandes de redirection avec leurs commandes principales
 *
 * @param head La tête de la liste de commandes
 */
void	merge_redirection_commands(t_cmd *head)
{
	t_cmd	*current;
	t_cmd	*to_remove;

	if (!head)
		return ;
	current = head;
	while (current && current->next)
	{
		if (is_redirection_target(current->next))
		{
			ft_putstr_fd("DEBUG: Fusion d'une commande de redirection\n", 2);
			transfer_redirections(current, current->next);
			to_remove = current->next;
			current->next = current->next->next;
			free_cmd_args(to_remove);
			free(to_remove);
		}
		else
			current = current->next;
	}
}

int validate_pipeline(t_cmd *head)
{
    if (!head)
        return (0);
    
    t_cmd *current = head;
    int valid = 1;
    
    while (current)
    {
        // Vérifier que chaque commande a au moins un argument valide
        if (!current->args || !current->args[0] || 
            ft_strcmp(current->args[0], "|") == 0)
        {
            printf("DEBUG: ERREUR - Commande invalide détectée\n");
            valid = 0;
            break;
        }
        current = current->next;
    }
    
    return (valid);
}

void debug_print_pipeline(t_cmd *head)
{
    int cmd_count = 0;
    t_cmd *current = head;
    
    while (current)
    {
        cmd_count++;
        printf("  Commande %d: ", cmd_count);
        
        if (!current->args || !current->args[0])
            printf("(sans arguments) ");
        else
            printf("'%s' ", current->args[0]);
            
        printf("builtin_id=%d, ", current->builtin_id);
        
        // Afficher les redirections
        if (current->redirs)
        {
            printf("redirections: ");
            t_redir *r = current->redirs;
            while (r)
            {
                if (r->type == 0)
                    printf("< %s ", r->file);
                else if (r->type == 1)
                    printf("> %s ", r->file);
                else if (r->type == 2)
                    printf(">> %s ", r->file);
                else if (r->type == 3)
                    printf("<< %s ", r->file);
                r = r->next;
            }
        }
        else
            printf("sans redirections");
            
        printf("\n");
        current = current->next;
    }
    printf("DEBUG: Total: %d commandes\n", cmd_count);
}


t_cmd *parse_command_new(char *line, t_list *env, int *status)
{
    printf("DEBUG: Début de parse_command_new\n");
    t_input *head = cy1_make_list(line);
    if (!head) {
        printf("DEBUG: cy1_make_list a retourné NULL\n");
        return (NULL);
    }
    printf("DEBUG: cy1_make_list terminé\n");
    
    // Vérification des tokens illégaux
    char *bad = NULL;
    if (has_illegal_token(head, &bad))
    {
        syntax_error(bad);
        cy0_free_input_list(head);
        if (status)
            *status = 258;
        return (NULL);
    }
    
    printf("DEBUG: Avant cy3_substi_check\n");
    
    // Convertir l'environnement en format compatible
    char **env_array = create_env_tab(env, 0);
    if (!env_array) {
        cy0_free_input_list(head);
        if (status)
            *status = 1;
        return (NULL);
    }
    
    // Utiliser env_array au lieu de env
    if (cy3_substi_check(&head, env_array, env)) {
        printf("DEBUG: cy3_substi_check a échoué\n");
        free_tab(env_array);
        cy0_free_input_list(head);
        if (status)
            *status = 1;
        return (NULL);
    }
    
    free_tab(env_array);
    printf("DEBUG: cy3_substi_check terminé\n");
    
    if (cy4_1wrong_char(head))
    {
        cy0_free_input_list(head);
        if (status)
            *status = 258;
        return (NULL);
    }
    
    // VÉRIFICATION QUE head EST VALIDE
    if (!head) {
        printf("ERROR: head est NULL avant cy2_convert_cmd\n");
        if (status)
            *status = 1;
        return (NULL);
    }
    
    // Afficher les nœuds d'entrée pour débogage
    printf("DEBUG: Avant cy2_convert_cmd\n");
    t_input *debug_current = head;
    while (debug_current) {
        printf("DEBUG: Nœud avec input '%s', type %d\n", 
               debug_current->input ? debug_current->input : "(null)", 
               debug_current->type);
        debug_current = debug_current->next;
    }
    
    printf("DEBUG: Tentative d'appel à cy2_convert_cmd\n");
    t_cmd *cmds = cy2_convert_cmd(head);  
    printf("DEBUG: Après cy2_convert_cmd, cmds=%p\n", cmds);
    
    if (!cmds) {
        printf("WARNING: cy2_convert_cmd a retourné NULL\n");
        cy0_free_input_list(head);
        if (status)
            *status = 258;
        return (NULL);
    } 
    
    printf("DEBUG: Commande créée avec succès\n");
    
    // Appliquer les corrections de pipeline
    printf("DEBUG: Application des corrections de pipeline\n");
    t_cmd *fixed_cmds = fix_pipeline_structure(cmds);
    
    // Si la correction retourne NULL (pipeline invalide), gérer l'erreur
    if (!fixed_cmds && cmds) {
        printf("DEBUG: fix_pipeline_structure a retourné NULL pour un pipeline invalide\n");
        cy0_free_input_list(head);
        if (status)
            *status = 258;
        return (NULL);
    }
    
    // Mettre à jour cmds avec la structure corrigée
    cmds = fixed_cmds;
    
    // Fusionner les commandes de redirection si nécessaire
    if (cmds) {
        printf("DEBUG: Fusion des commandes de redirection\n");
        merge_redirection_commands(cmds);
        
        // Afficher la structure finale
        printf("DEBUG: Structure finale du pipeline:\n");
        debug_print_pipeline(cmds);
    }
    
    cy0_free_input_list(head);
    if (!cmds)
    {
        if (status)
            *status = 258;
    }
    else if (status)
        *status = 0;
    
    return (cmds);
}
