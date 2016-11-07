#include "../filesystem.h"

// List the files in a directory
void ls (char *pathname)
{
  printf("ls() -----\n");
  int ino, dev = running->cwd->dev;
  MINODE *mip = running->cwd;
  printf("ls() -- mip = running->cwd\n");

  if (pathname)
  {
    printf("ls() -- pathname isn't null\n");
    if (pathname[0] == '/')
    {
      dev = root->dev;
      printf("ls() -- pathname starts with '/', root->dev\n");
    }

    printf("ls() -- calling getino\n");
    ino = getino(&dev, pathname);

    printf("ls() -- finished getino()\n");
    MINODE *mip = iget(dev, ino);
    printf("ls() -- finished iget()\n");
    // mip points at the minode;
    // Each datablock of mip->INODE contains DIR entries
    // print the neame of strings of the DIR entries

    printf("ls() -- calling printDir()\n");
    printDir(mip->INODE, dev);
  }
  else
  {
    printf("ls() -- calling printDir()\n");
    printDir(running->cwd->INODE, dev);
  }
}
