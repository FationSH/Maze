CC = g++

GCCFLAGS = -fpcc-struct-return
CFLAGS = -g
ifeq ($(OSTYPE),solaris)	
INCLUDE = -I/usr/local/include 
LDFLAGS = -L/usr/local/lib/ 
LDLIBS =  -lGL -lglut -lm 
else
INCLUDE = -I/usr/include
LDFLAGS = -L/usr/lib/
LDLIBS =  -lGL -lGLU -lglut -lSOIL
endif

maze: $(OBJECTS)
	$(CC) $(GCCFLAGS) $(INCLUDE) $(CFLAGS) maze.cpp $(LDFLAGS) $(LDLIBS) -o maze

