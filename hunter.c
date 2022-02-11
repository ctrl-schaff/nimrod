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

void forking(FILE* fork_file, long hash_limit, long fork_limit)
{
    long hash_count = 0;
    while(1)
    {
        long hash_val = hash(); 
        sleep(1);
        fprintf(fork_file, "Hash Value %ld | PID %ld\n", hash_val, (long)getpid());
        ++hash_count;
        if (hash_count >= hash_limit)
        {
            switch (fork()) {
                case -1:
                    fprintf(stderr, "Fork failure\n");
                    return;

                case 0:
                    if (fork_limit >= 0)
                    {
                        pid_t parent_pid = getppid();
                        fprintf(fork_file, "%ld -> %ld\n", (long)parent_pid, (long)getpid());
                        kill(parent_pid, SIGTERM);
                        --fork_limit;
                        hash_count = 0;
                        /* forking(fork_file, hash_limit,  fork_limit); */
                    }
                    else
                    {
                        kill(getppid(), SIGTERM);
                        return;
                    }
                default:
                    wait(NULL);
            }
        }   
    }  
}

int main()
{
    long hash_limit = 5;
    long fork_limit = 5;
    
    time_t time_seed;
    srand((unsigned) time(&time_seed));

    setbuf(stdout, NULL); // Disable buffering of stdout
    FILE* fork_file = fopen("./forkstat.log", "w");
    forking(fork_file, hash_limit, fork_limit);
    fclose(fork_file);
}
