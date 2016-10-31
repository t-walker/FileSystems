#ifndef FILESYSTEM_H
#define FILESYSTEM_H

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
