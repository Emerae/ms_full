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
    char *path = argv[1];
    if (!path)
        path = get_env_value(*envl, "HOME");
    if (!path)
    {
        ft_putendl_fd("minishell: cd: HOME not set", STDERR_FILENO);
        return (1);
    }
    char old[4096];
    if (!getcwd(old, sizeof(old)))
        old[0] = '\0';
    if (chdir(path) == -1)
    {
        perror("cd");
        return (1);
    }
    add_env("OLDPWD", old, envl, 1);
    char cwd[4096];
    if (getcwd(cwd, sizeof(cwd)))
        add_env("PWD", cwd, envl, 1);
    return (0);
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
    char **av = cmd->args;
    int result = -1;  // Valeur par défaut pour indiquer que ce n'est pas un builtin

    printf("DEBUG: run_builtin appelé avec cmd=%p\n", cmd);
    if (cmd && cmd->args) {
        printf("DEBUG: cmd->args[0]=%s\n", cmd->args[0]);
        if (cmd->args[1])
            printf("DEBUG: cmd->args[1]=%s\n", cmd->args[1]);
    }
    if (!av || !av[0])
        return (0);
    if (ft_strcmp(av[0], "echo") == 0)
        result = builtin_echo(av);
    else if (ft_strcmp(av[0], "pwd") == 0)
        result = builtin_pwd();
    else if (ft_strcmp(av[0], "env") == 0)
        result = builtin_env(*envl);
    else if (ft_strcmp(av[0], "exit") == 0)
        result = builtin_exit(av); /* never returns */
    else if (ft_strcmp(av[0], "cd") == 0)
        result = builtin_cd(av, envl);
    /* TODO export unset minimal handling */
    printf("DEBUG: run_builtin terminé avec result=%d\n", result);
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
static int launch_external(char **argv, t_list *envl)
{
    char **envp = env_to_char(envl);
    execve(argv[0], argv, envp);
    perror(argv[0]);
    /* free envp */
    for (int i=0; envp[i]; i++)
        free(envp[i]);
    free(envp);
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

    for (t_cmd *c = head; c; c = c->next)
    {
        if (c->next && pipe(pipefd) == -1)
            return (perror("pipe"), 1);
        pid_t pid = fork();
        if (pid == -1)
            return (perror("fork"), 1);
        if (pid == 0)
        {
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
            if (apply_redirs(c->redirs))
                exit(1);
            if (run_builtin(c, envl) != -1)
                exit(0);
            exit(launch_external(c->args, *envl));
        }
        if (in_fd != STDIN_FILENO)
            close(in_fd);
        if (c->next)
        {
            close(pipefd[1]);
            in_fd = pipefd[0];
        }
    }
    int status;
    while (wait(&status) > 0)
        last_status = WIFEXITED(status) ? WEXITSTATUS(status) : 128 + WTERMSIG(status);
    return (last_status);
}

/* ------------ public API ------------- */
int execute_cmds(t_cmd *cmds, t_list **envl, int *last_status)
{
    printf("DEBUG: execute_cmds appelé avec cmds=%p\n", cmds);
    if (cmds && cmds->args) {
        printf("DEBUG: Premier argument: %s\n", cmds->args[0]);
        if (cmds->args[1])
            printf("DEBUG: Deuxième argument: %s\n", cmds->args[1]);
    }

    if (!cmds)
        return ((*last_status = 1), 1);
    if (cmds->next)
        *last_status = execute_pipeline(cmds, envl);
    else
        *last_status = exec_simple(cmds, envl);
    
    printf("DEBUG: execute_cmds terminé avec status=%d\n", *last_status);
    return (*last_status);
}