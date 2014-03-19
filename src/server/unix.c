/*
 * $Id: unix.c,v 1.19 2001/11/13 18:31:33 alex Exp $
 *
 * Copyright (C) 2001 Void Technologies
 * Copyright (C) 2005,2006 Alexandre Fiori and Arnaldo Pereira
 *
 * VTmpeg2 is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * VTmpeg2 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "VTserver.h"

static int   server_fd;
static int   unix_command = 0;
//static int   playing_mpeg = -1;
static Evas_List *queue, *temp_queue;

static void *unix_loop   (void *arg);
static void  unix_client (int fd);

static int unix_list_count(void) { return evas_list_count(queue); }

/* guarda o nome do unix domain socket */
char *unix_sockname(void)
{
    int i = 0;
    struct stat st;
    static char temp[128], filename[128];

    while(*filename == '\0') {
        memset(temp, 0, sizeof(temp));
        snprintf(temp, sizeof(temp), "%s.%d", UNIX_PATH, i);
        if(stat(temp, &st) < 0) {
            strncpy(filename, temp, sizeof(filename));
            break;
        }
        i++;
    }
    return filename;
}

/* cria o server(unix domain sockets)
   pro client poder acessar o queue */
int unix_server(void)
{
    int fd;
    pthread_t th;
    struct sockaddr_un s;

    s.sun_family = AF_UNIX;
    snprintf(s.sun_path, sizeof(s.sun_path), unix_sockname()); 

    if((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) return 0;
    if(bind(fd, (struct sockaddr *) &s, sizeof(s)) < 0) return 0;
    if(listen(fd, 1) < 0) return 0;

    chmod(unix_sockname(), 0666);

    /* cria a lista */
    temp_queue = queue = NULL;

    server_fd = fd;
    pthread_create(&th, NULL, unix_loop, NULL);

    /* cria o symlink pra esse server */
    unlink(UNIX_PATH);
    symlink(unix_sockname(), UNIX_PATH);

    return 1;
}

VTmpeg *unix_getvideo(void)
{
    VTmpeg *mpeg = NULL;
    Evas_List *q = evas_list_nth_list(queue, 0);
    static int idx = 0;

    thread_lock();

    /* se não tiver o primeiro video... */
    if(q == NULL) {
        thread_unlock();
        return NULL;
    }

    mpeg = evas_list_nth(q, idx);
    if(mpeg)
        idx++;

    thread_unlock();
    return mpeg;
}

/* informa o atual comando executado pelo queue */
int unix_get_command(void)
{
    int command;

    //thread_lock();
    command = unix_command;
    unix_command = 0;
    //thread_unlock();

    return command;
}

/* fecha o server e mata a lista */
void unix_finish(void)
{
    shutdown(server_fd, 2);
    close(server_fd);

    evas_list_free(evas_list_nth_list(queue, 0));
    return;
}

/* atende os clientes */
void *unix_loop(void *arg)
{
    fd_set fds;
    int fd, cfd; 
    socklen_t len;
    struct timeval tv;
    struct sockaddr_un s;

    fd = server_fd;

    for(;;) {
        FD_ZERO(&fds);
        FD_SET(fd, &fds);
        tv.tv_sec = 1; tv.tv_usec = 0;

        if(select(fd + 1, &fds, NULL, NULL, &tv)) {
            thread_lock();

            len = sizeof(s);
            memset(&s, 0, sizeof(s));
            if((cfd = accept(fd, (struct sockaddr *) &s, &len)) < 0)
                perror("accept"), exit(1);

            /* trata os comandos */
            unix_client(cfd);

            shutdown(cfd, 2);
            close(cfd);

            thread_unlock();
        }
    }

    return NULL;
}

void unix_client(int fd)
{
    Evas_List *q       = NULL;
    int        pos     = 0;
    char       temp[2048];
    char       filename[1024], *p;
    int        i;

    memset(temp, 0, sizeof(temp));
    if(read(fd, temp, sizeof(temp)) < 0) return;

    switch(atoi(temp)) {
        case COMMAND_LIST:
            command_list(fd, evas_list_nth_list(queue, 0));
            break;

        case COMMAND_INSERT:
            memset(filename, 0, sizeof(filename));

            for(p = filename, i = 0; 
                    temp[i + 2] != ';' && i < sizeof(filename); 
                    i++, p++) {
                *p = temp[i + 2];
            }
            *p = 0;
            if((p = strrchr(temp, ';')))
                pos = atol(++p);

            printf("%d -- %s\n\n", pos, filename);
            q = command_insert(fd, queue, filename,
                    pos, unix_list_count());
            if(q != NULL) queue = evas_list_nth_list(q, 0);
            break;

        case COMMAND_REMOVE:
            sscanf(temp + 2, "%d\n", &pos);
            q = command_remove(fd, queue, pos);
            if(q != NULL) {
                unix_command = COMMAND_REMOVE;
                queue = evas_list_nth_list(q, 0);
            }
            break;

        case COMMAND_PLAY:
            unix_command = COMMAND_PLAY;
            break;

        case COMMAND_PAUSE:
            unix_command = COMMAND_PAUSE;
            break;

        case COMMAND_STOP:
            unix_command = COMMAND_STOP;
            break;

        case COMMAND_NEXT:
            fprintf(stderr, "COMMAND_NEXT: TODO\n");
            break;

        case COMMAND_PREV:
            fprintf(stderr, "COMMAND_PREV: TODO\n");
            break;

        case COMMAND_MUTE:
            unix_command = COMMAND_MUTE;
            break;

        default:
            dprintf(fd, "%c: Unknown command.\n%c\n",
                    COMMAND_ERROR, COMMAND_DELIM);
    }
    return;
}
