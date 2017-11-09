
#   TARGETS ...: TARGET-PATTERN: DEP-PATTERNS ...
#             COMMANDS
#
# `$<' is the automatic variable that holds the name of the
# prerequisite and `$@' is the automatic variable that holds the name of
# the target; 


# Basic stuff

CC       = g++
CFLAGS   = -O3 -g
INC      = -I/usr/X11R6/include  -I/usr/local/include -I/usr/local/include/GL -I/usr/local/lib/include/GL 
LIB      = -lm  -framework Cocoa -framework GLUT -framework OpenGL
PROGRAM  = GSilph
OBJ      = vecops.o\
	images.o\
	main.o


$(PROGRAM): $(OBJ) 
	$(CC) $(CFLAGS) -o $(PROGRAM) $(OBJ)  $(LIB) 

$(OBJ): %.o: %.cpp
	$(CC) $(CFLAGS) $(INC) -c $< 

clean:
	/bin/rm -f *.o
	/bin/rm -f $(PROGRAM)
	/bin/rm -f core
