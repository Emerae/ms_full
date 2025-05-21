#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include "libftfull.h"
#include "structures.h"
#include "parser_new.h"
#include "minishell.h"


int builtin_unset(char **args, t_list **envl)
{
    int i;
    int status;

    i = 1;
    status = 0;
    while (args[i])
    {
        // Vérifier si le nom de variable est valide
        if (authorized_char(args[i]))
        {
            // Parcourir la liste des variables d'environnement
            t_list *prev = NULL;
            t_list *curr = *envl;
            
            while (curr)
            {
                t_env *env_var = (t_env *)curr->content;
                
                // Ne pas supprimer les variables spéciales comme ?begin
                if (ft_strcmp(env_var->var, args[i]) == 0 && 
                    ft_strcmp(env_var->var, "?exitcode") != 0)
                {
                    // Retirer le nœud de la liste
                    if (prev)
                        prev->next = curr->next;
                    else
                        *envl = curr->next;
                    
                    // Libérer la mémoire
                    free_entry(curr->content);
                    free(curr);
                    break;
                }
                
                prev = curr;
                curr = curr->next;
            }
        }
        else
        {
            // Afficher un message d'erreur pour les identifiants invalides
            ft_putstr_fd("minishell: unset: `", STDERR_FILENO);
            ft_putstr_fd(args[i], STDERR_FILENO);
            ft_putstr_fd("': not a valid identifier\n", STDERR_FILENO);
            status = 1;
        }
        i++;
    }
    
    return (status);
}

/* ------------ helpers builtin simple ------------- */
int builtin_echo(char **argv)
{
    int i = 1;
    int newline = 1;
    
    // Vérifier si l'option -n est présente
    if (argv[1] && ft_strcmp(argv[1], "-n") == 0)
    {
        newline = 0;
        i = 2;
    }
    
    // Afficher tous les arguments
    while (argv[i])
    {
        write(STDOUT_FILENO, argv[i], ft_strlen(argv[i]));
        if (argv[i + 1])
            write(STDOUT_FILENO, " ", 1);
        i++;
    }
    
    // Ajouter une nouvelle ligne si nécessaire
    if (newline)
        write(STDOUT_FILENO, "\n", 1);
    
    return 0;
}

int builtin_pwd(void)
{
    char cwd[4096];
    if (getcwd(cwd, sizeof(cwd)))
    {
        ft_putendl_fd(cwd, STDOUT_FILENO);
        return (0);
    }
    perror("pwd");
    return (1);
}

char **env_to_char(t_list *envl)
{
    int len = ft_lstsize(envl);
    char **envp = malloc(sizeof(char*) * (len + 1));
    if (!envp)
        return (NULL);
    int i = 0;
    t_list *cur = envl;
    while (cur)
    {
        t_env *e = (t_env *)cur->content;
        char *tmp = ft_strjoin(e->var, "=");
        if (!tmp)
        {
            while (--i >= 0)
                free(envp[i]);
            free(envp);
            return (NULL);
        }
        char *final = ft_strjoin(tmp, e->value);
        free(tmp);
        if (!final)
        {
            while (--i >= 0)
                free(envp[i]);
            free(envp);
            return (NULL);
        }
        envp[i++] = final;
        cur = cur->next;
    }
    envp[i] = NULL;
    return (envp);
}

int builtin_env(t_list *envl)
{
    t_list *cur = envl;
    while (cur)
    {
        t_env *e = (t_env *)cur->content;
        if (e->exported >= 1 && e->value != NULL)
        {
            ft_putstr_fd(e->var, STDOUT_FILENO);
            ft_putchar_fd('=', STDOUT_FILENO);
            ft_putendl_fd(e->value, STDOUT_FILENO);
        }
        cur = cur->next;
    }
    return (0);
}

