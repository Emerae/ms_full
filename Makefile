CC      = cc
CFLAGS  = -Wall -Wextra -Werror
NAME    = minishell
LIBFT_DIR = ./libft
LIBFT   = $(LIBFT_DIR)/libft.a
INCLUDES= -I./includes -I$(LIBFT_DIR)/includes

SRCS    = sources/main.c \
          sources/execute_new/env_utils.c \
          sources/execute_new/execute_new.c \
          sources/execute_new/free_cmd.c \
          sources/execute_new/args.c \
          sources/execute_new/useful.c \
          sources/parser_new/env.c \
          sources/parser_new/parse_command_new.c \
          sources/parser_new/parser/pars/cy0_check_char.c \
          sources/parser_new/parser/pars/cy0_freeer.c \
          sources/parser_new/parser/pars/cy0_init_env.c \
          sources/parser_new/parser/pars/cy1_1_remove_space_nodes.c \
          sources/parser_new/parser/pars/cy1_env.c \
          sources/parser_new/parser/pars/cy1_input_list.c \
          sources/parser_new/parser/pars/cy1_input_list1.c \
          sources/parser_new/parser/pars/cy1_input_list2.c \
          sources/parser_new/parser/pars/cy2_1_fill_builtin.c \
          sources/parser_new/parser/pars/cy2_2_fill_redir.c \
          sources/parser_new/parser/pars/cy2_2_fill_redir2.c \
          sources/parser_new/parser/pars/cy2_3_free_first_node.c \
          sources/parser_new/parser/pars/cy2_convert_cmd.c \
          sources/parser_new/parser/pars/cy2_convert_cmd2.c \
          sources/parser_new/parser/pars/cy2_convert_cmd3.c \
          sources/parser_new/parser/pars/cy3_2_dollar.c \
          sources/parser_new/parser/pars/cy3_2_dollar2.c \
          sources/parser_new/parser/pars/cy3_2_dollar_alone.c \
          sources/parser_new/parser/pars/cy3_2_dollar_bang.c \
          sources/parser_new/parser/pars/cy3_2_dollar_braces.c \
          sources/parser_new/parser/pars/cy3_2_dollar_braces2.c \
          sources/parser_new/parser/pars/cy3_2_dollar_braces3.c \
          sources/parser_new/parser/pars/cy3_2_dollar_word.c \
          sources/parser_new/parser/pars/cy3_2_dollar_word2.c \
          sources/parser_new/parser/pars/cy3_subti_check.c \
          sources/parser_new/parser/pars/cy3_subti_fuse.c \
          sources/parser_new/parser/pars/cy4_1wrong_char.c \
          sources/parser_new/parser/pars/cy4_2wrong_redir.c \
          sources/parser_new/parser/pars/cy4_3wrong_pipe.c \
          sources/parser_new/parser/pars/cy4_4wrong_redir_log.c \
          sources/parser_new/parser/pars/cy4_5wrong_pipe_log.c \
          sources/parser_new/parser/pars/utils.c \
          sources/parser_new/parser/cyutil/cy_strchr.c \
          sources/parser_new/parser/cyutil/cy_strcmp.c \
          sources/parser_new/parser/cyutil/cy_strdup.c \
          sources/parser_new/parser/cyutil/cy_strlen.c \
          sources/parser_new/parser/cyutil/cy_strlcat.c \
          sources/parser_new/parser/cyutil/cy_strlcpy.c \
          sources/parser_new/parser/cyutil/cy_true_strdup.c \
          sources/parser_new/parser/cyutil/cy_memset.c \
          sources/expand/expansion.c \
          sources/expand/join.c \
          sources/expand/main.c \
          sources/expand/useful.c \
          sources/environment/declare.c \
          sources/environment/env.c \
          sources/environment/replace.c \
          sources/environment/update.c \
          sources/environment/useful.c \
          sources/utilities/errors.c \
          sources/utilities/free.c \
          sources/utilities/prompt.c \
          sources/utilities/reader.c \
          sources/utilities/reader_useful.c

OBJ     = $(SRCS:.c=.o)

all: $(NAME)

$(LIBFT):
	$(MAKE) -C $(LIBFT_DIR)

$(NAME): $(LIBFT) $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) $(LIBFT) -lreadline -o $(NAME)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	$(MAKE) -C $(LIBFT_DIR) clean
	rm -f $(OBJ)

fclean: clean
	$(MAKE) -C $(LIBFT_DIR) fclean
	rm -f $(NAME)

re: fclean all
