/*
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

#ifndef _VTQUEUE_H
#define _VTQUEUE_H

#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/un.h>
#include <sys/socket.h>

#include "config.h"

typedef enum {
    ADD = 0,
    REM,
    LIST
} VTCommandType;

typedef struct {
    VTCommandType cmd;
    char          uri[256];
    int           idx;
} VTCommand;

extern int send_cmd(int fd, const char *cmd);
extern char *get_cmd_result(FILE *fp);

#endif
