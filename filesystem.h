#ifndef FILESYSTEM_H
#define FILESYSTEM_H

/* INCLUDES */
#include <fcntl.h>
#include <sys/stat.h>
#include <math.h>
#include <time.h>

#include "type.h"
#include "core.h"

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
extern char buf[1024];
extern char *deviceName;

/* FILESYSTEM CORE */
int getino(char *path, int *dev);
MINODE *iget(int dev, int ino);
void iput(MINODE *mip);
int search(MINODE *mip, char *pathname);

/* ALLOC AND DEALLOC */
u32 ialloc(int dev);
u32 balloc(int dev);

/* FILESYTEM OPERATIONS */
void getCommands(char* currentPath);
/* LEVEL 1 */
void mount_root();
void mk_dir(char *pathname);
// void rmdir(char *pathname);
void ls (char *pathname); // ls [pathname]
void cd (char *pathname); // cd [pathname]
void pwd();
// void creat();
// void link();
// void unlink();
// void symlink();
// void stat();
void ch_mod(char *args[], int argv);
void touch(char *argv[], int argc);
void quit();

/* LEVEL 2 */
// void open();
// void close();
// void read();
// void write();
// void lseek();
// void cat();
// void cp();
// void mv();

/* LEVEL 3 */
// void mount();
// void unmount();
// File Permissions
#endif
