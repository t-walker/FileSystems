#include "../filesystem.h"

// Change the directory
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
