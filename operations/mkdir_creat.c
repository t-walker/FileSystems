#include "../filesystem.h"

kmkdir(MINODE *pmip, char *basename, int dev)
{
       int i = 0, inum = ialloc(dev);
       int  blk = balloc(dev);
       MINODE *mip = iget(dev, inum); //load INODE into and minode
       char buf[1024];
       DIR* dp;
       char* cp;

       //initialize mip->INODE as a DIR INODE
       mip->INODE.i_mode = 0x41ED; //mode is dir with permissions
       mip->INODE.i_uid = running->uid;
       mip->INODE.i_gid = running->gid;
       mip->INODE.i_size = 1024;

       mip->INODE.i_links_count = 2;
       //should set up time
       mip->INODE.i_atime = mip->INODE.i_ctime = mip->INODE.i_mtime = time(0L);

       mip->INODE.i_blocks = 2;
       mip->INODE.i_block[0] = blk;
       for (i=1; i < 15; i++)
	 mip->INODE.i_block[i] = 0;

       mip->dirty = 1;
       iput(mip); //write INODE back to disk

       //make data block
       memset(buf, 0, 1024);
       dp = (DIR*)buf;

       dp->inode = inum;
       strcpy(dp->name, ".");
       dp->name_len = 1;
       dp->rec_len = 12;

       cp = buf;
       cp += dp->rec_len;
       dp = (DIR*)cp;

       dp->inode = pmip->ino;
       dp->name_len = 2;
       strcpy(dp->name, "..");
       dp->rec_len= BLKSIZE - 12;

       put_block(dev, blk, buf);
       insert_dir_entry(pmip, inum, basename);
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

void insert_dir_entry(MINODE *pmip,int inum, char *basename)
{
  int need_len = 4*((8 + strlen(basename) + 3)/4);
  int ideal_len = 0, remain=0;
  int i = 0, blk = 0;
  char buf[1024], *cp;
  DIR *dp;
  //slightly different than psudo code
  for (i=0; i < 12;i++){
    if(pmip->INODE.i_block[i] == 0)
      break;

    blk = pmip->INODE.i_block[i];

    get_block(pmip->dev, blk, buf);
    dp = (DIR*)buf;
    cp = buf;
    //get to the last one
    while(cp + dp->rec_len < buf + BLKSIZE) {
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
      put_block(pmip->dev, blk, buf);
      return;
    }
    // moved after the for b/c it will handle two if/else cases else{ //no space, need to allocate}

  }
  //couldn't find a block to use

  blk = balloc(pmip->dev);
  pmip->INODE.i_block[i] = blk;

  get_block(pmip->dev, blk, buf);
  dp = (DIR*) buf;
  cp = buf;

  dp->inode = inum;
  dp->rec_len = BLKSIZE;
  dp->name_len = strlen(basename);
  //dp->file_type?
  strcpy(dp->name, basename);

  put_block(pmip->dev, blk, buf);
  return;
}

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

// List the files in a directory
void mk_dir (char *pathname)
{
        printf("mkdir() -----\n");
	int i=0, numstrs = 0, dev = 0;
	char *strs[100], dirname[256]= "", basename[100]="";
	int pinum = 0, inum = 0;
	MINODE *pmip = (MINODE*) malloc(sizeof(MINODE));

	//set up dev
	if(pathname[0] == '/')
	  dev = root->dev;
	else
	  dev = running->cwd->dev;

	// devide pathname into base and dirname
	numstrs = parse(pathname, "/",strs);
	for (i = 0; i < (numstrs-2); i++){
	  strcat(dirname, "/");
	  strcat(dirname, strs[i]);
	}
	strcpy(basename, strs[numstrs-1]);

	//dirname must exist and is a DIR;
	pinum = getino(dirname, dev);
	if(pinum ==0){
	  printf("&s does not exist\n", dirname);
	  return;
	}
	pmip = iget(dev, pinum);
	//is it a dir?
	if((pmip->INODE.i_mode & 0xF000) != 0x4000){ //got these numbers from lxr.free-electronics.com
	  printf("&s is not a directory\n", dirname);
	  return;
	}

	//basename can't exist in parent dir
	if(search(pmip, basename) != 0){
	  printf("%s already exists in %s\n", basename, dirname);
	  return;
	}

	//call kmkdir()
	kmkdir(pmip, basename, dev);

	pmip->INODE.i_links_count ++;
	pmip->dirty = 1;
	iput(pmip);
        printf("mkdir() -----End\n");
}
