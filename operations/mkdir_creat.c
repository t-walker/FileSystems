#include "../filesystem.h"
#include <sys/stat.h>
#include <time.h>
/******** make_dir function ***************
 * 1. Check for a invalid pathname
 * 2. Check for relative or absolute
 * 3. Get the parent and basenames
 * 4. Get the inode of the parent (check for dir type and make sure the child doesn't exist)
 * 5. Call mymkdir to add new dir to parent
 * 6. Link count++, dirty=1, Update a_time
 ******************************************/
void mk_dir(char *pathname)
{
  char temp_parent[256];
  char temp_child[256];
  char *parent, *child;

  int i;
  int pino, dev;
  MINODE *mip, *pip;

  //Checks for an invalid pathname
  if(pathname[0] == '\0')
  {
    printf("Missing argument, pathname to make directory\n");
    return;
  }

  //Checks for an absolute or relative path
  if(pathname[0] == '/')
  {
    mip = root;
    dev = root->dev;
    printf("root device = %d\n", dev);
  }else{
    mip = running->cwd;
    dev = running->cwd->dev;
    printf("current device = %d\n", dev);
  }

  //Split up the parent and basenames
  strcpy(temp_parent, pathname);
  strcpy(temp_child, pathname);

  parent = dirname(temp_parent);
  child = basename(temp_child);

  printf("parent pathname = %s\n", parent);
  printf("child pathname = %s\n", child);

  //Get the inode number for the parent (1. In CWD | 2. Root directory (i.e. /usr) | 3. A longer pathname /usr/tmp/newfile )
  if(0 == strcmp(parent, "."))
  {
    pino = running->cwd->ino;
  }else if(0 == strcmp(parent, "/") && 1 == strlen(parent))
  {
    pino = root->ino;
  }else{
    pino = getino(&dev, parent);
  }

  printf("device = %d, parent inode # = %d\n", dev, pino);

  //Invalid pathname
  if(pino == 0)
  {
    printf("The pathname %s was invalid\n", pathname);
    return; // This occurs when parent directory path does not exist.
  }

  //Load the
  pip = iget(dev, pino);

  //Verify parent inode is a directory
  if(!(S_ISDIR(pip->INODE.i_mode)))
  {
    printf("Pathname = %s is invalid, not a directory\n", pathname);
    iput(pip);
    return;
  }

  //Verify child does not exist in parent directory
  if(0 != search(pip, child))
  {
    printf("Child name = %s exists in parent directory\n", child);
    iput(pip);
    return;
  }

  //Creates the child entries and the
  mymkdir(pip, child, pino);

  //The new dir is a new link for the parent so we need to increment it
  //Also update the access time of the parent and set the minode to dirty
  pip->INODE.i_links_count++;
  pip->INODE.i_atime = time(0L);
  pip->dirty = 1;

  iput(pip);
}

/******** mymkdir functions ***************
 * 1. Get new block and inode number
 * 2. Intialize the new inode for the directory
 * 3. Add . and ..
 * 4. Call entername to add dir to parent
 ******************************************/
int mymkdir(MINODE *pip, char *name, int pino)
{
  int ino, bno, i;
  MINODE *mip;
  INODE *ip;
  char mybuf[BLOCK_SIZE];

  //Get a new inode and block number for the new dir
  ino = ialloc(pip->dev);
  bno = balloc(pip->dev);
  printf("ino = %d\n", ino);
  printf("bno = %d\n", bno);

  //Get the new inode in memory
  mip = iget(pip->dev, ino);
  ip = &mip->INODE;

  //Set directory attributes
  ip->i_mode = 0x41ED;
  ip->i_uid = running->uid;
  //ip->i_gid = running->gid; // add later
  ip->i_size = BLOCK_SIZE;
  ip->i_links_count = 2;
  ip->i_atime = time(0L);
  ip->i_ctime = time(0L);
  ip->i_mtime = time(0L);
  ip->i_blocks = 2;
  ip->i_block[0] = bno;
  for(i = 1; i < 15; i++)
  {
    ip->i_block[i] = 0;
  }

  //Mark as dirty put the inode back in memory
  mip->dirty = 1;
  iput(mip);

  /************************************
   * 1. Read the new block into the buf
   * 2. Cast to a DIR *
   * 3. Set the inodenum, name, name_len and rec_len
   * 4. Move to the start of the next entry
   * 5. Set the inodenum, name, name_len
   * 6. And then set the rec_len = block_size - 12 = 1012
   *
   ************************************/
  //create directory entries for . and ..
  get_block(pip->dev, bno, mybuf);

  char *cp = mybuf;
  DIR *dp = (DIR *)cp;

  dp->inode = ino;
  dp->rec_len = 12;
  dp->name_len = 1;
  dp->name[0] = '.';

  cp += dp->rec_len;
  dp = (DIR *)cp;

  dp->inode = pino;
  dp->rec_len = 1012;
  dp->name_len = 2;
  dp->name[0] = '.';
  dp->name[1] = '.';

  //Put the block back after changes
  put_block(pip->dev, bno, mybuf);

  //Will add the actual directory to the parent
  enter_name(pip, ino, name);
}

