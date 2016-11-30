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
        char buf[BLOCK_SIZE];

        DIR *dp = (DIR *) buf;
        char *cp = buf;
        MINODE *mip;

        if(ino == 2)
        {
                return 2;
        }

        mip = iget(dev, ino);
        get_block(dev, mip->INODE.i_block[0], buf);

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

        char buf[BLOCK_SIZE];
        int i;
        DIR *dp = (DIR *) buf;
        char *cp = buf, c;


        for(i = 0; i < 12; i++)
        {
                get_block(dev, mip->INODE.i_block[i], buf);
                while(cp < buf + BLOCK_SIZE)
                {
                        if(dp->inode == local_ino)
                        {
                                c = dp->name[dp->name_len];
                                dp->name[dp->name_len] = 0;
                                printf("/%s", dp->name);
                                c = dp->name[dp->name_len];
                                return 1;
                        }
                        cp += dp->rec_len;
                        dp = (DIR *) cp;
                }
        }
        return 0;
}
