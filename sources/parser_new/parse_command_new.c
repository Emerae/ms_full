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

/*
 * Répare les structures de pipeline incorrectes
 * Cette fonction doit être appelée après le parsing et avant l'exécution
 */
void fix_pipeline_structure(t_cmd *head)
{
    if (!head)
        return;
        
    t_cmd *current = head;
    
    while (current && current->next)
    {
        // Vérifier si la commande actuelle est suivie d'une "commande pipe"
        if (current->next->args && current->next->args[0] && 
            current->next->args[0][0] == '|' && current->next->args[0][1] == '\0')
        {
            printf("DEBUG: Réparation d'une structure de pipeline incorrecte\n");
            
            // Trouver la commande réelle après le pipe
            t_cmd *real_next_cmd = NULL;
            
            if (current->next->args[1])
            {
                // Le nom de la vraie commande est dans les arguments
                // Ex: args=["I", "grep", "hello"]
                
                // Créer une nouvelle commande avec les bons arguments
                real_next_cmd = malloc(sizeof(t_cmd));
                if (!real_next_cmd)
                    return;
                    
                // Calculer le nombre d'arguments (tous sauf le "|")
                int arg_count = 0;
                while (current->next->args[arg_count + 1])
                    arg_count++;
                    
                // Allouer le tableau d'arguments
                real_next_cmd->args = malloc(sizeof(char*) * (arg_count + 1));
                if (!real_next_cmd->args)
                {
                    free(real_next_cmd);
                    return;
                }
                
                // Copier les arguments (en décalant pour sauter le "|")
                for (int i = 0; i < arg_count; i++)
                {
                    real_next_cmd->args[i] = ft_strdup(current->next->args[i + 1]);
                    if (!real_next_cmd->args[i])
                    {
                        // Libérer ce qui a déjà été alloué
                        for (int j = 0; j < i; j++)
                            free(real_next_cmd->args[j]);
                        free(real_next_cmd->args);
                        free(real_next_cmd);
                        return;
                    }
                }
                real_next_cmd->args[arg_count] = NULL;
                
                // Copier les redirections éventuelles
                real_next_cmd->redirs = current->next->redirs;
                current->next->redirs = NULL; // Pour éviter la double libération
                
                // Initialiser le reste de la structure
                real_next_cmd->builtin_id = -1; // À déterminer plus tard
                real_next_cmd->next = current->next->next;
                
                // Libérer l'ancienne "commande pipe"
                t_cmd *to_free = current->next;
                current->next = real_next_cmd;
                
                // Libérer l'ancienne commande
                for (int i = 0; to_free->args[i]; i++)
                    free(to_free->args[i]);
                free(to_free->args);
                free(to_free);
            }
            else if (current->next->next)
            {
                // Le pipe et la commande ont été séparés en deux structures
                // Supprimer simplement le noeud de pipe intermédiaire
                t_cmd *to_free = current->next;
                current->next = current->next->next;
                
                // Libérer la "commande pipe"
                for (int i = 0; to_free->args[i]; i++)
                    free(to_free->args[i]);
                free(to_free->args);
                free(to_free);
            }
        }
        else
        {
            // Avancer au prochain noeud
            current = current->next;
        }
    }
    
    // Maintenant, vérifier chaque commande pour détecter les builtins
    current = head;
    while (current)
    {
        if (current->args && current->args[0])
        {
            // Vérifier si c'est une builtin
            if (ft_strcmp(current->args[0], "echo") == 0)
                current->builtin_id = (current->args[1] && ft_strcmp(current->args[1], "-n") == 0) ? 1 : 2;
            else if (ft_strcmp(current->args[0], "cd") == 0)
                current->builtin_id = 3;
            else if (ft_strcmp(current->args[0], "pwd") == 0)
                current->builtin_id = 4;
            else if (ft_strcmp(current->args[0], "export") == 0)
                current->builtin_id = 5;
            else if (ft_strcmp(current->args[0], "unset") == 0)
                current->builtin_id = 6;
            else if (ft_strcmp(current->args[0], "env") == 0)
                current->builtin_id = 7;
            else if (ft_strcmp(current->args[0], "exit") == 0)
                current->builtin_id = 8;
        }
        current = current->next;
    }
}

/*
 * Fusionne les commandes qui sont en réalité des cibles de redirection
 * Cette fonction doit être appelée après le parsing et avant l'exécution
 */
void merge_redirection_commands(t_cmd *head)
{
    if (!head)
        return;
        
    t_cmd *current = head;
    
    while (current && current->next)
    {
        // Vérifier si la commande suivante est en réalité une cible de redirection
        // (premiers signes: a une redirection et son nom n'est pas une commande réelle)
        if (current->next->args && current->next->args[0] && 
            current->next->redirs &&
            // Vérification sommaire - peut être améliorée
            access(current->next->args[0], X_OK) != 0)
        {
            printf("DEBUG: Fusion d'une commande de redirection: %s\n", 
                  current->next->args[0]);
            
            // Transférer les redirections à la commande actuelle
            if (!current->redirs)
            {
                current->redirs = current->next->redirs;
            }
            else
            {
                // Trouver la fin de la liste de redirections
                t_redir *last_redir = current->redirs;
                while (last_redir->next)
                    last_redir = last_redir->next;
                    
                // Attacher les redirections de la commande suivante
                last_redir->next = current->next->redirs;
            }
            
            // Éviter double free en désactivant les redirections transférées
            current->next->redirs = NULL;
            
            // Supprimer la commande suivante de la chaîne
            t_cmd *to_remove = current->next;
            current->next = current->next->next;
            
            // Libérer la mémoire de la commande supprimée
            if (to_remove->args)
            {
                for (int i = 0; to_remove->args[i]; i++)
                    free(to_remove->args[i]);
                free(to_remove->args);
            }
            free(to_remove);
            
            // Ne pas avancer current car on a peut-être une autre fusion à faire
        }
        else
        {
            // Passer à la commande suivante
            current = current->next;
        }
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