/******** enter_name *******************
 * 1. Look through the direct blocks
 * 2. If we find one then add new dir
 * 3. Otherwise allocate a new block and add the dir
 ******************************************/
int enter_name(MINODE *pip, int ino, char *name)
{
  int i,j, bno;
  int need_length, ideal_length, remain;
  char mybuf[BLOCK_SIZE], c;

  DIR *dp;
  char *cp;

  //Look through all of the direct blocks until we find room
  for(i = 0; i < 12; i++)
  {
    //If we hit an empty block, break
    if(0 == pip->INODE.i_block[i])
    {
      break;
    }

    //Read the current block into the buffer
    get_block(pip->dev,pip->INODE.i_block[i], mybuf);

    dp = (DIR *)mybuf;
    cp = mybuf;

    //Go to the last entry
    while(cp + dp->rec_len < mybuf + BLOCK_SIZE)
    {
      cp += dp->rec_len;
      dp = (DIR *)cp;
    }

    //Calculate the need, ideal, and remain lengths
    need_length = 4 * ( (8 + strlen(name) + 3) / 4); //need length of new entry
    ideal_length = 4 * ( (8 + dp->name_len + 3) / 4); //ideal length of last item in directory entries
    remain = dp->rec_len - ideal_length;

    //If there is enough room in the directory for the new entry
    if(remain >= need_length)
    {
      //Set the rec_len to the ideal length so we can go to the end of the entry
      dp->rec_len = ideal_length;

      cp += dp->rec_len;
      dp = (DIR *)cp;

      //Cut the extra length off the of then entries rec_Len
      dp->rec_len = remain;

      //Set the inode, name and name_len
      dp->inode = ino;
      dp->name_len = strlen(name);

      for(j = 0; j < strlen(name); j++) // no null byte
      {
        dp->name[j] = name[j];
      }

      //Write the block back
      put_block(pip->dev, pip->INODE.i_block[i], mybuf);
      return 1;
    }
  }

  //If we need to allocate another block (NEW BLOCK)
  bno = balloc(pip->dev);
  pip->INODE.i_size += BLOCK_SIZE;
  get_block(pip->dev, bno, mybuf);

  dp = (DIR *)mybuf;

  // No space in existing data blocks
  dp->inode = ino;
  dp->rec_len = BLOCK_SIZE;
  dp->name_len = strlen(name);
  for(j = 0; j < strlen(name); j++) // no null byte
  {
    dp->name[j] = name[j];
  }

  put_block(pip->dev, bno, mybuf);
}
/******** end of make directory functions ********/








