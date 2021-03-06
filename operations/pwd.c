#include "../filesystem.h"

void pwd()
{
        int ino = running->cwd->ino;
        int dev = running->cwd->dev;

        if(ino == 2)
        {
                printf("/"); //the root
        }
        else {
                searchDirectories(ino, dev);
        }
}

int searchDirectories(int ino, int dev)
{
        int local_ino, pointer_ino;
        char mybuf[BLOCK_SIZE];

        DIR *dp = (DIR *) mybuf;
        char *cp = mybuf;
        MINODE *mip;

        if(ino == 2)
        {
                return 2;
        }

        mip = iget(dev, ino);
        get_block(dev, mip->INODE.i_block[0], mybuf);

        local_ino = dp->inode;

        cp += dp->rec_len;
        dp = (DIR *) cp;

        pointer_ino = dp->inode;
        searchDirectories(pointer_ino, dev);

        getName(local_ino, dev, pointer_ino);

        iput(mip);
}

int getName(int local_ino, int local_dev, int pointer_ino)
{
        MINODE *mip = iget(local_dev, pointer_ino);

        char mybuf[BLOCK_SIZE];
        int i;
        DIR *dp = (DIR *) mybuf;
        char *cp = mybuf, c;


        for(i = 0; i < 12; i++)
        {
                get_block(dev, mip->INODE.i_block[i], mybuf);
                while(cp < mybuf + BLOCK_SIZE)
                {
                        if(dp->inode == local_ino)
                        {
                                c = dp->name[dp->name_len];
                                dp->name[dp->name_len] = 0;
                                printf("/%s", dp->name);
                                c = dp->name[dp->name_len];
                                iput(mip);

                                return 1;
                        }
                        cp += dp->rec_len;
                        dp = (DIR *) cp;
                }
        }

        iput(mip);
        return 0;
}
