/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: seerel <seerel@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/11 16:45:34 by bucolak           #+#    #+#             */
/*   Updated: 2025/08/10 02:40:36 by seerel           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <stdlib.h>

static void	free_and_null(void **ptr)
{
	if (ptr && *ptr)
	{
		free(*ptr);
		*ptr = NULL;
	}
}

void	remove_env_node(t_env **env, t_env *current, t_env *prev)
{
	if (!prev)
		*env = current->next;
	else
		prev->next = current->next;
	free_and_null((void **)&current->key);
	free_and_null((void **)&current->data);
	free_and_null((void **)&current);
}

void	remove_env_var(t_env **env, char *var_name)
{
	t_env	*current;
	t_env	*prev;
	size_t	name_len;

	if (!env || !*env || !var_name)
		return ;
	name_len = ft_strlen(var_name);
	current = *env;
	prev = NULL;
	while (current)
	{
		if (ft_strcmp(current->key, var_name) == 0 && (!current->key[name_len]
				|| current->key[name_len] == '='))
		{
			remove_env_node(env, current, prev);
			return ;
		}
		prev = current;
		current = current->next;
	}
}

void	unset_cmd(t_general *list, t_env **env)
{
	int	i;

	i = 1;
	while (list->acces_args->args[i])
	{
		if (list->acces_args->args[i]->str)
			remove_env_var(env, list->acces_args->args[i]->str);
		i++;
	}
	list->dqm = 0;
}
