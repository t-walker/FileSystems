#include "../filesystem.h"

/*int read_file()
{
  Preparations:
    ASSUME: file is opened for RD or RW;
    ask for a fd  and  nbytes to read;
    verify that fd is indeed opened for RD or RW;
    return(myread(fd, buf, nbytes));
}

int myread(int fd, char buf[ ], nbytes) behaves EXACTLY the same as the
read() system call in Unix/Linux.
The algorithm of myread() can be best explained in terms of the following
diagram.

(1).  PROC              (2).                          |
     =======   |--> OFT oft[ ]                        |
     | pid |   |   ============                       |
     | cwd |   |   |mode=flags|                       |
     | . ..|   |   |minodePtr ------->  minode[ ]     |      BlockDevice
     | fd[]|   |   |refCount=1|       =============   |   ==================
 fd: | .------>|   |offset    |       |  INODE    |   |   | INODE -> blocks|
     |     |       |===|======|       |-----------|   |   ==================
     =======           |              |  dev,ino  |   |
                       |              =============   |
                       |
                       |<------- avil ------->|
    -------------------|-----------------------
    |    |    | ...  |lbk  |   |  ...| .......|
    -------------------|---|------------------|-
lbk   0    1 .....     |rem|                   |
                     start                   fsize

------------------------------------------------------------------------------
                 Data structures for reading file

(1). Assume that fd is opened for READ.
(2). The offset in the OFT points to the current byte position in the file from
     where we wish to read nbytes.
(3). To the kernel, a file is just a sequence of contiguous bytes, numbered from
     0 to file_size - 1. As the figure shows, the current byte position, offset
     falls in a LOGICAL block (lbk), which is
               lbk = offset / BLKSIZE

     the byte to start read in that logical block is
             start = offset % BLKSIZE

     and the number of bytes remaining in the logical block is
             remain = BLKSIZE - start.

     At this moment, the file has
             avil = file_size - offset

     bytes available for read.

     These numbers are used in the read algorithm.


(4). myread() behaves exactly the same as the read(fd, buf, nbytes) syscall of
     Unix/Linux. It tries to read nbytes from fd to buf[ ], and returns the
     actual number of bytes read.

(5). ============ Algorithm and pseudo-code of myread() =======================

int myread(int fd, char *buf, int nbytes)
{
 1. int count = 0;
    avil = fileSize - OFT's offset // number of bytes still available in file.
    char *cq = buf;                // cq points at buf[ ]

 2. while (nbytes && avil){

       Compute LOGICAL BLOCK number lbk and startByte in that block from offset;

             lbk       = oftp->offset / BLKSIZE;
             startByte = oftp->offset % BLKSIZE;

       // I only show how to read DIRECT BLOCKS. YOU do INDIRECT and D_INDIRECT

       if (lbk < 12){                     // lbk is a direct block
           blk = mip->INODE.i_block[lbk]; // map LOGICAL lbk to PHYSICAL blk
       }
       else if (lbk >= 12 && lbk < 256 + 12) {
            //  indirect blocks
       }
       else{
            //  double indirect blocks
       }

       // get the data block into readbuf[BLKSIZE]
       get_block(mip->dev, blk, readbuf);

       // copy from startByte to buf[ ], at most remain bytes in this block
       char *cp = readbuf + startByte;
       remain = BLKSIZE - startByte;   // number of bytes remain in readbuf[]
       while (remain > 0){
            *cq++ = *cp++;             // copy byte from readbuf[] into buf[]
             oftp->offset++;           // advance offset
             count++;                  // inc count as number of bytes read
             avil--; nbytes--;  remain--;
             if (nbytes <= 0 || avil <= 0)
                 break;
       }

       // if one data block is not enough, loop back to OUTER while for more ...

   }
   printf("myread: read %d char from file descriptor %d\n", count, fd);
   return count;   // count is the actual number of bytes read
}

                  OPTMIAZATION OF THE READ CODE:

Instead of reading one byte at a time and updating the counters on each byte,
TRY to calculate the maximum number of bytes available in a data block and
the number of bytes still needed to read. Take the minimum of the two, and read
that many bytes in one operation. Then adjust the counters accordingly. This
would make the read loops more efficient.

REQUIRED: optimize the read algorithm in your project.

==========================  HOW TO cat ======================================
cat filename:

   char mybuf[1024], dummy = 0;  // a null char at end of mybuf[ ]
   int n;

1. int fd = open filename for READ;
2. while( n = read(fd, mybuf[1024], 1024)){
       mybuf[n] = 0;             // as a null terminated string
       // printf("%s", mybuf);   <=== THIS works but not good
       spit out chars from mybuf[ ] but handle \n properly;
   }
3. close(fd);*/

//Not from KC
/******** lseek function ***************
 * 1. Given an lseek location
 * 2. Check the value of the new offset
 * 3. Determine where they the marker should be placed
 ******************************************/

