/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helper_function.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: seerel <seerel@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/24 14:45:21 by seerel            #+#    #+#             */
/*   Updated: 2025/08/24 14:50:08 by seerel           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int		signal_ec = 0;

int	is_in_quotes(const char *line, int pos)
{
	int		i;
	int		in_quotes;
	char	quote_type;

	i = 0;
	in_quotes = 0;
	quote_type = 0;
	while (i < pos)
	{
		if ((line[i] == '"' || line[i] == '\'') && !in_quotes)
		{
			in_quotes = 1;
			quote_type = line[i];
		}
		else if (line[i] == quote_type)
		{
			in_quotes = 0;
			quote_type = 0;
		}
		i++;
	}
	return (in_quotes);
}
void	pipe_parse(t_general **pipe_block, char *line)
{
	int			i;
	int			start;
	t_general	*tmp;
	char		*trimmed;

	i = 0;
	start = 0;
	tmp = *pipe_block;
	while (line[i])
	{
		if (line[i] == '|' && !is_in_quotes(line, i))
		{
			trimmed = ft_substr(line, start, i - start);
			tmp->blocs = ft_strtrim(trimmed, " ");
			free(trimmed);
			tmp->next = create_general_node(tmp->dqm);
			tmp = tmp->next;
			start = i + 1;
		}
		i++;
	}
	trimmed = ft_substr(line, start, i - start);
	tmp->blocs = ft_strtrim(trimmed, " ");
	free(trimmed);
}

int	has_redireciton(t_general *pipe_blocks)
{
	int	i;

	i = 0;
	while (pipe_blocks->acces_args->args[i])
	{
		if ((ft_strcmp(pipe_blocks->acces_args->args[i]->str, "<") == 0
				|| ft_strcmp(pipe_blocks->acces_args->args[i]->str, "<<") == 0
				|| ft_strcmp(pipe_blocks->acces_args->args[i]->str, ">") == 0
				|| ft_strcmp(pipe_blocks->acces_args->args[i]->str, ">>") == 0)
			&& (pipe_blocks->acces_args->args[i]->flag == 5))
			return (1);
		i++;
	}
	return (0);
}

void	signal_handler(void)
{
	signal(SIGINT, handle_signal);
	signal(SIGQUIT, SIG_IGN);
}

int	has_heredoc(t_general *list)
{
	int	i;

	i = 0;
	if (!list || !list->acces_args || !list->acces_args->args)
		return (0);
	while (list->acces_args->args[i])
	{
		if (list->acces_args->args[i]->str
			&& ft_strcmp(list->acces_args->args[i]->str, "<<") == 0
			&& (list->acces_args->args[i]->flag == 2
				|| list->acces_args->args[i]->flag == 5))
			return (1);
		i++;
	}
	return (0);
}

void	init_pipe(t_pipe *pipe, t_general *list)
{
	pipe->count = 0;
	pipe->tmp = list;
	while (pipe->tmp)
	{
		pipe->count++;
		pipe->tmp = pipe->tmp->next;
	}
	pipe->fd = malloc(sizeof(int *) * (pipe->count - 1));
	pipe->pid = malloc(sizeof(pid_t) * pipe->count);
}

void	create_pipe(int count, int **fd)
{
	int	i;

	i = 0;
	while (i < count - 1)
	{
		fd[i] = malloc(sizeof(int) * 2);
		if (pipe(fd[i]) == -1)
		{
			perror("pipe");
			exit(1);
		}
		i++;
	}
}

int	is_flag_6(t_general *list, t_env *env)
{
	t_env		*tenv;
	int			i;
	t_general	*tmp;

	tmp = list;
	while (tmp)
	{
		i = 0;
		while (tmp->acces_args->args[i])
		{
			tenv = env;
			if (is_redirection(tmp->acces_args->args[i]->str) == 1
				&& (tmp->acces_args->args[i]->flag == 5
					|| tmp->acces_args->args[i]->flag == 2))
			{
				while (tenv)
				{
					if (ft_strcmp(tenv->data,
							tmp->acces_args->args[i]->str) == 0 && tenv->f == 3)
					{
						return (1);
					}
					tenv = tenv->next;
				}
			}
			i++;
		}
		tmp = tmp->next;
	}
	return (0);
}

int	count_m(t_general *tmp, int i, t_env *env)
{
	int		j;
	int		c;
	int		start;
	char	*str;
	char	*a;

	c = 0;
	j = 0;
	if (!tmp->acces_args->args[i]->str || !tmp->acces_args->args[i]->str[0])
		return (0);
	while (tmp->acces_args->args[i]->str[j])
	{
		if (tmp->acces_args->args[i]->str[j] == '$'
			&& tmp->acces_args->args[i]->str[j + 1] != '\0'
			&& tmp->acces_args->args[i]->str[j + 1] != '?'
			&& tmp->acces_args->args[i]->flag != 1
			&& tmp->acces_args->args[i]->str[j + 1] != ' ')
		{
			j++;
			c++;
			start = j;
			while (tmp->acces_args->args[i]->str[j]
				&& (ft_isalnum(tmp->acces_args->args[i]->str[j])
					|| tmp->acces_args->args[i]->str[j] == '_'))
			{
				j++;
			}
			if (j > start)
			{
				a = ft_substr(tmp->acces_args->args[i]->str, start, j - start);
				if (a && tmp->acces_args->args[i]->flag != 1
					&& tmp->acces_args->args[i]->flag != 6)
				{
					str = get_getenv(env, a);
					if (str && str[0])
					{
						c += ft_strlen(str);
					}
					else if (!str)
					{
						if (tmp->acces_args->args[i]->flag == 0)
						{
							c += ft_strlen(tmp->acces_args->args[i]->str);
						}
					}
				}
				free(a);
			}
			continue ;
		}
		else
			c++;
		j++;
	}
	return (c);
}

void	expand_dolar(t_general *list, t_env *env)
{
	t_general	*tmp;
	char		*new;
	char		*str;
	char		*a;
	int			i;
	int			start;
	int			l;
	int			j;
	int			k;
	int			flag;

	new = NULL;
	str = NULL;
	(void)flag;
	tmp = list;
	while (tmp)
	{
		i = 0;
		while (tmp->acces_args->args[i])
		{
			j = 0;
			if (i > 0 && tmp->acces_args->args[i - 1]
				&& ft_strcmp(tmp->acces_args->args[i - 1]->str, "<<") == 0
				&& (tmp->acces_args->args[i - 1]->flag == 5
					|| tmp->acces_args->args[i - 1]->flag == 2))
			{
				i++;
				continue ;
			}
			new = malloc(sizeof(char) * (count_m(tmp, i, env) + 1));
			if (!new)
				return ;
			k = 0;
			while (tmp->acces_args->args[i]->str[j])
			{
				if (tmp->acces_args->args[i]->str[j] == '$'
					&& tmp->acces_args->args[i]->str[j + 1]
					&& tmp->acces_args->args[i]->str[j + 1] != '?'
					&& tmp->acces_args->args[i]->flag != 1
					&& tmp->acces_args->args[i]->str[j + 1] != ' ')
				{
					j++;
					start = j;
					while (tmp->acces_args->args[i]->str[j]
						&& (ft_isalnum(tmp->acces_args->args[i]->str[j])
							|| tmp->acces_args->args[i]->str[j] == '_'))
					{
						j++;
					}
					if (j > start)
					{
						a = ft_substr(tmp->acces_args->args[i]->str, start, j
								- start);
						if (a && tmp->acces_args->args[i]->flag != 1
							&& tmp->acces_args->args[i]->flag != 6)
						{
							str = get_getenv(env, a);
							if (str && str[0])
							{
								ft_memcpy(new + k, str, ft_strlen(str));
								k += ft_strlen(str);
							}
							else if (!str)
							{
								if (tmp->acces_args->args[i]->flag == 0)
								{
									ft_memcpy(new + k,
										tmp->acces_args->args[i]->str,
										ft_strlen(tmp->acces_args->args[i]->str));
									k
										+= ft_strlen(tmp->acces_args->args[i]->str);
								}
							}
						}
						if (a)
							free(a);
					}
				}
				else
				{
					new[k++] = tmp->acces_args->args[i]->str[j];
					j++;
				}
			}
			new[k] = '\0';
			if (tmp->acces_args->args[i] && tmp->acces_args->args[i]->str)
			{
				free(tmp->acces_args->args[i]->str);
			}
			tmp->acces_args->args[i]->str = new;
			if (tmp->acces_args->args[i] && !tmp->acces_args->args[i]->str[0])
			{
				if (tmp->acces_args->args[i]->str)
					free(tmp->acces_args->args[i]->str);
				if (tmp->acces_args->args[i])
					free(tmp->acces_args->args[i]);
				l = i;
				while (tmp->acces_args->args[l + 1])
				{
					tmp->acces_args->args[l] = tmp->acces_args->args[l + 1];
					l++;
				}
				tmp->acces_args->args[l] = NULL;
				continue ;
			}
			i++;
		}
		tmp = tmp->next;
	}
}

char	*get_getenv(t_env *env, char *key)
{
	t_env	*tmp;

	tmp = env;
	while (tmp)
	{
		if (ft_strcmp(key, tmp->key) == 0)
			return (tmp->data);
		tmp = tmp->next;
	}
	return (NULL);
}
