.POSIX:
.SUFFIXES:

CC = cc
CFLAGS = -W -O -g -Wall -Werror -Wextra -Wpedantic -Wconversion -Wcast-align -Wunused -Wshadow -Wpointer-arith 
LDLIBS = -lm

all: nimrod
nimrod: agent_sim.o hunter.o vulture.o task.o
	$(CC) $(LDFLAGS) -o nimrod agent_sim.o hunter.o vulture.o task.o $(LDLIBS)
agent_sim.o: agent_sim.c hunter.h vulture.h
	$(CC) -c $(CFLAGS) agent_sim.c
hunter.o: hunter.c hunter.h
	$(CC) -c $(CFLAGS) hunter.c
vulture.o: vulture.c vulture.h task.h
	$(CC) -c $(CFLAGS) vulture.c
task.o: task.c task.h
	$(CC) -c $(CFLAGS) task.c 
clean:
	rm -f nimrod agent_sim.o hunter.o vulture.o task.o
