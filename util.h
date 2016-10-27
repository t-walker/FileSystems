#include <string.h>

int debug = 0;

// Parse str on delim and return them in strs

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
  return i;
}

int get_block(int fd, int blk, char buf[])
{
  lseek(fd, (long)blk*BLKSIZE, 0);
  read(fd, buf, BLKSIZE);
}

int put_block(int fd, int blk, char buf[])
{
  lseek(fd, (long)blk*BLKSIZE, 0);
  write(fd, buf, BLKSIZE);
}

//TODO: make this a more detailed printout
void printDir(INODE ptr, int dev)
{
	char buf[1024];
	char *cp;
	get_block(dev, ptr.i_block[0],buf);
	dp = (DIR *)buf;
	cp = buf;
	
	while(cp < buf + 1024) {
		printf ("%s\n", dp->name);
		cp += dp->rec_len;
		dp = (DIR *) cp; //shut up
	}
}
