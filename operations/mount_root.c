#include "../filesystem.h"

// MOUNT_ROOT
// Mounts the root of the device given the device name.
void mount_root(char* deviceName) // Mount root file system, establish / and CWDs
{
        // Open device for RW (get a file descriptor dev for the opened device)
        // read SUPER block to verify it's an EXT2 FS
        printf("mount_root() ------name:%s\n", deviceName);

        dev = open(deviceName, O_RDWR);
        printf("mount_root() -- opened device.\n");

        if (dev < 0)
        {
                printf("mount_root() -- open failed.\n");
                exit(1);
        }

        //read SUPER block
        get_block(dev, 1, buf);
        printf("mount_root() -- read superblock.\n");

        sp = (SUPER *)buf;
        printf("mount_root() -- setting sp to superblock.\n");

        //check for EXT2 magic number:

        printf("s_magic = %x\n", sp->s_magic);
        if (sp->s_magic != 0xEF53)
        {
                printf("NOT an EXT2 FS\n");
                exit(1);
        }

        nblocks = sp->s_blocks_count;
        ninodes = sp->s_inodes_count;
        printf("setting nblocks & ninodes\n");

        get_block(dev, 2, buf);
        printf("mount_root() -- got block.\n");
        gp = (GD* )buf;
        printf("mount_root() -- set the group descriptor\n");

        inode_start = gp->bg_inode_table;
        printf("mount_root() -- set the starting inode.\n");

        //set up root
        root = iget(dev, 2);
        printf("mount_root() -- setting up root.\n");

        if(root == 0)
        {
                printf("mount_root() -- root failed to open.\n");
                return;
        }

        root->mptr = (MNTABLE*)malloc(sizeof(MNTABLE));
        root->mptr->ninodes = ninodes;
        root->mptr->nblocks = nblocks;
        root->mptr->dev = dev;
        root->mptr->busy = 1;
        root->mptr->mounted_inode = root;
	//imap = gp->bg_inode_bitmap;
	//bmap = gp->bg_block_bitmap;
        printf("mount_root() -- set all of the values for root.\n");

        strcpy(root->mptr->name,"/");
        printf("mount_root() -- copied '/' to the root name.\n");

        strcpy(root->mptr->mount_name, deviceName);
        printf("mount_root() -- copied deviceName to root mount_name.\n");

        // Let CWD of both P0 and P1 point at the root minode (refCount = 3)
        proc[0].cwd = iget(dev, 2);
        proc[1].cwd = iget(dev, 2);
        printf("mount_root() -- set CWD of P0 and P1.\n");

        printf("mount_root() finished\n\n");
}
