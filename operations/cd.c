#include "../filesystem.h"

// Change the directory
void cd(char *pathname)
{
        printf("cd() -------\n");

        int ino = running->cwd->ino;
        int dev = running->cwd->dev;
        MINODE *mip;

        printf("cd() -- assigned variables and finished parsing\n");

        if (pathname) // Check to make sure the input isn't a newline.
        {
                printf("cd() -- pathname isn't null\n");

                parse(pathname, "/", path);

                if (path[0] == '/') // If the character starts at root, it's not relative.
                {
                        dev = root->dev; // Start it at the root.
                        printf("cd() -- assigning to root\n");
                }

                printf("cd() -- getting the inum\n");
                ino = getino(pathname, dev);
                printf("cd() -- got the inum: %d\n", ino);

                mip = iget(dev, ino);

                if (S_ISDIR(mip->INODE.i_mode))
                {
                        running->cwd = mip;
                        printf("cd() -- changed the directory\n");
                        printf("cd() -- running->cwd->ino: %d\n", running->cwd->ino);
                }
                else
                {
                        printf("cd() -- cannot cd into a file\n");
                }

                iput(mip);

        }
        else
        {
                printf("cd() -- setting to drive root");
                running->cwd = root;
        }
}
