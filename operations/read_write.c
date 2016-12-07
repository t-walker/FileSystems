#include "../filesystem.h"

int my_read(int fd, char buffer[], int count)
{

        printf("read() ----------------\n");
        printf("read() -- fd: %d   count: %d\n", fd, count);

        char d_buf[256];
        char di_buf[256];
        char read_buffer[BLOCK_SIZE];

        OFT *oftEntry = running->fd[fd];
        MINODE *mip = oftEntry->mptr;

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
                        get_block(mip->dev, mip->INODE.i_block[12], d_buf);
                        blk = d_buf[lbk - 12];
                }
                else
                {
                        printf("read() -- DOUBLE INDIRECT\n");

                        get_block(mip->dev, mip->INODE.i_block[13],d_buf);
                        i_blk = (lbk - (256 + 12)) / 256;
                        get_block(mip->dev, d_buf[i_blk], di_buf);
                        blk = di_buf[(lbk - (256 + 12)) % 256];
                }

                get_block(mip->dev, blk, buffer);

                cr = buf + start_byte;

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

                        memcpy(cr, cp, read_in);

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