int builtin_cd(char **argv, t_list **envl)
{
    printf("DEBUG: Entrée dans builtin_cd\n");
    
    // Vérifier si envl est valide
    if (!envl || !*envl) {
        printf("ERROR: Environnement invalide\n");
        return 1;
    }
    
    // Créer une copie de l'argument pour éviter le use-after-free
    char *path = NULL;
    
    if (argv[1]) {
        printf("DEBUG: argv[1]=%p\n", argv[1]);
        path = ft_strdup(argv[1]);  // Dupliquer l'argument
        if (!path) {
            perror("ft_strdup");
            return 1;
        }
        printf("DEBUG: Copie de argv[1]=%s\n", path);
    } else {
        printf("DEBUG: Recherche de HOME dans l'environnement\n");
        char *home = get_env_value(*envl, "HOME");
        if (!home) {
            ft_putendl_fd("minishell: cd: HOME not set", STDERR_FILENO);
            return 1;
        }
        path = ft_strdup(home);
        if (!path) {
            perror("ft_strdup");
            return 1;
        }
        printf("DEBUG: HOME=%s\n", path);
    }
    
    // Obtenir le répertoire courant AVANT de changer
    char old_dir[4096] = {0};
    if (!getcwd(old_dir, sizeof(old_dir))) {
        printf("DEBUG: Impossible d'obtenir le répertoire courant initial\n");
        // Continuer quand même
    }
    
    // Changer de répertoire
    printf("DEBUG: Changement de répertoire vers %s\n", path);
    if (chdir(path) == -1) {
        perror("cd");
        free(path);
        return 1;
    }
    
    // Libérer path immédiatement - évite un oubli
    free(path);
    path = NULL;
    
    // Obtenir le nouveau répertoire courant
    char new_dir[4096] = {0};
    if (!getcwd(new_dir, sizeof(new_dir))) {
        printf("DEBUG: Impossible d'obtenir le nouveau répertoire courant\n");
        return 0;  // Succès quand même, le chdir a fonctionné
    }
    
    // Mettre à jour les variables d'environnement avec des copies persistantes
    if (old_dir[0] != '\0') {
        printf("DEBUG: Mise à jour de OLDPWD=%s\n", old_dir);
        char *oldpwd_copy = ft_strdup(old_dir);
        if (oldpwd_copy) {
            add_env("OLDPWD", oldpwd_copy, envl, 1);
        }
    }
    
    printf("DEBUG: Mise à jour de PWD=%s\n", new_dir);
    char *pwd_copy = ft_strdup(new_dir);
    if (pwd_copy) {
        add_env("PWD", pwd_copy, envl, 1);
    }
    
    printf("DEBUG: builtin_cd terminé avec succès\n");
    return 0;
}

int builtin_export(char **argv, t_list **envl)
{
    // Si pas d'arguments, afficher les variables exportées
    if (!argv[1])
    {
        // Afficher les variables d'environnement au format export
        t_list *cur = *envl;
        while (cur)
        {
            t_env *e = (t_env *)cur->content;
            if (e->exported && ft_strcmp(e->var, "?exitcode") != 0 && ft_strcmp(e->var, "_") != 0)
            {
                ft_putstr_fd("declare -x ", STDOUT_FILENO);
                ft_putstr_fd(e->var, STDOUT_FILENO);
                if (e->value)
                {
                    ft_putstr_fd("=\"", STDOUT_FILENO);
                    ft_putstr_fd(e->value, STDOUT_FILENO);
                    ft_putstr_fd("\"", STDOUT_FILENO);
                }
                ft_putstr_fd("\n", STDOUT_FILENO);
            }
            cur = cur->next;
        }
        return (0);
    }

    // Traiter les arguments
    int i = 1;
    int status = 0;
    
    while (argv[i])
    {
        // Rechercher l'opérateur = dans l'argument
        char *equals = ft_strchr(argv[i], '=');
        
        if (!equals)
        {
            // Variable sans valeur (export VAR)
            char *var = ft_strdup(argv[i]);
            if (!var)
                return (1);
                
            // Vérifier si la variable existe déjà
            t_list *cur = *envl;
            int found = 0;
            
            while (cur)
            {
                t_env *e = (t_env *)cur->content;
                if (ft_strcmp(e->var, var) == 0)
                {
                    // Marquer comme exportée
                    e->exported = e->value ? 2 : 1;
                    found = 1;
                    break;
                }
                cur = cur->next;
            }
            
            // Si la variable n'existe pas, l'ajouter
            if (!found)
            {
                add_env(var, NULL, envl, 1);
            }
            
            free(var);
        }
        else
        {
            // Variable avec valeur (export VAR=value)
            *equals = '\0';  // Séparer la variable et la valeur
            char *var = ft_strdup(argv[i]);
            char *value = ft_strdup(equals + 1);
            
            if (!var || !value)
            {
                if (var) free(var);
                if (value) free(value);
                return (1);
            }
            
            // Ajouter ou mettre à jour la variable
            add_env(var, value, envl, 2);
            
            free(var);
            // value sera libéré par add_env
            
            *equals = '=';  // Restaurer l'argument original
        }
        
        i++;
    }
    
    return (status);
}

