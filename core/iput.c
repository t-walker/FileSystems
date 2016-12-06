#include "../core.h"

// IPUT
// Puts an minode itno the core.
void iput(MINODE *mip)
{
        char buffer[BLOCK_SIZE];
        int block_number;
        int offset;
        int inoBlock;

        printf("iput() ------\n");
        mip->refCount--;
        printf("iput() -- refCount is now: %d\n", mip->refCount);

        //printf("iput() -- Decrementing the refCount of the MINODE.\n");
        if(mip->refCount > 0)
        {
          return;
        }

        if(mip->dirty == 0)
        {
          return;
        }else{
          get_block(mip->dev, 2, buffer);
          gp = (GD *)buffer;
          inoBlock = gp->bg_inode_table;

          block_number = (mip->ino - 1) / 8 + inoBlock;
          offset = (mip->ino - 1) % 8;

          get_block(mip->dev, block_number, buffer);
          ip = (INODE *) buffer + offset;

          *ip = mip->INODE;
          put_block(mip->dev, block_number, buffer);
        }
      /*  if (mip->refCount == 0)
        {
                printf("iput() -- mip->dev: %d.\n", mip->dev);

                get_block(mip->dev, 2, buffer);
                gp = (GD *)buffer;
                inoBlock = gp->bg_inode_table;
                printf("iput() -- inoBlock: %d.\n", inoBlock);
                printf("iput() -- block_number = (%d - 1) / 8 + %d = %d.\n", mip->ino, inoBlock, ((mip->ino - 1) / 8 + inoBlock));

                block_number = (mip->ino - 1) / 8 + inoBlock;
                la_number = (mip->ino - 1) % 8;
                printf("iput() -- block_number: %d.\n", block_number);
                printf("iput() -- la_number: %d.\n", la_number);
                printf("iput() -- mip->dev: %d.\n", mip->dev);

                get_block(mip->dev, block_number, buffer);
                ip = (INODE *) buffer + la_number;

                *ip = mip->INODE;
                put_block(mip->dev, block_number, buffer);
        }
        else
        {
                //printf("iput() -- refCount not zero\n");
        }*/

}
