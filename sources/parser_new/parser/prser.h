#ifndef PARSER_H
# define PARSER_H

# include <stdlib.h>
# include <unistd.h>
# include <stdio.h>
# include <string.h>
# include <sys/stat.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <readline/rltypedefs.h>
# include <limits.h>
# include <fcntl.h>

typedef struct s_redir
{
	int				type;		// 0: < | 1: > | 2: >> | 3: <<
	char			*file;
	struct s_redir	*next;
}	t_redir;

typedef struct s_cmd
{
	char			**args;		// ["ls", "-l", NULL]
	t_redir			*redirs;	// liste de redirections
	int				builtin_id;	// -1 si c’est pas un builtin
	struct s_cmd	*next;		// pour les pipes
}	t_cmd;

typedef struct s_input
{
	char			*input;
	char			*input_type;
	char			*input_num;
	struct s_input	*prev;
	struct s_input	*next;
	int				number;
	int				type; //1 space, 2 txt, 3 ' ', 4 " "
}	t_input;

typedef struct s_fill_redir
{
	t_input		*node;
	t_redir		*head;
	t_redir		*last;
	int			nb_skip_head;
	int			flag;
}	t_fill_redir;

typedef struct s_cmdconvert
{
	t_cmd	*head_cmd;
	t_cmd	*current_cmd;
	t_input	*head_input;
	t_input	*current_input;
	int		n_delimiter;
	int		nature_delimiter;
	int		skip_nb;
}	t_cmdconvert;

typedef struct s_dollar_alone
{
	int		lold;
	int		k;
	int		p;
	char	*new_input;
	char	*new_type;
	char	*new_num;
}	t_dollar_alone;

typedef struct s_dollar_bang
{
	char	*last_env;
	int		e;
	int		vlen;
	int		lold;
	char	*new_input;
	char	*new_type;
	char	*new_num;
	int		replaced_len;
	int		p;
	int		k;
}	t_dollar_bang;

typedef struct s_dollar_braces
{
	int		len;
	char	*new_input;
	char	*new_type;
	char	*new_num;
	int		k;
	int		e;
	char	key[256];
	int		keylen;
	char	*equal;
	char	*value;
	int		vlen;
	int		lold;
	int		p;
}	t_dollar_braces;

// typedef struct s_dollar_word {
// 	char	*new_input;
// 	char	*new_type;
// 	char	*new_num;
// 	char	*value;
// 	int		lold;
// 	int		vlen;
// 	int		p;
// 	int		k;
// 	int		i;
// 	int		j;
// 	int		e;
// } t_dollar_word;

typedef struct s_dollar_word
{
	char	*new_input;
	char	*new_type;
	char	*new_num;
	char	*value;
	int		lold;
	int		vlen;
	int		p;
	int		k;
	int		i;
	int		j;
	int		e;
	char	key[256];
	int		keylen;
}	t_dollar_word;

char	*cy_strdup(char *s, int start, int end);
char	*cy_true_strdup(char *s);
int		cy_strlcpy(char *dst, char *src, int siz);
int		cy_strlen(char *str);
int		cy_strlen2(const char *s);
int		cy_strcmp(const char *s1, const char *s2);
void	*cy_memset(void *s, int c, size_t n);
size_t	cy_strlcat(char *dst, const char *src, size_t siz);
char	*cy_strchr(const char *s, int c);
int		cy_strncmp(const char *s1, const char *s2, size_t n);

void	print_input_list(t_input *head_input);

char	**cy0_copy_env_from_proc(void);
// relicat ?
// char	**cy0_init_env(void);
int		cy0_analyse_char(char c);
int		cy0_analyse_char2(char c);
void	cy0_free_input_list(t_input *head);
void	cy0_free_env(char **env, int i);
void	cy00_free_env(char **env);
void	cy0_free_cmd_list(t_cmd *cmd);
int		cy00_modify_env(char ***env_ptr, int mode);

t_input	*cy1_make_list(char *input);
int		cy1_identify_end(char *input, int *start);
int		cy1_append_input(t_input **head, int start, int end, char *input);
t_input	*cy1_create_node(int start, int end, char *input);
void	cy1_remove_space_nodes(t_input **head_input);

