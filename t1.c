#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ext2fs/ext2_fs.h>
#include <string.h>
#include <libgen.h>
#include <sys/stat.h>

#include "type.h"
#include "util.h"

MINODE minode[NMINODE];
MINODE *root;
PROC proc[NPROC], *running;

int dev;
int nblocks;
int ninodes;
int bmap;
int imap;
int inode_start;

char path[200], buf[1024], *deviceName = "disk";

void init()
{
        printf("init() ------\n");

        // Initialize data structures of LEVEL-1
        int i, j;
        MINODE *mip;
        PROC *p;

        printf("iget() -- initalized variables\n");

        // 1) 2 PROCs, P0 with uid=0, P1 with uid=1, all PROC.cwd = 0
        for (i = 0; i < NPROC; i++) {
                proc[i].uid = i;
                proc[i].pid = i+1;
                proc[i].cwd = 0;
                proc[i].status = FREE;
        }

        printf("iget() -- added two empty procs\n");

        running = malloc(sizeof(PROC));
        running = &proc[0];

        printf("iget() -- set running\n");

        // 2) MINODE minode[100]; all with refCount = 0
        for (i = 0; i < NMINODE; i++) {
                minode[i].refCount = 0;
                minode[i].dirty = 0;
                minode[i].mounted = 0;
                minode[i].mptr = 0;
        }

        printf("iget() -- created empty MINODES\n");

        // 3) MINODE *root = 0;
        root = 0;
        printf("iget() -- set the root to 0\n");

}

int main(int argc, char *argv[], char *env[])
{
        char *pathname = "/";
        printf("main() ------\n");

        printf("main() -- calling init()\n");
        init();

        printf("main() -- calling mount_root()\n");
        mount_root(deviceName);

        getCommands(pathname); //need to pass the current working directory
        // // ask for a command string (ls pathame)
        ls(pathname);
        // // ask for a command string (cd pathname)
        // cd(pathname);


        // ask for a command string (stat pathname)
        //stat(pathname, &mystat); // struct stat mystat; print mystat information
        return 0;
}

int quit()
{
        int i = 0;

        for (i = 0; i < NMINODE; i++)
        {
                minode[i].refCount = 0;
                iput(&minode[i]);
        }

        return 0;
}
