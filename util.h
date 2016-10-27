#include <string.h>
#include "types.h"
int debug = 0;
/*parse str on delim and return them in strs*/
int parse(const char* str,const char* delim, char *strs[100])
{
  int i = 0;
  char *token, *temp = strdup(str);
  if(debug) {printf("In parse str=%s\n",str);}
  token = strtok (temp, delim);
  while (token != NULL)
  {
    strs[i++] = token;
    token = strtok (NULL, delim);
  }
  strs[i] = NULL;
}

int get_block(int fd, int blk, char buf[])
{
	lseek(fd, (long)blk*BLKSIZE, 0);
	read(fd, buf, BLKSIZE);
}

SUPER* getSuper(int fd)
{
	char buf[1024];
	//read SUPER block
	get_block(fd, 1, buf);
	sp = (SUPER *)buf;
	
	//check for EXT2 magic number:

	printf("s_magic = %x\n", sp->s_magic);
	if (sp->s_magic != 0xEF53) {
		printf("NOT an EXT2 FS\n");
		exit(1);
	}
	return sp;
}

int getino(char* name[256], int n)
{
	int blk, iblock, rootblock, ino, offset, nextBlock;
	int i, isFile = 1;

	//read SUPER block
	get_block(fd, 1, buf);
	sp = (SUPER *)buf;
	
//	printf("s_magic = %x\n", sp->s_magic);
	if (sp->s_magic != 0xEF53) {
		printf("NOT an EXT2 FS\n");
		exit(1);
	}

//	nblocks = sp->s_blocks_count;
//	printf("nblocks = %d\n", nblocks);
	
	//read Group Descriptor - 0
	get_block(fd, 2, buf);
	gp = (GD*)buf;
	
	//get the InodesBeginBlock
	iblock = gp->bg_inode_table; //get inode start block#
	
	get_block(fd, iblock, buf);
	ip = (INODE *)buf + 1; //ip points at 2nd INODE

		//read InodeBeginBlock to get inode of /, witch is INODE #2
	rootblock = ip->i_block[0];
	get_block(fd, rootblock, dbuf);
	dp = (DIR*)dbuf;
	cp = dbuf;
	printf("%d\n", ip);

	//Start from root INODE search for name[0] in its data blocks
		//assume there are only 12 direct blocks
	for (i = 0; i < n; i++) {
	  ino = search(ip, name[i]);
	  if (!ino) { //can't find name[i], 
	    printf("file not found\n");
	    exit(1);
	  }
		//	use ino to read in INODE and let ip point to INODE
	  blk = (ino - 1)/8 + iblock;
	  offset = (ino - 1)% 8;
	  get_block(fd, blk, buf);
	  printf("blk: %d, off: %d\n", blk, offset);
	  ip = (INODE*) buf + offset;
		
	  if(S_ISDIR(ip->i_mode )) {
	    printf("nameToken:%s inode:%d\n", name[i], ip);
	    if (i < n - 1) {	
	       	//next dir
	      printf("Entering Directory: %s\n", name[i]);
	      nextBlock = ip->i_block[0];
	      get_block(fd, nextBlock, dbuf);
	      dp = (DIR*)dbuf;
	      cp = dbuf;
	    }
	    else {
	      isFile = 0;
	    }
	  }
		
	}

}
