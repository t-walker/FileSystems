#include "../filesystem.h"

#define u16 unsigned int

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

void ch_mod(char *args[], int argv)
{
  dev = running->cwd->dev;
  MINODE *mip;

  int i = 0, ino;

  printf("chmod() --------------\n");

  char *permissions = args[1];
  char *path = args[2];

  u16 permission_i = convertOctalToDecimal(atoi(permissions));
  printf("Permission octal: %3o\n", permission_i);
  u16 permission_o = 0;

  if (args[1][0] == '/')
  {
    dev = root->dev;
  }

  ino = getino(path, dev);

  printf("chmod() -- path: %s\n", path);
  printf("chmod() -- permissions: %s\n", permissions);

  if(ino == 0)
  {
    printf("chmod() -- could not find path");
    return;
  }

  if (strlen(permissions) <= 4)
  {
    mip = iget(dev, ino);

    permission_o = mip->INODE.i_mode &= 61440;
    permission_o |= permission_i;

    mip->INODE.i_mode = permission_o;

    printf("chmod() -- permission: %3o\n", permission_o);

    iput(mip);
  }
  else
  {

  }


}
