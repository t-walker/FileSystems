#include "filesystem.h"

MINODE *iget(int dev, int ino);
void iput(MINODE *mip);
int search(MINODE *mip, char *pathname);
int getino(char *path, int *dev);

int get_block(int fd, int blk, char buf[]);
int put_block(int fd, int blk, char buf[]);
