/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection_first.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: seerel <seerel@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/09 12:51:00 by bucolak           #+#    #+#             */
/*   Updated: 2025/08/10 07:38:54 by seerel           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	is_redirection(char *str)
{
	int		i;
	char	*redirection[5];

	redirection[0] = "<";
	redirection[1] = "<<";
	redirection[2] = ">";
	redirection[3] = ">>";
	redirection[4] = NULL;
	i = 0;
	while (redirection[i])
	{
		if (ft_strcmp(redirection[i], str) == 0)
			return (1);
		i++;
	}
	return (0);
}

void	renew_else_block(t_arg ***new, t_general *tmp, int *i, int *j)
{
	(*new)[*j] = malloc(sizeof(t_arg));
	(*new)[*j]->str = ft_strdup(tmp->acces_args->args[*i]->str);
	(*new)[*j]->flag = tmp->acces_args->args[*i]->flag;
	(*new)[*j]->s = tmp->acces_args->args[*i]->s;
}

void	renew_block2(t_general *list)
{
	int			i;
	t_arg		**new;
	t_general	*tmp;
	int			j;

	i = 0;
	j = 0;
	tmp = list;
	while (tmp->acces_args->args[i])
		i++;
	new = ft_calloc(sizeof(t_arg *), (i + 1));
	i = 0;
	while (tmp->acces_args->args[i])
	{
		if (is_redirection(tmp->acces_args->args[i]->str) == 1)
			i += 2;
		else
		{
			renew_else_block(&new, tmp, &i, &j);
			j++;
			i++;
		}
	}
	new[j] = NULL;
	tmp->acces_args->args = new;
}

void	check_file_permissions(char *file, t_general *list)
{
	if (access(file, F_OK) != 0)
		error_msg(2, file, 0, list);
	if (access(file, W_OK) != 0)
	{
		ft_putstr_fd("bash: ", 2);
		ft_putstr_fd(file, 2);
		ft_putstr_fd(": Permission denied\n", 2);
		list->dqm = 1;
		free_pipe_blocks(list);
		exit(list->dqm);
	}
}

void	open_outfile(char *file, int *fd, t_general *list)
{
	*fd = open(file, O_CREAT | O_WRONLY | O_TRUNC, 0644);
	if (*fd < 0)
		error_msg(2, file, 0, list);
}
