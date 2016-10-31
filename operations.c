#include "filesystem.h"

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

void cd(char *pathname)
{
  int iNodeNum;
  MINODE *mip = running->cwd->dev;
  parse(pathname, '/', path);
  dev = running->cwd->dev;

  if (path[0] == '\n') // Check to make sure the input isn't a newline.
  {
    if (path[0] == '/') // If the character starts at root, it's not relative.
    {
      dev = root->dev; // Start it at the root.
    }

    iNodeNum = getino(mip, pathname);
    running->cwd = mip;
  }
  else
  {
    running->cwd = root;
  }
}