int builtin_exit(char **argv)
{
    int code = 0;
    
    // Si un code de sortie est fourni, le convertir en entier
    if (argv[1])
    {
        // Vérifier si le code est un nombre valide
        int i = 0;
        while (argv[1][i])
        {
            if (!ft_isdigit(argv[1][i]) && !(i == 0 && argv[1][i] == '-'))
            {
                ft_putstr_fd("minishell: exit: ", STDERR_FILENO);
                ft_putstr_fd(argv[1], STDERR_FILENO);
                ft_putstr_fd(": numeric argument required\n", STDERR_FILENO);
                return 2;
            }
            i++;
        }
        
        code = ft_atoi(argv[1]);
    }
    
    // Si nous sommes dans un pipeline, juste retourner le code
    // et ne pas quitter le processus principal
    if (isatty(STDIN_FILENO) && isatty(STDOUT_FILENO))
    {
        ft_putstr_fd("exit\n", STDERR_FILENO);
        exit(code);
    }
    
    return code;
}

static int run_builtin(t_cmd *cmd, t_list **envl)
{
    ft_putstr_fd("\n==== EXÉCUTION D'UNE COMMANDE BUILTIN ====\n", STDERR_FILENO);
    
    if (!cmd || !cmd->args || !cmd->args[0])
    {
        ft_putstr_fd("ERREUR: Commande builtin invalide\n", STDERR_FILENO);
        return 0;
    }
    
    ft_putstr_fd("Commande builtin: '", STDERR_FILENO);
    ft_putstr_fd(cmd->args[0], STDERR_FILENO);
    ft_putstr_fd("' (builtin_id = ", STDERR_FILENO);
    ft_putstr_fd(ft_itoa(cmd->builtin_id), STDERR_FILENO);
    ft_putstr_fd(")\n", STDERR_FILENO);
    
    // Afficher les arguments
    int arg_idx = 0;
    ft_putstr_fd("Arguments:\n", STDERR_FILENO);
    while (cmd->args[arg_idx])
    {
        ft_putstr_fd("  arg[", STDERR_FILENO);
        ft_putstr_fd(ft_itoa(arg_idx), STDERR_FILENO);
        ft_putstr_fd("] = '", STDERR_FILENO);
        ft_putstr_fd(cmd->args[arg_idx], STDERR_FILENO);
        ft_putstr_fd("'\n", STDERR_FILENO);
        arg_idx++;
    }
    
    // Vérifier l'état des descripteurs de fichiers standards
    ft_putstr_fd("État des descripteurs standards:\n", STDERR_FILENO);
    ft_putstr_fd("  STDIN_FILENO (0): ", STDERR_FILENO);
    if (fcntl(STDIN_FILENO, F_GETFL) == -1) {
        ft_putstr_fd("FERMÉ\n", STDERR_FILENO);
    } else {
        ft_putstr_fd("OUVERT\n", STDERR_FILENO);
    }
    
    ft_putstr_fd("  STDOUT_FILENO (1): ", STDERR_FILENO);
    if (fcntl(STDOUT_FILENO, F_GETFL) == -1) {
        ft_putstr_fd("FERMÉ\n", STDERR_FILENO);
    } else {
        ft_putstr_fd("OUVERT\n", STDERR_FILENO);
    }
    
    int result = -1;
    
    // Identifier la commande builtin et l'exécuter
    if (cmd->builtin_id == 1 || cmd->builtin_id == 2)  // echo avec ou sans option -n
    {
        ft_putstr_fd("Exécution de la builtin 'echo'\n", STDERR_FILENO);
        result = builtin_echo(cmd->args);
    }
    else if (cmd->builtin_id == 3)  // cd
    {
        ft_putstr_fd("Exécution de la builtin 'cd'\n", STDERR_FILENO);
        result = builtin_cd(cmd->args, envl);
    }
    else if (cmd->builtin_id == 4)  // pwd
    {
        ft_putstr_fd("Exécution de la builtin 'pwd'\n", STDERR_FILENO);
        result = builtin_pwd();
    }
    else if (cmd->builtin_id == 5)  // export
    {
        ft_putstr_fd("Exécution de la builtin 'export'\n", STDERR_FILENO);
        result = builtin_export(cmd->args, envl);
    }
    else if (cmd->builtin_id == 6)  // unset
    {
        ft_putstr_fd("Exécution de la builtin 'unset'\n", STDERR_FILENO);
        result = builtin_unset(cmd->args, envl);
    }
    else if (cmd->builtin_id == 7)  // env
    {
        ft_putstr_fd("Exécution de la builtin 'env'\n", STDERR_FILENO);
        result = builtin_env(*envl);
    }
    else if (cmd->builtin_id == 8)  // exit
    {
        ft_putstr_fd("Exécution de la builtin 'exit'\n", STDERR_FILENO);
        result = builtin_exit(cmd->args);
    }
    else
    {
        ft_putstr_fd("ERREUR: builtin_id inconnu\n", STDERR_FILENO);
    }
    
    ft_putstr_fd("Résultat de la commande builtin: ", STDERR_FILENO);
    ft_putstr_fd(ft_itoa(result), STDERR_FILENO);
    ft_putstr_fd("\n", STDERR_FILENO);
    ft_putstr_fd("==== FIN D'EXÉCUTION DE COMMANDE BUILTIN ====\n\n", STDERR_FILENO);
    
    return result;
}


