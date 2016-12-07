#include "../filesystem.h"

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
  while (r = my_read(fd1, mybuf, BLKSIZE))
  {
    //last bit of data
    if (r < BLKSIZE)
      break;
    //write the data
    my_write(fd2, mybuf, r);
    memset(mybuf, 0, BLKSIZE);//clear out the buffer
  }

  //write the rest of the data
  my_write(fd2, mybuf, r);

  //close the files
  my_close(fd1);
  my_close(fd2);
}

void my_mv(char *old_file, char *new_file)
{
  my_cp(old_file, new_file);
  my_rm(old_file);
}
