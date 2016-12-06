#include "../filesystem.h"

void quit()
{
        int i;

        for(i = 0; i < NMINODE; i++)
        {
                minode[i].refCount = 1;
                iput(&minode[i]);
        }

        printf("exiting program\n");
        exit(1);
}