/* Traite le heredoc et écrit son contenu dans le pipe */
void process_heredoc(int pipe_fd, char *delimiter)
{
    char buffer[4096];
    char c;
    int i;
    int stdout_copy;
    
    stdout_copy = dup(STDOUT_FILENO);
    
    while (1)
    {
        ft_putstr_fd("> ", stdout_copy);
        
        i = 0;
        while (i < 4095)
        {
            if (read(STDIN_FILENO, &c, 1) <= 0)
            {
                close(pipe_fd);
                close(stdout_copy);
                return;
            }
            
            write(stdout_copy, &c, 1);
            
            if (c == '\n')
                break;
                
            buffer[i++] = c;
        }
        buffer[i] = '\0';
        
        if (ft_strcmp(buffer, delimiter) == 0)
            break;
            
        write(pipe_fd, buffer, i);
        write(pipe_fd, "\n", 1);
    }
    
    close(pipe_fd);
    close(stdout_copy);
}


/* ------------ redirections --------------- */
/* Trouve la dernière redirection de chaque type */
static void find_last_redirs(t_redir *r, t_redir **input, 
                           t_redir **output, t_redir **heredoc)
{
    t_redir *current;

    *input = NULL;
    *output = NULL;
    *heredoc = NULL;
    current = r;
    
    while (current)
    {
        if (current->type == 0)
            *input = current;
        else if (current->type == 1 || current->type == 2)
            *output = current;
        else if (current->type == 3)
            *heredoc = current;
        current = current->next;
    }
}

/* Applique une redirection de sortie */
static int apply_output_redir(t_redir *output)
{
    int fd;
    int flags;
    
    if (!output)
        return (0);
    
    flags = O_CREAT | O_WRONLY;
    if (output->type == 1)
        flags |= O_TRUNC;
    else
        flags |= O_APPEND;
    
    fd = open(output->file, flags, 0644);
    if (fd == -1)
    {
        perror(output->file);
        return (1);
    }
    
    if (dup2(fd, STDOUT_FILENO) == -1)
    {
        close(fd);
        return (1);
    }
    close(fd);
    return (0);
}

/* Applique une redirection d'entrée */
static int apply_input_redir(t_redir *input)
{
    int fd;
    
    if (!input)
        return (0);
    
    fd = open(input->file, O_RDONLY);
    if (fd == -1)
    {
        perror(input->file);
        return (1);
    }
    
    if (dup2(fd, STDIN_FILENO) == -1)
    {
        close(fd);
        return (1);
    }
    close(fd);
    return (0);
}

/* Applique un heredoc */
static int apply_heredoc(t_redir *heredoc)
{
    int pipefd[2];
    pid_t pid;
    int status;
    
    if (!heredoc)
        return (0);
    
    if (pipe(pipefd) == -1)
        return (1);
    
    pid = fork();
    if (pid == -1)
    {
        close(pipefd[0]);
        close(pipefd[1]);
        return (1);
    }
    
    if (pid == 0)
    {
        close(pipefd[0]);
        process_heredoc(pipefd[1], heredoc->file);
        exit(0);
    }
    
    close(pipefd[1]);
    waitpid(pid, &status, 0);
    
    if (dup2(pipefd[0], STDIN_FILENO) == -1)
    {
        close(pipefd[0]);
        return (1);
    }
    close(pipefd[0]);
    return (0);
}

