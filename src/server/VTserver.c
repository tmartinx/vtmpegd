/*
 * $Id: main.c,v 1.26 2001/11/13 18:31:33 alex Exp $
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

static void finish  (void);
static void sfinish (int sig);
static int  already_finished = 0;

void show_copyright(void)
{
    fprintf(stderr, PROGRAM_DESCRIPTION "\n");
    fprintf(stderr, PROGRAM_AUTHORS "\n");
}

int videoloop(void *data)
{
    VTmpeg *mpeg = data;
    if(!video_is_playing() && (mpeg = unix_getvideo()) != NULL) {
        video_stop();
        usleep(1000000);
        video_play(mpeg->filename);
    }
    //fprintf(stdout, "videoloop(): Tick timer: %3.2f\n", ecore_time_get());
    return 1;
}

int main(int argc, char **argv)
{
    VTmpeg *mpeg = NULL;
    int r;

    signal(SIGINT,  sfinish);
    signal(SIGTERM, sfinish);
    signal(SIGSEGV, sfinish);
    signal(SIGHUP,  SIG_IGN);
    signal(SIGPIPE, SIG_IGN);

    /* pra deixar o programa mais 'verboso', como diz
       o bacana do Thiago */
    show_copyright();

    r = video_init(NULL, PROGRAM_DESCRIPTION, 720, 480, 0);
    if(r < 0) {
        fprintf(stderr, "video_init() failed, aborting.\n");
        finish();
    }

    /* daemonize */
    //daemon(0, 0);

    /* cria o server de unix domain sockets */
    if(!unix_server()) {
        fprintf(stderr, "VTmpegd: Cannot create the server.\n");
        return 0;
    }

    ecore_timer_add(0.5, videoloop, mpeg);

    /* start the main event loop */
    ecore_main_loop_begin();

    return 1;
}

/* finaliza o processo */
void finish(void) 
{
    thread_lock();

    /* para não ser feito por todos
       os processos(main e thread) */
    if(already_finished) {
        thread_unlock();
        exit(EXIT_SUCCESS);
    }

    /* se tiver algum video passando, da stop 
       e já limpa a lista */
    unix_finish();

    /* remove o socket */
    unlink(unix_sockname());

    fprintf(stderr, "Goodbye.\n");
    already_finished = 1;

    thread_unlock();
    video_close();
    exit(EXIT_SUCCESS);
}

void sfinish(int sig)
{
    fprintf(stderr, "VTmpegd: Received signal %d, exiting.\n", sig);
    close(0); close(1); close(2);
    finish();
}
