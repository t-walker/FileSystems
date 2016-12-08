#include "../filesystem.h"

#define u16 unsigned int

/* Helper Function */
u16 convertOctalToDecimal(int octalNumber)
{
        int decimalNumber = 0, i = 0;

        while(octalNumber != 0)
        {
                decimalNumber += (octalNumber % 10) * pow(8,i);
                ++i;
                octalNumber/=10;
        }

        i = 1;

        return decimalNumber;
}
/* End Helper Function */

void ch_mod(char *args[], int argv)
{
        dev = running->cwd->dev; // get the current working directory
        MINODE *mip;             // initalize an empty inode

        int i = 0, ino;

        printf("chmod() --------------\n");

        char *permissions = args[1]; // grab the permissions from the args
        char *path = args[2];        // grab the path from the args

        u16 permission_i = convertOctalToDecimal(atoi(permissions)); // convert the permissions from decimal
        printf("Permission octal: %3o\n", permission_i);             // print out the permission in octal
        u16 permission_o = 0; // initalize the out permissions

        if (args[1][0] == '/') // the path is starting from root not relative
        {
                dev = root->dev; // the dev starts at root
        }

        ino = getino(path, dev); // grab the ino given the path and dev

        printf("chmod() -- path: %s\n", path);                // print out the user entered path
        printf("chmod() -- permissions: %s\n", permissions);  // print out the user entered permissions

        if(ino == 0) // if there's no ino then there's no file/directory
        {
                printf("chmod() -- could not find path");
                return; // just break out
        }

        if (strlen(permissions) <= 4) // check to make sure the permissions string is the correct length
        {
                mip = iget(dev, ino); // get the mip given the dev and ino.

                permission_o = mip->INODE.i_mode &= 61440; // mask the i_mode's file type
                permission_o += permission_i;              // add the desired permissions

                mip->INODE.i_mode = permission_o;          // store the INODE's i_mode

                printf("chmod() -- permission: %3o\n", mip->INODE.i_mode);
        }
        else
        {

        }

        mip->dirty = 1; // we need to make changes
        iput(mip);      // store the inode
}