/* Fonction principale pour appliquer les redirections */
int apply_redirs(t_redir *r)
{
    t_redir *last_input;
    t_redir *last_output;
    t_redir *last_heredoc;
    
    if (!r)
        return (0);
    
    /* Trouver les dernières redirections */
    find_last_redirs(r, &last_input, &last_output, &last_heredoc);
    
    /* Appliquer dans l'ordre : heredoc ou input, puis output */
    if (last_heredoc)
    {
        if (apply_heredoc(last_heredoc))
            return (1);
    }
    else if (last_input)
    {
        if (apply_input_redir(last_input))
            return (1);
    }
    
    if (last_output)
    {
        if (apply_output_redir(last_output))
            return (1);
    }
    
    return (0);
}

/* ------------ execution helpers ------------- */
int launch_external(char **args, t_list *envl)
{
    ft_putstr_fd("\n==== LANCEMENT D'UNE COMMANDE EXTERNE ====\n", STDERR_FILENO);
    
    // Vérifier les arguments
    if (!args || !args[0])
    {
        ft_putstr_fd("ERREUR: Arguments de commande manquants\n", STDERR_FILENO);
        return 127;
    }
    
    // Afficher tous les arguments
    int arg_idx = 0;
    ft_putstr_fd("Arguments de la commande:\n", STDERR_FILENO);
    while (args[arg_idx])
    {
        ft_putstr_fd("  arg[", STDERR_FILENO);
        ft_putstr_fd(ft_itoa(arg_idx), STDERR_FILENO);
        ft_putstr_fd("] = '", STDERR_FILENO);
        ft_putstr_fd(args[arg_idx], STDERR_FILENO);
        ft_putstr_fd("'\n", STDERR_FILENO);
        arg_idx++;
    }
    
    // Vérifier l'état des descripteurs de fichiers
    ft_putstr_fd("État des descripteurs standards:\n", STDERR_FILENO);
    ft_putstr_fd("  STDIN_FILENO (0): ", STDERR_FILENO);
    if (fcntl(STDIN_FILENO, F_GETFL) == -1) {
        ft_putstr_fd("FERMÉ\n", STDERR_FILENO);
    } else {
        ft_putstr_fd("OUVERT\n", STDERR_FILENO);
    }
    
    ft_putstr_fd("  STDOUT_FILENO (1): ", STDERR_FILENO);
    if (fcntl(STDOUT_FILENO, F_GETFL) == -1) {
        ft_putstr_fd("FERMÉ\n", STDERR_FILENO);
    } else {
        ft_putstr_fd("OUVERT\n", STDERR_FILENO);
    }
    
    // Créer le tableau d'environnement
    ft_putstr_fd("Création du tableau d'environnement...\n", STDERR_FILENO);
    char **env_array = create_env_tab(envl, 0);  // 0 pour inclure toutes les variables
    if (!env_array)
    {
        ft_putstr_fd("ERREUR: Impossible de créer le tableau d'environnement\n", STDERR_FILENO);
        return 127;
    }
    
    // Afficher les 5 premières variables d'environnement pour vérification
    ft_putstr_fd("Échantillon des variables d'environnement:\n", STDERR_FILENO);
    int env_idx = 0;
    while (env_array[env_idx] && env_idx < 5)
    {
        ft_putstr_fd("  env[", STDERR_FILENO);
        ft_putstr_fd(ft_itoa(env_idx), STDERR_FILENO);
        ft_putstr_fd("] = '", STDERR_FILENO);
        ft_putstr_fd(env_array[env_idx], STDERR_FILENO);
        ft_putstr_fd("'\n", STDERR_FILENO);
        env_idx++;
    }
    
    // Vérifier si c'est un chemin absolu ou relatif
    if (args[0][0] == '/' || (args[0][0] == '.' && args[0][1] == '/'))
    {
        ft_putstr_fd("Chemin absolu ou relatif détecté: '", STDERR_FILENO);
        ft_putstr_fd(args[0], STDERR_FILENO);
        ft_putstr_fd("'\n", STDERR_FILENO);
        
        // Vérifier si le fichier existe et est exécutable
        if (access(args[0], F_OK) == -1) {
            ft_putstr_fd("ERREUR: Le fichier n'existe pas\n", STDERR_FILENO);
            free_tab(env_array);
            return 127;
        }
        
        if (access(args[0], X_OK) == -1) {
            ft_putstr_fd("ERREUR: Permission d'exécution refusée\n", STDERR_FILENO);
            free_tab(env_array);
            return 126;
        }
        
        ft_putstr_fd("Tentative d'exécution avec execve...\n", STDERR_FILENO);
        execve(args[0], args, env_array);
        
        // Si on arrive ici, execve a échoué
        ft_putstr_fd("ERREUR: execve a échoué: ", STDERR_FILENO);
        ft_putstr_fd(strerror(errno), STDERR_FILENO);
        ft_putstr_fd(" (errno=", STDERR_FILENO);
        ft_putstr_fd(ft_itoa(errno), STDERR_FILENO);
        ft_putstr_fd(")\n", STDERR_FILENO);
        
        free_tab(env_array);
        return 127;
    }
    
    // Chercher dans PATH
    ft_putstr_fd("Recherche de la commande dans le PATH...\n", STDERR_FILENO);
    char *path = get_env_value(envl, "PATH");
    
    if (!path)
    {
        ft_putstr_fd("ERREUR: Variable PATH non définie\n", STDERR_FILENO);
        free_tab(env_array);
        return 127;
    }
    
    ft_putstr_fd("PATH = '", STDERR_FILENO);
    ft_putstr_fd(path, STDERR_FILENO);
    ft_putstr_fd("'\n", STDERR_FILENO);
    
    char *path_copy = ft_strdup(path);
    if (!path_copy)
    {
        ft_putstr_fd("ERREUR: Impossible de dupliquer PATH\n", STDERR_FILENO);
        free_tab(env_array);
        return 127;
    }
    
    // Parcourir chaque répertoire du PATH
    ft_putstr_fd("Parcours des répertoires du PATH:\n", STDERR_FILENO);
    char *dir = path_copy;
    char *next_dir = NULL;
    char full_path[4096];
    int found = 0;
    int dir_count = 0;
    
    while (dir && *dir && !found)
    {
        dir_count++;
        next_dir = ft_strchr(dir, ':');
        if (next_dir)
            *next_dir = '\0';
        
        ft_putstr_fd("  Répertoire ", STDERR_FILENO);
        ft_putstr_fd(ft_itoa(dir_count), STDERR_FILENO);
        ft_putstr_fd(": '", STDERR_FILENO);
        ft_putstr_fd(dir, STDERR_FILENO);
        ft_putstr_fd("'\n", STDERR_FILENO);
        
        // Construire le chemin complet
        ft_memset(full_path, 0, 4096);
        ft_strlcpy(full_path, dir, 4096);
        ft_strlcat(full_path, "/", 4096);
        ft_strlcat(full_path, args[0], 4096);
        
        ft_putstr_fd("  Chemin complet: '", STDERR_FILENO);
        ft_putstr_fd(full_path, STDERR_FILENO);
        ft_putstr_fd("'\n", STDERR_FILENO);
        
        // Vérifier si le fichier existe et est exécutable
        if (access(full_path, F_OK) == -1) {
            ft_putstr_fd("  Fichier non trouvé\n", STDERR_FILENO);
        } 
        else if (access(full_path, X_OK) == -1) {
            ft_putstr_fd("  Permission d'exécution refusée\n", STDERR_FILENO);
        }
        else {
            ft_putstr_fd("  Fichier exécutable trouvé! Tentative d'exécution...\n", STDERR_FILENO);
            execve(full_path, args, env_array);
            
            // Si on arrive ici, execve a échoué
            ft_putstr_fd("  ERREUR: execve a échoué: ", STDERR_FILENO);
            ft_putstr_fd(strerror(errno), STDERR_FILENO);
            ft_putstr_fd(" (errno=", STDERR_FILENO);
            ft_putstr_fd(ft_itoa(errno), STDERR_FILENO);
            ft_putstr_fd(")\n", STDERR_FILENO);
            found = 1;
        }
        
        if (next_dir)
            dir = next_dir + 1;
        else
            dir = NULL;
    }
    
    free(path_copy);
    free_tab(env_array);
    
    ft_putstr_fd("ERREUR FINALE: Commande '", STDERR_FILENO);
    ft_putstr_fd(args[0], STDERR_FILENO);
    ft_putstr_fd("' non trouvée\n", STDERR_FILENO);
    ft_putstr_fd("==== FIN DU LANCEMENT DE COMMANDE EXTERNE ====\n\n", STDERR_FILENO);
    
    return 127;
}

