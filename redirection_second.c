/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection_second.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: seerel <seerel@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/10 08:53:55 by seerel            #+#    #+#             */
/*   Updated: 2025/08/10 08:55:04 by seerel           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	count_heredoc_limiters(t_general *list)
{
	int	i;
	int	count;

	i = 0;
	count = 0;
	while (list->acces_args->args[i])
	{
		if (ft_strcmp(list->acces_args->args[i]->str, "<<") == 0)
			count++;
		i++;
	}
	return (count);
}

static void	fill_limiter(t_general *list)
{
	int	i;
	int	j;
	int	count;

	if (!list || !list->acces_args || !list->acces_args->args)
		return ;
	count = count_heredoc_limiters(list);
	list->limiter = malloc(sizeof(char *) * (count + 1));
	if (!list->limiter)
		return ;
	i = 0;
	j = 0;
	while (list->acces_args->args[i])
	{
		if (ft_strcmp(list->acces_args->args[i]->str, "<<") == 0
			&& list->acces_args->args[i + 1])
			list->limiter[j++] = ft_strdup(list->acces_args->args[i + 1]->str);
		i++;
	}
	list->limiter[j] = NULL;
}

static void	read_heredoc_line(int write_fd, char *limiter)
{
	char	*line;

	while (1)
	{
		line = readline("heredoc > ");
		if (!line || ft_strcmp(line, limiter) == 0)
		{
			free(line);
			break ;
		}
		ft_putstr_fd(line, write_fd);
		ft_putstr_fd("\n", write_fd);
		free(line);
	}
}

static int	handle_one_redirect(t_general *tmp, int *i, int *j)
{
	int	fd[2];

	if (!tmp->acces_args->args[*i + 1])
	{
		ft_putstr_fd("bash: syntax error near unexpected token `newline'\n", 2);
		return (1);
	}
	pipe(fd);
	read_heredoc_line(fd[1], tmp->limiter[*j]);
	close(fd[1]);
	(*j)++;
	if (!tmp->limiter[*j])
		tmp->heredoc_fd = dup(fd[0]);
	close(fd[0]);
	*i += 2;
	return (0);
}

static int	process_heredoc_one(t_general *tmp)
{
	int	i;
	int	j;

	i = 0;
	j = 0;
	fill_limiter(tmp);
	while (tmp->acces_args->args[i])
	{
		if (ft_strcmp(tmp->acces_args->args[i]->str, "<<") == 0)
		{
			if (handle_one_redirect(tmp, &i, &j))
				return (1);
		}
		else
			i++;
	}
	return (0);
}
