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

int main(int argc, char *argv[], char *env[])
{
  return 0;
}