int exec_simple(t_cmd *cmd, t_list **envl)
{
    printf("DEBUG: exec_simple appelé avec cmd=%p\n", cmd);
    if (cmd && cmd->args) {
        printf("DEBUG: cmd->args[0]=%s\n", cmd->args[0]);
        if (cmd->args[1])
            printf("DEBUG: cmd->args[1]=%s\n", cmd->args[1]);
    }
    
    if (!cmd->args || !cmd->args[0])
        return (0);
        
    /* parent builtins */
    if (!cmd->next && (ft_strcmp(cmd->args[0], "cd") == 0 ||
                       ft_strcmp(cmd->args[0], "exit") == 0 ||
                       ft_strcmp(cmd->args[0], "export") == 0 ||
                       ft_strcmp(cmd->args[0], "unset") == 0))
    {
        return (run_builtin(cmd, envl));
    }

    pid_t pid = fork();
    if (pid == -1)
        return (perror("fork"), 1);
    
    if (pid == 0)
    {
        if (cmd->redirs) {
            printf("DEBUG: Redirections pour la commande %s:\n", 
                cmd->args[0]);
            t_redir *r_debug = cmd->redirs;
            int redir_count = 0;
            while (r_debug) {
                printf("DEBUG: Redirection %d: type=%d, file=%s\n", 
                    redir_count++, r_debug->type, r_debug->file);
                r_debug = r_debug->next;
            }
        }
        
        if (apply_redirs(cmd->redirs))
            exit(1);
        if (run_builtin(cmd, envl) != -1)
            exit(0);
        exit(launch_external(cmd->args, *envl));
    }
    
    int status;
    waitpid(pid, &status, 0);
    if (WIFEXITED(status))
        return (WEXITSTATUS(status));
    return (128 + WTERMSIG(status));
}

