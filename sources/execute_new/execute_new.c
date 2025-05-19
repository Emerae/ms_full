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
                    ft_strcmp(env_var->var, "?begin") != 0)
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
            if (e->exported && ft_strcmp(e->var, "?begin") != 0 && ft_strcmp(e->var, "_") != 0)
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
    if (!cmd || !cmd->args || !cmd->args[0])
        return (0);
    
    int result = -1;
    
    // Utiliser directement le builtin_id déterminé par le parser
    if (cmd->builtin_id == 1 || cmd->builtin_id == 2)  // echo avec ou sans option -n
        result = builtin_echo(cmd->args);
    else if (cmd->builtin_id == 3)  // cd
        result = builtin_cd(cmd->args, envl);
    else if (cmd->builtin_id == 4)  // pwd
        result = builtin_pwd();
    else if (cmd->builtin_id == 5)  // export
        result = builtin_export(cmd->args, envl);
    else if (cmd->builtin_id == 6)  // unset
        result = builtin_unset(cmd->args, envl);
    else if (cmd->builtin_id == 7)  // env
        result = builtin_env(*envl);
    else if (cmd->builtin_id == 8)  // exit
        result = builtin_exit(cmd->args);
    
    return (result);
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
    // Créer un tableau d'environnement à partir de la liste chaînée
    char **env_array = create_env_tab(envl, 2); // 2 pour n'inclure que les variables exportées
    if (!env_array) {
        ft_putstr_fd("minishell: environnement non disponible\n", STDERR_FILENO);
        return (127);
    }

    // Si c'est un chemin absolu ou relatif
    if (args[0][0] == '/' || (args[0][0] == '.' && args[0][1] == '/'))
    {
        execve(args[0], args, env_array);
        perror(args[0]);
        free_tab(env_array);
        return (127);
    }
    
    // Chercher dans PATH
    char *path = get_env_value(envl, "PATH");
    if (path)
    {
        char *path_copy = ft_strdup(path);
        if (!path_copy) {
            free_tab(env_array);
            return (127);
        }
            
        char *current = path_copy;
        char *next_colon;
        char full_path[4096];
        
        while (current && *current)
        {
            next_colon = ft_strchr(current, ':');
            if (next_colon)
                *next_colon = '\0';
                
            ft_memset(full_path, 0, sizeof(full_path));
            ft_strlcpy(full_path, current, sizeof(full_path));
            ft_strlcat(full_path, "/", sizeof(full_path));
            ft_strlcat(full_path, args[0], sizeof(full_path));
            
            execve(full_path, args, env_array);
            
            if (!next_colon)
                break;
                
            current = next_colon + 1;
        }
        
        free(path_copy);
        free_tab(env_array);
    }
    
    ft_putstr_fd(args[0], STDERR_FILENO);
    ft_putstr_fd(": command not found\n", STDERR_FILENO);
    return (127);
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

