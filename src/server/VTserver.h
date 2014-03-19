/*
 * $Id: main.h,v 1.14 2001/11/13 04:44:48 alex Exp $
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

#ifndef _MAIN_H
#define _MAIN_H 1

/* leia /usr/include/features.h */
#define _GNU_SOURCE 1

#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/un.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <ctype.h>

#include <Evas.h>
#include <Ecore.h>
#include <Ecore_Evas.h>
#include <Emotion.h>

/* informações tanto do server
   quanto do client */
#include "config.h"

#define DEF_WIDTH   1024
#define DEF_HEIGHT  768

typedef struct {
    char        filename[1024];
    int        played;
} VTmpeg;

/* emotion-backend.c */
int  video_init(char *display, char *win_title, int width, int height, int fs);
int  video_play(char *filename);
void video_stop(void);
int  video_is_playing(void);
void video_close(void);

/* unix.c */
extern char    *unix_sockname   (void);
extern int      unix_server     (void);
extern VTmpeg  *unix_getvideo   (void);
extern int      unix_get_command(void);
extern void     unix_finish     (void);

/* commands.c */
extern void     command_list (int fd, Evas_List *queue);
extern Evas_List   *command_insert (int fd, Evas_List *queue, const char *filename,
                int pos, int max_pos);
extern Evas_List   *command_remove (int fd, Evas_List *queue, int pos);

/* thread.c */
extern inline void thread_lock (void);
extern inline void thread_unlock (void);

/* copyright.c */
#define        PROGRAM_DESCRIPTION "oO VTmpeg - MPEG video player daemon for Linux Oo"
#define        PROGRAM_AUTHORS     "  Alexandre Fiori - <fiorix@gmail.com>\n" \
                                   "  Arnaldo Pereira - <egghunt@gmail.com>\n"
extern        void show_copyright (void);

#endif /* VTserver.h */

