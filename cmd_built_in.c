/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_built_in.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: buket <buket@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 22:48:09 by buket             #+#    #+#             */
/*   Updated: 2025/04/24 23:40:34 by buket            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void cd_cmd(t_arg **args)
{
    if (!args[1])
    {
        char *line;
        line = getenv("HOME");
        if (line)
            chdir(line);
    }
    else
        chdir(args[1]->str);
}

void pwd_cmd(char **ar)
{
    char *line;
    if(ft_strncmp(ar[0],"pwd",3)==0)
    {
        line = getcwd(NULL, 0);
        if(!line)
        {
            printf("Error\n");
            free(line);
            //free_split(ar);
            exit(1);
        }
        printf("%s\n", line);
    }
}