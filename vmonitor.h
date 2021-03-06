#ifndef MONITOR_H_
#define MONITOR_H_

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


pid_t read_vulture_pid(const char* vulture_pid);
void vwatch(const char* vulture_pid);
void _vwatch(FILE* vmonitor_log, const char* vulture_pid);
void monitor(FILE* vulture_log, pid_t monitor_pid);
#endif // MONITOR_H_