/*kmkdir(MINODE *pmip,const char *basename, int pinum)
{
    printf("kmkdir() ------\n");
    printf("kmkdir() --- basename = %s\n", basename);
    int i = 0, inum = ialloc(pmip->dev);
       int  blk = balloc(pmip->dev);
       MINODE *mip = iget(pmip->dev, inum); //load INODE into and minode
       char mybuf[BLKSIZE];
       DIR* dp;
       char* cp;
       char* bname = basename;
printf("kmkdir() --- basename1 = %s\n", basename);

       //initialize mip->INODE as a DIR INODE
       mip->INODE.i_mode = 0x41ED; //mode is dir with permissions
       mip->INODE.i_uid = running->uid;
       mip->INODE.i_gid = running->gid;
       mip->INODE.i_size = BLKSIZE;
printf("kmkdir() --- basename2 = %s\n", basename);
       mip->INODE.i_links_count = 2;
       //should set up time
       mip->INODE.i_atime = mip->INODE.i_ctime = mip->INODE.i_mtime = time(0L);
printf("kmkdir() --- basename3 = %s\n", basename);
       mip->INODE.i_blocks = 2;
       mip->INODE.i_block[0] = blk;

       for (i=1; i < 15; i++){
	      mip->INODE.i_block[i] = 0;
      }
printf("kmkdir() --- basename4 = %s\n", basename);
       mip->dirty = 1;
       iput(mip); //write INODE back to disk
printf("kmkdir() --- basename5 = %s\n", basename);

       //make data block
       get_block(pmip->dev, blk, mybuf);
       dp = (DIR*)mybuf;
printf("kmkdir() --- basename6 = %s\n", basename);
       printf("kmkdir--- inum: %d\n",inum);
       dp->inode = inum;
       strcpy(dp->name, ".");
       dp->name_len = 1;
       dp->rec_len = 12;
printf("kmkdir() --- basename7 = %s\n", basename);
       cp = mybuf;
       cp += dp->rec_len;
       dp = (DIR*)cp;
printf("kmkdir() --- basename8 = %s\n", basename);
       printf("kmkdir--- pmip->in0: %d\n",pmip->ino);
       dp->inode = pmip->ino;
       dp->name_len = 2;
       strcpy(dp->name, "..");
       dp->rec_len= BLKSIZE - 12;

       put_block(dev, blk, mybuf);

       printf("kmkdir() --- bname = %s\n", bname);

       insert_dir_entry(pmip, inum, basename);
       printf("kmkdir() ------End\n");
}
/* Algorithm of mkdir ***
int mkdir(char *pathname)
{
    1. if (pathname is absolute //pathname[0] == '/')
            dev = root->dev;
       else
            dev = PROC's cwd->dev //running ->cwd->dev
    2. divide pathname into dirname and basename
            //numstrs = parse(path);
            //dirname = strs[0-numstrs - 2] with '/'
	    //basename = strs[numstrs -1]
    3. //dirname numst exist and is a DIR:
            pino = getino(dirname, dev)//check syntax
	    pmip = iget(dev, pino);
	    check pmip->INODE is a DIR
    4. //basename numst not exist in parent DIR
            search(pmip, basename) must return 0;
    5. call kmkdir(pmip, basename) to create a DIR;
            kmkdir() has 4 steps:
	    5.1 allocate an INODE and a disk block:
	           ino = ialloc(dev);
		   blk = balloc(dev);
		   mip = iget(dev, ino); //load INODE into and minode
	    5.2 initialize mip->INODE as a DIR INODE
	           mip->INODE.i_block[0] = blk;
		   other i_block[] are 0;
		   mark minode modified (dirty);
		   iput(mip); //write INODE back to disk
	    5.3 make data block 0 of INODE to contain . and .. entries;
	           write to disk block blk (iput?)
	    5.4 enter_child(pmip, ino, basename);which enters (ino, basename) as a DIR entry into the parent INODE -> call insert_dir_entry ?
     6. increment parent INODE's links_count by 1 and make pmip dirty; iput(pmip);
     }*/

/*void insert_dir_entry(MINODE *pmip,int inum, const char *basename)
{
   printf("insert_dir_entry() ---\n");

   printf("insert_dir_entry() --- basename = %s\n", basename);
  int need_len = 4*((8 + strlen(basename) + 3)/4);
  int ideal_len = 0, remain=0;
  int i = 0, blk = 0;
  char mybuf[1024], *cp;
  DIR *dp;
  //slightly different than psudo code
  for (i=0; i < 12;i++){
    if(pmip->INODE.i_block[i] == 0)
      break;

    blk = pmip->INODE.i_block[i];

    get_block(pmip->dev, blk, mybuf);
    dp = (DIR*)mybuf;
    cp = mybuf;
    //get to the last one
    printf("insert_dir_entry() --- while\n");
    while(cp + dp->rec_len < mybuf + BLKSIZE) {
      cp += dp->rec_len;
      dp = (DIR*) cp;
    }
    //now dp is at the end;
    ideal_len = 4*((8 + dp->name_len + 3)/4);
    remain = dp->rec_len - ideal_len;
    if(remain >= need_len){
      dp->rec_len = ideal_len;
      cp += dp->rec_len;
      dp = (DIR*)cp;

      dp->inode = inum;
      dp->rec_len = remain;
      dp->name_len = strlen(basename);
      //dp->file_type?
      strcpy(dp->name, basename);
      printf("insert_dir_entry() --- dp->name = %s\n", dp->name);
      put_block(pmip->dev, blk, mybuf);
       printf("insert_dir_entry() --- inside if(remain >= need_len)\n");
      return;
    }
    // moved after the for b/c it will handle two if/else cases else{ //no space, need to allocate}

  }
  //couldn't find a block to use

  blk = balloc(pmip->dev);
  pmip->INODE.i_block[i] = blk;

  get_block(pmip->dev, blk, mybuf);
  dp = (DIR*) mybuf;
  cp = mybuf;

  dp->inode = inum;
  dp->rec_len = BLKSIZE;
  dp->name_len = strlen(basename);
  //dp->file_type?
  strcpy(dp->name, basename);

  put_block(pmip->dev, blk, mybuf);
  printf("insert_dir_entry() --- END\n");

  return;
}*/

