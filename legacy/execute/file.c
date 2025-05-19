
#include "minishell.h"
/**
 * @brief Joins a directory path with a command name
 *
 * This function creates a path by joining a directory path with a command name.
 * It first joins the directory path with a "/" separator, then appends the command name.
 * If the destination string already exists, it's freed first to prevent memory leaks.
 *
 * @param to_join The directory path to join with
 * @param file Pointer to the string where the result will be stored
 * @param cmd Structure containing command information, including the command name
 */
static void	join_path(char *to_join, char **file, t_info *cmd);

/**
 * @brief Searches for an executable in PATH directories
 *
 * This function attempts to locate an executable file by searching through
 * all directories specified in the PATH environment variable. For each directory,
 * it constructs a full path by joining the directory with the command name,
 * then tries to open the file to check if it exists and is accessible.
 *
 * @param cmd Structure containing command information
 * @param envl Linked list containing environment variables
 * @param file Pointer to store the complete path of the executable if found
 *
 * @return SUCCESS (typically 0) if executable is found, ERROR otherwise
 */
int			open_executable(t_info *cmd, t_list *envl, char **file);

static void	join_path(char *to_join, char **file, t_info *cmd)
{
	char	*tmp;

	tmp = ft_strjoin(to_join, "/");
	if (*file)
		free(*file);
	*file = ft_strjoin(tmp, cmd->argv[cmd->offset]);
	free(tmp);
}

int			open_executable(t_info *cmd, t_list *envl, char **file)
{
	char	**path_list;
	char	*path;
	int		i;
	int		fd;

	*file = NULL;
	path = search_in_env(envl, "PATH");
	if (!path)
		return (ERROR);
	path_list = ft_split(path, ":=");
	fd = -1;
	i = 0;
	while (path_list[i] && fd < 0)
	{
		join_path(path_list[i], file, cmd);
		fd = open(*file, O_RDONLY);
		i++;
	}
	free_tab(path_list);
	if (fd > 0)
	{
		close(fd);
		return (SUCCESS);
	}
	return (ERROR);
}
