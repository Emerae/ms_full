#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include "libftfull.h"
#include "structures.h"
#include "parser_new.h"
#include "minishell.h"

/* ------------ helpers builtin simple ------------- */
static int builtin_echo(char **argv)
{
    printf("DEBUG: builtin_echo appelé\n");
    int i = 1;
    int newline = 1;
    
    if (argv[1] && ft_strcmp(argv[1], "-n") == 0)
    {
        printf("DEBUG: Option -n détectée\n");
        newline = 0;
        i = 2;
    }
    
    printf("DEBUG: Début de l'affichage des arguments\n");
    while (argv[i])
    {
        printf("DEBUG: Affichage de l'argument %d: '%s'\n", i, argv[i]);
        ft_putstr_fd(argv[i], STDOUT_FILENO);
        if (argv[i + 1])
            ft_putchar_fd(' ', STDOUT_FILENO);
        i++;
    }
    
    if (newline)
        ft_putchar_fd('\n', STDOUT_FILENO);
    
    printf("DEBUG: builtin_echo terminé\n");
    return (0);
}

static int builtin_pwd(void)
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

static char **env_to_char(t_list *envl)
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

static int builtin_env(t_list *envl)
{
    t_list *cur = envl;
    while (cur)
    {
        t_env *e = (t_env *)cur->content;
        if (e->exported)
        {
            ft_putstr_fd(e->var, STDOUT_FILENO);
            ft_putchar_fd('=', STDOUT_FILENO);
            ft_putendl_fd(e->value, STDOUT_FILENO);
        }
        cur = cur->next;
    }
    return (0);
}

static int builtin_cd(char **argv, t_list **envl)
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

static int builtin_export(char **argv, t_list **envl)
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

static int builtin_exit(char **argv)
{
    int code = 0;
    if (argv[1])
        code = ft_atoi(argv[1]);
    exit(code);
}

static int run_builtin(t_cmd *cmd, t_list **envl)
{
    if (!cmd || !cmd->args || !cmd->args[0])
        return (0);
    
    int result = -1;
    int fd_out = STDOUT_FILENO;  // Utiliser la sortie standard par défaut
    
    // Adapter l'appel en fonction du builtin_id
    if (cmd->builtin_id == 1 || cmd->builtin_id == 2)  // echo
        result = builtin_echo(cmd, fd_out);
    else if (cmd->builtin_id == 3)  // cd
        result = ft_cd((t_info*)cmd, envl);
    else if (cmd->builtin_id == 4)  // pwd
        result = ft_pwd((t_info*)cmd, envl);
    else if (cmd->builtin_id == 5)  // export
        result = ft_export((t_info*)cmd, envl);
    else if (cmd->builtin_id == 6)  // unset
        result = ft_unset((t_info*)cmd, envl);
    else if (cmd->builtin_id == 7)  // env
        result = ft_env((t_info*)cmd, envl);
    else if (cmd->builtin_id == 8)  // exit
        result = ft_exit((t_info*)cmd, envl);
    
    return (result);
}

/* ------------ redirections --------------- */
static int apply_redirs(t_redir *r)
{
    int fd;
    while (r)
    {
        if (r->type == 0)
            fd = open(r->file, O_RDONLY);
        else if (r->type == 1)
            fd = open(r->file, O_CREAT|O_WRONLY|O_TRUNC, 0644);
        else if (r->type == 2)
            fd = open(r->file, O_CREAT|O_WRONLY|O_APPEND, 0644);
        else
        {
            ft_printf_fd(2, "minishell: heredoc not supported\n");
            return (1);
        }
        if (fd == -1)
        {
            perror(r->file);
            return (1);
        }
        if (r->type == 0 && dup2(fd, STDIN_FILENO) == -1)
            return (perror("dup2"), 1);
        if ((r->type == 1 || r->type == 2) && dup2(fd, STDOUT_FILENO) == -1)
            return (perror("dup2"), 1);
        close(fd);
        r = r->next;
    }
    return (0);
}

