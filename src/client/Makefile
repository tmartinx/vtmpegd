#
# $Id: Makefile,v 1.2 2001/11/08 09:54:13 flv Exp $
#

CC	= cc
RM	= rm -f

NAME	= VTqueue

INCLUDE	=
LIBS	=

CFLAGS	= -Wall -O2 $(INCLUDE) -I../include

OBJECTS	=	VTqueue.o cmd.o

.SUFFIXES: .c
.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

all:	$(OBJECTS)
	$(CC) -o $(NAME) $(OBJECTS) $(LIBS)

clean:
	$(RM) $(NAME) $(OBJECTS)
