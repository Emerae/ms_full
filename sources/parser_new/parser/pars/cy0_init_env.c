#include "../prser.h"

void	print_env(char **env)
{
	int	i;

	i = 0;
	while (env[i])
	{
		printf("l%d:%s\n", i, env[i]);
		i = i + 1;
	}
}

static int	cy0_count_env_vars(char *buffer, long int size)
{
	int	i;
	int	nb_var;

	i = 0;
	nb_var = 0;
	while (i < size)
	{
		if (buffer[i] == '\0')
			nb_var = nb_var + 1;
		i = i + 1;
	}
	return (nb_var);
}

static char	**cy0_copy_env_from_proc_1(char *buffer, int nb_var)
{
	char	**env;
	int		i;
	char	*ptr;

	env = malloc(sizeof(char *) * (nb_var + 1));
	if (!env)
		return (NULL);
	i = 0;
	ptr = buffer;
	while (i < nb_var)
	{
		env[i] = cy_true_strdup(ptr);
		if (!env[i])
		{
			cy0_free_env(env, i);
			return (NULL);
		}
		ptr = ptr + cy_strlen(ptr) + 1;
		i = i + 1;
	}
	env[i] = NULL;
	print_env(env);
	return (env);
}

char	**cy0_copy_env_from_proc(void)
{
	int			fd;
	long int	size;
	char		buffer[100000];
	int			nb_var;

	fd = open("/proc/self/environ", O_RDONLY);
	if (fd == -1)
		return (NULL);
	size = read(fd, buffer, sizeof(buffer) - 1);
	close(fd);
	if (size <= 0)
		return (NULL);
	buffer[size] = '\0';
	nb_var = cy0_count_env_vars(buffer, size);
	return (cy0_copy_env_from_proc_1(buffer, nb_var));
}
