#include "../filesystem.h"

// List the files in a directory
void ls (char *pathname)
{
        printf("ls() -----\n");
        int ino = running->cwd->ino, dev = running->cwd->dev;
        MINODE *mip = iget(dev, running->cwd->ino);
        printf("ls() -- running->cwd->ino: %d\n", running->cwd->ino);
        printf("ls() -- mip = running->cwd\n");

        if (pathname)
        {
                printf("ls() -- pathname isn't null\n");
                printf("ls() -- pathname is: %s\n", pathname);

                if (pathname[0] == '/')
                {
                        dev = root->dev;
                        printf("ls() -- pathname starts with '/', root->dev\n");
                }

                printf("ls() -- calling getino\n");
                ino = getino(pathname, dev);
                printf("ls() -- finished getino()\n");

                mip = iget(dev, ino);
                printf("ls() -- finished iget()\n");
                // mip points at the minode;
                // Each datablock of mip->INODE contains DIR entries
                // print the neame of strings of the DIR entries

                printf("ls() -- calling printDir()\n");
                // if(mip->dINODE.i_mode & 0xF000) == 0xA000)
                // {
                //
                // }
                printDir(mip->INODE, dev);
        }
        else
        {

                printDir(mip->INODE, dev);
        }
}
