/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection_second_2.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: seerel <seerel@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/10 08:37:15 by seerel            #+#    #+#             */
/*   Updated: 2025/08/10 08:55:18 by seerel           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	count_arg(t_arg **args)
{
	int	i;

	i = 0;
	while (args[i])
		i++;
	return (i);
}

void	fill_new_args(t_arg **args, t_arg ***new_arg)
{
	int	i;
	int	j;

	i = 0;
	j = 0;
	while (args[i])
	{
		if (ft_strcmp(args[i]->str, "<<") == 0)
		{
			i += 2;
			continue ;
		}
		(*new_arg)[j++] = args[i++];
	}
	(*new_arg)[j] = NULL;
}

void	free_old_args_heredoc(t_arg **old_arg)
{
	int	i;

	i = 0;
	while (old_arg[i])
	{
		if (ft_strcmp(old_arg[i]->str, "<<") == 0)
		{
			free(old_arg[i]->str);
			free(old_arg[i]);
			i++;
			if (old_arg[i])
			{
				free(old_arg[i]->str);
				free(old_arg[i]);
				i++;
			}
			continue ;
		}
		i++;
	}
	free(old_arg);
}

void	remove_heredoc(t_general *list)
{
	int		size;
	t_arg	**old_arg;

	if (!list || !list->acces_args || !list->acces_args->args)
		return ;
	old_arg = list->acces_args->args;
	size = count_arg(old_arg);
	list->acces_args->args = malloc(sizeof(t_arg *) * (size + 1));
	if (!list->acces_args->args)
		return ;
	fill_new_args(old_arg, &(list->acces_args->args));
	free_old_args_heredoc(old_arg);
}

void	handle_heredoc(t_general *list)
{
	t_general	*tmp;

	tmp = list;
	while (tmp)
	{
		if (process_heredoc_one(tmp))
			return ;
		remove_heredoc(tmp);
		tmp = tmp->next;
	}
}
