/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: seerel <seerel@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/14 16:22:33 by bucolak           #+#    #+#             */
/*   Updated: 2025/08/24 14:48:20 by seerel           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"


int	main(int argc, char *argv[], char **envp)
{
	char		*line;
	t_general	*pipe_blocs;
	t_env		*env;
	t_now		*get;
	t_full		full;
	t_pipe		*pipe;
	int			last_dqm;
	static int	first_run;
	int			exit_code;

	(void)argc;
	(void)argv;
	first_run = 1;
	last_dqm = 0;
	pipe = NULL;
	pipe_blocs = NULL;
	get = NULL;
	full.get = NULL;
	full.node = NULL;
	full.pipe = NULL;
	full.pipe_blocks = NULL;
	full.new = NULL;
	env = create_env_node();
	if (first_run)
	{
		get_env(env, envp);
		full.node = env;
		first_run = 0;
	}
	while (1)
	{
		signal_handler();
		pipe_blocs = create_general_node(last_dqm);
		line = readline("Our_shell% ");
		if (signal_ec == 1)
		{
			last_dqm = 130;
			pipe_blocs->dqm = 130;
			signal_ec = 0;
		}
		if (!line)
		{
			if (get)
			{
				free_envp(get);
				get = NULL;
			}
			if (env)
			{
				free_env(env);
				env = NULL;
			}
			if (pipe)
			{
				free_pipe(pipe);
				pipe = NULL;
			}
			close_heredoc_fd(pipe_blocs);
			if (pipe_blocs->heredoc_fd != -1)
				close(pipe_blocs->heredoc_fd);
			if (pipe_blocs)
			{
				exit_code = pipe_blocs->dqm;
				free_pipe_blocks(pipe_blocs);
				pipe_blocs = NULL;
			}
			close_all_open_fds();
			exit(exit_code);
		}
		if (line[0] == '\0')
		{
			free_pipe_blocks(pipe_blocs);
			pipe_blocs = NULL;
			free(line);
			continue ;
		}
		add_history(line);
		pipe_parse(&pipe_blocs, line);
		parse_input(pipe_blocs);
		expand_dolar(pipe_blocs, env);
		connect_count_malloc(pipe_blocs);
		remove_null(pipe_blocs);
		control_redireciton(pipe_blocs, env);
		// print_pipes(pipe_blocs);
		full.pipe_blocks = pipe_blocs;
		get = malloc(sizeof(t_now));
		get->envp = malloc(sizeof(char *) * (ft_lsttsize(env) + 1));
		fill_env(env, get);
		full.get = get;
		if (pipe_blocs->next)
		{
			pipe = malloc(sizeof(t_pipe));
			init_pipe(pipe, pipe_blocs);
			create_pipe(pipe->count, pipe->fd);
			full.pipe = pipe;
			handle_pipe(pipe_blocs, env, pipe, &full);
			free_pipe(pipe);
			pipe = NULL;
		}
		else if (pipe_blocs->acces_args && pipe_blocs->acces_args->args[0])
		{
			if (!has_redireciton(pipe_blocs)
				&& is_built_in(pipe_blocs->acces_args->args[0]->str))
			{
				check_cmd_built_in(pipe_blocs, &env, pipe, get);
			}
			else
			{
				check_cmd_sys_call(pipe_blocs, env, pipe, &full);
			}
		}
		free_envp(get);
		get = NULL;
		last_dqm = pipe_blocs->dqm;
		free_pipe_blocks(pipe_blocs);
		pipe_blocs = NULL;
		free(line);
	}
	free_env(env);
	free_envp(get);
	free_pipe_blocks(pipe_blocs);
	return (0);
}
