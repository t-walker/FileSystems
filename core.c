#include "filesystem.h"

// GETINO
// gets the inode number given a path and an minode.
int getino(char *path, MINODE *mp)
{
  printf("getino() ------\n.");

	int i, n, inum = 0, bnum, offset;
  printf("getino() -- initalized variables.\n");

	MINODE *temp = (MINODE*) malloc(sizeof(MINODE));
  printf("getino() -- created a temporary MINODE.\n");

	char *strs[100];


	if(path[0] == '/') {
		dev = root->dev;
		inum = root->ino;
    printf("getino() -- the path begins at root.\n");
	}
	else {
		dev = running->cwd->dev;
		inum = root->ino;
    printf("getino() -- the path is relative.\n");
	}

	n = parse(path, "/", strs);
  printf("getino() -- parsing the path.\n");

	for (i = 0; i < n; i++)
  {
		printf("path %s\n", strs[i]);
    inum = search(mp, strs[i]);

		if(inum == 0)
    {
			printf("File not found\n");
			return 0;
	  }

		offset = (inum - 1) % 8; //for char
		get_block(dev, ((inum - 1)/8) + inode_start, buf);
		mp = (MINODE*)buf + offset;
    printf("getino() -- setting mp to buf + offset.\n");

		iput(mp);
	}
  printf("getino() -- returning inum.\n");

	return inum;
}

// IGET
// Retrieves an inode given a dev and an inode.
MINODE *iget(int dev, int ino)
{
    printf("iget() ------\n");
    MINODE *mip = malloc(sizeof(MINODE));
    int i, blk_num, offset;
    printf("iget() -- allocated *mip.\n");

    //Loop through all of the minodes to see if the one we want is in memory
    for (i = 0; i < NMINODE; i++) {
      if (minode[i].ino == ino) // Is this the inode we're looking for?
      {
          minode[i].refCount++; // Add the reference of it.
          return &minode[i];
      }
    }

    printf("iget() -- found the MINODE.\n");

    //Use mailman's to get the block location
    blk_num = ((ino - 1)/8) + inode_start;
    offset = (ino - 1) % 8;
    printf("iget() -- mailman's algorithm:\n");
    printf("iget() -- blk_num %d.\n", blk_num);
    printf("iget() -- offset %d.\n", offset);

    //Get the block
    get_block(dev, blk_num, buf);
    printf("iget() -- got the block for the blk_num.\n");

    ip = (INODE *)buf + offset;
    printf("iget() -- got the INODE\n");


    //Loop through all of the minodes until we find one that is empty
    for (i = 0; i < NMINODE; i++){
      if (minode[i].refCount == 0) //Check to make sure it's empty
      {
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

          printf("iget() -- Empty MINODE found.\n");
          printf("iget() -- Returning MINODE.\n");

          //Return the index of the inode
          return &minode[i];
      }
    }
}

// IPUT
// Puts an minode itno the core.
void iput(MINODE *mip)
{
  printf("iput() ------\n");

  int blk_num, offset;
  mip->refCount -= 1;
  printf("iput() -- Decrementing the refCount of the MINODE.\n");

  if (mip->refCount == 0)
  {
    printf("iput() -- MINODE refCount is 0.\n");

    blk_num = ((mip->ino - 1)/8) + inode_start;
    offset = (mip->ino-1) % 8;
    printf("iput() -- blk_num: %d.\n", blk_num);
    printf("iput() -- offset: %d.\n", offset);

    get_block(mip->dev, blk_num, buf);
    printf("iput() -- got the block.\n");

    ip = (INODE *)buf + offset;
    memcpy(ip, &(mip->INODE), sizeof(INODE));
    printf("iput() -- copied the block to memory.\n");

    put_block(dev, blk_num, buf);
    printf("iput() -- put the block\n");
  }
}

// SEARCH
// Searches for a path within an inode.
int search(MINODE *mip, char *pathname)
{
  printf("search() ------\n");

  int i;
  char *cp, *tmp;

  char x;

  for(i = 0; i < 12; i++)
  {
    printf("search() -- loop: %d.\n", i);
    if(mip->INODE.i_block[i])
    {
      printf("----- mip->INODE.i_block[%d] isn't null", i);
      get_block(dev, mip->INODE.i_block[i], buf);
      dp = (DIR *)buf;
      cp = buf;
      getc(x);

      while(cp < buf + 1024)
      {
        printf("search whileloop\n");

        if (strcmp(dp->name, pathname) == 0)
        {
          return dp->inode;
        }

        cp += dp->rec_len;
        dp = (DIR *)cp;
      }
      exit(1);
    }
  }
  return 0;
}
