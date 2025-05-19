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
    if (cy3_substi_check(&head, env_array)) {
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
    
    // ASSUREZ-VOUS D'UTILISER LA BONNE SIGNATURE
    t_cmd *cmds = cy2_convert_cmd(head);  // ou &head selon la signature correcte
    
    printf("DEBUG: Après cy2_convert_cmd\n");
    
    // AJOUT DE DÉBOGAGE POUR CMDS
    if (!cmds) {
        printf("WARNING: cy2_convert_cmd a retourné NULL\n");
    } else {
        printf("DEBUG: Commande créée avec succès\n");
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
