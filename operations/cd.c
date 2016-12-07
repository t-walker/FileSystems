#include "../filesystem.h"

// Change the directory
void cd(char *pathname)
{
        printf("cd() -------\n");

        int ino = running->cwd->ino; // get the running directory's ino
        int dev = running->cwd->dev; // get the running directory's device

        MINODE *mip;  // initalize an mip

        printf("cd() -- assigned variables and finished parsing\n");

        if (pathname) // Check to make sure there's a pathname
        {
                printf("cd() -- pathname isn't null\n");

                parse(pathname, "/", path); // parse the pathname into path.

                if (path[0] == '/') // If the character starts at root, it's not relative.
                {
                        dev = root->dev; // Reassign to root
                        printf("cd() -- assigning to root\n");
                }

                printf("cd() -- getting the ino\n");
                ino = getino(pathname, dev); // get the ino of the current pathname
                printf("cd() -- got the ino: %d\n", ino);

                mip = iget(dev, ino); // get the mip given a dev and ino

                if (S_ISDIR(mip->INODE.i_mode)) // if it's a directory, change the directory.
                {
                        running->cwd = mip;
                        printf("cd() -- changed the directory\n");
                        printf("cd() -- running->cwd->ino: %d\n", running->cwd->ino);
                }
                else // otherwise you can't do anything
                {
                        printf("cd() -- cannot cd into a file\n");
                }

                iput(mip); // return the mip

        }
        else // the user just entered in 'cd'
        {
                printf("cd() -- setting to drive root");
                running->cwd = root; // set the cwd back to the root
        }
}
