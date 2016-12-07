#include "../filesystem.h"

/************** Algorithm of open() ***********/
// int open(file, flags)
// {

// 1. get file's minode:
// ino = getino(&dev, file);
// if (ino==0 && O_CREAT){
// creat(file); ino = getino(&dev, file);
// }
// mip = iget(dev, ino);

// 2. check file INODE's access permission;
// for non-special file, check for incompatible open modes;

// 3. allocate an openTable entry;
// initialize openTable entries;
// set byteOffset = 0 for R|W|RW; set to file size for APPEND mode;

// 4. Search for a FREE fd[ ] entry with the lowest index fd in PROC;
// let fd[fd]point to the openTable entry;

// 5. unlock minode;
// return fd as the file descriptor;
// }

int my_open(char *pathname, int flag)
{
        int ino = running->cwd->ino;
        int dev = running->cwd->dev;
        int mode;
        MINODE *mip;

        // flags:
        // 0 | R
        // 1 | W
        // 2 | RW
        // 3 | APPEND
        // 4 | CREATE (?)

        // Get the file's minode;
        ino = getino(pathname, dev);

        if(!ino && flag == O_CREAT)
        {
                my_creat(pathname);
                ino = getino(pathname, dev);

        }

        mip = iget(dev, ino);

        mode = flag;

        if (!S_ISREG(mip->INODE.i_mode))
        {
                printf("open() -- Not a regular file\n");
                iput(mip);
                return 0;
        }

        if (!mip->INODE.i_uid != running->uid && running->uid != 0)
        {
                printf("open() -- Permission error\n");
                iput(mip);
                return 0;
        }

        // Check the INODE's access permission;
        // for non-special

        // Allocate an openTable entry;kl
        // initialize the openTable entry;
        // set byteOffset = 0 for R | W | RW;
        // set file size for APPEND mode;
        OFT *oftEntry = (OFT *) malloc (sizeof(OFT));

        oftEntry->mode = mode;
        oftEntry->refCount = 1;
        oftEntry->mptr = mip;

        if(mode == O_RDONLY) // R
        {
                oftEntry->offset = 0;
        }

        if(mode == O_WRONLY) // W
        {
                //Need to truncate();
                oftEntry->offset = 0;
        }

        if(mode == O_RDWR) // RW
        {
                oftEntry->offset = 0;
        }

        if(mode == O_APPEND) // APPEND
        {
                oftEntry->offset = mip->INODE.i_size;
        }

        // Search for a FREE fd[] entry with lowest index fd in PROC;
        // let fd[fd] point to the openTable entry;
        int i = 0;
        for(i = 0; i < NFD; i++)
        {
                if(running->fd[i] == 0)
                {
                        running->fd[i] = oftEntry;
                        break;
                }
        }

        // for(i = 0; i < NFD; i++)
        // {
        //   if(running->fd[i])
        //   {
        //     printf("offset: %d\n", running->fd[i]->offset);
        //     printf("size: %d\n", running->fd[i]->mptr->INODE.i_size);
        //   }
        // }

        // unlock minode;
        // return the fd as the file descriptor;
        mip->dirty = 1;
        printf("open() -- My fd is: %d\n", i);
        return i;
}

int my_close(int fd)
{
        if(fd >= 0 && fd <= NFD)
        {
                OFT* tmp = running->fd[fd];

                if(!tmp)
                {
                        printf("close() -- No fd found.\n");
                        return 1;

                }

                running->fd[fd] = 0;

                tmp->refCount -= 1;

                if(tmp->refCount > 0)
                {
                        printf("close() -- File is still in use.\n");
                        return 1;
                }
                else
                {
                        printf("close() -- Closing file.\n");
                        if(tmp->mptr->dirty)
                        {
                                tmp->mptr->INODE.i_mtime = time(0L);
                        }
                        iput(tmp->mptr);
                }
        }
        else
        {
                printf("close() -- invalid fd\n");
                return 1;
        }
}
