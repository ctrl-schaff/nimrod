/*
 * Agent for attempting to avoid detection from
 * hunter by bisecting its processing when signaled
 * and splitting to a child process while killing the
 * parent
 */
#define _POSIX_SOURCE

#include "vmonitor.h"
#include "task.h"
#include "vulture.h"
#include "vtime.h"

static unsigned int FORK_FLAG = 0;
static pid_t VMONITOR_PID;

void vulture_handler(int sig)
{
    if (sig == SIGINT)
    {
        FORK_FLAG = 1;
    }
    else if (sig == SIGTERM)
    {
        kill(VMONITOR_PID, SIGTERM);
        _exit(0);
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

void circle(FILE* vulture_log, char* vulture_pid, int vsleep)
{
    while(1)
    {
        long hash_val = hash(); 
        fprintf(vulture_log, "%ld %ld\n", hash_val, (long)getpid());

        msleep(vsleep);

        if (FORK_FLAG == 1)
        {
            FORK_FLAG = 0;
            pid_t fresh_proc;
            switch (fork()) 
            {
                case -1:
                    fprintf(vulture_log, "Fork failure\n");
                    return;

                case 0:
                    fresh_proc = getpid();
                    write_pid(vulture_pid, fresh_proc);
                    break;

                default:
                    return;
            }
        }
        fflush(vulture_log);
    }
    fclose(vulture_log);
}

void vulture(const int vsleep, const char* vulture_log_path)
{
    FILE* vulture_log = fopen(vulture_log_path, "w");
    const char* monitor_proc = "vmonitor";
    char* vulture_pid = "./pid/vulture.pid";

    struct sigaction sa;
    sa.sa_flags = 0;
    sa.sa_handler = vulture_handler;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    switch (VMONITOR_PID = fork()) 
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
            vwatch(vulture_pid);
            break;

        default:
           write_pid(vulture_pid, getpid());
           circle(vulture_log, vulture_pid, vsleep);
           break;
    }
}