int my_read(int fd, char buffer[], int count)
{

        printf("read() ----------------\n");
        printf("read() -- fd: %d   count: %d\n", fd, count);

        char i_buf[256];
        char di_buf[256];
        char read_buffer[BLOCK_SIZE];

        OFT *oftEntry = running->fd[fd];
        MINODE *mip = oftEntry->mptr;

        if (oftEntry->mode != O_RDONLY && oftEntry->mode != O_RDWR)
        {
                printf("Cannot read this file.\n");
                return;
        }

        int r_count = 0;
        int read_in = 0;
        int remain;

        int avail = mip->INODE.i_size - oftEntry->offset;

        char *cp = buffer;
        char *cr;

        printf("read() -- avail: %d\n", avail);

        int blk;
        int lbk;
        int i_blk;
        int start_byte;

        while(count && avail)
        {
                cp = buffer;

                lbk = oftEntry->offset / BLOCK_SIZE;
                start_byte = oftEntry->offset % BLOCK_SIZE;

                printf("read() -- lbk: %d\n", lbk);
                printf("read() -- start_byte: %d\n", start_byte);

                if (lbk < 12)
                {
                        printf("read() -- DIRECT\n");
                        blk = mip->INODE.i_block[lbk];
                        printf("read() -- lbk < 12\n");

                }
                else if (lbk >= 12 && lbk < (256 + 12))
                {
                        printf("read() -- INDIRECT\n");

                        printf("read() -- lbk > 12 && lbk < (256 + 12)\n");
                        get_block(mip->dev, mip->INODE.i_block[12], i_buf);
                        blk = i_buf[lbk - 12];
                }
                else
                {
                        printf("read() -- DOUBLE INDIRECT\n");

                        get_block(mip->dev, mip->INODE.i_block[13],i_buf);
                        i_blk = (lbk - (256 + 12)) / 256;
                        get_block(mip->dev, i_buf[i_blk], di_buf);
                        blk = di_buf[(lbk - (256 + 12)) % 256];
                }

                get_block(mip->dev, blk, read_buffer);
                printf("read() -- buffer contains: %s", buffer);

                cr = read_buffer + start_byte;

                remain = BLOCK_SIZE - start_byte;

                while (remain > 0)
                {
                        printf("read() -- data remains\n");

                        if (avail <= remain)
                        {
                                read_in = avail;
                                printf("read() -- read_in = avail = %d\n", avail);

                        }
                        else
                        {
                                read_in = remain;
                                printf("read() -- read_in = remain = %d\n", remain);
                        }

                        memcpy(cp, cr, read_in);


                        r_count += read_in;
                        printf("read() -- r_count = %d\n", r_count);

                        avail -= read_in;
                        printf("read() -- avail = %d\n", avail);

                        count -= read_in;
                        printf("read() -- count = %d\n", count);

                        remain -= read_in;
                        printf("read() -- remain = %d\n", remain);

                        oftEntry->offset += read_in;
                        printf("read() -- oftEntry->offset = %d\n", oftEntry->offset);

                        if (count <= 0 || avail <= 0)
                        {
                                printf("read() -- count or avail is 0\n");

                                break;
                        }
                }
        }

        return r_count;
}

