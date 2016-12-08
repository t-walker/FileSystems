#include "../filesystem.h"

/*=============================================================================

                      HOW TO cp ONE file:

cp src dest:

1. fd = open src for READ;

2. gd = open dst for WR|CREAT;

   NOTE:In the project, you may have to creat the dst file first, then open it
        for WR, OR  if open fails due to no file yet, creat it and then open it
        for WR.

3. while( n=read(fd, buf[ ], BLKSIZE) ){
       write(gd, buf, n);  // notice the n in write()
   }

==============================================================================

                    HOW TO mv (rename)
mv src dest:

1. verify src exists; get its INODE in ==> you already know its dev
2. check whether src is on the same dev as src

              CASE 1: same dev:
3. Hard link dst with src (i.e. same INODE number)
4. unlink src (i.e. rm src name from its parent directory and reduce INODE's
               link count by 1).

              CASE 2: not the same dev:
3. cp src to dst
4. unlink src*/

void my_cp(char *old_file, char *new_file)
{
  char mybuf[BLKSIZE];
  //open old for read
  int fd1 = my_open(old_file, O_RDONLY);

  //make sure we create or already have new_file in the system
  int fd2 = my_open(new_file, O_CREAT);
  my_close(fd2);

  //open new for write
  fd2 = my_open(new_file, O_WRONLY);

  //write from old to new
  int r;
  while (r)
  {
    r = my_read(fd1, mybuf, BLKSIZE);
    //write the data
    my_write(fd2, mybuf, r);
    memset(mybuf, 0, BLKSIZE);//clear out the buffer
  }
  //close the files
  my_close(fd1);
  my_close(fd2);
}

void my_mv(char *old_file, char *new_file)
{
  //1. verify src exists; get its INODE in ==> you already know its dev
  int ino = getino(old_file, running->cwd->dev);
  if(!ino)
  {
    return;
  }
  //2. check whether src is on the same dev as dest

              //  CASE 1: same dev:
  //3. Hard link dst with src (i.e. same INODE number)
  my_link (old_file, new_file);
  //4. unlink src (i.e. rm src name from its parent directory and reduce INODE's link count by 1).
  my_unlink(old_file);
    //            CASE 2: not the same dev: //didn't impliment mount/umount
  //3. cp src to dst
//  4. unlink src
//  my_cp(old_file, new_file);
}
