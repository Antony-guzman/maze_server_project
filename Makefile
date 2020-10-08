# Makefile to run the Maze Challenge
# 
# Group name: BASIX
# cs50, Winter 2020

PROG = AMStartup 
OBJS = AMStartup.o avatar.o maze.o set.o visualize.o
# TESTING=-DMEMTEST

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(TESTING) -Ilib

LLIBS = lib/libcs50-given.a


all: $(PROG)

# Build the excecutable.
$(PROG): $(OBJS) 
	$(CC) $(CFLAGS) $^ $(LLIBS) -lpthread -o $@

# Dependencies.
AMStartup.o: avatar.h maze.h visualize.h
avatar.o: avatar.h maze.h visualize.h
maze.o: maze.h set.h visualize.h
set.o: set.h
visualize.o: visualize.h maze.h

.PHONY: clean

clean:
	rm -f *~ *.o *.dSYM
	rm -f $(PROG)
	rm -f *.dSYM
	rm -f core
	rm -f vgcore.*
	rm -f core
	rm -f nfs*
	rm -f set.clearo
