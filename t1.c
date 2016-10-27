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

int search(MINODE *mip, char *pathname)
{
  int i;
  char *cp, *tmp;

  char buf[1024];

  for(i = 0; i < 12; i++)
  {
    if(mip->INODE.i_block[i])
    {
      get_block(dev, mip->INODE.i_block[i], buf);
    

      dp = (DIR *)buf; 
      cp = buf;

      while(cp < buf + BLKSIZE)
      {
        if (strcmp(dp->name, pathname) == 0)
        {
          return dp->inode;
        }

        cp += dp->rec_len;
        dp = (DIR *)cp;
      }
  }

  return 0;
}

void init() 
{
  // Initialize data structures of LEVEL-1
  int i, j;
  MINODE *mip;
  PROC *p;

  printf("init()\n");
  
  // 1) 2 PROCs, P0 with uid=0, P1 with uid=1, all PROC.cwd = 0
  for (i = 0; i < NPROC; i++) {
    p = &minode[i];
    p->uid = i;
    p->pid = i+1;
    p->cwd = 0;
    p->status = FREE;
  }
  
  running = malloc(sizeof(PROC));
  running = &proc[0];
  
  // 2) MINODE minode[100]; all with refCount = 0
  for (i = 0; i < NMINODE; i++) {
    mip = &minode[i];
    mip->dev = mip->ino = 0;
    mip->refCount = 0;
  }
  
  
  // 3) MINODE *root = 0;
  root = 0;
}

// Write C code for: 
// int ino = getino(int *dev, char *pathname);
// MINODE *mip = iget(dev, ino);

mount_root() // Mount root file system, establish / and CWDs
{
  // Open device for RW (get a file descriptor dev for the opened device)
  // read SUPER block to verify it's an EXT2 FS
  dev = open("myDisk", O_RDWR);
  if (dev < 0) {
     printf("open failed\n");
     exit(1);
  }
	
  getSuper(dev);
	
  root = iget(dev, 2); 

  // Let CWD of both P0 and P1 point at the root minode (refCount = 3)
  proc[0].cwd = iget(dev, 2);
  proc[1].cwd = iget(dev, 2);
}

// ls [pathname] command:
void ls (char *pathname, char *command)
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
    
    print_dir_entries(mip->INODE);
  }
  else
  {
    print_dir_entries(running->cwd->dev);
  }
}

void cd(char *pathname)
{
  int iNodeNumber; 
  
  MINODE *mip = running->cwd->dev;
  
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
