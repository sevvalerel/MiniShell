/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection_first_2.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: seerel <seerel@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/10 07:37:13 by seerel            #+#    #+#             */
/*   Updated: 2025/08/10 07:38:10 by seerel           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	handle_output(t_general *list, int i)
{
	int		fd;
	char	*last_input;

	if (ft_strcmp(list->acces_args->args[i]->str, ">") == 0)
	{
		if (!list->acces_args->args[i + 1])
			error_msg(2, NULL, 3, list);
		last_input = list->acces_args->args[++i]->str;
		check_file_permissions(last_input, list);
		open_outfile(last_input, &fd, list);
		dup2(fd, 1);
		close(fd);
	}
}

void	close_if_open(int fd)
{
	if (fd != -1)
		close(fd);
}

void	exit_with_error(t_general *list, int code)
{
	int	exit_code;

	exit_code = code;
	free_pipe_blocks(list);
	exit(exit_code);
}

int	open_infile(t_general *list, char *filename)
{
	int	fd;

	if (access(filename, F_OK) != 0)
	{
		error_msg(2, filename, 0, list);
		exit_with_error(list, list->dqm);
	}
	if (access(filename, R_OK) != 0)
	{
		ft_putstr_fd("bash: ", 2);
		ft_putstr_fd(filename, 2);
		ft_putstr_fd(": Permission denied\n", 2);
		list->dqm = 1;
		exit_with_error(list, list->dqm);
	}
	fd = open(filename, O_RDONLY, 0644);
	if (fd < 0)
	{
		error_msg(0, filename, 0, list);
		list->dqm = 1;
		exit_with_error(list, list->dqm);
	}
	return (fd);
}

void	handle_input(t_general *list, int i)
{
	int		fd;
	char	*last_input;
	int		last_fd;

	last_fd = -1;
	if (ft_strcmp(list->acces_args->args[i]->str, "<") == 0)
	{
		if (!list->acces_args->args[i + 1])
		{
			error_msg(2, NULL, 3, list);
			exit_with_error(list, list->dqm);
		}
		last_input = list->acces_args->args[++i]->str;
		fd = open_infile(list, last_input);
		close_if_open(last_fd);
		last_fd = fd;
	}
	if (last_fd != -1)
	{
		dup2(last_fd, 0);
		close(last_fd);
	}
}
