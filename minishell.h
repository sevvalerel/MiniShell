/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: seerel <seerel@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/14 16:05:46 by bucolak           #+#    #+#             */
/*   Updated: 2025/05/01 16:13:26 by seerel           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
#define MINISHELL_H

#include "libft/libft.h"

// temel giriş/çıkış
#include <stdio.h>       // printf, perror
#include <stdlib.h>      // malloc, free, exit, getenv
#include <unistd.h>      // write, access, read, close, fork, getcwd, chdir, execve, isatty, ttyname, ttyslot

// dosya işlemleri
#include <fcntl.h>       // open, O_RDONLY, O_WRONLY, vs.
#include <sys/stat.h>    // stat, lstat, fstat
#include <dirent.h>      // opendir, readdir, closedir

// sinyal işlemleri
#include <signal.h>      // signal, sigaction, sigemptyset, sigaddset, kill

// işlem kontrolü
#include <sys/wait.h>    // wait, waitpid, wait3, wait4

// hata işlemleri
#include <string.h>      // strerror

// terminal özellikleri
#include <termios.h>     // tcsetattr, tcgetattr
#include <termcap.h>     // tgetent, tgetflag, tgetnum, tgetstr, tgoto, tputs
#include <sys/ioctl.h>   // ioctl

// readline
#include <readline/readline.h>
#include <readline/history.h>

typedef struct s_arg
{
    char *str;
    int flag;
}   t_arg;

typedef struct s_pipeafter
{
    t_arg **args;
}   t_pipeafter;

typedef struct s_general
{
    char *blocs;
    t_pipeafter *acces_args;
    struct s_general *next;
} t_general;

void add_flag(t_general *a);
void echo_cont(t_general *a);
void dolar_control(t_general *a);
void parse_input( t_general *a);
void check_cmd(t_general *pipe_blocs);

// cmd_built_in.c
void cd_cmd(t_arg **args);
void pwd_cmd(char **ar);


int message(int i);
#endif