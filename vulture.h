#ifndef VULTURE_H_
#define VULTURE_H_

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/inotify.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <wait.h>


void signal_forker(int sig);
void circle();
void monitor();
void vulture();
#endif // VULTURE_H_
