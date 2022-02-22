#define _POSIX_SOURCE
#define _XOPEN_SOURCE 500

#include "monitor.h"

void monitor_handler() {}

void pid_wait()
{
    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGUSR1);
    sigprocmask(SIG_BLOCK, &sigset, NULL);
    pause();
}

pid_t read_vulture_pid(const char* vulture_pid)
{
    FILE* vpidf = fopen(vulture_pid, "r");

    int vpid = 0;
    fscanf(vpidf, "%d", &vpid);
    return (pid_t)vpid;
}

void monitor(FILE* vulture_log, pid_t monitor_pid)
{
    if (monitor_pid > 300)
    {
        char watch_dir[256];
        snprintf(watch_dir, sizeof(watch_dir), "/proc/%d", monitor_pid);

        static const int event_size = sizeof(struct inotify_event);
        static const int event_buffer_length = (1024 * (event_size + 16));
        char buffer[event_buffer_length];

        int fd = inotify_init();
        if (fd < 0) 
        {
            fprintf(vulture_log, "Issue with inotify_init");
            return;
        }

        int wd = inotify_add_watch(fd, watch_dir, IN_ACCESS | IN_OPEN);

        // Blocking read to determine the event change happens on the watch directory 
        long int length = read(fd, buffer, event_buffer_length); 

        if (length < 0) 
        {
            fprintf(vulture_log, "Unable to read the notify buffer");
        }  
        else
        {
            inotify_rm_watch(fd, wd);
            close(fd);
            fprintf(vulture_log, "Sending SIGINT to %d\n", monitor_pid);
            kill(monitor_pid, SIGINT);
        }
    }
    else
    {
        fprintf(vulture_log, "Provided PID is a kernel level PID %d", monitor_pid);
        return;
    }
}

void vwatch(FILE* vulture_log, const char* vulture_pid)
{

    struct sigaction sa;
    sa.sa_flags = 0;
    sa.sa_handler = monitor_handler;
    sigaction(SIGUSR1, &sa, NULL);

    while(1)
    {
        pid_t vpid = read_vulture_pid(vulture_pid);
        fprintf(vulture_log, "Monitor -> %ld\n", (long)vpid);
        monitor(vulture_log, vpid);
    }
}
