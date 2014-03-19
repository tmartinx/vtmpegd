CC = cc
RM = rm -f

NAME = VTserver

CFLAGS = -Wall -O2 -I../include                           \
		 `evas-config --cflags` `emotion-config --cflags`
LDFLAGS = `evas-config --libs` `emotion-config --libs`

OBJECTS = VTserver.o unix.o commands.o thread.o emotion-backend.o

.SUFFIXES: .c
.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

all: $(NAME)

$(NAME): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJECTS) $(LDFLAGS)

clean:
	$(RM) $(NAME) $(OBJECTS)