/* insert_dir_entry*
(1). need_len = 4*((8 + name_len + 3)/4); //new entry need length
(2). for each existing data block do {
        (3). if(block has only one entry with inode number == 0)
	        enter new entry as first entry in block
        (4). else {
	        go to last entry in block;
		ideal_len = 4*((8+last_entry's name_len * 3)/4);
		remain = last entry's rec_len - ideal_len
		if(remain >= need _len){
		        trim last entry's rec_len to ideal_len;
			enter new entry as last entry with rec_len = remain;
		}
		else{
		        allocate a new data block;
			enter new entry as first entry in the data block;
			increase DIR's size by BLKSSIZE;
		}
	}
	write block to disk; (iput())
	(5). mark DIR's minode modified for write back (dirty);*/



/* Algorithm of creat() ***
creat(char* fileName)
{
    this is similar to mkdir except
        1. the INODE.i_mode field is set to REG file type, permission bits set to 0644 for rw-r--r--, and
	2. no data block is allocated for it, so the file size is 0
	3. Do not increment parent INODE's link count
}*/
char* dname(const char *str)
{
  printf("dname() ------ on: %s\n", str);
  char temp[100];
  char ret[256];
  ret[0] = '\0';
  int i=0;
  int j=0;

  while(str[i])
  {
    temp[j++] = str[i];
    if(str[i] == '/'){
      temp[j] = '\0';
      strcat(ret, temp);
      j=0;
      temp[0] = '\0';
    }
    i++;
  }
  if(strcmp(ret, "")==0)
    strcpy(ret, ".");
  printf("dname() ------ returning: %s\n", ret);
  return ret;
}

char* bname(const char* str)
{
   printf("bname() ------\n");
   char ret[100], temp[256];
   temp[0] = '\0';
   int i=0,j=0;
  while(str[i])
  {
    ret[j++] = str[i];
    if(str[i] == '/'){
      ret[j] = '\0';
      strcat(temp, ret);
      j=0;
      ret[0] = '\0';
    }
    i++;
  }
  ret[j] = '\0';
  printf("bname() ------ returning: %s\n", ret);
  return ret;
}

// List the files in a directory
/*void mk_dir (char *pathname)
{
        printf("mkdir() -----\n");
	int i=0, numstrs = 0, dev = 0;
	char *strs[100], dirname[256], basename[100];
	int pinum = 0, inum = 0;
	MINODE *pmip = (MINODE*) malloc(sizeof(MINODE));
	MINODE *mip;

	//set up dev
	if(pathname[0] == '/'){
	  mip = root;
	  dev = root->dev;
	}
	else{
	  mip = running->cwd;
	  dev = running->cwd->dev;
	}

	// devide pathname into base and dirname
  strcpy(dirname, dname(pathname));
	strcpy(basename, bname(pathname));
	printf("mkdir() ---dirname: %s\n", dirname);
	printf("mkdir() --basename: %s\n", basename);

	//dirname must exist and is a DIR;
	if(strcmp(dirname,".")==0) {
	  pinum = running->cwd->ino;
	}
	else if(strcmp(dirname,"/") == 0){
	  pinum = root->ino;
	}
	else{
	  pinum = getino(dirname, dev);
	}

	if(pinum ==0){
	  printf("&s does not exist\n", dirname);
	  return;
	}
	pmip = iget(dev, pinum);
	//is it a dir?
	if(!(S_ISDIR(pmip->INODE.i_mode))){
	  printf("&s is not a directory\n", dirname);
	  iput(pmip);
	  return;
	}

  printf("mkdir() --- basename = %s\n", basename);
	//basename can't exist in parent dir
	if(search(pmip, basename) != 0){
	  printf("%s already exists in %s\n", basename, dirname);
	  iput(pmip);
	  return;
	}

  printf("mkdir() --- basename = %s\n", basename);
	//call kmkdir()
	kmkdir(pmip, basename, pinum);

	pmip->INODE.i_links_count ++;
	pmip->INODE.i_atime = time(0L);
	pmip->dirty = 1;
	iput(pmip);
        printf("mkdir() -----End\n");
}
*/
