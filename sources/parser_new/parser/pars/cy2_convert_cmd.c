#include "parser_new.h"
#include "libftfull.h"
#include <stdio.h>

int	find_delimiter2(char *input, int *nature, int type)
{
	if (cy_strcmp(input, "<") == 0
		|| cy_strcmp(input, ">") == 0
		|| cy_strcmp(input, ">>") == 0
		|| cy_strcmp(input, "<<") == 0)
	{
		if (type == 2)
		{
			*nature = 1;
			return (1);
		}
	}
	else if (cy_strcmp(input, "|") == 0 && type == 2)
	{
		*nature = 2;
		return (1);
	}
	return (0);
}

// Suppression du paramètre non utilisé 'ret' et de la variable non utilisée 'start'
int find_delimiter1(t_input **current_input, t_input *node, int *nature)
{
    if (!node)
    {
        printf("DEBUG-FIND: find_delimiter1 node is NULL\n");
        *nature = 3;  // Fin de commande
        *current_input = NULL;
        return (0);
    }
    
    // Log pour déboguer
    char *input_text = "(null)";
    if (node->input)
    {
        input_text = node->input;
    }
    printf("DEBUG-FIND: find_delimiter1 processing node '%s' with type %d\n", 
           input_text, node->type);
           
    // Si le premier nœud est un espace, avancer jusqu'à un non-espace
    while (node && node->type == 1)
    {
        printf("DEBUG-FIND: Skipping space node\n");
        node = node->next;
    }
    
    // Si arrivé à la fin après avoir sauté les espaces
    if (!node)
    {
        printf("DEBUG-FIND: End of input after skipping spaces\n");
        *nature = 3;  // Fin de commande
        *current_input = NULL;
        return (0);
    }
    
    // Si on a trouvé un pipe ou une redirection directement
    if (node->input)
    {
        if (node->input[0] == '|')
        {
            printf("DEBUG-FIND: Found pipe '%s'\n", node->input);
            *nature = 2;  // Pipe
            *current_input = node;
            return (0);
        }
        else if (node->input[0] == '<' || node->input[0] == '>')
        {
            printf("DEBUG-FIND: Found redirection '%s'\n", node->input);
            *nature = 1;  // Redirection
            *current_input = node;
            return (1);  // Retourner 1 pour créer la commande avant
        }
    }
    
    // Traiter les arguments normaux
    int i = 0;
    
    while (node)
    {
        // Ignorer les espaces
        if (node->type == 1)
        {
            node = node->next;
            continue;
        }
        
        if (!node->input)
        {
            printf("DEBUG-FIND: NULL input in node\n");
            break;
        }
        
        // Si on trouve un pipe ou une redirection, arrêter
        if (node->input[0] == '|' || node->input[0] == '<' || node->input[0] == '>')
        {
            printf("DEBUG-FIND: Found delimiter '%s'\n", node->input);
            if (node->input[0] == '|')
            {
                *nature = 2;  // Pipe
            }
            else
            {
                *nature = 1;  // Redirection
            }
            
            *current_input = node;
            return (i);  // Retourner le nombre d'arguments trouvés
        }
        
        // Si c'est le dernier nœud
        if (!node->next)
        {
            printf("DEBUG-FIND: Last node reached\n");
            *nature = 3;  // Fin de commande
            *current_input = node;
            return (i + 1);  // +1 pour inclure ce dernier nœud
        }
        
        i = i + 1;  // Un argument de plus
        node = node->next;
    }
    
    // On ne devrait pas arriver ici normalement
    printf("DEBUG-FIND: End of function reached unexpectedly\n");
    *current_input = node;
    return (i);
}

int find_delim(t_input **current_input, int *nature)
{
    t_input *node;

    node = *current_input;
    if (!current_input || !*current_input || !nature)
    {
        printf("Error: current_input is NULL\n");
        return (-1);
    }
    
    // Afficher clairement où nous en sommes
    printf("DEBUG-FIND: Starting find_delimiter with input '%s'\n", 
           node && node->input ? node->input : "(null)");
    
    // Appeler la fonction principale - SUPPRESSION DE L'ARGUMENT 'ret'
    int result = find_delimiter1(current_input, node, nature);
    
    // Afficher le résultat
    printf("DEBUG-FIND: find_delimiter returning %d with nature %d, current_input now '%s'\n", 
           result, 
           *nature, 
           *current_input && (*current_input)->input ? (*current_input)->input : "(null)");
    
    return result;
}
