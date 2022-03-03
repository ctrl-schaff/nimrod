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
    {"hunterfile", 'h', "HUNTERFILE", 0, "Hunter agent log file path", 0},
    {"huntersleep", 'i', "HSLEEP", 0, "Hunter agent sleep time (ms)", 0},
    {"vulturefile", 'v', "VULTUREFILE", 0, "Vulture agent log file path", 0},
    {"vulturesleep", 'w', "VSLEEP", 0, "Vulture agent sleep time (ms)", 0},
    { 0, 0, 0, 0, 0, 0}
};

struct arguments
{
    char* hlog;
    int hsleep;
    char* vlog;
    int vsleep;
};

static error_t parse_opt(int key, char* arg, struct argp_state* state)
{
    struct arguments* arguments = state->input;

    switch (key)
    {
        case 'h':
            arguments->hlog = arg;
            break;

        case 'i':
            arguments->hsleep = atoi(arg);
            break;

        case 'v':
            arguments->vlog = arg;
            break;

        case 'w':
            arguments->vsleep = atoi(arg);
            break;

        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

static struct argp argp = { options, parse_opt, args_doc, doc, 0, 0, 0 };

int main(int argc, char* argv[])
{
    time_t time_seed;
    srand((unsigned) time(&time_seed));

    struct arguments argp_arg;
	argp_arg.hsleep = 2000;
    argp_arg.vsleep = 100;

    char hlog[200];
    sprintf(hlog, "./log/hunter%d.log", argp_arg.hsleep);
    char vlog[200]; 
    sprintf(vlog, "./log/vulture%d.log", argp_arg.vsleep);

	argp_arg.hlog = hlog;
	argp_arg.vlog = vlog;
	argp_parse(&argp, argc, argv, 0, 0, &argp_arg);

    const char* vulture_proc = "vulture";
    const char* hunter_proc = "hunter";

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
            vulture(argp_arg.vsleep, argp_arg.vlog);
            break;
        default:
            if (prctl(PR_SET_NAME, (unsigned long) hunter_proc) < 0)
            {
                fprintf(stderr, "Unable to name parent process to %s", hunter_proc);
                return -1;
            }
            sleep(5);
            hunter(argp_arg.hsleep, argp_arg.hlog);
            break;
    }
}
