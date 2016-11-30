#include "../filesystem.h"

// Change the directory
void cd(char *pathname)
{
        printf("cd() -------\n");
        int ino = running->cwd->ino, dev = running->cwd->dev;

        parse(pathname, "/", path);

        printf("cd() -- assigned variables and finished parsing\n");

        if (path[0] != '\n') // Check to make sure the input isn't a newline.
        {
                if (path[0] == '/') // If the character starts at root, it's not relative.
                {
                        dev = root->dev; // Start it at the root.
                        printf("cd() -- assigning to root\n");

                }
                printf("cd() -- getting the inum\n");
                ino = getino(pathname, dev);
                printf("cd() -- got the inum: %d\n", ino);

                running->cwd = iget(dev, ino);
                printf("cd() -- changed the directory\n");
                printf("cd() -- running->cwd->ino: %d\n", running->cwd->ino);
        }
        else
        {
                running->cwd = root;
        }
}
