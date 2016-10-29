CC = g++
CFLAGS = -Wall -g
PROG = CGAssignment

SRCS = main.cpp imageloader.cpp
LIBS = -framework OpenGL -framework GLUT


all: $(PROG)

$(PROG):	$(SRCS)
	$(CC) $(CFLAGS) -o $(PROG) $(SRCS) $(LIBS)

clean:
	rm -f $(PROG)
