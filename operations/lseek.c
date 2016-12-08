#include "../filesystem.h"

void l_seek(int fd, int offset)
{
  int size;
  int currentOffset;

  if(fd >= 0 && fd < 10)
  {
    OFT *oftEntry = running->fd[fd];      // get the fd from the running process
    currentOffset = oftEntry->offset;     // get the current offset
    size = oftEntry->mptr->INODE.i_size;  // get the size of the fd's mip

    if (offset < 0) // if the offset is less than zero
    {
      offset = 0;   // set it to zer0
    }
    else if(size < offset) // if the offset is greater than the side
    {
      offset = size;       // set the offset to the size / end of file
    }

    running->fd[fd]->offset = offset; // set the actual file size
  }
}
