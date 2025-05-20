
#include "minishell.h"
/**
 * @file echo.c
 * @brief Implementation of the echo built-in command
 * 
 * This file contains functions to implement the echo built-in command
 * for the minishell project. It supports the '-n' option to suppress
 * the trailing newline.
 */

/**
 * @brief Prints arguments with spaces between them
 * 
 * Prints all arguments from the array starting at index i, with a space
 * between each argument. If option is 0, appends a newline character
 * at the end of the output.
 * 
 * @param args Array of strings to print
 * @param option Flag to control newline output (1: no newline, 0: with newline)
 * @param i Starting index in args array
 */

/**
 * @brief Processes echo command options
 * 
 * Checks for the presence of the '-n' option in the echo command arguments.
 * Returns the index of the first non-option argument.
 * 
 * @param cmd Structure containing command information
 * @return Index of the first non-option argument in the args array
 */

/**
 * @brief Implements the echo built-in command
 * 
 * Executes the echo command with support for the '-n' option, which suppresses
 * the trailing newline. Creates a child process to handle the command execution
 * and properly redirects input/output as needed.
 * 
 * @param cmd Structure containing command information including arguments
 * @param envl Pointer to the environment variables list (unused in this function)
 * @return SUCCESS (0) if the command executed successfully, or an error code
 */

static int	is_option_n(char *s)
{
	if (!s || *s != '-')             /* NULL or doesn’t start with '-'       */
		return (0);
	++s;
	if (!*s)                         /* a lone '-' is not a valid option     */
		return (0);
	while (*s == 'n')
		++s;
	return (*s == '\0');             /* accept only pure n’s                 */
}

int builtin_echo(char **argv)
{
    ft_putstr_fd("\n*** DÉBUT BUILTIN ECHO ***\n", STDERR_FILENO);
    
    int i = 1;
    int newline = 1;
    
    // Vérifier l'état des descripteurs de fichiers
    ft_putstr_fd("État des descripteurs pour echo:\n", STDERR_FILENO);
    ft_putstr_fd("  STDOUT_FILENO (1): ", STDERR_FILENO);
    if (fcntl(STDOUT_FILENO, F_GETFL) == -1) {
        ft_putstr_fd("FERMÉ! Ceci est un problème critique!\n", STDERR_FILENO);
    } else {
        ft_putstr_fd("OUVERT\n", STDERR_FILENO);
    }
    
    // Vérifier si l'option -n est présente
    ft_putstr_fd("Vérification de l'option -n...\n", STDERR_FILENO);
    if (argv[1] && ft_strcmp(argv[1], "-n") == 0)
    {
        ft_putstr_fd("Option -n détectée\n", STDERR_FILENO);
        newline = 0;
        i = 2;
    }
    else {
        ft_putstr_fd("Option -n non détectée\n", STDERR_FILENO);
    }
    
    // Compter les arguments
    int arg_count = 0;
    while (argv[arg_count])
        arg_count++;
    
    ft_putstr_fd("Nombre total d'arguments: ", STDERR_FILENO);
    ft_putstr_fd(ft_itoa(arg_count), STDERR_FILENO);
    ft_putstr_fd("\n", STDERR_FILENO);
    
    // Afficher tous les arguments
    ft_putstr_fd("Arguments à afficher:\n", STDERR_FILENO);
    while (argv[i])
    {
        ft_putstr_fd("  argv[", STDERR_FILENO);
        ft_putstr_fd(ft_itoa(i), STDERR_FILENO);
        ft_putstr_fd("] = '", STDERR_FILENO);
        ft_putstr_fd(argv[i], STDERR_FILENO);
        ft_putstr_fd("'\n", STDERR_FILENO);
        i++;
    }
    i = 1;
    
    // Traitement de l'option -n
    if (argv[1] && ft_strcmp(argv[1], "-n") == 0)
        i = 2;
    
    // Afficher les arguments sur la sortie standard
    ft_putstr_fd("Écriture sur la sortie standard...\n", STDERR_FILENO);
    while (argv[i])
    {
        size_t len = ft_strlen(argv[i]);
        ft_putstr_fd("  Écriture de '", STDERR_FILENO);
        ft_putstr_fd(argv[i], STDERR_FILENO);
        ft_putstr_fd("' (longueur=", STDERR_FILENO);
        ft_putstr_fd(ft_itoa(len), STDERR_FILENO);
        ft_putstr_fd(")\n", STDERR_FILENO);
        
        ssize_t written = write(STDOUT_FILENO, argv[i], len);
        
        ft_putstr_fd("  Résultat de write(): ", STDERR_FILENO);
        ft_putstr_fd(ft_itoa(written), STDERR_FILENO);
        ft_putstr_fd("\n", STDERR_FILENO);
        
        if (written < 0) {
            ft_putstr_fd("  ERREUR D'ÉCRITURE: ", STDERR_FILENO);
            ft_putstr_fd(strerror(errno), STDERR_FILENO);
            ft_putstr_fd("\n", STDERR_FILENO);
        }
        
        if (argv[i + 1]) {
            ft_putstr_fd("  Écriture d'un espace\n", STDERR_FILENO);
            write(STDOUT_FILENO, " ", 1);
        }
        
        i++;
    }
    
    // Ajouter une nouvelle ligne si nécessaire
    if (newline) {
        ft_putstr_fd("Écriture du retour à la ligne\n", STDERR_FILENO);
        write(STDOUT_FILENO, "\n", 1);
    } else {
        ft_putstr_fd("Pas de retour à la ligne (option -n)\n", STDERR_FILENO);
    }
    
    // Vider le tampon de sortie
    ft_putstr_fd("Synchronisation de la sortie standard (fsync)...\n", STDERR_FILENO);
    if (fsync(STDOUT_FILENO) == -1) {
        ft_putstr_fd("  AVERTISSEMENT: fsync() a échoué: ", STDERR_FILENO);
        ft_putstr_fd(strerror(errno), STDERR_FILENO);
        ft_putstr_fd("\n", STDERR_FILENO);
    }
    
    ft_putstr_fd("*** FIN BUILTIN ECHO ***\n\n", STDERR_FILENO);
    return 0;
}
