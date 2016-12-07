#include "../filesystem.h"

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

                        oftEntry->offset += read_in;
                        printf("read() -- oftEntry->offset = %d\n", oftEntry->offset);

                        r_count += read_in;
                        printf("read() -- r_count = %d\n", r_count);

                        avail -= read_in;
                        printf("read() -- avail = %d\n", avail);

                        count -= read_in;
                        printf("read() -- count = %d\n", count);

                        remain -= read_in;
                        printf("read() -- remain = %d\n", remain);

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
