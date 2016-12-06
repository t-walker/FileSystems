#include "../core.h"

int get_block(int fd, int blk, char buf[])
{
        //printf("get_block() -- fd: %d\n", fd);
        //printf("get_block() -- blk: %d\n", blk);
        lseek(fd, (long)blk*BLKSIZE, 0);
        read(fd, buf, BLKSIZE);
}

int put_block(int fd, int blk, char buf[])
{
        //printf("put_block() -- fd: %d\n", fd);
        //printf("put_block() -- blk: %d\n", blk);

        lseek(fd, (long)blk*BLKSIZE, 0);
        write(fd, buf, BLKSIZE);
}
