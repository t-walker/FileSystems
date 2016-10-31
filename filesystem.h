#ifndef FILESYSTEM_H
#define FILESYSTEM_H

/* INCLUDES */
#include <fcntl.h>
#include "type.h"

/* EXTERNS */
extern MINODE minode[NMINODE];
extern MINODE *root;
extern PROC proc[NPROC], *running;

extern int dev;
extern int nblocks;
extern int ninodes;
extern int bmap;
extern int imap;
extern int inode_start;

extern char path[200];
extern char  buf[1024];
extern char *deviceName;

/* FILESYSTEM CORE */
void mount_root();
int getino(char *path, MINODE *mp);
MINODE *iget(int dev, int ino);
void iput(MINODE *mip);
int search(MINODE *mip, char *pathname);

/* FILESYTEM OPERATIONS */
void ls (char *pathname); // ls [pathname]
void cd (char *pathname); // cd [pathname]

#endif
