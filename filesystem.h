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
int ialloc(int dev);
int balloc(int dev);


/* FILESYTEM OPERATIONS */
void getCommands(char* currentPath);
void quit();

/* LEVEL 1 */
void mount_root();
void mk_dir(char *pathname);
void rm_dir(char *pathname);
void ls (char *pathname); // ls [pathname]
void cd (char *pathname); // cd [pathname]
void pwd();
void my_creat(char *pathname);
void my_link(char *old_file, char *new_file);
void my_unlink(char *filename);
// void symlink();
void my_stat(char *pathname);
void ch_mod(char *args[], int argv);
void touch(char *argv[], int argc);
void quit();

/* LEVEL 2 */
int my_open(char *pathname, int flag);
int my_close(int fd);
int my_read(int fd, char buffer[], int count);
int my_write(int fd, char buf[], int count);
// void lseek();
void cat(char *pathname);
void my_cp(char *old_file, char *new_file);
void my_mv(char *old_file, char *new_file);

/* LEVEL 3 */
// void mount();
// void unmount();
// File Permissions
#endif
