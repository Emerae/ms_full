// Nouveau fichier: sources/parser_new/parser/pars/cy2_filter_redirs.c

#include "parser_new.h"
#include "libftfull.h"

/**
 * @brief Détermine si un argument est un symbole de redirection
 */
static int is_redirection_symbol(char *arg)
{
    if (!arg)
        return (0);
    return (ft_strcmp(arg, "<") == 0 || 
            ft_strcmp(arg, ">") == 0 || 
            ft_strcmp(arg, ">>") == 0 || 
            ft_strcmp(arg, "<<") == 0);
}

/**
 * @brief Filtre les symboles de redirection et leurs fichiers associés des arguments de commande
 */
void filter_cmd_redirections(t_cmd *cmd)
{
    if (!cmd || !cmd->args)
        return;
    
    // Compter combien d'arguments ne sont pas des redirections
    int filtered_count = 0;
    int i = 0;
    
    while (cmd->args[i])
    {
        // Vérifier si c'est un symbole de pipe - l'ignorer complètement
        if (ft_strcmp(cmd->args[i], "|") == 0)
        {
            i++;
            continue;
        }
        
        // Vérifier si c'est une redirection ou son fichier associé
        if (!is_redirection_symbol(cmd->args[i]))
        {
            // Ne pas compter comme argument si c'est un fichier associé à une redirection
            if (i == 0 || !is_redirection_symbol(cmd->args[i - 1]))
                filtered_count++;
        }
        i++;
    }
    
    // Créer un tableau filtré (code existant...)
}