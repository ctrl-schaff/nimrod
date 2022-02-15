#define _POSIX_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <wait.h>

static unsigned int FORK_FLAG = 0;

/* https://codereview.stackexchange.com/questions/29198/random-string-generator-in-c */
static char* form_rand_string(char* str, size_t size)
{
    const char charset[] = "abcdefghijklmnopqrstuvwxyz";
    int charset_size = 25;
    int key = 0;
    if (size > 0) 
    {
        --size;
        for (size_t n = 0; n < size; n++) 
        {
            key = rand() % charset_size;
            str[n] = charset[key];
        }
        str[size] = '\0';
    }
    return str;
}

static long hash()
{
    long hash = 0;
    size_t str_size = 10;
    char rand_str[str_size]; 
    form_rand_string(rand_str, str_size);
    const int a = rand() * 10;
    const int m = rand() * 15;
    for (int i = 0; i < str_size; ++i)
    {
        hash += (long)pow(a, str_size - (i+1)) * rand_str[i];
        hash = hash % m;
    }
    return hash;
}

static void signal_forker(int sig)
{
    FORK_FLAG = 1;
}

void forking()
{
    FILE* fork_file = fopen("./forkstat.log", "w");
    fprintf(fork_file, "Hash Value | PID \n");
    struct sigaction sa;
    sa.sa_flags = 0;
    sa.sa_handler = signal_forker;
    sigaction(SIGINT, &sa, NULL);
    while(1)
    {
        long hash_val = hash(); 
        sleep(1);
        fprintf(fork_file, "%ld %ld\n", hash_val, (long)getpid());

        if (FORK_FLAG == 1)
        {
            FORK_FLAG = 0;
            switch (fork()) {
                case -1:
                    fprintf(stderr, "Fork failure\n");
                    return;

                case 0:
                    fprintf(fork_file, "%ld -> %ld\n", 
                            (long)getppid(), (long)getpid());
                    kill(getppid(), SIGTERM);
                default:
                    wait(NULL);
            }
        }
        fflush(fork_file);
    }  
    fclose(fork_file);
}

int main()
{
    time_t time_seed;
    srand((unsigned) time(&time_seed));

    setbuf(stdout, NULL); // Disable buffering of stdout
    forking();
}
