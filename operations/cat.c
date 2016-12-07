#include "../filesystem.h"

void cat(char *pathname)
{
        printf("cat() -- finished iput\n");

        int fd = my_open(pathname, O_RDONLY);

        char buffer[BLOCK_SIZE]; // create a buffer to store data in
        int count = 1;           // set a non-zero count

        while(count) // continue while there is data to read
        {
                count = my_read(fd, buffer, BLOCK_SIZE); // read the data into buffer
                printf("%s", buffer);                    // print what's in the buffer (no \n)
        }

        my_close(fd); // close the file descriptor

        printf("cat() -- finished iput\n");

}
