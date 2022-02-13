#define _POSIX_SOURCE
#include <argp.h>
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
    {"pid", 'p', -1, 0, "Process PID to send signal", 0},
    { 0, 0, 0, 0, 0, 0} // Last entry should be all zeros in all fields
};

/* Used by main to communicate with parse_opt. */
struct arguments
{
    int pid;
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
            arguments->pid = atoi(arg);
            break;

        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

static struct argp argp = { options, parse_opt, args_doc, doc, 0, 0, 0 };

void trigger(pid_t proc)
{
    printf("Sending SIGINT to %d\n", proc);
    kill(proc, SIGINT);
}

int main(int argc, char* argv[])
{
    struct arguments argp_arg;
	argp_arg.pid = 0;

	argp_parse(&argp, argc, argv, 0, 0, &argp_arg);
    trigger(argp_arg.pid);
    return 0;
}
