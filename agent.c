/*
 * Main interface for controling the vulture and hunter 
 * agents
 */
#include <argp.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/prctl.h>
#include <sys/stat.h>
#include <time.h>

#include "vulture.h"
#include "hunter.h"

static char doc[] = "trigger -- mechanism for triggering a fork";
static char args_doc[] = "";  

// Order of fields: {NAME, KEY, ARG, FLAGS, DOC, GROUP}.
static struct argp_option options[] = {
    {"huntersleep", 'h', "SLEEP", 0, "Hunter agent sleep time (ms)", 0},
    { 0, 0, 0, 0, 0, 0}
};

struct arguments
{
    int hsleep;
};

static error_t parse_opt(int key, char* arg, struct argp_state* state)
{
    struct arguments* arguments = state->input;

    switch (key)
    {
        case 'h':
            arguments->hsleep = atoi(arg);
            break;

        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

static struct argp argp = { options, parse_opt, args_doc, doc, 0, 0, 0 };

int main(int argc, char* argv[])
{
    struct arguments argp_arg;
	argp_arg.hsleep = 2000;

	argp_parse(&argp, argc, argv, 0, 0, &argp_arg);

    const char* vulture_proc = "vulture";
    const char* hunter_proc = "hunter";
    time_t time_seed;
    srand((unsigned) time(&time_seed));
    
    switch (fork()) 
    {
        case -1:
            fprintf(stderr, "Fork failure\n");
            return -1;
        case 0:
            if (prctl(PR_SET_NAME, (unsigned long) vulture_proc) < 0)
            {
                fprintf(stderr, "Unable to name child process to %s", vulture_proc);
                return -1;
            }
            vulture();
            break;
        default:
            if (prctl(PR_SET_NAME, (unsigned long) hunter_proc) < 0)
            {
                fprintf(stderr, "Unable to name parent process to %s", hunter_proc);
                return -1;
            }
            sleep(5);
            hunter(argp_arg.hsleep);
            break;
    }
}
