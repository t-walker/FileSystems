#include "../core.h"

// GETINO
// gets the inode number given a path and an minode.
int getino(char *path, int *dev)
{
        printf("getino() ------\n");

        int i, inum = 0, bnum, offset;
        int num_paths;
        char x;
        printf("getino() -- initalized variables.\n");

        MINODE *mip;
        INODE *ip = (INODE*) malloc(sizeof(INODE));

        printf("getino() -- created a temporary MINODE.\n");

        char *strs[100];


        if(path[0] == '/') {
                dev = root->dev;
                inum = root->ino;
                printf("getino() -- the path begins at root.\n");
        }
        else {
                dev = running->cwd->dev;
                inum = running->cwd->ino;
                printf("getino() -- the path is relative.\n");
        }


        printf("getino() -- parsing the path.\n");
        printf("getino() -- path: %s.\n", path);

        num_paths = parse(path, "/", strs);

        for (i = 0; i < num_paths; i++)
        {
                mip = iget(dev, inum);
                printf("path %s\n", strs[i]);
                inum = search(mip, strs[i]);
                printf("getino() -- inum: %d.\n", inum);

                if(inum == 0)
                {
                        printf("File not found\n");
                        return 0;
                }

                printf("getino() -- setting mp to buf + offset.\n");

                iput(mip);
        }
        printf("getino() -- returning inum\n");

        return inum;
}