int my_write(int fd, char buf[], int count)
{
        printf("write() ------------------------\n");
        printf("write() -- fd: %d, count: %d\n", fd, count);
        printf("write() -- buf contains: %s\n", buf);

        int lbk;
        int blk;

        int tmpBlk;
        int i_blk;
        int di_blk;

        int remain = 0;
        int w_count = 0;
        int start_byte = 0;
        int written;


        char i_buf[256];
        char di_buf[256];
        char tmpBuf[BLOCK_SIZE];
        char write_buffer[BLOCK_SIZE];

        OFT *oftEntry = running->fd[fd];
        MINODE *mip = oftEntry->mptr;

        if (oftEntry->mode == O_RDONLY)
        {
                printf("Cannot write to this file.\n");
                return;
        }

        char *cq;
        char *cp;

        while(count > 0)
        {
                cq = buf;

                lbk = oftEntry->offset / BLOCK_SIZE;
                start_byte = oftEntry->offset % BLOCK_SIZE;

                printf("write() -- lbk: %d, start_byte: %d\n", lbk, start_byte);

                if (lbk < 12) // direct block
                {
                        printf("write() -- lbk < 12\n");

                        if(mip->INODE.i_block[lbk] == 0) // no data yet
                        {
                                printf("write() -- i_block[lbk] == 0\n");

                                mip->INODE.i_block[lbk] = balloc(mip->dev); // allocates a block
                                get_block(mip->dev, mip->INODE.i_block[lbk], tmpBuf); // puts block into tmpBuf
                                memset(tmpBuf, '\0', BLOCK_SIZE); // fill the tmpBuf with null
                                printf("write() -- passed memset\n");
                                put_block(mip->dev, mip->INODE.i_block[lbk], tmpBuf); // puts tmpBuf into file
                        }
                        blk = mip->INODE.i_block[lbk];
                        printf("write() -- blk = mip->INODE.i_block[lbk]\n");

                }
                else if (lbk >= 12 && lbk < (256 + 12))
                {
                        if(mip->INODE.i_block[12] == 0) // If i_block[12] is emtpy we need to fill it.
                        {
                                mip->INODE.i_block[12] = balloc(mip->dev); // balloc ensures there's data.
                                get_block(mip->dev, mip->INODE.i_block[12], tmpBuf); // get the block of data from disk.
                                memset(tmpBuf, '\0', BLOCK_SIZE); // fills tmpBuf up with null.
                                put_block(mip->dev, mip->INODE.i_block[12], tmpBuf); // put's the block into the buf.
                        }
                        tmpBlk = mip->INODE.i_block[12]; // stores the i_block[12] in the tmbBlk.

                        get_block(mip->dev, tmpBlk, i_buf); // get's the tmpBlk and puts it into i_buf.

                        if (i_buf[lbk - 12] == 0) // if block is empty fill it
                        {
                                i_buf[lbk - 12] = balloc(mip->dev);
                                get_block(mip->dev, i_buf[lbk -12], tmpBuf);
                                memset(tmpBuf, '\0', BLOCK_SIZE);
                                put_block(mip->dev, i_buf[lbk - 12], tmpBuf);
                        }
                        blk = i_buf[lbk - 12];
                        put_block(mip->dev, tmpBlk, i_buf);
                }
                else
                {
                        if(mip->INODE.i_block[13] == 0) // It's empty, we need to fill.
                        {
                                mip->INODE.i_block[13] = balloc(mip->dev);

                                get_block(mip->dev, mip->INODE.i_block[13], tmpBuf);
                                memset(tmpBuf, '\0', BLOCK_SIZE);
                                put_block(mip->dev, mip->INODE.i_block[13], tmpBuf);
                        }
                        tmpBlk = mip->INODE.i_block[13];
                        get_block(mip->dev, tmpBlk, i_buf);

                        i_blk = (lbk - (256 + 12) / 256);

                        if (i_buf[i_blk] == 0)
                        {
                                i_buf[i_blk] = balloc(mip->dev);

                                get_block(mip->dev, i_buf[i_blk], tmpBuf);
                                memset(tmpBuf, '\0', BLOCK_SIZE);
                                put_block(mip->dev, i_buf[i_blk], tmpBuf);
                        }
                        put_block(mip->dev, tmpBlk, i_buf);

                        get_block(mip->dev, i_buf[i_blk], di_buf);
                        di_blk = (lbk - (256 + 12) % 256);
                        if(di_buf[di_blk] == 0)
                        {
                                di_buf[di_blk] = balloc(mip->dev);

                                get_block(mip->dev, di_buf[di_blk], tmpBuf);
                                memset(tmpBuf, '\0', BLOCK_SIZE);
                                put_block(mip->dev, di_buf[di_blk], tmpBuf);
                        }

                        blk = di_buf[di_blk];
                        put_block(mip->dev, i_buf[di_blk], di_buf);
                }

                get_block(mip->dev, blk, write_buffer);
                printf("write() -- got block\n");

                cp = write_buffer + start_byte;
                printf("write() -- cp = write_buffer + start_byte\n");

                remain = BLOCK_SIZE - start_byte;
                printf("write() -- remain: %d\n", remain);

                written = 0;

                while (remain > 0)
                {
                        if (count < BLOCK_SIZE)
                        {
                                printf("write() -- written = count = %d\n", count);

                                written = count;
                        }
                        else
                        {
                                printf("write() -- written = BLOCK_SIZE = %d\n", BLOCK_SIZE);

                                written = BLOCK_SIZE;
                        }

                        memcpy(cp, cq, written);
                        printf("write() -- memcpy(cp, cq, written)\n");

                        w_count += written;
                        count -= written;
                        remain -= written;
                        printf("write() -- w_count: %d\n", w_count);
                        printf("write() -- count: %d\n", count);
                        printf("write() -- remain: %d\n", remain);


                        oftEntry->offset += written;
                        printf("write() -- oftEntry->offset: %d\n", oftEntry->offset);


                        if(oftEntry->offset > mip->INODE.i_size)
                        {
                                mip->INODE.i_size += written;
                                printf("write() -- expanding to: %d\n", mip->INODE.i_size);

                        }
                        if (count <= 0)
                        {
                                printf("write() -- nothing else to write\n");
                                break;
                        }
                }
                put_block(mip->dev, blk, write_buffer);
                printf("write() -- putting the block\n");
        }
        mip->dirty = 1;
        printf("write() -- setting the mip to dirty\n");

        return w_count;
}

void my_lseek(int fd, int position)
{

}