t_cmd	*cy2_convert_cmd(t_input *head_input);
int		cy2_convert_cmd2(t_cmdconvert *c);
int		append_cmd(t_cmd **current_cmd, int n_delimiter, t_input **head_input);
int		find_delim(t_input **current_input, int *nature);
void	cy2_fill_builtin_id(t_cmd **current_cmd);
int		cy2_fill_redir(t_cmd **current_cmd,
			t_input **current_input, int *nature);
int		cy2_fill_redir_1(t_input *node, t_redir **head, t_redir **last);
int		cy2_fill_redir_2(t_input *node, int *nature, int *flag);
int		cy2_fill_redir_loop_body(t_fill_redir *s, int *nature);
void	cy2_free_first_cmd_node(t_cmd **head);

int		cy3_substi_check(t_input **head_input, char **env);
int		cy3_fuse_nospace(t_input *head);
int		cy3_scan_dollar_syntax(t_input *head, char **env);
int		cy3_scan_dollar_syntax_1_1_1(t_input *current, int i);
int		cy3_scan_dollar_syntax_1_1_2(t_input *current, int i, int j);
int		cy3_scan_dollar_syntax_1_1(t_input *current, int i, char **env);
int		cy3_scan_dollar_syntax_2_1_1(t_input *current, int i, int j);
int		cy3_scan_dollar_syntax_2_1(t_input *current, int i, char **env);
int		cy3_scan_dollar_syntax_2(t_input *current, int i, char **env);
int		cy3_scan_dollar_syntax_dollar_1(t_input *current,
			int *i, char **env);
int		cy3_scan_dollar_syntax_dollar_2(t_input *current,
			int *i, char **env);
int		cy3_scan_dollar_syntax_dollar(t_input *current,
			int *i, char **env);
int		cy3_handle_dollar_brace(t_input *current, int i, int j, char **env);
int		cy3_handle_dollar_brace1(t_input *current,
			int i, int j, t_dollar_braces *s);
int		cy3_handle_dollar_brace1a(t_input *current, int i, t_dollar_braces *s);
void	cy3_handle_dollar_brace1b(t_input *current, int j, t_dollar_braces *s);
void	cy3_handle_dollar_brace2(t_dollar_braces *s,
			t_input *current, int i, int j);
void	cy3_handle_dollar_brace3(t_dollar_braces *s, char **env);
int		cy3_handle_dollar_brace4(t_input *current,
			int i, int j, t_dollar_braces *s);
int		cy3_handle_dollar_brace5(t_input *current,
			int i, int j, t_dollar_braces *s);
void	cy3_handle_dollar_brace6(t_input *current, t_dollar_braces *s);
void	cy3_handle_dollar_brace7(t_input *current, int i, t_dollar_braces *s);
void	cy3_handle_dollar_brace8(t_input *current, int i, t_dollar_braces *s);
void	cy3_handle_dollar_brace9(t_input *current, int j, t_dollar_braces *s);

int		cy3_handle_dollar_word(t_input *current, int i, int j, char **env);
void	cy3_handle_dollar_word_key(t_input *current, t_dollar_word *s);
void	cy3_handle_dollar_word_key(t_input *current, t_dollar_word *s);
void	cy3_handle_dollar_word_findenv(t_dollar_word *s, char **env);
int		cy3_handle_dollar_word_1(t_input *current,
			char **env, t_dollar_word *s);
int		cy3_handle_dollar_word_2(t_input *current, t_dollar_word *s);
int		cy3_handle_dollar_word_2a(t_input *current, t_dollar_word *s);
int		cy3_handle_dollar_word_2b(t_input *current, t_dollar_word *s);
int		cy3_handle_dollar_word_3(t_input *current, int i, int j);
int		cy3_handle_dollar_word_3a(t_input *current, int i, t_dollar_word *s);
int		cy3_handle_dollar_word_3b(t_input *current, int j, t_dollar_word *s);
int		cy3_handle_dollar_alone(t_input *current, int i);
int		cy3_handle_dollar_bang(t_input *current, int i, int j, char **env);

int		cy4_1wrong_char(t_input *head);
int		cy4_2wrong_redir(t_input *head);
int		cy4_3wrong_pipe(t_input *head);
int		cy4_4wrong_redir_log(t_input *head);
int		cy4_5wrong_pipe_log(t_input *head);

// A delet
void	print_cmd_list(t_cmd *head_cmd);
void	print_input_list(t_input *head_input);

#endif