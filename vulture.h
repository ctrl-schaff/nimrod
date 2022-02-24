#ifndef VULTURE_H_
#define VULTURE_H_

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/inotify.h>
#include <sys/prctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <wait.h>


void circle(FILE* vulture_log, char* vulture_pid);
void write_pid(char* pidfile, pid_t pid_val);
void vulture();
void vulture_fork(int sig);
#endif // VULTURE_H_
