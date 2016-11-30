#include "../filesystem.h"

void pwd()
{
        int ino = running->cwd->ino;
        int dev = running->cwd->dev;

        if(2 == ino)
        {
                printf("/");
        }else{
                searchDirectories(ino, dev);
        }
}

int searchDirectories(int ino, int dev)
{
        int local_ino, pino;
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

        pino = dp->inode;

        searchDirectories(pino, dev);

        getName(local_ino, dev, pino);

        iput(mip);
}

int getName(int local_ino, int local_dev, int pino)
{
        char buf[BLOCK_SIZE];
        int i;
        DIR *dp = (DIR *) buf;
        char *cp = buf;
        char c;
        MINODE *mip;

        mip = iget(local_dev, pino);
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
