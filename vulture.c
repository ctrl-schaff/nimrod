/*
 * Agent for attempting to avoid detection from
 * hunter by bisecting its processing when signaled
 * and splitting to a child process while killing the
 * parent
 */
#define _POSIX_SOURCE
#include "task.h"
#include "vulture.h"


static unsigned int FORK_FLAG = 0;
void signal_forker(int sig)
{
    if (sig == SIGINT)
    {
        FORK_FLAG = 1;
    }
}


void circle(FILE* fork_file)
{
    while(1)
    {
        long hash_val = hash(); 
        sleep(1);
        fprintf(fork_file, "%ld %ld\n", hash_val, (long)getpid());

        if (FORK_FLAG == 1)
        {
            FORK_FLAG = 0;
            switch (fork()) 
            {
                case -1:
                    fprintf(stderr, "Fork failure\n");
                    return;

                case 0:
                    fprintf(fork_file, "%ld -> %ld\n", 
                            (long)getppid(), (long)getpid());
                    kill(getppid(), SIGTERM);
                    break;

                default:
                    wait(NULL);
                    break;
            }
        }
        fflush(fork_file);
    }  
    fclose(fork_file);
}

void monitor()
{
    char watch_dir[256];
    snprintf(watch_dir, sizeof(watch_dir), "/proc/%d", getpid());

    static const int event_size = sizeof(struct inotify_event);
    static const int event_buffer_length = (1024 * (event_size + 16));
    char buffer[event_buffer_length];

    int fd = inotify_init();
    if (fd < 0) 
    {
        fprintf(stderr, "Issue with inotify_init");
        return;
    }

    int wd = inotify_add_watch(fd, watch_dir, IN_ACCESS | IN_OPEN);

    // Blocking read to determine the event change happens on the watch directory 
    long int length = read(fd, buffer, event_buffer_length); 

    if (length < 0) 
    {
        fprintf(stderr, "Unable to read the notify buffer");
    }  
    else
    {
        inotify_rm_watch(fd, wd);
        close(fd);

        printf("Sending SIGINT to %d\n", getppid());
        kill(getppid(), SIGINT);
    }
}

void vulture()
{
    FILE* fork_file = fopen("./log/vulture.log", "w");
    fprintf(fork_file, "Hash Value | PID \n");

    struct sigaction sa;
    sa.sa_flags = 0;
    sa.sa_handler = signal_forker;
    sigaction(SIGINT, &sa, NULL);

    switch (fork()) 
    {
        case -1:
            fprintf(stderr, "Fork failure\n");
            return;

        case 0:
            monitor();
            break;

        default:
            circle(fork_file);
            break;
    }
}

