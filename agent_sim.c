/*
 * Main interface for controling the vulture and hunter 
 * agents
 */
#include <time.h>
#include <stdlib.h>

#include "vulture.h"
#include "hunter.h"


int main()
{
    time_t time_seed;
    srand((unsigned) time(&time_seed));
    
    switch (fork()) 
    {
        case -1:
            fprintf(stderr, "Fork failure\n");
            return -1;
        case 0:
            vulture();
            break;
        default:
            sleep(1);
            hunter();
            break;
    }
}