/* ------------ execution helpers ------------- */
static int launch_external(char **args, t_list *envl)
{
    // Si c'est un chemin absolu ou relatif
    if (args[0][0] == '/' || (args[0][0] == '.' && args[0][1] == '/'))
    {
        execve(args[0], args, NULL);
        perror(args[0]);
        return (127);
    }
    
    // Chercher dans PATH
    char *path = get_env_value(envl, "PATH");
    if (path)
    {
        // Faire une copie de PATH pour pouvoir la parcourir
        char *path_copy = ft_strdup(path);
        if (!path_copy)
            return (127);
            
        char *current = path_copy;
        char *next_colon;
        char full_path[4096];
        
        // Parcourir chaque répertoire du PATH
        while (current && *current)
        {
            // Trouver le prochain séparateur ":"
            next_colon = ft_strchr(current, ':');
            if (next_colon)
                *next_colon = '\0';
                
            // Construire le chemin complet
            ft_memset(full_path, 0, sizeof(full_path));
            ft_strlcpy(full_path, current, sizeof(full_path));
            ft_strlcat(full_path, "/", sizeof(full_path));
            ft_strlcat(full_path, args[0], sizeof(full_path));
            
            // Tenter d'exécuter
            execve(full_path, args, NULL);
            
            // Si on arrive ici, l'exécution a échoué
            if (!next_colon)
                break;
                
            current = next_colon + 1;
        }
        
        free(path_copy);
    }
    
    // Si on arrive ici, la commande n'a pas été trouvée
    ft_putstr_fd(args[0], STDERR_FILENO);
    ft_putstr_fd(": command not found\n", STDERR_FILENO);
    return (127);
}

static int exec_simple(t_cmd *cmd, t_list **envl)
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
        return (run_builtin(cmd, envl));

    pid_t pid = fork();
    if (pid == -1)
        return (perror("fork"), 1);
    if (pid == 0)
    {
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

static int execute_pipeline(t_cmd *head, t_list **envl)
{
    int in_fd = STDIN_FILENO;
    int last_status = 0;
    int pipefd[2];

    t_cmd *c = head;
    while (c)
    {
        if (c->next)
        {
            if (pipe(pipefd) == -1)
            {
                perror("pipe");
                return (1);
            }
        }
            
        pid_t pid = fork();
        if (pid == -1)
        {
            perror("fork");
            return (1);
        }
            
        if (pid == 0)
        {
            // Gestion des descripteurs
            if (in_fd != STDIN_FILENO)
            {
                dup2(in_fd, STDIN_FILENO);
                close(in_fd);
            }
            if (c->next)
            {
                dup2(pipefd[1], STDOUT_FILENO);
                close(pipefd[0]);
                close(pipefd[1]);
            }
            
            // Appliquer les redirections
            if (apply_redirs(c->redirs))
                exit(1);
                
            // Exécuter la commande
            if (c->builtin_id != -1)
            {
                // C'est un builtin
                int result = run_builtin(c, envl);
                exit(result);
            }
            else if (c->args && c->args[0])
            {
                // Commande externe
                exit(launch_external(c->args, *envl));
            }
            else
            {
                exit(0);  // Aucune commande
            }
        }
        
        // Parent
        if (in_fd != STDIN_FILENO)
            close(in_fd);
        if (c->next)
        {
            close(pipefd[1]);
            in_fd = pipefd[0];
        }
        
        c = c->next;
    }
    
    // Attendre tous les processus enfants
    int status;
    pid_t pid;
    while ((pid = wait(&status)) > 0)
    {
        if (WIFEXITED(status))
            last_status = WEXITSTATUS(status);
        else if (WIFSIGNALED(status))
            last_status = 128 + WTERMSIG(status);
    }
        
    return (last_status);
}
