/*
 * $Id: thread.c,v 1.3 2001/11/13 17:05:13 alex Exp $
 *
 * (C) 2001 Void Technologies
 * Author: Alex Fiori <alex@void.com.br>
 */

#include "VTserver.h"

pthread_cond_t  cond   = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex  = PTHREAD_MUTEX_INITIALIZER;

inline void thread_lock (void)
{
	while (pthread_mutex_trylock (&mutex) == EBUSY)
		pthread_cond_wait (&cond, &mutex);
	
	return;
}

inline void thread_unlock (void)
{
	pthread_mutex_unlock (&mutex);
	pthread_cond_broadcast (&cond);

	return;
}
