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
 */
static int	is_pipe_command(t_cmd *cmd)
{
	if (!cmd || !cmd->args || !cmd->args[0])
		return (0);
	return (ft_strcmp(cmd->args[0], "|") == 0);
}

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

t_cmd *fix_pipeline_structure(t_cmd *cmds)
{
    t_cmd *current;
    t_cmd *temp;
    t_cmd *head = cmds;  // Conserver la tête initiale

    printf("DEBUG: Début de fix_pipeline_structure avec head=%p\n", head);
    if (!cmds)
        return NULL;
    
    // Étape 1: Vérifier si la première commande est un pipe et la supprimer si nécessaire
    while (head && is_pipe_command(head))
    {
        printf("DEBUG: Suppression d'une commande pipe en tête de liste\n");
        temp = head;
        head = head->next;
        free_pipe_command(temp);
    }
    
    if (!head)
    {
        printf("DEBUG: Après suppression, aucune commande ne reste\n");
        return NULL;
    }
        
    // Étape 2: Supprimer les commandes pipe internes
    current = head;
    while (current && current->next)
    {
        if (is_pipe_command(current->next))
        {
            printf("DEBUG: Suppression d'une commande pipe interne\n");
            temp = current->next;
            current->next = temp->next;
            free_pipe_command(temp);
        }
        else
        {
            current = current->next;
        }
    }
    
    // Étape 3: Ajuster les types de commande (builtin_id)
    current = head;
    while (current)
    {
        if (current->args && current->args[0])
        {
            // Réinitialiser builtin_id pour s'assurer qu'il est correctement attribué
            if (ft_strcmp(current->args[0], "echo") == 0)
            {
                if (current->args[1] && ft_strcmp(current->args[1], "-n") == 0)
                    current->builtin_id = 1;
                else
                    current->builtin_id = 2;
                printf("DEBUG: Commande '%s' identifiée comme builtin (id=%d)\n", 
                       current->args[0], current->builtin_id);
            }
            else if (ft_strcmp(current->args[0], "cd") == 0)
            {
                current->builtin_id = 3;
                printf("DEBUG: Commande '%s' identifiée comme builtin (id=%d)\n", 
                       current->args[0], current->builtin_id);
            }
            else if (ft_strcmp(current->args[0], "pwd") == 0)
            {
                current->builtin_id = 4;
                printf("DEBUG: Commande '%s' identifiée comme builtin (id=%d)\n", 
                       current->args[0], current->builtin_id);
            }
            else if (ft_strcmp(current->args[0], "export") == 0)
            {
                current->builtin_id = 5;
                printf("DEBUG: Commande '%s' identifiée comme builtin (id=%d)\n", 
                       current->args[0], current->builtin_id);
            }
            else if (ft_strcmp(current->args[0], "unset") == 0)
            {
                current->builtin_id = 6;
                printf("DEBUG: Commande '%s' identifiée comme builtin (id=%d)\n", 
                       current->args[0], current->builtin_id);
            }
            else if (ft_strcmp(current->args[0], "env") == 0)
            {
                current->builtin_id = 7;
                printf("DEBUG: Commande '%s' identifiée comme builtin (id=%d)\n", 
                       current->args[0], current->builtin_id);
            }
            else if (ft_strcmp(current->args[0], "exit") == 0)
            {
                current->builtin_id = 8;
                printf("DEBUG: Commande '%s' identifiée comme builtin (id=%d)\n", 
                       current->args[0], current->builtin_id);
            }
            else
            {
                current->builtin_id = -1; // Commande externe
                printf("DEBUG: Commande '%s' identifiée comme externe\n", 
                       current->args[0]);
            }
        }
        current = current->next;
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

/**
 * @brief Détermine si une commande est une cible de redirection
 *
 * @param cmd La commande à vérifier
 * @return int 1 si c'est une cible de redirection, 0 sinon
 */
static int is_redirection_target(t_cmd *cmd)
{
    // Une commande est une cible de redirection si:
    // 1. Elle a des redirections
    // 2. Son premier argument est "|" ou n'est pas une commande builtin connue
    if (!cmd || !cmd->args || !cmd->args[0])
        return (0);
    
    // Vérifier si c'est un token pipe
    if (ft_strcmp(cmd->args[0], "|") == 0)
        return (1);
    
    // Vérifier si ce n'est pas une builtin connue
    if (ft_strcmp(cmd->args[0], "echo") != 0 &&
        ft_strcmp(cmd->args[0], "cd") != 0 &&
        ft_strcmp(cmd->args[0], "pwd") != 0 &&
        ft_strcmp(cmd->args[0], "export") != 0 &&
        ft_strcmp(cmd->args[0], "unset") != 0 &&
        ft_strcmp(cmd->args[0], "env") != 0 &&
        ft_strcmp(cmd->args[0], "exit") != 0)
    {
        // Si ce n'est pas une commande connue et qu'elle a des redirections
        if (cmd->redirs)
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
    
    // AJOUT DE DÉBOGAGE ICI
    printf("DEBUG: Avant cy2_convert_cmd\n");
    
    // VÉRIFICATION QUE head EST VALIDE
    if (!head) {
        printf("ERROR: head est NULL avant cy2_convert_cmd\n");
        if (status)
            *status = 1;
        return (NULL);
    }
    
    // AJOUT D'UN POINT D'ARRÊT POUR LE DÉBOGAGE
    t_input *debug_current = head;
    while (debug_current) {
        printf("DEBUG: Nœud avec input '%s', type %d\n", 
               debug_current->input ? debug_current->input : "(null)", 
               debug_current->type);
        debug_current = debug_current->next;
    }
    printf("DEBUG: Tentative d'appel à cy2_convert_cmd\n");
    // ASSUREZ-VOUS D'UTILISER LA BONNE SIGNATURE
    t_cmd *cmds = cy2_convert_cmd(head);  // ou &head selon la signature correcte
    printf("DEBUG: Après cy2_convert_cmd, cmds=%p\n", cmds);
    
    // AJOUT DE DÉBOGAGE POUR CMDS
    if (!cmds) {
        printf("WARNING: cy2_convert_cmd a retourné NULL\n");
    } else {
        printf("DEBUG: Commande créée avec succès\n");
        
        // Appliquer les corrections de pipeline - ATTENTION AU RETOUR!
        printf("DEBUG: Application des corrections de pipeline\n");
        t_cmd *fixed_cmds = fix_pipeline_structure(cmds);
        
        // Mettre à jour cmds avec la nouvelle structure
        cmds = fixed_cmds;
        
        // Fusionner les commandes de redirection si nécessaire
        if (cmds) {
            printf("DEBUG: Fusion des commandes de redirection\n");
            merge_redirection_commands(cmds);
        }
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
