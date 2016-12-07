#include "../filesystem.h"

int my_read(int fd, char buffer[], int count)
{
  u32  d_buf[256];
  u32 di_buf[256];

  char read_buffer[BLOCK_SIZE];

  OFT *oftEntry = running->fd[fd];
  MINODE *mip = oftEntry->mptr;

  int count = 0;
  int read_in = 0;
  int remain;

  int avail = mip->INODE.i_size - oftEntry->offset;
  char *cp = buffer;
  char *cr;

  int blk;
  int lbk;
  int i_blk;
  int start_byte;

  while(count && avail)
  {
    cp = buffer;

    lbk = oftEntry->offset / BLOCK_SIZE;
    start_byte = oftEntry->offset % BLOCK_SIZE;

    if (lbk < 12)
    {
      blk = mip->INODE.i_block[lbk];
    }
    else if (lbk >= 12 && lbk < 256 + 12)
    {
      get_block(mip->dev, mip->INODE.i_block[12], (char *)d_buf);
      blk = d_buf[lbk - 12];
    }
    else
    {
      get_block(mip->dev, mip->INODE.i_block[13], (char *)d_buf);
      i_blk = (lbk - (256 + 12)) / 256;
      get_block(mip->dev, d_buf[i_blk], (char *)di_buf);
      blk = di_buf[(lbk - (256 + 12)) % 256];
    }

    get_block(mip->dev, blk, read_buffer);

    cr = buf + start_byte;

    remain = BLOCK_SIZE - start_byte;

    while (remain > 0)
    {
      if (avail <= remain)
      {
        read_in = avail;
      }
      else
      {
        read_in = remain;
      }

      memcpy(cr, cp, read_in);

      oftEntry->offset += read_in;

      count += read_in;

      avail -= read_in;

      count -= read_in;

      remain -= read_in;

      if (count <= 0 || avail <= 0)
      {
        break;
      }
    }
  }

  return count;
}
