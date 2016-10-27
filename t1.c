#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ext2fs/ext2_fs.h>
#include <string.h>
#include <libgen.h>
#include <sys/stat.h>

#include "type.h"
#include "util.h"

MINODE minode[NMINODE];
MINODE *root;
PROC proc[NPROC], *running; 

int dev; 
int nblocks; 
int ninodes;
int bmap;
int imap;
int inode_start; 

char path[200], buf[1024], *deviceName = "disk";

MINODE *iget(int dev, int ino)
{
    MINODE *mip = malloc(sizeof(MINODE));
    int i,
        blk_num,
        offset;

    //Loop through all of the minodes to see if the one we want is in memory
    for (i = 0; i < NMINODE; i++) {
            if (minode[i].ino == ino) {     //Do the values match
                minode[i].refCount++;
                return &minode[i];
            }
    }

    //Use mailman's to get the block location
    blk_num = ((ino - 1)/8) + inode_start;
    offset = (ino - 1) % 8;
    //Get the block
    get_block(dev, blk_num, buf);
    ip = (INODE *)buf + offset;

    //Loop through all of the minodes until we find one that is empty
    for (i = 0; i < NMINODE; i++){
        if (minode[i].refCount == 0){           //Check to make sure it's empty
            /***
             * 1. Set the minode's inode to the temorary inode
             * 2. Set the dirty value to 0
             * 3. Set the device number (works like a fd)
             * 4. Set the inode number
             * 5. Increment the refcount
             ***/
            minode[i].INODE = *ip;
            minode[i].dirty = 0;
            minode[i].dev = dev;
            minode[i].ino = ino;
            minode[i].refCount = 1;

            //Return the index of the inode
            return &minode[i];
        }
    }
}

void iput(MINODE *mip)
{
  int blk_num, offset;
  mip->refCount -= 1; 

  if (mip->refCount == 0)
  {
    blk_num = ((mip->ino - 1)/8) + inode_start;
    offset = (mip->ino-1) % 8;

    get_block(mip->dev, blk_num, buf);

    ip = (INODE *)buf + offset;
    memcpy(ip, &(mip->INODE), sizeof(INODE));

    put_block(dev, blk_num, buf);
  }
}

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
int getino(char *path, MINODE *mp)
{
	int i, n, inum = 0, bnum, offset;
	MINODE *temp = (MINODE*) malloc(sizeof(MINODE));
	char *strs[100];
	if(path[0] == '/') {
		dev = root->dev;
		inum = root->ino;
	}
	else {
		dev = running->cwd->dev;
		inum = root->ino;
	}
	
	n = parse(path, "/", strs);
	
	for (i = 0; i < n; i++) {
		inum = search(mp, strs[i]);
		
		if(inum == 0) {
			printf("File not found\n");
			return 0;
		}
		
		offset = (inum - 1) % 8; //for char
		get_block(dev, ((inum - 1)/8) + inode_start, but);
		mp = (MINODE*)buf + offset;
		
		iput(mp);
	}
	return inum;
}

void mount_root() // Mount root file system, establish / and CWDs
{
  // Open device for RW (get a file descriptor dev for the opened device)
  // read SUPER block to verify it's an EXT2 FS
  printf ("mounting root\n");
  dev = open(deviceName, O_RDWR);
  if (dev < 0) {
     printf("open failed\n");
     exit(1);
  }
	
  
  //read SUPER block
  get_block(dev, 1, buf);
  sp = (SUPER *)buf;
  
  //check for EXT2 magic number:

  printf("s_magic = %x\n", sp->s_magic);
  if (sp->s_magic != 0xEF53) {
	printf("NOT an EXT2 FS\n");
	exit(1);
   }
   nblocks = sp->s_blocks_count;
   ninodes = sp->s_inodes_count;
   
   get_block(dev, 2, buf);
   gp = (GD* )buf;
   inode_start = gp->bg_inode_table;
   //set up root
  root = iget(dev, 2); 
  
  root->mptr = (struct mntable*)malloc(sizeof(struct mntable));
  root->mptr->ninodes = ninodes;
  root->mptr->nblocks = nblocks;
  root->mptr->dev = dev;
  root->mptr->busy = 1;
  root->mptr->mounted_inode = root;
  strcpy(root->mptr->name,"/");
  strcpy(root->mptr->mount_name, deviceName);
  // Let CWD of both P0 and P1 point at the root minode (refCount = 3)
  proc[0].cwd = iget(dev, 2);
  proc[1].cwd = iget(dev, 2);
  
  printf("mount root finished\n\n");
}

// ls [pathname] command:
void ls (char *pathname)
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
    
    printDir(mip->INODE, dev);
  }
  else
  {
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


int main(int argc, char *argv[], char *env[])
{
  char *pathname = "/";

  init();
  mount_root();
  
  // ask for a command string (ls pathame)
  ls(pathname);

  // ask for a command string (cd pathname)
  cd(pathname);  

  
  // ask for a command string (stat pathname)
  //stat(pathname, &mystat); // struct stat mystat; print mystat information
  return 0;
}

int quit()
{
  int i = 0;

  for (i = 0; i < NMINODE; i++)
  {
    minode[i].refCount = 0;
    iput(&minode[i]);
  }

  return 0;
}
