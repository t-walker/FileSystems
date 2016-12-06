#include "filesystem.h"

// GETINO
// gets the inode number given a path and an minode.
int getino(char *path, int *dev)
{
        //printf("getino() ------\n");

        int i, inum = 0, bnum, offset;
        int num_paths;
        char x;
        //printf("getino() -- initalized variables.\n");

        MINODE *mip;
        INODE *ip = (INODE*) malloc(sizeof(INODE));

        //printf("getino() -- created a temporary MINODE.\n");

        char *strs[100];


        if(path[0] == '/') {
                dev = root->dev;
                inum = root->ino;
                //printf("getino() -- the path begins at root.\n");
        }
        else {
                dev = running->cwd->dev;
                inum = running->cwd->ino;
                //printf("getino() -- the path is relative.\n");
        }


        //printf("getino() -- parsing the path.\n");
        //printf("getino() -- path: %s.\n", path);

        num_paths = parse(path, "/", strs);

        for (i = 0; i < num_paths; i++)
        {
                mip = iget(dev, inum);
                //printf("path %s\n", strs[i]);
                inum = search(mip, strs[i]);
                //printf("getino() -- inum: %d.\n", inum);

                if(inum == 0)
                {
                        //printf("File not found\n");
                        return 0;
                }

                //printf("getino() -- setting mp to buf + offset.\n");

                iput(mip);
        }
        //printf("getino() -- returning inum\n");

        return inum;
}

// IGET
// Retrieves an inode given a dev and an inode.
MINODE *iget(int dev, int ino)
{
  //printf("iget() ------\n");
        MINODE *mip = malloc(sizeof(MINODE));
        int i, blk_num, offset;
        //printf("iget() -- allocated *mip.\n");

        //Loop through all of the minodes to see if the one we want is in memory
        for (i = 0; i < NMINODE; i++) {
	  //printf("iget() --- inode index in minode: %d\n",i);
                if (minode[i].ino == ino) // Is this the inode we're looking for?
                {
		  //printf("iget() --- inode already in memory\n",i);
                        minode[i].refCount++; // Add the reference of it.
                        return &minode[i];
                }
        }

        //printf("iget() -- found the MINODE.\n");

        //Use mailman's to get the block location
        blk_num = ((ino - 1)/8) + inode_start;
        offset = (ino - 1) % 8;
        //printf("iget() -- mailman's algorithm:\n");
        //printf("iget() -- blk_num %d.\n", blk_num);
        //printf("iget() -- offset %d.\n", offset);

        //Get the block
        get_block(dev, blk_num, buf);
        //printf("iget() -- got the block for the blk_num.\n");

        ip = (INODE *)buf + offset;

        //printf("iget() -- got the INODE\n");


        //Loop through all of the minodes until we find one that is empty
        for (i = 0; i < NMINODE; i++) {
	  //printf("iget() --- inode index in minode: %d\n",i);
                if (minode[i].refCount == 0) //Check to make sure it's empty
                {
		  // printf("iget() --- found an empty space in minode\n",i);
                        /***
                         * 1. Set the minode's inode to the temorary inode
                         * 2. Set the dirty value to 0
                         * 3. Set the device number (works like a fd)
                         * 4. Set the inode number
                         * 5. Increment the refcount
                         ***/
                        minode[i].INODE = *ip;
                        minode[i].dirty = 0;
                        minode[i].dev = dev;
                        minode[i].ino = ino;
                        minode[i].refCount = 1;

                        //printf("iget() -- Empty MINODE found.\n");
                        //printf("iget() -- Returning MINODE.\n");

                        //Return the index of the inode
                        return &minode[i];
                }
        }
}

// IPUT
// Puts an minode itno the core.
void iput(MINODE *mip)
{
        //printf("iput() ------\n");

        int blk_num, offset;
        mip->refCount -= 1;
        //printf("iput() -- Decrementing the refCount of the MINODE.\n");

        if (mip->refCount == 0)
        {
                //printf("iput() -- MINODE refCount is 0.\n");

                blk_num = ((mip->ino - 1)/8) + inode_start;
                offset = (mip->ino-1) % 8;
                //printf("iput() -- blk_num: %d.\n", blk_num);
                //printf("iput() -- offset: %d.\n", offset);

                get_block(mip->dev, blk_num, buf);
                //printf("iput() -- got the block.\n");

                ip = (INODE *)buf + offset;
                memcpy(ip, &(mip->INODE), sizeof(INODE));
                //printf("iput() -- copied the block to memory.\n");

                put_block(dev, blk_num, buf);
                //printf("iput() -- put the block\n");
        }
}

// SEARCH
// Searches for a path within an inode.
int search(MINODE *mip, char *pathname)
{
        //printf("search() ------\n");

        int i, j;
        char *cp, *tmp;
        int dev;

        char dir[200];
        char mybuf[BLKSIZE];

        for(i = 0; i < 12; i++)
        {
                //printf("search() -- loop: %d.\n", i);
                //printf("search() -- minode dev: %d ino: %d\n", mip->dev, mip->ino);
                dev = mip->dev;

                if(mip->INODE.i_block[i])
                {
                        //printf("search() -- mip->INODE.i_block[%d] isn't null\n", i);
                        get_block(dev, mip->INODE.i_block[i], mybuf);
                        dp = (DIR *)mybuf;
                        cp = mybuf;

                        while(cp < mybuf + 1024)
                        {
                                //printf("search() -- %s %d vs %s %d\n", dp->name, strlen(dp->name), pathname, strlen(pathname));
                                j=0;
                                while(j < dp->name_len)
                                {
                                  dir[j] = dp->name[j];
                                  j++;
                                }
                                dir[j] = '\0';

                                if (strcmp(dir, pathname) == 0)
                                {
                                        //printf("search() -- found directory: %s\n", dp->name);
                                        return dp->inode;
                                }

                                cp += dp->rec_len;
                                dp = (DIR *)cp;
                        }
                }
        }
        return 0;
}
