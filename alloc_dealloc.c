
/*********** alloc_dealloc.c file *************/
#include "filesystem.h"
//#include "type.h"

int tst_bit(char *buf, int bit)
{
  int i, j;

  i = bit / 8;
  j = bit % 8;
  if (buf[i] & (1 << j)){
    return 1;
  }
  return 0;
}

int clr_bit(char *buf, int bit)
{
  int i, j;
  i = bit / 8;
  j = bit % 8;
  buf[i] &= ~(1 << j);
  return 0;
}

int set_bit(char *buf, int bit)
{
  int i, j;
  i = bit / 8;
  j = bit % 8;
  buf[i] |= (1 << j);
  return 0;
}

int incFreeInodes(int dev)
{
  char mybuf[BLKSIZE];

  // inc free inodes count in SUPER and GD
  get_block(dev, 1, mybuf);
  sp = (SUPER *)mybuf;
  sp->s_free_inodes_count++;
  put_block(dev, 1, mybuf);

  get_block(dev, 2, mybuf);
  gp = (GD *)mybuf;
  gp->bg_free_inodes_count++;
  put_block(dev, 2, mybuf);
}

int decFreeInodes(int dev)
{
  char mybuf[BLKSIZE];
  // YOU DO IT
   get_block(dev, 1, mybuf);
  sp = (SUPER *)mybuf;
  sp->s_free_inodes_count--;
  put_block(dev, 1, mybuf);

  get_block(dev, 2, mybuf);
  gp = (GD *)mybuf;
  gp->bg_free_inodes_count--;
  put_block(dev, 2, mybuf);
}

int incFreeBlocks(int dev)
{
  char mybuf[BLKSIZE];

  // inc free block count in SUPER and GD
  get_block(dev, 1, mybuf);
  sp = (SUPER *)mybuf;
  sp->s_free_blocks_count++;
  put_block(dev, 1, mybuf);

  get_block(dev, 2, mybuf);
  gp = (GD *)mybuf;
  gp->bg_free_blocks_count++;
  put_block(dev, 2, mybuf);
}

int decFreeBlocks(int dev)
{
  char mybuf[BLKSIZE];
  // YOU DO IT
  get_block(dev, 1, mybuf);
  sp = (SUPER *)mybuf;
  sp->s_free_blocks_count--;
  put_block(dev, 1,mybuf);

  get_block(dev, 2, mybuf);
  gp = (GD *)mybuf;
  gp->bg_free_blocks_count--;
  put_block(dev, 2, mybuf);
}


int ialloc(int dev)
{
 int i;
 char mybuf[BLKSIZE];

 // get inode Bitmap into buf
 get_block(dev, imap, mybuf);
 printf("ialloc() --- imap = %d\n",imap);
 for (i=0; i < ninodes; i++){
   if (tst_bit(mybuf, i)==0){
     set_bit(mybuf, i);
     // update free inode count in SUPER and GD
     decFreeInodes(dev);
     put_block(dev, imap, mybuf);

     printf("ialloc: ino=%d\n", i+1);
     return (i+1);
   }
 }
 return 0;
}

int idealloc(int dev, int ino)
{
  int i;
  char mybuf[BLKSIZE];
  ino--;
  /*if (ino > ninodes){
    printf("inumber %d out of range\n", ino);
    return;
  }*/

  // get inode bitmap block
  get_block(dev, imap, mybuf);
  clr_bit(mybuf, ino);

  // update free inode count in SUPER and GD
  incFreeInodes(dev);
  // write buf back
  put_block(dev, imap, mybuf);

}

int balloc(int dev)
{
  // YOU DO IT

 int i;
 char mybuf[BLKSIZE];

 // get inode Bitmap into buf
 get_block(dev, bmap, mybuf);

 for (i=0; i < nblocks; i++){
   if (tst_bit(mybuf, i)==0){
     set_bit(mybuf, i);
     // update free inode count in SUPER and GD
     decFreeBlocks(dev);
     put_block(dev, bmap, mybuf);

     printf("balloc: bit=%d\n", i+1); //blocks are zero based logically
     return i+1;
   }
 }
 return 0;
}


int bdealloc(int dev, int bit)
{
  // YOU DO IT

  int i;
  char mybuf[BLKSIZE];
  bit--;
  /*if (bit > nblocks){
    printf("bit %d out of range\n", bit);
    return;
  }*/

  // get inode bitmap block
  get_block(dev, bmap, mybuf);
  clr_bit(mybuf, bit);

  // update free inode count in SUPER and GD
  incFreeBlocks(dev);
  // write buf back
  put_block(dev, bmap, mybuf);
}
