#include "../filesystem.h"

void touch(char *argv[], int argc)
{
        int ino;
        MINODE *mip;
        char *path = argv[1];

        printf("touch() ----------------\n");

        if (path[0] == '/')
        {
                mip = root;
                dev = root->dev;
        }
        else
        {
                mip = running->cwd;
                dev = running->cwd->dev;
        }

        ino = getino(path, &dev);

        if (ino == 0)
        {
                printf("touch() -- couldn't find directory");
                return;
        }

        mip = iget(dev, ino);

        mip->INODE.i_atime = time(NULL);
        mip->INODE.i_mtime = time(NULL);

        time_t t;

        t = mip->INODE.i_atime;
        printf("touch() -- i_atime is now: %s\n", ctime(&t));
        t = mip->INODE.i_mtime;
        printf("touch() -- i_mtime is now: %s\n", ctime(&t));

        iput(mip);
}
