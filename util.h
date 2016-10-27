#include <string.h>

int debug = 0;

// Parse str on delim and return them in strs

int parse(const char* str,const char* delim, char *strs[100])
{
  printf("parse() -------\n");
  printf("-- parameters:\n");
  printf("---- str: %s\n", str);
  printf("---- delim: %c\n", delim);

  int i = 0;
  char *token, *temp = strdup(str);
  printf("parse() -- duplicating str into temp.\n");
  token = strtok (temp, delim);

  printf("parse() -- assigning to strs[].\n");

  while (token != NULL)
  {
    strs[i++] = token;
    printf("----- strs[%d] = %s.\n", i-1, token);
    token = strtok (NULL, delim);
  }

  strs[i] = NULL;
  printf("parse() -- assigning strs[i] to null.\n");
  printf("parse() -- returning i.\n");
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
  printf("printDir()\n");
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
  printf("finished printDir()\n");
}
