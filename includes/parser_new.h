#ifndef PARSER_NEW_H
#define PARSER_NEW_H

#include <stdlib.h>
#include <limits.h>
#include "libftfull.h"
#include "structures.h"

/* ===== data structures from original prser.h ===== */
typedef struct s_input
{
    char            *input;
    char            *input_type;
    int             input_num;
    int             number;
    int             type; /* 1 space, 2 text, 3 single, 4 double */
    struct s_input  *prev;
    struct s_input  *next;
}   t_input;

typedef struct s_redir
{
    int             type;   /* 0 <, 1 >, 2 >>, 3 << */
    char            *file;
    struct s_redir  *next;
}   t_redir;

typedef struct s_cmd
{
    char            **args;
    t_redir         *redirs;
    int             builtin_id;
    struct s_cmd    *next;
}   t_cmd;

/* ===== prototypes (from minifun parser) ===== */
t_input *cy1_make_list(char *line);
t_cmd   *cy2_convert_cmd(t_input **head);
int     cy3_substi_check(t_input **head, t_list *env);
int     cy4_1wrong_char(t_input *head);
void    cy0_free_input_list(t_input *head);
void    free_cmd_list(t_cmd *cmd);
void    cy1_remove_space_nodes(t_input **head);

/* ===== cyutil functions ===== */
char    *cy_strdup(char *s, int start, int end);
char    *cy_true_strdup(char *s);
int     cy_strlcpy(char *dst, char *src, int siz);
int     cy_strlen(char *str);
int     cy_strlen2(const char *s);
int     cy_strcmp(const char *s1, const char *s2);
void    *cy_memset(void *s, int c, size_t n);
size_t  cy_strlcat(char *dst, const char *src, size_t siz);
char    *cy_strchr(const char *s, int c);
int     cy_strncmp(const char *s1, const char *s2, size_t n);

/* ===== additional parser functions ===== */
int     cy1_identify_end(char *input, int *start);
int     cy1_append_input(t_input **head, int start, int end, char *input);
int     cy0_analyse_char(char c);
int     cy0_analyse_char2(char c);

/* wrapper */
t_cmd   *parse_command_new(char *line, t_list *env, int *status);

#endif
