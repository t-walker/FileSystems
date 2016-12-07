#include "../filesystem.h"

void l_seek(int fd, int offset)
{
  int size;
  int currentOffset;

  if(fd >= 0 && fd < 10)
  {
    OFT *oftEntry = running->fd[fd];
    currentOffset = oftEntry->offset;
    size = oftEntry->mptr->INODE.i_size;

    if (offset < 0)
    {
      offset = 0;
    }
    else if(size < offset)
    {
      offset = size;
    }

    running->fd[fd]->offset = offset;
  }
}
