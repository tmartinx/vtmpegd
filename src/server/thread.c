/*
 * $Id: thread.c,v 1.3 2001/11/13 17:05:13 alex Exp $
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

pthread_cond_t  cond   = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex  = PTHREAD_MUTEX_INITIALIZER;

inline void thread_lock(void)
{
    while(pthread_mutex_trylock(&mutex) == EBUSY)
        pthread_cond_wait(&cond, &mutex);
    
    return;
}

inline void thread_unlock(void)
{
    pthread_mutex_unlock(&mutex);
    pthread_cond_broadcast(&cond);

    return;
}
