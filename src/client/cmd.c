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

#include "VTqueue.h"

static char result_buf[MAX_RESULT_LINE_LEN];

/* return:
 * -1 on system error
 * 0 on error
 * 1 on success
 */
int send_cmd(int fd, const char *cmd)
{
       fd_set fds;
       char buf[2];
    struct timeval tv;
    
       if(!cmd)
           return -1;
    
    if(!dprintf(fd, cmd))
           return -1;
    //usleep(500);

    FD_ZERO(&fds);
    FD_SET(fd, &fds);
    tv.tv_sec = 1;
    tv.tv_usec = 0;

    if(select(fd + 1, &fds, NULL, NULL, &tv)) {
        memset(buf, 0, sizeof(buf));
        if(!read(fd, buf, sizeof(buf)))
               return -1;

        if(*buf == COMMAND_ERROR)
               return 0;
    }
   
    return 1;
}

/* return
 * the result msg on success
 * NULL on error or end of msg
 */
char *get_cmd_result(FILE *fp)
{
       fd_set fds;
       int fd;
    struct timeval tv;
    
    fd = fileno(fp);
    
    FD_ZERO(&fds);
    FD_SET(fd, &fds);
    tv.tv_sec = 1;
    tv.tv_usec = 0;

    if(select(fd + 1, &fds, NULL, NULL, &tv)) {
        memset(result_buf, 0, MAX_RESULT_LINE_LEN);    
        if(!fgets(result_buf, MAX_RESULT_LINE_LEN, fp))
               return NULL;

        if(*result_buf == COMMAND_DELIM)
               return NULL;
    }
        
    return result_buf;
}
