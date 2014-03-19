CC = cc
RM = rm -f

NAME = VTserver

CFLAGS = -Wall -O2 -I../include 										\
		`pkg-config --cflags gtk+-2.0 gstreamer-0.8 gdk-pixbuf-2.0`  	\
		-DG_DISABLE_DEPRECATED          								\
        -DGDK_DISABLE_DEPRECATED        								\
		-DGDK_PIXBUF_DISABLE_DEPRECATED 								\
		-DGTK_DISABLE_DEPRECATED	    								\
		-DDATA_DIR=\"../../\"

LIBS = `pkg-config --libs gtk+-2.0 gstreamer-0.8 gdk-pixbuf-2.0` -lgstinterfaces-0.8

OBJECTS = VTserver.o unix.o commands.o thread.o gst-backend.o video.o

.SUFFIXES: .c
.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

all: $(NAME)

$(NAME): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJECTS) $(LIBS)

clean:
	$(RM) $(NAME) $(OBJECTS)
