#include "../filesystem.h"

void cat(char *pathname)
{
        // printf("cat() ---------------------\n");
        // MINODE *mip;
        // char data[BLOCK_SIZE];
        // char i_data[BLOCK_SIZE];
        // char di_data[BLOCK_SIZE];
        //
        // int ino = running->cwd->ino;
        // int dev = running->cwd->dev;
        // int *indirect, *d_indirect, *di_indirect;
        //
        // printf("cat() -- Initalized variables\n");
        //
        //
        //
        // ino = getino(pathname, dev);
        // printf("cat() -- got ino\n");
        //
        // if (!ino)
        // {
        //         return;
        // }
        //
        // mip = iget(dev, ino);
        // printf("cat() -- finished iget\n");

        // if(!S_ISREG(mip->INODE.i_mode))
        // {
        //         iput(mip);
        //         printf("cat() -- cannot cat this\n");
        //         return 1;
        // }


        int fd = my_open(pathname, O_RDONLY);

        char buffer[50];
        int count = my_read(fd, buffer, BLOCK_SIZE);
        int total_bytes = count;

        while(count)
        {
          printf("%s", buffer);
          count = my_read(fd, buffer, BLOCK_SIZE);
          getchar();
        }

        my_close(fd);

        // int i = 0, j = 0;
        //
        // for(i = 0; i < 12; i++)
        // {
        //         if(mip->INODE.i_block[i])
        //         {
        //                 get_block(dev, mip->INODE.i_block[i], data);
        //                 printf("%s", data);
        //         }
        //         else
        //         {
        //                 return;
        //         }
        // }
        //
        // if(mip->INODE.i_block[12])
        // {
        //         get_block(dev, mip->INODE.i_block[12], data);
        //         indirect = (int *)data;
        //
        //         for(i = 0; i < 256; i++)
        //         {
        //                 if(*indirect)
        //                 {
        //                         get_block(dev, *indirect, data);
        //                         printf("%s", data);
        //                         indirect++;
        //                 }
        //                 else
        //                 {
        //                         return;
        //                 }
        //         }
        // }
        //
        // if(mip->INODE.i_block[13])
        // {
        //         get_block(dev, mip->INODE.i_block[13], data);
        //         indirect = (int *)data;
        //
        //         for(i = 0; i < 256; i++)
        //         {
        //                 if(*indirect)
        //                 {
        //                         get_block(dev, *indirect, i_data);
        //                         d_indirect = (int *)i_data;
        //                         for(j = 0; j < 256; j++)
        //                         {
        //                           if(*d_indirect)
        //                           {
        //                             get_block(dev, *d_indirect, di_data);
        //                             printf("%s", di_data);
        //                             d_indirect++;
        //                           }
        //                         }
        //                 }
        //                 else
        //                 {
        //                   return;
        //                 }
        //         }
        // }

// iput(mip);
printf("cat() -- finished iput\n");

}
