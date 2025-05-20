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

/* ------------ redirections --------------- */
int apply_redirs(t_redir *r)
{
    int fd;
    t_redir *current;
    
    // Première étape: créer/tronquer tous les fichiers de sortie
    current = r;
    while (current)
    {
        // Pour les redirections de sortie (> ou >>)
        if (current->type == 1)
        {
            printf("DEBUG-EXEC: Creating/truncating file '%s'\n", current->file);
            fd = open(current->file, O_CREAT|O_WRONLY|O_TRUNC, 0644);
            if (fd == -1)
            {
                perror(current->file);
                return (1);
            }
            close(fd);
        }
        else if (current->type == 2)
        {
            printf("DEBUG-EXEC: Opening file for append '%s'\n", current->file);
            fd = open(current->file, O_CREAT|O_WRONLY|O_APPEND, 0644);
            if (fd == -1)
            {
                perror(current->file);
                return (1);
            }
            close(fd);
        }
        current = current->next;
    }
    
    // Deuxième étape: appliquer seulement la dernière redirection de chaque type
    t_redir *last_input = NULL;
    t_redir *last_output = NULL;
    t_redir *last_heredoc = NULL;
    
    current = r;
    while (current)
    {
        if (current->type == 0) // Redirection d'entrée (<)
            last_input = current;
        else if (current->type == 1 || current->type == 2) // Redirection de sortie (> ou >>)
            last_output = current;
        else if (current->type == 3) // Heredoc (<<)
            last_heredoc = current;
        current = current->next;
    }
    
    // Appliquer la dernière redirection de sortie
    if (last_output)
    {
        printf("DEBUG-EXEC: Applying last output redirection to '%s'\n", last_output->file);
        int flags = O_CREAT|O_WRONLY;
        if (last_output->type == 1)
            flags |= O_TRUNC;
        else
            flags |= O_APPEND;
            
        fd = open(last_output->file, flags, 0644);
        if (fd == -1)
        {
            perror(last_output->file);
            return (1);
        }
        
        if (dup2(fd, STDOUT_FILENO) == -1)
        {
            close(fd);
            return (perror("dup2"), 1);
        }
        close(fd);
    }
    
    // Appliquer le heredoc si présent (priorité sur la redirection d'entrée classique)
    if (last_heredoc)
    {
        printf("DEBUG-EXEC: Processing heredoc with delimiter '%s'\n", last_heredoc->file);
        
        int pipefd[2];
        if (pipe(pipefd) == -1)
        {
            perror("pipe");
            return (1);
        }
        
        pid_t pid = fork();
        if (pid == -1)
        {
            perror("fork");
            close(pipefd[0]);
            close(pipefd[1]);
            return (1);
        }
        
        if (pid == 0)
        {
            // Processus enfant: lit l'entrée utilisateur
            close(pipefd[0]);  // Ferme le côté lecture
            
            char buffer[4096];
            size_t len;
            
            // Configurer la sortie pour que l'utilisateur puisse voir le prompt du heredoc
            int stdout_copy = dup(STDOUT_FILENO);
            
            while (1)
            {
                // Afficher le prompt sur la sortie standard originale
                ft_putstr_fd("> ", stdout_copy);
                
                // Lire une ligne de l'entrée standard
                int i = 0;
                while (i < 4095)
                {
                    if (read(STDIN_FILENO, &buffer[i], 1) <= 0)
                        exit(1);
                    
                    // Afficher le caractère lu sur la sortie standard originale
                    write(stdout_copy, &buffer[i], 1);
                    
                    if (buffer[i] == '\n')
                    {
                        buffer[i] = '\0';
                        break;
                    }
                    i++;
                }
                buffer[i] = '\0';
                
                // Vérifier si c'est le délimiteur
                if (ft_strcmp(buffer, last_heredoc->file) == 0)
                    break;
                
                // Écrire la ligne dans le pipe (avec un retour à la ligne)
                len = ft_strlen(buffer);
                write(pipefd[1], buffer, len);
                write(pipefd[1], "\n", 1);
            }
            
            close(pipefd[1]);
            close(stdout_copy);
            exit(0);
        }
        else
        {
            // Processus parent
            close(pipefd[1]);  // Ferme le côté écriture
            
            // Attendre la fin du processus enfant
            int status;
            waitpid(pid, &status, 0);
            
            // Rediriger l'entrée standard vers le pipe
            if (dup2(pipefd[0], STDIN_FILENO) == -1)
            {
                perror("dup2");
                close(pipefd[0]);
                return (1);
            }
            
            close(pipefd[0]);
        }
    }
    // Appliquer la dernière redirection d'entrée (seulement si pas de heredoc)
    else if (last_input)
    {
        printf("DEBUG-EXEC: Applying last input redirection from '%s'\n", last_input->file);
        fd = open(last_input->file, O_RDONLY);
        if (fd == -1)
        {
            perror(last_input->file);
            return (1);
        }
        
        if (dup2(fd, STDIN_FILENO) == -1)
        {
            close(fd);
            return (perror("dup2"), 1);
        }
        close(fd);
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
    printf("DEBUG: exec_simple called with cmd=%p\n", cmd);
    
    if (!cmd || !cmd->args || !cmd->args[0])
        return 0;
    
    /* parent builtins */
    if (!cmd->next && (cmd->builtin_id == 3 || cmd->builtin_id == 8 || 
                      cmd->builtin_id == 5 || cmd->builtin_id == 6))
    {
        // Pour les builtins qui s'exécutent dans le processus parent,
        // on doit gérer les redirections différemment
        int stdin_backup = -1;
        int stdout_backup = -1;
        
        // Sauvegarder les descripteurs standard si nécessaire
        if (cmd->redirs) {
            stdin_backup = dup(STDIN_FILENO);
            stdout_backup = dup(STDOUT_FILENO);
            
            // Appliquer les redirections
            if (apply_redirs(cmd->redirs)) {
                // En cas d'erreur, restaurer les descripteurs et retourner
                if (stdin_backup != -1) {
                    dup2(stdin_backup, STDIN_FILENO);
                    close(stdin_backup);
                }
                if (stdout_backup != -1) {
                    dup2(stdout_backup, STDOUT_FILENO);
                    close(stdout_backup);
                }
                return 1;
            }
        }
        
        // Exécuter la builtin
        int result = run_builtin(cmd, envl);
        
        // Restaurer les descripteurs standard
        if (cmd->redirs) {
            if (stdin_backup != -1) {
                dup2(stdin_backup, STDIN_FILENO);
                close(stdin_backup);
            }
            if (stdout_backup != -1) {
                dup2(stdout_backup, STDOUT_FILENO);
                close(stdout_backup);
            }
        }
        
        return result;
    }
    
    // Pour les autres commandes, fork et exécuter
    pid_t pid = fork();
    if (pid == -1)
        return (perror("fork"), 1);
    
    if (pid == 0) { // Processus enfant
        // Appliquer les redirections
        if (cmd->redirs && apply_redirs(cmd->redirs))
            exit(1);
        
        // Exécuter la commande
        if (cmd->builtin_id != -1)
            exit(run_builtin(cmd, envl));
        else
            exit(launch_external(cmd->args, *envl));
    }
    
    // Processus parent
    int status;
    waitpid(pid, &status, 0);
    if (WIFEXITED(status))
        return WEXITSTATUS(status);
    return (128 + WTERMSIG(status));
}

int execute_pipeline(t_cmd *head, t_list **envl)
{
    int pipes[2][2];
    int current_pipe;
    int last_status;
    pid_t *pids;
    int cmd_count;
    int valid_cmd_count;
    
    ft_putstr_fd("\n==== DÉBUT D'EXÉCUTION DU PIPELINE ====\n", STDERR_FILENO);
    
    // Initialisation
    current_pipe = 0;
    last_status = 0;
    cmd_count = 0;
    valid_cmd_count = 0;
    
    // Initialiser les pipes à -1
    pipes[0][0] = -1; pipes[0][1] = -1;
    pipes[1][0] = -1; pipes[1][1] = -1;
    
    // Compter les commandes valides
    t_cmd *count = head;
    ft_putstr_fd("Liste des commandes du pipeline:\n", STDERR_FILENO);
    while (count) {
        cmd_count++;
        if (count->args && count->args[0]) {
            valid_cmd_count++;
            ft_putstr_fd("CMD ", STDERR_FILENO);
            ft_putstr_fd(ft_itoa(valid_cmd_count), STDERR_FILENO);
            ft_putstr_fd(": ", STDERR_FILENO);
            ft_putstr_fd(count->args[0], STDERR_FILENO);
            ft_putstr_fd(" (builtin_id: ", STDERR_FILENO);
            ft_putstr_fd(ft_itoa(count->builtin_id), STDERR_FILENO);
            ft_putstr_fd(")\n", STDERR_FILENO);
        } else {
            ft_putstr_fd("IGNORÉ: Commande vide détectée\n", STDERR_FILENO);
        }
        count = count->next;
    }
    
    ft_putstr_fd("Nombre total de commandes: ", STDERR_FILENO);
    ft_putstr_fd(ft_itoa(cmd_count), STDERR_FILENO);
    ft_putstr_fd(" (dont ", STDERR_FILENO);
    ft_putstr_fd(ft_itoa(valid_cmd_count), STDERR_FILENO);
    ft_putstr_fd(" valides)\n", STDERR_FILENO);
    
    // Allocation du tableau de PID seulement pour les commandes valides
    pids = malloc(sizeof(pid_t) * valid_cmd_count);
    if (!pids) {
        ft_putstr_fd("ERREUR: Échec d'allocation pour les PIDs\n", STDERR_FILENO);
        return 1;
    }
    
    // Exécuter les commandes valides
    t_cmd *cmd = head;
    int cmd_index = 0;
    int valid_index = 0;
    t_cmd *prev_valid_cmd = NULL;
    t_cmd *next_valid_cmd = NULL;
    
    while (cmd)
    {
        // Ignorer les commandes vides
        if (!cmd->args || !cmd->args[0]) {
            ft_putstr_fd("Ignoré: Commande vide\n", STDERR_FILENO);
            cmd = cmd->next;
            cmd_index++;
            continue;
        }
        
        // Trouver la prochaine commande valide pour le pipe
        next_valid_cmd = cmd->next;
        while (next_valid_cmd && (!next_valid_cmd->args || !next_valid_cmd->args[0])) {
            next_valid_cmd = next_valid_cmd->next;
        }
        
        ft_putstr_fd("\nEXÉCUTION DE LA COMMANDE ", STDERR_FILENO);
        ft_putstr_fd(ft_itoa(valid_index + 1), STDERR_FILENO);
        ft_putstr_fd("/", STDERR_FILENO);
        ft_putstr_fd(ft_itoa(valid_cmd_count), STDERR_FILENO);
        ft_putstr_fd("\n", STDERR_FILENO);
        
        // Créer un nouveau pipe si ce n'est pas la dernière commande valide
        if (next_valid_cmd)
        {
            ft_putstr_fd("Création de pipe[", STDERR_FILENO);
            ft_putstr_fd(ft_itoa(current_pipe), STDERR_FILENO);
            ft_putstr_fd("]\n", STDERR_FILENO);
            
            if (pipe(pipes[current_pipe]) == -1)
            {
                ft_putstr_fd("ERREUR: Échec de création du pipe\n", STDERR_FILENO);
                perror("pipe");
                free(pids);
                return 1;
            }
            
            ft_putstr_fd("Pipe créé: [", STDERR_FILENO);
            ft_putstr_fd(ft_itoa(pipes[current_pipe][0]), STDERR_FILENO);
            ft_putstr_fd(", ", STDERR_FILENO);
            ft_putstr_fd(ft_itoa(pipes[current_pipe][1]), STDERR_FILENO);
            ft_putstr_fd("]\n", STDERR_FILENO);
        }
        else {
            ft_putstr_fd("Pas de pipe nécessaire (dernière commande valide)\n", STDERR_FILENO);
        }
        
        // Fork pour exécuter la commande
        ft_putstr_fd("Création d'un processus enfant (fork)...\n", STDERR_FILENO);
        pids[valid_index] = fork();
        
        if (pids[valid_index] == -1)
        {
            ft_putstr_fd("ERREUR: Échec du fork\n", STDERR_FILENO);
            perror("fork");
            free(pids);
            return 1;
        }
        
        if (pids[valid_index] == 0)  // Processus enfant
        {
            ft_putstr_fd("[ENFANT] Début de configuration du processus enfant\n", STDERR_FILENO);
            
            // Si ce n'est pas la première commande valide, rediriger l'entrée
            if (prev_valid_cmd)
            {
                ft_putstr_fd("[ENFANT] Redirection de l'entrée standard depuis le pipe précédent\n", STDERR_FILENO);
                close(pipes[1 - current_pipe][1]);
                
                if (dup2(pipes[1 - current_pipe][0], STDIN_FILENO) == -1) {
                    ft_putstr_fd("[ENFANT] ERREUR: dup2 pour stdin a échoué\n", STDERR_FILENO);
                    perror("dup2 (stdin)");
                    exit(1);
                }
                
                close(pipes[1 - current_pipe][0]);
            }
            
            // Si ce n'est pas la dernière commande valide, rediriger la sortie
            if (next_valid_cmd)
            {
                ft_putstr_fd("[ENFANT] Redirection de la sortie standard vers le pipe actuel\n", STDERR_FILENO);
                close(pipes[current_pipe][0]);
                
                if (dup2(pipes[current_pipe][1], STDOUT_FILENO) == -1) {
                    ft_putstr_fd("[ENFANT] ERREUR: dup2 pour stdout a échoué\n", STDERR_FILENO);
                    perror("dup2 (stdout)");
                    exit(1);
                }
                
                close(pipes[current_pipe][1]);
            }
            
            // Appliquer les redirections spécifiées dans la commande
            ft_putstr_fd("[ENFANT] Application des redirections spécifiées...\n", STDERR_FILENO);
            if (apply_redirs(cmd->redirs)) {
                ft_putstr_fd("[ENFANT] ERREUR: Échec des redirections\n", STDERR_FILENO);
                exit(1);
            }
            
            // Exécuter la commande
            if (cmd->builtin_id != -1) // Commande built-in
            {
                ft_putstr_fd("[ENFANT] Exécution d'une commande builtin (id: ", STDERR_FILENO);
                ft_putstr_fd(ft_itoa(cmd->builtin_id), STDERR_FILENO);
                ft_putstr_fd(")\n", STDERR_FILENO);
                
                int result = run_builtin(cmd, envl);
                exit(result);
            }
            else // Commande externe
            {
                ft_putstr_fd("[ENFANT] Exécution d'une commande externe: '", STDERR_FILENO);
                ft_putstr_fd(cmd->args[0], STDERR_FILENO);
                ft_putstr_fd("'\n", STDERR_FILENO);
                
                exit(launch_external(cmd->args, *envl));
            }
        }
        
        // Processus parent
        ft_putstr_fd("[PARENT] Processus parent continue\n", STDERR_FILENO);
        
        // Fermer les descripteurs du pipe précédent
        if (prev_valid_cmd)
        {
            ft_putstr_fd("[PARENT] Fermeture du pipe précédent\n", STDERR_FILENO);
            close(pipes[1 - current_pipe][0]);
            close(pipes[1 - current_pipe][1]);
        }
        
        // Mettre à jour les variables pour la prochaine itération
        prev_valid_cmd = cmd;
        cmd = cmd->next;
        cmd_index++;
        valid_index++;
        current_pipe = 1 - current_pipe;
    }
    
    // Attendre tous les processus enfants
    ft_putstr_fd("\n[PARENT] Attente de la fin des processus enfants...\n", STDERR_FILENO);
    int i = 0;
    while (i < valid_cmd_count)
    {
        int status;
        ft_putstr_fd("[PARENT] Attente du processus ", STDERR_FILENO);
        ft_putstr_fd(ft_itoa(pids[i]), STDERR_FILENO);
        ft_putstr_fd("\n", STDERR_FILENO);
        
        waitpid(pids[i], &status, 0);
        
        if (WIFEXITED(status)) {
            ft_putstr_fd("[PARENT] Processus ", STDERR_FILENO);
            ft_putstr_fd(ft_itoa(pids[i]), STDERR_FILENO);
            ft_putstr_fd(" terminé. Exit code: ", STDERR_FILENO);
            ft_putstr_fd(ft_itoa(WEXITSTATUS(status)), STDERR_FILENO);
            ft_putstr_fd("\n", STDERR_FILENO);
            
            if (i == valid_cmd_count - 1)  // Dernier processus valide
                last_status = WEXITSTATUS(status);
        } 
        else if (WIFSIGNALED(status)) {
            ft_putstr_fd("[PARENT] Processus ", STDERR_FILENO);
            ft_putstr_fd(ft_itoa(pids[i]), STDERR_FILENO);
            ft_putstr_fd(" terminé. Killed by signal: ", STDERR_FILENO);
            ft_putstr_fd(ft_itoa(WTERMSIG(status)), STDERR_FILENO);
            ft_putstr_fd("\n", STDERR_FILENO);
            
            if (i == valid_cmd_count - 1)  // Dernier processus valide
                last_status = 128 + WTERMSIG(status);
        }
        
        i++;
    }
    
    free(pids);
    ft_putstr_fd("\n[PARENT] Pipeline terminé avec code: ", STDERR_FILENO);
    ft_putstr_fd(ft_itoa(last_status), STDERR_FILENO);
    ft_putstr_fd("\n==== FIN D'EXÉCUTION DU PIPELINE ====\n\n", STDERR_FILENO);
    
    return last_status;
}




int execute_cmds(t_cmd *cmds, t_list **envl, int *last_status)
{
    printf("DEBUG: execute_cmds appelé avec cmds=%p\n", cmds);
    
    if (!cmds)
        return (0);
    
    // Afficher le premier argument et les redirections pour le débogage
    if (cmds->args && cmds->args[0]) {
        // MODIFICATION: Ne considérer comme "factice" que si c'est exactement "_redir_placeholder_"
        // et non pas une commande réelle comme "echo"
        if (ft_strcmp(cmds->args[0], "_redir_placeholder_") == 0) {
            printf("DEBUG: Commande factice avec redirections uniquement\n");
            // Exécuter uniquement les redirections
            if (cmds->redirs) {
                printf("DEBUG: Application des redirections\n");
                // Code pour appliquer les redirections sans exécuter la commande
                pid_t pid = fork();
                if (pid == 0) {
                    if (apply_redirs(cmds->redirs) == 0) {
                        // Succès - simplement sortir avec statut 0
                        exit(0);
                    }
                    exit(1);
                } else if (pid > 0) {
                    int status;
                    waitpid(pid, &status, 0);
                    *last_status = WIFEXITED(status) ? WEXITSTATUS(status) : 1;
                    return *last_status;
                } else {
                    perror("fork");
                    *last_status = 1;
                    return 1;
                }
            }
            *last_status = 0;
            return 0;
        }
        
        // Il s'agit d'une vraie commande
        printf("DEBUG: Premier argument: %s\n", cmds->args[0]);
        
        // Afficher les redirections
        if (cmds->redirs) {
            printf("DEBUG: Redirections pour la commande:\n");
            t_redir *r = cmds->redirs;
            int count = 0;
            while (r) {
                printf("DEBUG:   [%d] type=%d, file='%s'\n", count++, r->type, r->file);
                r = r->next;
            }
        } else {
            printf("DEBUG: Pas de redirections pour cette commande\n");
        }
    }
    
    // Vérifier si c'est une commande simple ou un pipeline
    if (!cmds->next) {
        // Commande simple, sans pipeline
        *last_status = exec_simple(cmds, envl);
    } else {
        // Pipeline de commandes
        *last_status = execute_pipeline(cmds, envl);
    }
    
    printf("DEBUG: execute_cmds terminé avec status=%d\n", *last_status);
    return (*last_status);
}
