/*
 * $Id: commands.c,v 1.22 2001/11/13 02:44:54 alex Exp $
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

void command_list(int fd, Evas_List *queue)
{
    int        i    = 0;
    VTmpeg    *mpeg = NULL;
    Evas_List *l    = evas_list_nth_list(queue, 0);
    int        t    = evas_list_count(queue);

    if(l == NULL) {
        dprintf(fd, "%c\nEmpty list.\n%c\n",
             COMMAND_ERROR, COMMAND_DELIM);
        return;
    }

    dprintf(fd, "%c\n", COMMAND_OK);
    dprintf(fd, "VTmpeg queue list\n");

    for(; l; i++, l = evas_list_next(l)) {
        mpeg =(VTmpeg *) l->data;
        if(mpeg != NULL)
            dprintf(fd, "%d%c%s\n", (i + 1), COMMAND_DELIM, mpeg->filename);
    }

    dprintf(fd, "%d video%s in queue.\n", t, t > 1 ? "s " : "");
    dprintf(fd, "%c\n", COMMAND_DELIM);
}

Evas_List *command_insert(int fd, Evas_List *queue, const char *filename,
               int pos, int max_pos)
{
    Evas_List *q = queue;
    VTmpeg *mpeg = (VTmpeg *) malloc(sizeof(VTmpeg));

    if(pos > max_pos) pos = 0;

    if(mpeg == NULL) {
        dprintf(fd, "%c\nNo enough memory, aborting insertion.\n%c\n",
             COMMAND_ERROR, COMMAND_DELIM);
        return NULL;
    
    /* guarda o nome do filename */
    } else {
        memset(mpeg, 0, sizeof(VTmpeg));
        strncpy(mpeg->filename, filename, sizeof(mpeg->filename));
    }

    if(!pos)
        q = evas_list_append(q, mpeg);
    else {
        void *relative = evas_list_nth(q, (pos - 1));
        q = evas_list_append_relative(q, mpeg, relative);
    }

    if(q == NULL) {
        dprintf(fd, "%c\nCannot %s on the list.\n%c\n",
             COMMAND_ERROR, !pos ? "append" : "insert", COMMAND_DELIM);
        return NULL;
    }

    dprintf(fd, "%c\nFilename %s OK\n%c\n", COMMAND_OK, filename, COMMAND_DELIM);
    return q;
}

Evas_List *command_remove(int fd, Evas_List *queue, int pos)
{
    VTmpeg *mpeg;
    Evas_List *q = queue;

    if(!pos) {
        invalid_position:
        dprintf(fd, "%c\nInvalid position.\n%c\n",
             COMMAND_ERROR, COMMAND_DELIM);
        return NULL;
    }

    mpeg = evas_list_nth(q, (pos - 1));
    if(mpeg) {
        q = evas_list_remove(q, mpeg);
        free(mpeg);
    } else goto invalid_position;

    if(q == NULL)
        dprintf(fd, "%c\nCannot remove position %d\n%c\n",
             COMMAND_ERROR, pos, COMMAND_DELIM);
    else
        dprintf(fd, "%c\nRemove position %d OK\n%c\n",
             COMMAND_OK, pos, COMMAND_DELIM);

    return q;
}