int execute_pipeline(t_cmd *head, t_list **envl)
{
    int pipes[2][2];  // Double buffer pour gérer les pipes entre processus
    int current_pipe = 0;
    int last_status = 0;
    pid_t *pids;
    int cmd_count = 0;
    
    // Compter le nombre de commandes pour allouer le tableau de PIDs
    t_cmd *count = head;
    while (count) {
        cmd_count++;
        count = count->next;
    }
    
    pids = malloc(sizeof(pid_t) * cmd_count);
    if (!pids)
        return 1;
    
    // Exécuter les commandes
    t_cmd *cmd = head;
    int cmd_index = 0;
    
    while (cmd)
    {
        // Créer un nouveau pipe si ce n'est pas la dernière commande
        if (cmd->next)
        {
            if (pipe(pipes[current_pipe]) == -1)
            {
                perror("pipe");
                free(pids);
                return 1;
            }
        }
        
        // Fork pour exécuter la commande
        pids[cmd_index] = fork();
        if (pids[cmd_index] == -1)
        {
            perror("fork");
            free(pids);
            return 1;
        }
        
        if (pids[cmd_index] == 0)  // Processus enfant
        {
            // Si ce n'est pas la première commande, rediriger l'entrée depuis le pipe précédent
            if (cmd_index > 0)
            {
                dup2(pipes[1 - current_pipe][0], STDIN_FILENO);
                close(pipes[1 - current_pipe][0]);
                close(pipes[1 - current_pipe][1]);
            }
            
            // Si ce n'est pas la dernière commande, rediriger la sortie vers le pipe actuel
            if (cmd->next)
            {
                close(pipes[current_pipe][0]);
                dup2(pipes[current_pipe][1], STDOUT_FILENO);
                close(pipes[current_pipe][1]);
            }
            
            // Fermer tous les descripteurs de fichiers non utilisés
            for (int i = 3; i < 256; i++)
                if (i != STDIN_FILENO && i != STDOUT_FILENO && i != STDERR_FILENO)
                    close(i);
            
            // Appliquer les redirections spécifiées dans la commande
            if (apply_redirs(cmd->redirs))
                exit(1);
            
            // Exécuter la commande
            int status;
            if (cmd->builtin_id != -1) // Commande built-in
            {
                status = run_builtin(cmd, envl);
                exit(status);
            }
            else // Commande externe
            {
                // Convertir envl en tableau d'environnement
                char **env_array = create_env_tab(*envl, 2);
                if (!env_array)
                    exit(127);
                
                // Si c'est un chemin absolu
                if (cmd->args[0][0] == '/' || (cmd->args[0][0] == '.' && cmd->args[0][1] == '/'))
                {
                    execve(cmd->args[0], cmd->args, env_array);
                    perror(cmd->args[0]);
                }
                else // Chercher dans le PATH
                {
                    char *path = get_env_value(*envl, "PATH");
                    if (path)
                    {
                        char *path_copy = ft_strdup(path);
                        if (path_copy)
                        {
                            char *token = strtok(path_copy, ":");
                            while (token)
                            {
                                char full_path[4096];
                                ft_strlcpy(full_path, token, sizeof(full_path));
                                ft_strlcat(full_path, "/", sizeof(full_path));
                                ft_strlcat(full_path, cmd->args[0], sizeof(full_path));
                                
                                execve(full_path, cmd->args, env_array);
                                token = strtok(NULL, ":");
                            }
                            free(path_copy);
                        }
                    }
                }
                free_tab(env_array);
                ft_putstr_fd(cmd->args[0], STDERR_FILENO);
                ft_putstr_fd(": command not found\n", STDERR_FILENO);
                exit(127);
            }
        }
        
        // Processus parent
        if (cmd_index > 0)
        {
            // Fermer le pipe précédent qui a été dupliqué dans l'enfant
            close(pipes[1 - current_pipe][0]);
            close(pipes[1 - current_pipe][1]);
        }
        
        // Passer à la commande suivante
        cmd = cmd->next;
        cmd_index++;
        current_pipe = 1 - current_pipe;  // Alterner entre les deux pipes
    }
    
    // Attendre la fin de tous les processus enfants
    for (int i = 0; i < cmd_count; i++)
    {
        int status;
        waitpid(pids[i], &status, 0);
        
        // Sauvegarder le statut du dernier processus
        if (i == cmd_count - 1)
        {
            if (WIFEXITED(status))
                last_status = WEXITSTATUS(status);
            else if (WIFSIGNALED(status))
                last_status = 128 + WTERMSIG(status);
        }
    }
    
    free(pids);
    return last_status;
}



int execute_cmds(t_cmd *cmds, t_list **envl, int *last_status)
{
    printf("DEBUG: execute_cmds appelé avec cmds=%p\n", cmds);
    
    if (!cmds)
        return (0);
    
    // Afficher le premier argument pour le débogage
    if (cmds->args && cmds->args[0]) {
        printf("DEBUG: Premier argument: %s\n", cmds->args[0]);
        if (cmds->args[1])
            printf("DEBUG: Deuxième argument: %s\n", cmds->args[1]);
    }
    
    // Nous ne clonons pas la commande ici car ce sera fait dans les fonctions 
    // exec_simple ou execute_pipeline
    
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
