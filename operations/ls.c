#include "../filesystem.h"

// List the files in a directory
void ls (char *pathname)
{
        //printf("ls() -----\n");
        int i;                         // initalize the empty int
        int ino = running->cwd->ino;   // get the current ino
        int dev = running->cwd->dev;   // get the current dev

        MINODE *mip; // initalize a *mip

        //printf("ls() -- running->cwd->ino: %d\n", running->cwd->ino);
        //printf("ls() -- mip = running->cwd\n");

        if (pathname) // check to see if we need to change directories at all
        {
                //printf("ls() -- pathname isn't null\n");
                //printf("ls() -- pathname is: %s\n", pathname);

                if (pathname[0] == '/') // the path isn't relative
                {
                        dev = root->dev; // assign back to the root
                        //printf("ls() -- pathname starts with '/', root->dev\n");
                }

                //printf("ls() -- calling getino\n");
                ino = getino(pathname, dev); // get the ino given the pathname and dev
                //printf("ls() -- finished getino()\n");

                mip = iget(dev, ino); // get the mip given the dev and ino

                //printf("ls() -- finished iget()\n");
                // mip points at the minode;
                // Each datablock of mip->INODE contains DIR entries
                // print the neame of strings of the DIR entries

                //printf("ls() -- calling printDir()\n");

                if (S_ISDIR(mip->INODE.i_mode)) // Check to make sure it's a directory and not a file
                {
                        //printf("IS directory\n");
                        print_child_inodes(mip, dev); // print the child's information
                }
                else
                {
                        printf("IS NOT A DIRECTORY"); // it's not a directory
                }

                iput(mip); // put the mip back

        }
        else
        {
                mip = iget(dev, running->cwd->ino); // the mip is the just the current ino
                print_child_inodes(mip, dev);       // print out the child inos
                iput(mip);                          // put the ino back
        }

}

void print_child_inodes(MINODE *mip, int dev)
{
        int i = 0;
        char data[BLOCK_SIZE];
        char *cp = '\0';
        DIR *dp;
        MINODE *tmp_mip;

        // Direct Blocks
        for(i = 0; i < 12; i++)
        {
                if(mip->INODE.i_block[i])
                {
                        get_block(dev, mip->INODE.i_block[i], data);
                        cp = data;
                        dp = (DIR *)cp;

                        while(cp < data + BLOCK_SIZE)
                        {
                                dp->name[dp->name_len] = '\0';

                                tmp_mip = iget(dev, dp->inode);

                                print_dir_entry(dev, tmp_mip, dp->name);

                                iput(tmp_mip);

                                cp += dp->rec_len;
                                dp = (DIR *)cp;
                        }
                }
                else
                {
                        return;
                }
        }

        // Indirect Blocks
        if(mip->INODE.i_block[12])
        {

        }

        // Indirect Blocks
        if(mip->INODE.i_block[13])
        {

        }
}


void print_dir_entry(int dev, MINODE *mip, char *entry_name)
{
        time_t t;
        int i = 0;
        char mybuf[BLKSIZE];

        char *t1 = "xwrxwrxwr-------";
        char *t2 = "----------------";

        if(S_ISREG(mip->INODE.i_mode))
        {
                printf("-");
        }

        if(S_ISDIR(mip->INODE.i_mode))
        {
                printf("d");
        }

        if(S_ISLNK(mip->INODE.i_mode))
        {
                printf("l");
        }

        for(i = 0; i <= 8; i++)
        {
                if(mip->INODE.i_mode & (1 << i))
                {
                        printf("%c", t1[i]);
                }
                else
                {
                        printf("%c", t2[i]);
                }
        }

        printf("%5d ", mip->INODE.i_gid);
        printf("%5d ", mip->INODE.i_uid);
        printf("%10d ", mip->INODE.i_size);

        t = mip->INODE.i_mtime;

        printf("%10s", entry_name);
        if(S_ISLNK(mip->INODE.i_mode))
        {
                printf("-->");
                get_block(mip->dev, mip->INODE.i_block[0], mybuf);
                printf("%s",mybuf);
                put_block(mip->dev, mip->INODE.i_block[0], mybuf);
        }
        printf("\t ");
        printf("%s", ctime(&t));



}
