/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection_second.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 15:16:36 by bucolak           #+#    #+#             */
/*   Updated: 2025/08/20 19:56:22 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static volatile int f = 0;

void remove_heredoc(t_general *list)
{
	int i;
	int j;
	t_arg **new_arg;
	t_arg **old_arg;
	if (!list || !list->acces_args || !list->acces_args->args)
		return;
	i = 0;
	old_arg = list->acces_args->args;
	while (list->acces_args->args[i])
	{
		i++;
	}
	new_arg = malloc(sizeof(t_arg *) * (i + 1));
	if (!new_arg)
		return;
	i = 0;
	j = 0;
	while (list->acces_args->args[i])
	{
		if (ft_strcmp(list->acces_args->args[i]->str, "<<") == 0)
		{
			i += 2;
			continue;
		}
		new_arg[j++] = list->acces_args->args[i++];
	}
	new_arg[j] = NULL;
	list->acces_args->args = new_arg;
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
			continue;
		}
		i++;
	}
	free(old_arg);
}

void fill_limiter(t_general *list)
{
	int i;
	int j;
	int c;

	i = 0;
	j = 0;
	c = 0;
	if (!list || !list->acces_args || !list->acces_args->args)
		return;
	while (list->acces_args->args[i])
	{
		if (ft_strcmp(list->acces_args->args[i]->str, "<<") == 0)
			c++;
		i++;
	}
	list->limiter = malloc(sizeof(char *) * (c + 1));
	i = 0;
	while (list->acces_args->args[i])
	{
		if (ft_strcmp(list->acces_args->args[i]->str, "<<") == 0)
		{
			if (list->acces_args->args[i + 1])
			{
				list->limiter[j] = ft_strdup(list->acces_args->args[i + 1]->str);
				j++;
			}
		}
		i++;
	}
	list->limiter[j] = NULL;
}

void signal_handler_heredoc(int signo)
{
	if (signo == SIGINT)
	{
		write(1, "\n", 1);
		rl_on_new_line();
		rl_replace_line("", 0);
		exit(130);
	}
}

void signal_handler_heredoc2(int signo)
{
	if (signo == SIGINT)
	{
		rl_on_new_line();
		rl_replace_line("", 0);
		exit(130);
	}
}

void handle_heredoc(t_general *list, t_full *full)
{
	int i;
	int fd[2];
	int j;
	int s = 0;
	char *line;
	t_general *tmp;
	(void)full;
	
	tmp = list;
	fill_limiter(tmp);
	s++;
	i = 0;
	j = 0;
	
	while (tmp->acces_args->args[i])
	{
		if (ft_strcmp(tmp->acces_args->args[i]->str, "<<") == 0)
		{
			tmp->flag_heredoc = 1;
			if (!tmp->acces_args->args[i + 1])
			{
				ft_putstr_fd("bash: syntax error near unexpected token `newline'\n", 2);
				return;
			}
			
			pipe(fd);
			signal(SIGINT, SIG_IGN);
			pid_t pid = fork();
			
			if (pid == 0)
			{
				// Child process - heredoc input handling
				signal(SIGINT, SIG_DFL);
				signal(SIGQUIT, SIG_DFL);
				close(fd[0]); // Child sadece yazma ucunu kullanır
				
				while (1)
				{
					line = readline("heredoc > ");
					if (!line) // Ctrl+D durumu - direkt çık
					{
						close(fd[1]);
						exit(2); // EOF için özel exit code (program çıkış sinyali)
					}
					
					if (ft_strcmp(line, tmp->limiter[j]) == 0)
					{
						free(line);
						close(fd[1]);
						exit(0); // Normal heredoc tamamlanması
					}
					
					ft_putstr_fd(line, fd[1]);
					ft_putstr_fd("\n", fd[1]);
					free(line);
				}
			}
			else
			{
				// Parent process
				int status;
				close(fd[1]); // Parent sadece okuma ucunu kullanır
				waitpid(pid, &status, 0);
				signal(SIGINT, handle_signal);
				
				if (WIFEXITED(status))
				{
					int exit_code = WEXITSTATUS(status);
					if (exit_code == 2) // Ctrl+D durumu - programı kapat
					{
						close(fd[0]);
						// Cleanup işlemleri
						if (full && full->get)
						{
							free_envp(full->get);
						}
						if (full && full->node)
						{
							free_env(full->node);
						}
						if (full && full->pipe_blocks)
						{
							free_pipe_blocks(full->pipe_blocks);
						}
						// Program çıkışı
						exit(0);
					}
					else if (exit_code == 0) // Normal tamamlanma
					{
						tmp->heredoc_fd = fd[0];
						tmp->dqm = 0;
					}
					else
					{
						close(fd[0]);
						tmp->heredoc_fd = -1;
						tmp->dqm = exit_code;
					}
				}
				else if (WIFSIGNALED(status) && WTERMSIG(status) == SIGINT)
				{
					// Ctrl+C durumu
					tmp->a = 1;
					close(fd[0]);
					tmp->heredoc_fd = -1;
					tmp->dqm = 130;
					write(1, "\n", 1);
					remove_heredoc(tmp);
					return;
				}
				else
				{
					// Diğer durumlar
					close(fd[0]);
					tmp->heredoc_fd = -1;
					tmp->dqm = 1;
				}
			}
			j++;
		}
		i++;
	}
	remove_heredoc(tmp);
}