/* Nettoie les pipes (ferme les descripteurs et libère la mémoire) */
static void	cleanup_pipes(int **pipes, int pipe_count)
{
	int	i;

	i = 0;
	while (i < pipe_count)
	{
		close(pipes[i][0]);
		close(pipes[i][1]);
		free(pipes[i]);
		i++;
	}
	free(pipes);
}

/* Nettoie les pipes et libère le tableau de pids */
static void	cleanup_pipes_and_pids(int **pipes, int pipe_count, pid_t *pids)
{
	cleanup_pipes(pipes, pipe_count);
	free(pids);
}

/* Attend la fin des processus enfants et récupère le statut */
static void	wait_for_children(pid_t *pids, int cmd_count, int *last_status)
{
	int	i;
	int	status;

	i = 0;
	while (i < cmd_count)
	{
		waitpid(pids[i], &status, 0);
		if (i == cmd_count - 1)
		{
			if (WIFEXITED(status))
				*last_status = WEXITSTATUS(status);
			else if (WIFSIGNALED(status))
				*last_status = 128 + WTERMSIG(status);
		}
		i++;
	}
}

/* Fonction pour initialiser les pipes */
static int	init_pipes(int ***pipes, int pipe_count)
{
	int	i;
	int	j;

	*pipes = malloc(sizeof(int*) * pipe_count);
	if (!(*pipes))
		return (1);
	i = 0;
	while (i < pipe_count)
	{
		(*pipes)[i] = malloc(sizeof(int) * 2);
		if (!(*pipes)[i])
		{
			j = 0;
			while (j < i)
			{
				free((*pipes)[j]);
				j++;
			}
			free(*pipes);
			return (1);
		}
		if (pipe((*pipes)[i]) == -1)
		{
			j = 0;
			while (j < i)
			{
				close((*pipes)[j][0]);
				close((*pipes)[j][1]);
				free((*pipes)[j]);
				j++;
			}
			free(*pipes);
			return (1);
		}
		i++;
	}
	return (0);
}

/* Ferme les descripteurs non utilisés dans l'enfant */
static void	close_unused_child(int **pipes, int i, int pipe_count)
{
	int	j;

	j = 0;
	while (j < pipe_count)
	{
		if (j == i - 1)
			close(pipes[j][1]); /* Fermer write du pipe d'entrée */
		else if (j == i)
			close(pipes[j][0]); /* Fermer read du pipe de sortie */
		else
		{
			close(pipes[j][0]); /* Fermer tous les autres pipes */
			close(pipes[j][1]);
		}
		j++;
	}
}

/* Configure stdin et stdout dans l'enfant */
static void	setup_child_io(int **pipes, int i, int cmd_count)
{
	if (i > 0)
	{
		if (dup2(pipes[i - 1][0], STDIN_FILENO) == -1)
		{
			perror("dup2 stdin");
			exit(1);
		}
		close(pipes[i - 1][0]); /* Fermer après dup2 */
	}
	if (i < cmd_count - 1)
	{
		if (dup2(pipes[i][1], STDOUT_FILENO) == -1)
		{
			perror("dup2 stdout");
			exit(1);
		}
		close(pipes[i][1]); /* Fermer après dup2 */
	}
}

