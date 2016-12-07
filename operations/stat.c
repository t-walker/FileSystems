#include "../filesystem.h"

void my_stat(char *pathname)
{
        printf("cat() ---------------------\n");
        MINODE *mip;

        int ino = running->cwd->ino;
        int dev = running->cwd->dev;

        ino = getino(pathname, dev);


        if (!ino)
        {
                return;
        }

        mip = iget(dev, ino);

        printf("device: %4d  indode:  %4d\n", dev, ino);
        printf("size:   %4d  nlink:   %4d\n", mip->INODE.i_size, mip->INODE.i_links_count);
        printf("file mode: %o  gid: %d uid: %d\n", mip->INODE.i_mode, mip->INODE.i_gid, mip->INODE.i_uid);

        iput(mip);
}
