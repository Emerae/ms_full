#include "../prser.h"

void	local_free_env(char **env)
{
	int	i;

	i = 0;
	if (!env)
		return ;
	while (env[i])
	{
		free(env[i]);
		i = i + 1;
	}
	free(env);
}

static int	cy00_modify_env_1(char **env, char **new_env, int count, int mode)
{
	int	i;

	i = 0;
	while ((mode == 0 && i < count) || (mode != 0 && i < count - 1))
	{
		new_env[i] = cy_true_strdup(env[i]);
		if (!new_env[i])
		{
			local_free_env(new_env);
			return (-1);
		}
		i = i + 1;
	}
	return (i);
}

static int	cy00_modify_env_2(char **new_env, int i, int mode)
{
	if (mode == 0 || mode == 1)
	{
		new_env[i] = cy_true_strdup("minishell0");
		if (!new_env[i])
			return (-1);
	}
	else if (mode == -1)
	{
		new_env[i] = cy_true_strdup("minishell1");
		if (!new_env[i])
			return (-1);
	}
	return (0);
}

static char	**cy00_modify_env_3(int count, int mode)
{
	if (mode == 0)
		return (malloc(sizeof(char *) * (count + 2)));
	return (malloc(sizeof(char *) * (count + 1)));
}

int	cy00_modify_env(char ***env_ptr, int mode)
{
	char	**env;
	char	**new_env;
	int		count;
	int		i;

	env = *env_ptr;
	if (!env)
		return (1);
	count = 0;
	while (env[count])
		count = count + 1;
	new_env = cy00_modify_env_3(count, mode);
	if (!new_env)
		return (1);
	i = cy00_modify_env_1(env, new_env, count, mode);
	if (i == -1 || cy00_modify_env_2(new_env, i, mode) == -1)
	{
		local_free_env(new_env);
		return (1);
	}
	i = i + 1;
	new_env[i] = NULL;
	local_free_env(env);
	*env_ptr = new_env;
	return (0);
}

// mode 0 add line, mode -1 replace if fail, mode 1 replace if ok