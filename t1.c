#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ext2fs/ext2_fs.h>
#include <string.h>
#include <libgen.h>
#include <sys/stat.h>

#include "type.h"

MINODE minode[NMINODE];
MINODE *root;
PROC proc[NPROC], *running; 

int dev; 
int nblocks; 
int ninodes;
int bmap;
int imap;
int inode_start; 

init() 
{
  // Initialize data structures of LEVEL-1

  // 1) 2 PROCs, P0 with uid=0, P1 with uid=1, all PROC.cwd = 0

  // 2) MINODE minode[100]; all with refCount = 0
  
  // 3) MINODE *root = 0; 
}

// Write C code for: 
// int ino = getino(int *dev, char *pathname);
// MINODE *mip = iget(dev, ino);

mount_root() // Mount root file system, establish / and CWDs
{
  // Open device for RW (get a file descriptor dev for the opened device)
  // read SUPER block to verify it's an EXT2 FS

  root = iget(dev, 2); 

  // Let CWD of both P0 and P1 point at the root minode (refCount = 3)
  P0.cwd = iget(dev, 2);
  P1.cwd = iget(dev, 2);
}

// ls [pathname] command:
ls (char *pathname, char *command)
{
  int ino, dev = running->cwd->dev;
  MINODE *mip = running->cwd;

  if (pathname) 
  {
    if (pathname[0] == '/')
    {
      dev = root->dev;
    }

    ino = getino(&dev, pathname);
    MINODE *mip = iget(dev, ino);

    // mip points at the minode;
    // Each datablock of mip->INODE contains DIR entries
    // print the neame of strings of the DIR entries 
    }
  }
}

cd(char *pathname)
{
  // if no pathame, cd to root.
  // else cd to the pathname
}


int main(int argc, char *argv[], char *env[])
{
  init();
  mount_root();

  // ask for a command string (ls pathame)
  ls(pathname);

  // ask for a command string (cd pathname)
  cd(pathname);  

  // ask for a command string (stat pathname)
  stat(pathname, &mystat); // struct stat mystat; print mystat information
  return 0;
}

int quit()
{
  // iput all DIRTY minodes before shutdown
}
