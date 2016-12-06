#include "../core.h"

int put_block(int fd, int blk, char buf[])
{
        printf("put_block() -- fd: %d\n", fd);
        printf("put_block() -- blk: %d\n", blk);

        lseek(fd, (long)blk*BLKSIZE, 0);
        write(fd, buf, BLKSIZE);
}
