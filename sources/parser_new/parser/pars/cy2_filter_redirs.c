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
    int i;
    int j;
    int filtered_count;
    char **filtered_args;

    if (!cmd || !cmd->args)
        return;

    // Compter combien d'arguments ne sont pas des redirections
    filtered_count = 0;
    i = 0;
    while (cmd->args[i])
    {
        if (!is_redirection_symbol(cmd->args[i]))
        {
            // Ne compter que si ce n'est pas un fichier suivant une redirection
            if (i == 0 || !is_redirection_symbol(cmd->args[i - 1]))
                filtered_count++;
        }
        i++;
    }

    // Créer un nouveau tableau d'arguments filtrés
    filtered_args = malloc(sizeof(char *) * (filtered_count + 1));
    if (!filtered_args)
        return;

    // Remplir le tableau filtré
    j = 0;
    i = 0;
    while (cmd->args[i])
    {
        if (!is_redirection_symbol(cmd->args[i]))
        {
            // N'ajouter que si ce n'est pas un fichier suivant une redirection
            if (i == 0 || !is_redirection_symbol(cmd->args[i - 1]))
            {
                filtered_args[j] = ft_strdup(cmd->args[i]);
                j++;
            }
        }
        i++;
    }
    filtered_args[j] = NULL;

    // Remplacer l'ancien tableau par le nouveau
    free_tab(cmd->args);
    cmd->args = filtered_args;
}
