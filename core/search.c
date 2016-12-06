#include "../core.h"

// SEARCH
// Searches for a path within an inode.
int search(MINODE *mip, char *pathname)
{
        printf("search() ------\n");

        int i, j;
        char *cp, *tmp;
        int dev;

        char dir[200];

        for(i = 0; i < 12; i++)
        {
                printf("search() -- loop: %d.\n", i);
                printf("search() -- minode dev: %d ino: %d\n", mip->dev, mip->ino);
                dev = mip->dev;

                if(mip->INODE.i_block[i])
                {
                        printf("search() -- mip->INODE.i_block[%d] isn't null\n", i);
                        get_block(dev, mip->INODE.i_block[i], buf);
                        dp = (DIR *)buf;
                        cp = buf;

                        while(cp < buf + 1024)
                        {
                                printf("search() -- %s %d vs %s %d\n", dp->name, strlen(dp->name), pathname, strlen(pathname));
                                j=0;
                                while(j < dp->name_len)
                                {
                                        dir[j] = dp->name[j];
                                        j++;
                                }
                                dir[j] = '\0';

                                if (strcmp(dir, pathname) == 0)
                                {
                                        printf("search() -- found directory: %s\n", dp->name);
                                        return dp->inode;
                                }

                                cp += dp->rec_len;
                                dp = (DIR *)cp;
                        }
                }
        }
        return 0;
}