/* Ferme les descripteurs dans le parent après fork */
static void	close_parent_fds(int **pipes, int i, int cmd_count)
{
	if (i > 0)
		close(pipes[i - 1][0]); /* Fermer read du pipe précédent */
	if (i < cmd_count - 1 && i > 0)
		close(pipes[i - 1][1]); /* Fermer write du pipe précédent */
}

int	execute_pipeline(t_cmd *head, t_list **envl)
{
	int		pipe_count;
	int		**pipes;
	pid_t	*pids;
	int		last_status;
	t_cmd	*cmd;
	int		cmd_count;
	int		i;

	/* Compter les commandes valides */
	cmd_count = 0;
	cmd = head;
	while (cmd)
	{
		if (cmd->args && cmd->args[0])
			cmd_count++;
		cmd = cmd->next;
	}
	if (cmd_count == 0)
		return (0);

	/* Initialiser et vérifier les pipes */
	pipe_count = cmd_count - 1;
	if (init_pipes(&pipes, pipe_count))
		return (1);

	/* Allouer les PIDs */
	pids = malloc(sizeof(pid_t) * cmd_count);
	if (!pids)
	{
		/* Nettoyage en cas d'erreur */
		cleanup_pipes(pipes, pipe_count);
		return (1);
	}

	/* Exécuter les commandes */
	i = 0;
	cmd = head;
	while (cmd && i < cmd_count)
	{
		if (!cmd->args || !cmd->args[0])
		{
			cmd = cmd->next;
			continue;
		}
		pids[i] = fork();
		if (pids[i] == -1)
		{
			cleanup_pipes_and_pids(pipes, pipe_count, pids);
			return (1);
		}
		if (pids[i] == 0)
		{
			/* Processus enfant */
			close_unused_child(pipes, i, pipe_count);
			setup_child_io(pipes, i, cmd_count);
			if (cmd->redirs && apply_redirs(cmd->redirs))
				exit(1);
			if (cmd->builtin_id != -1)
				exit(run_builtin(cmd, envl));
			else
				exit(launch_external(cmd->args, *envl));
		}
		else
		{
			/* Processus parent */
			close_parent_fds(pipes, i, cmd_count);
		}
		cmd = cmd->next;
		i++;
	}

	/* Fermer les derniers descripteurs et libérer les ressources */
	cleanup_pipes(pipes, pipe_count);
	wait_for_children(pids, cmd_count, &last_status);
	free(pids);
	return (last_status);
}




int execute_cmds(t_cmd *cmds, t_list **envl, int *last_status)
{
    printf("DEBUG: execute_cmds appelé avec cmds=%p\n", cmds);
    
    if (!cmds)
        return (0);
    
    // Fix pipeline structure - TRÈS IMPORTANT: CONSERVER LE RETOUR!
    t_cmd *fixed_cmds = fix_pipeline_structure(cmds);
    if (!fixed_cmds)
    {
        *last_status = 0;
        return (0);
    }
    
    // Afficher les informations sur la structure corrigée
    t_cmd *temp = fixed_cmds;
    int cmd_count = 0;
    printf("DEBUG: Structure du pipeline après correction:\n");
    while (temp)
    {
        cmd_count++;
        printf("DEBUG: Commande %d: %s (builtin_id=%d)\n", 
               cmd_count, 
               temp->args && temp->args[0] ? temp->args[0] : "(null)",
               temp->builtin_id);
        temp = temp->next;
    }
    
    // Vérifier si c'est une commande simple ou un pipeline
    if (cmd_count > 1)
    {
        printf("DEBUG: Exécution d'un pipeline de %d commandes\n", cmd_count);
        *last_status = execute_pipeline(fixed_cmds, envl);
    }
    else
    {
        printf("DEBUG: Exécution d'une commande simple\n");
        *last_status = exec_simple(fixed_cmds, envl);
    }
    printf("DEBUG: Exécution %s: première commande '%s', cmd_count=%d, fixed_cmds=%p\n", 
       cmd_count > 1 ? "d'un pipeline" : "d'une commande simple",
       fixed_cmds->args[0], cmd_count, fixed_cmds);
    
    printf("DEBUG: execute_cmds terminé avec status=%d\n", *last_status);
    return (*last_status);
}
