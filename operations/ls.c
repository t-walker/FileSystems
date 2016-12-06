#include "../filesystem.h"

// List the files in a directory
void ls (char *pathname)
{
        //printf("ls() -----\n");
        int i;
        int ino = running->cwd->ino;
        int dev = running->cwd->dev;

        MINODE *mip;
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

                if (S_ISDIR(mip->INODE.i_mode))
                {
                        //printf("IS directory\n");
                        print_child_inodes(mip, dev);
                }
                else
                {
                        printf("IS NOT A DIRECTORY");
                }

                iput(mip);

        }
        else
        {
                mip = iget(dev, running->cwd->ino);
                print_child_inodes(mip, dev);
                iput(mip);
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

  for(i = 0; i < 8; i++)
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

  printf("%10s\t ", entry_name);
  printf("%s", ctime(&t));



}
