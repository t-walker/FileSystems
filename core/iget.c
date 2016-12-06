#include "../core.h"

// IGET
// Retrieves an inode given a dev and an inode.
MINODE *iget(int dev, int ino)
{
        printf("iget() ------\n");
        MINODE *mip = malloc(sizeof(MINODE));
        int i, blk_num, offset;
        printf("iget() -- allocated *mip.\n");

        //Loop through all of the minodes to see if the one we want is in memory
        for (i = 0; i < NMINODE; i++) {
                if (minode[i].ino == ino) // Is this the inode we're looking for?
                {
                        minode[i].refCount++; // Add the reference of it.
                        return &minode[i];
                }
        }

        printf("iget() -- found the MINODE.\n");

        //Use mailman's to get the block location
        blk_num = ((ino - 1)/8) + inode_start;
        offset = (ino - 1) % 8;
        printf("iget() -- mailman's algorithm:\n");
        printf("iget() -- blk_num %d.\n", blk_num);
        printf("iget() -- offset %d.\n", offset);

        //Get the block
        get_block(dev, blk_num, buf);
        printf("iget() -- got the block for the blk_num.\n");

        ip = (INODE *)buf + offset;

        printf("iget() -- got the INODE\n");


        //Loop through all of the minodes until we find one that is empty
        for (i = 0; i < NMINODE; i++) {
                if (minode[i].refCount == 0) //Check to make sure it's empty
                {
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

                        printf("iget() -- Empty MINODE found.\n");
                        printf("iget() -- Returning MINODE.\n");

                        //Return the index of the inode
                        return &minode[i];
                }
        }
}
