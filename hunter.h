#ifndef HUNTER_H_
#define HUNTER_H_

#include <dirent.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wait.h>

void shot(pid_t proc);
int is_pid_dir(const struct dirent *entry);

//https://stackoverflow.com/questions/63372288/getting-list-of-pids-from-proc-in-linux
pid_t hunt(char* psearch);
void hunter();

#endif // HUNTER_H_