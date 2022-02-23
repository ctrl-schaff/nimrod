/*
 * Main interface for controling the vulture and hunter 
 * agents
 */
#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/prctl.h>
#include <sys/stat.h>
#include <time.h>

#include "vulture.h"
#include "hunter.h"


int main()
{
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
            hunter();
            break;
    }
}
