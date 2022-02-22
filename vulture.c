/*
 * Agent for attempting to avoid detection from
 * hunter by bisecting its processing when signaled
 * and splitting to a child process while killing the
 * parent
 */
#define _POSIX_SOURCE

#include "monitor.h"
#include "task.h"
#include "vulture.h"

static unsigned int FORK_FLAG = 0;
void vulture_fork(int sig)
{
    if (sig == SIGINT)
    {
        FORK_FLAG = 1;
    }
}

void write_pid(char* pidfile, pid_t pid_val)
{

    FILE* vulture_pid = fopen(pidfile, "w");
    if (vulture_pid != NULL)
    {
        fprintf(vulture_pid, "%d", (int)pid_val);
        fclose(vulture_pid);
    }
}

void circle(FILE* vulture_log, char* vulture_pid, pid_t monitor_pid)
{
    while(1)
    {
        long hash_val = hash(); 
        sleep(1);
        fprintf(vulture_log, "%ld %ld\n", hash_val, (long)getpid());

        if (FORK_FLAG == 1)
        {
            FORK_FLAG = 0;
            pid_t dead_proc;
            pid_t fresh_proc;
            switch (fork()) 
            {
                case -1:
                    fprintf(vulture_log, "Fork failure\n");
                    return;

                case 0:
                    dead_proc = getppid();
                    fresh_proc = getpid();
                    fprintf(vulture_log, "%ld -> %ld\n",
                            (long)dead_proc, (long)fresh_proc);
                    fprintf(vulture_log,
                            "Sending SIGTERM to %d\n", dead_proc);
                    kill(getppid(), SIGTERM);
                    write_pid(vulture_pid, fresh_proc);
                    kill(SIGUSR1, monitor_pid);
                    break;

                default:
                    return;
            }
        }
        fflush(vulture_log);
    }
    fclose(vulture_log);
}


void vulture()
{
    FILE* vulture_log = fopen("./log/vulture.log", "w");
    const char* monitor_proc = "vmonitor";
    char* vulture_pid = "./pid/vulture.pid";

    struct sigaction sa;
    sa.sa_flags = 0;
    sa.sa_handler = vulture_fork;
    sigaction(SIGINT, &sa, NULL);

    pid_t monitor_pid;
    switch (monitor_pid = fork()) 
    {
        case -1:
            fprintf(vulture_log, "Fork failure\n");
            return;

        case 0:
            if (prctl(PR_SET_NAME, (unsigned long) monitor_proc) < 0)
            {
                fprintf(vulture_log, "Unable to set child name to %s", monitor_proc);
                return;
            }
            vwatch(vulture_log, vulture_pid);
            break;

        default:
           write_pid(vulture_pid, getpid());
           circle(vulture_log, vulture_pid, monitor_pid);
           break;
    }
}
