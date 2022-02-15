#define _POSIX_SOURCE
#include <argp.h>
#include <dirent.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wait.h>


// Set up the argument parser
static char doc[] = "trigger -- mechanism for triggering a fork";
static char args_doc[] = "";  

// Order of fields: {NAME, KEY, ARG, FLAGS, DOC, GROUP}.
static struct argp_option options[] = {
    {"pname", 'p', "DEFAULT", 0, "Process name to search", 0},
    { 0, 0, 0, 0, 0, 0} // Last entry should be all zeros in all fields
};

/* Used by main to communicate with parse_opt. */
struct arguments
{
    char* pname;
};

// Order of parameters: KEY, ARG, STATE.
// Parse a single option.
static error_t parse_opt (int key, char *arg, struct argp_state *state)
{
    /* Get the input argument from argp_parse, which we
     know is a pointer to our arguments structure. */
    struct arguments *arguments = state->input;

    // Figure out which option we are parsing, and decide
    // how to store it
    switch (key)
    {
        case 'p':
            arguments->pname = arg;
            break;

        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

static struct argp argp = { options, parse_opt, args_doc, doc, 0, 0, 0 };

void shot(pid_t proc)
{
    printf("Sending SIGTERM to %d\n", proc);
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

pid_t hunt(char* psearch)
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


    while (entry = readdir(dir))
    {
        if (is_pid_dir(entry))
        {
            // Try to open /proc/<PID>/stat.
            snprintf(proc_path, sizeof(proc_path), "/proc/%s/stat", entry->d_name);
            proc_fp = fopen(proc_path, "r");

            if (!proc_fp) 
            {
                perror(proc_path);
                continue;
            }

            fscanf(proc_fp, "%d %s ", &pid, &proc_path);
            if (strcmp(psearch, proc_path) == 0)
            {
                printf("%5d %-20s\n", pid, proc_path);
                fclose(proc_fp);
                closedir(dir);
                return pid;
            }
            fclose(proc_fp);
        }
    }
    pid = -1;
    printf("%5d %-20s\n", pid, "UNKNOWN");
    closedir(dir);
    return pid;
}

int main(int argc, char* argv[])
{
    struct arguments argp_arg;
	argp_arg.pname = "DEFAULT";

	argp_parse(&argp, argc, argv, 0, 0, &argp_arg);
    pid_t disc_proc = hunt(argp_arg.pname);
    if (disc_proc != -1)
    {
        shot(disc_proc);
    }
    return 0;
}
