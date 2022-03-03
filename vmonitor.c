#define _POSIX_SOURCE
#define _XOPEN_SOURCE 500

#include "vmonitor.h"
#include "hunter.h"

void vmonitor_handler(int sig)
{
    if (sig == SIGTERM)
    {
        _exit(0);
    }
}

pid_t read_vulture_pid(const char* vulture_pid)
{
    FILE* vpidf = fopen(vulture_pid, "r");

    int vpid = 0;
    fscanf(vpidf, "%d", &vpid);
    return (pid_t)vpid;
}

void monitor(FILE* vmonitor_log, pid_t monitor_pid)
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
            fprintf(vmonitor_log, "Issue with inotify_init");
            return;
        }

        int wd = inotify_add_watch(fd, watch_dir, IN_ACCESS | IN_OPEN);

        // Blocking read to determine the event change happens on the watch directory 
        long int length = read(fd, buffer, event_buffer_length); 

        if (length < 0) 
        {
            fprintf(vmonitor_log, "Unable to read the notify buffer");
        }  
        else
        {
            inotify_rm_watch(fd, wd);
            close(fd);
            fflush(vmonitor_log);
            kill(monitor_pid, SIGINT);
        }
    }
    else
    {
        fprintf(vmonitor_log, "Provided PID is a kernel level PID %d", monitor_pid);
        return;
    }
    fflush(vmonitor_log);
}

void _vwatch(FILE* vmonitor_log, const char* vulture_pid)
{
    while(1)
    {
        pid_t vpid = read_vulture_pid(vulture_pid);
        if (vpid > 300)
        {
            monitor(vmonitor_log, vpid);
            fflush(vmonitor_log);
        }
    }
    fclose(vmonitor_log);
}

void vwatch(const char* vulture_pid)
{
    struct sigaction sa;
    sa.sa_flags = 0;
    sa.sa_handler = vmonitor_handler;
    sigaction(SIGTERM, &sa, NULL);

    FILE* vmonitor_log = fopen("./log/monitor.log", "w");
    switch (fork()) 
    {
        case -1:
            fprintf(vmonitor_log, "Fork failure\n");
            return;
        case 0:
            sleep(1);
            hunt(vmonitor_log, "(vulture)");
            break;
        default:
            _vwatch(vmonitor_log, vulture_pid);
            break;
    }
}
