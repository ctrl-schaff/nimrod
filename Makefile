.POSIX:
.SUFFIXES:

CC = cc
CFLAGS = -W -O -g -Wall -Werror -Wextra -Wpedantic -Wconversion -Wcast-align -Wunused -Wshadow -Wpointer-arith
LDLIBS = -lm

all: nimrod
nimrod: agent.o hunter.o vulture.o task.o monitor.o
	$(CC) $(LDFLAGS) -o nimrod agent.o hunter.o vulture.o task.o monitor.o $(LDLIBS)
agent.o: agent.c hunter.h vulture.h
	$(CC) -c $(CFLAGS) agent.c
vulture.o: vulture.c vulture.h task.h monitor.h
	$(CC) -c $(CFLAGS) vulture.c
monitor.o: monitor.c monitor.h hunter.h
	$(CC) -c $(CFLAGS) monitor.c 
hunter.o: hunter.c hunter.h
	$(CC) -c $(CFLAGS) hunter.c
task.o: task.c task.h
	$(CC) -c $(CFLAGS) task.c 
clean:
	rm -f nimrod agent.o hunter.o vulture.o task.o monitor.o
