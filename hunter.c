/*
 * Agent for searching for the specified target through the 
 * /proc filesystem and attempting to terminate it before it
 * can bisect to another instance
 */

#define _POSIX_SOURCE
#define _XOPEN_SOURCE 500

#include "hunter.h"
#include "vtime.h"

void shot(pid_t proc)
{
    kill(proc, SIGTERM);
}

int is_pid_dir(const struct dirent *entry) 
{
    const char *p;
    for (p = entry->d_name; *p; ++p) 
    {
        if (!isdigit(*p))
        {
            return 0;
        }
    }
    return 1;
}

pid_t hunt(FILE* hunter_log, char* psearch)
{
    struct dirent* entry;
    const char* proc_dir = "/proc";
    DIR* dir = opendir(proc_dir);
    if (!dir)
    {
        fprintf(stderr, "Unable to open %s | %d", proc_dir, errno);
        perror(NULL);
        exit(EXIT_FAILURE);
    }

    const int pathlen = 266; // 256(d_name) + 5(/proc) + 5(/stat)
    char proc_path[pathlen];
    int pid = -1;
    FILE* proc_fp = NULL;

    while ((entry = readdir(dir)))
    {
        if (is_pid_dir(entry))
        {
            // Try to open /proc/<PID>/stat.
            int trunc = snprintf(proc_path, sizeof(proc_path), "/proc/%s/stat", entry->d_name);
            if (trunc < 0)
            {
                continue;
            }
            proc_fp = fopen(proc_path, "r");
            if (!proc_fp) 
            {
                perror(proc_path);
                continue;
            }

            fscanf(proc_fp, "%d %s ", &pid, proc_path);
            if (strcmp(psearch, proc_path) == 0)
            {
                fprintf(hunter_log, "%5d %-20s\n", pid, proc_path);
                fclose(proc_fp);
                closedir(dir);
                return pid;
            }
            fclose(proc_fp);
        }
    }
    pid = -1;
    fprintf(hunter_log, "%5d %-20s\n", pid, "UNKNOWN");
    closedir(dir);
    return pid;
}

void hunter(int sleep_time, const char* hunter_log_path)
{
    char* target_name = "(vulture)";
    FILE* hunter_log = fopen(hunter_log_path, "w");
    int wstatus;
    while(1)
    {
        pid_t target_pid = hunt(hunter_log, target_name);
        if (target_pid > 300)
        {
            msleep(sleep_time);
            shot(target_pid);
            waitpid(target_pid, &wstatus, 0);
        }
        else
        {
            break;    
        }
        fflush(hunter_log);
    }
    fclose(hunter_log);
}
