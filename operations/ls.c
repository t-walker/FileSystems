#include "../filesystem.h"

// List the files in a directory
void ls (char *pathname)
{
        //printf("ls() -----\n");
        int i;
        int ino = running->cwd->ino;
        int dev = running->cwd->dev;
        char buffer[BLOCK_SIZE];
        DIR *dp = (DIR *)buffer;
        char *cp = buffer;
        char c;

        MINODE *mip = iget(dev, ino);
        //printf("ls() -- running->cwd->ino: %d\n", running->cwd->ino);
        //printf("ls() -- mip = running->cwd\n");

        if (pathname)
        {
                //printf("ls() -- pathname isn't null\n");
                //printf("ls() -- pathname is: %s\n", pathname);

                if (pathname[0] == '/')
                {
                        dev = root->dev;
                        //printf("ls() -- pathname starts with '/', root->dev\n");
                }

                //printf("ls() -- calling getino\n");
                ino = getino(pathname, dev);
                //printf("ls() -- finished getino()\n");

                mip = iget(dev, ino);
                //printf("ls() -- finished iget()\n");
                // mip points at the minode;
                // Each datablock of mip->INODE contains DIR entries
                // print the neame of strings of the DIR entries

                //printf("ls() -- calling printDir()\n");
        }

        for (i = 0; i < 12; i++)
        {
                dp = buffer;
                cp = buffer;
                c = NULL;

                if (mip->INODE.i_block[i] == 0)
                {
                        iput(mip);
                        return 0;
                }

                get_block(dev, mip->INODE.i_block[i], buffer);

                while (cp < buf + BLOCK_SIZE)
                {
                  printDetail(dp->inode, dev, dp->name);
                }

                cp += dp->rec_len;
                dp = (DIR *)cp;
        }

        if (S_ISDIR(mip->INODE.i_mode))
        {
                //printf("IS directory\n");
                //printDir(mip->INODE, dev);
        }
        else
        {
                printf("IS NOT A DIRECTORY");
        }

        iput(mip);
}

void printDetail(int ino, int dev, char *fname)
{

}

// void printDir(INODE ptr, int dev)
// {
//         if((ptr.i_mode & 0x4000) == 0x4000)
//         {
//                 time_t t;
//                 //printf("printDir()\n");
//                 char buf[1024];
//                 char *cp;
//                 get_block(dev, ptr.i_block[0],buf);
//                 dp = (DIR *)buf;
//                 cp = buf;
//
//                 while(cp < buf + 1024) {
//                         t = ptr.i_mtime;
//                         printf("%6o %4d  %4d  %4d  %15s \t %20s", ptr.i_mode, dp->inode, dp->rec_len, dp->name_len, dp->name, ctime(&t)); //prints out traversal
//                         cp += dp->rec_len;
//                         dp = (DIR *) cp; //shut up
//                 }
//                 //printf("finished printDir()\n");
//         }
//         else
//         {
//                 //printf("not a directory!\n");
//         }
// }
