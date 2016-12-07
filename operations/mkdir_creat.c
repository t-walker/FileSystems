#include "../filesystem.h"
#include <sys/stat.h>
#include <time.h>

kmkdir(MINODE *pmip,const char *basename, int pinum)
{
        printf("kmkdir() ------\n");
        printf("kmkdir() --- basename = %s\n", basename);
        int i = 0, inum = ialloc(pmip->dev);
        int blk = balloc(pmip->dev);
        MINODE *mip = iget(pmip->dev, inum); //load INODE into and minode
        char mybuf[BLKSIZE];
        DIR* dp;
        char* cp;

        //initialize mip->INODE as a DIR INODE
        mip->INODE.i_mode = 0x41ED; //mode is dir with permissions
        mip->INODE.i_uid = running->uid;
        mip->INODE.i_gid = running->gid;
        mip->INODE.i_size = BLKSIZE;
        mip->INODE.i_links_count = 2;
        //should set up time
        mip->INODE.i_atime = mip->INODE.i_ctime = mip->INODE.i_mtime = time(0L);
        mip->INODE.i_blocks = 2;
        mip->INODE.i_block[0] = blk;

        for (i=1; i < 15; i++) {
                mip->INODE.i_block[i] = 0;
        }

        mip->dirty = 1;
        iput(mip); //write INODE back to disk

        //make data block
        get_block(pmip->dev, blk, mybuf);
        dp = (DIR*)mybuf;

        printf("kmkdir--- inum: %d\n",inum);
        dp->inode = inum;
        dp->name[0] = '.';
        dp->name_len = 1;
        dp->rec_len = 12;

        cp = mybuf;
        cp += dp->rec_len;
        dp = (DIR*)cp;

        printf("kmkdir--- pmip->ino: %d\n",pmip->ino);
        dp->inode = pmip->ino;
        dp->name_len = 2;
        dp->name[0] = '.';
        dp->name[1] = '.';
        dp->rec_len= BLKSIZE - 12;

        put_block(dev, blk, mybuf);

        insert_dir_entry(pmip, inum, basename);
        printf("kmkdir() ------End\n");
}
/* Algorithm of mkdir ***
   int mkdir(char *pathname)
   {
    1. if (pathname is absolute //pathname[0] == '/')
            dev = root->dev;
       else
            dev = PROC's cwd->dev //running ->cwd->dev
    2. divide pathname into dirname and basename
            //numstrs = parse(path);
            //dirname = strs[0-numstrs - 2] with '/'
      //basename = strs[numstrs -1]
    3. //dirname numst exist and is a DIR:
            pino = getino(dirname, dev)//check syntax
      pmip = iget(dev, pino);
      check pmip->INODE is a DIR
    4. //basename numst not exist in parent DIR
            search(pmip, basename) must return 0;
    5. call kmkdir(pmip, basename) to create a DIR;
            kmkdir() has 4 steps:
      5.1 allocate an INODE and a disk block:
             ino = ialloc(dev);
       blk = balloc(dev);
       mip = iget(dev, ino); //load INODE into and minode
      5.2 initialize mip->INODE as a DIR INODE
             mip->INODE.i_block[0] = blk;
       other i_block[] are 0;
       mark minode modified (dirty);
       iput(mip); //write INODE back to disk
      5.3 make data block 0 of INODE to contain . and .. entries;
             write to disk block blk (iput?)
      5.4 enter_child(pmip, ino, basename);which enters (ino, basename) as a DIR entry into the parent INODE -> call insert_dir_entry ?
     6. increment parent INODE's links_count by 1 and make pmip dirty; iput(pmip);
     }*/

void insert_dir_entry(MINODE *pmip,int inum, const char *basename)
{
        printf("insert_dir_entry() ---\n");

        printf("insert_dir_entry() --- basename = %s\n", basename);
        int baslen = strlen(basename);
        int need_len = 4*((8 + baslen + 3)/4); //(4 for ino, 2 for rlen, 2 for nlen = 8)+namelen + 3 for 
        int ideal_len = 0, remain=0;
        int i = 0, j = 0, blk = 0;
        char mybuf[1024], *cp;
        DIR *dp;
        //slightly different than psudo code
        for (i=0; i < 12; i++) {
                if(pmip->INODE.i_block[i] == 0)
                        break;

                blk = pmip->INODE.i_block[i];

                get_block(pmip->dev, blk, mybuf);
                dp = (DIR*)mybuf;
                cp = mybuf;
                //get to the last one
                printf("insert_dir_entry() --- while\n");
                while(cp + dp->rec_len < mybuf + BLKSIZE) {
                        cp += dp->rec_len;
                        dp = (DIR*) cp;
                }
                //now dp is at the end;
                ideal_len = 4*((8 + dp->name_len + 3)/4);
                remain = dp->rec_len - ideal_len;
                if(remain >= need_len) {
                        dp->rec_len = ideal_len;
                        cp += dp->rec_len;
                        dp = (DIR*)cp;

                        dp->inode = inum;
                        dp->rec_len = remain;
                        dp->name_len = baslen;
                        //dp->file_type?
                        for(j = 0; j<baslen; j++)
                        {
                          dp->name[j] = basename[j];
                        }
                        //printf("insert_dir_entry() --- dp->name = %s\n", dp->name);
                        put_block(pmip->dev, blk, mybuf);
                        printf("insert_dir_entry() --- inside if(remain >= need_len)\n");
                        return;
                }
                // moved after the for b/c it will handle two if/else cases else{ //no space, need to allocate}

        }
        //couldn't find a block to use

        blk = balloc(pmip->dev);
        pmip->INODE.i_block[i] = blk;

        get_block(pmip->dev, blk, mybuf);
        dp = (DIR*) mybuf;
        cp = mybuf;

        dp->inode = inum;
        dp->rec_len = BLKSIZE;
        dp->name_len = baslen;
        //dp->file_type?
        for(j = 0; j<baslen; j++)
        {
          dp->name[j] = basename[j];
        }

        put_block(pmip->dev, blk, mybuf);
        printf("insert_dir_entry() --- END\n");

        return;
}

/* insert_dir_entry*
   (1). need_len = 4*((8 + name_len + 3)/4); //new entry need length
   (2). for each existing data block do {
        (3). if(block has only one entry with inode number == 0)
          enter new entry as first entry in block
        (4). else {
          go to last entry in block;
    ideal_len = 4*((8+last_entry's name_len * 3)/4);
    remain = last entry's rec_len - ideal_len
    if(remain >= need _len){
            trim last entry's rec_len to ideal_len;
      enter new entry as last entry with rec_len = remain;
    }
    else{
            allocate a new data block;
      enter new entry as first entry in the data block;
      increase DIR's size by BLKSSIZE;
    }
   }
   write block to disk; (iput())
   (5). mark DIR's minode modified for write back (dirty);*/

char* dname(const char *str)
{
        printf("dname() ------ on: %s\n", str);
        char temp[100];
        char ret[256];
        ret[0] = '\0';
        int i=0;
        int j=0;

        while(str[i])
        {
                temp[j++] = str[i];
                if(str[i] == '/') {
                        temp[j] = '\0';
                        strcat(ret, temp);
                        j=0;
                        temp[0] = '\0';
                }
                i++;
        }
        if(strcmp(ret, "")==0)
                strcpy(ret, ".");
        printf("dname() ------ returning: %s\n", ret);
        return ret;
}

char* bname(const char* str)
{
        printf("bname() ------\n");
        char ret[100], temp[256];
        temp[0] = '\0';
        int i=0,j=0;
        while(str[i])
        {
                ret[j++] = str[i];
                if(str[i] == '/') {
                        ret[j] = '\0';
                        strcat(temp, ret);
                        j=0;
                        ret[0] = '\0';
                }
                i++;
        }
        ret[j] = '\0';
        printf("bname() ------ returning: %s\n", ret);
        return ret;
}

// List the files in a directory
void mk_dir (char *pathname)
{
        printf("mkdir() -----\n");
        int i=0, numstrs = 0, dev = 0;
        char *strs[100], dirname[256], basename[100];
        int pinum = 0, inum = 0;
        MINODE *pmip = (MINODE*) malloc(sizeof(MINODE));
        MINODE *mip;

        //set up dev
        if(pathname[0] == '/') {
                mip = root;
                dev = root->dev;
        }
        else{
                mip = running->cwd;
                dev = running->cwd->dev;
        }

        dirname[0] = '\0';
        basename[0] = '\0';

        // devide pathname into base and dirname
        strcpy(dirname, dname(pathname));
        strcpy(basename, bname(pathname));
        printf("mkdir() ---dirname: %s\n", dirname);
        printf("mkdir() --basename: %s\n", basename);

        //dirname must exist and is a DIR;
        if(strcmp(dirname,".")==0) {
                pinum = running->cwd->ino;
        }
        else if(strcmp(dirname,"/") == 0) {
                pinum = root->ino;
        }
        else{
                pinum = getino(dirname, dev);
        }

        if(pinum ==0) {
                printf("&s does not exist\n", dirname);
                return;
        }
        pmip = iget(dev, pinum);
        //is it a dir?
        if(!(S_ISDIR(pmip->INODE.i_mode))) {
                printf("&s is not a directory\n", dirname);
                iput(pmip);
                return;
        }

        printf("mkdir() --- basename = %s\n", basename);
        //basename can't exist in parent dir
        if(search(pmip, basename) != 0) {
                printf("%s already exists in %s\n", basename, dirname);
                iput(pmip);
                return;
        }

        printf("mkdir() --- basename = %s\n", basename);
        //call kmkdir()
        kmkdir(pmip, basename, pinum);

        pmip->INODE.i_links_count++;
        pmip->INODE.i_atime = time(0L);
        pmip->dirty = 1;
        iput(pmip);
        printf("mkdir() -----End\n");
}

/* Algorithm of creat() ***
   creat(char* fileName)
   {
    this is similar to mkdir except
        1. the INODE.i_mode field is set to REG file type, permission bits set to 0644 for rw-r--r--, and
   2. no data block is allocated for it, so the file size is 0
   3. Do not increment parent INODE's link count
   }*/
void my_creat (char *pathname)
{
        printf("creat() -----\n");
        int i=0, numstrs = 0, dev = 0;
        char *strs[100], dirname[256], basename[100];
        int pinum = 0, inum = 0;
        MINODE *pmip = (MINODE*) malloc(sizeof(MINODE));
        MINODE *mip;

        //set up dev
        if(pathname[0] == '/') {
                mip = root;
                dev = root->dev;
        }
        else{
                mip = running->cwd;
                dev = running->cwd->dev;
        }

        // devide pathname into base and dirname
        strcpy(dirname, dname(pathname));
        strcpy(basename, bname(pathname));
        printf("creat() ---dirname: %s\n", dirname);
        printf("creat() --basename: %s\n", basename);

        //dirname must exist and is a DIR;
        if(strcmp(dirname,".")==0) {
                pinum = running->cwd->ino;
        }
        else if(strcmp(dirname,"/") == 0) {
                pinum = root->ino;
        }
        else{
                pinum = getino(dirname, dev);
        }

        if(pinum ==0) {
                printf("&s does not exist\n", dirname);
                return;
        }
        pmip = iget(dev, pinum);
        //is parent a dir?
        if(!(S_ISDIR(pmip->INODE.i_mode))) {
                printf("&s is not a directory\n", dirname);
                iput(pmip);
                return;
        }

        printf("creat() --- basename = %s\n", basename);
        //basename can't exist in parent dir
        if(search(pmip, basename) != 0) {
                printf("%s already exists in %s\n", basename, dirname);
                iput(pmip);
                return;
        }

        printf("creat() --- basename = %s\n", basename);
        //call kmkdir()
        kcreat(pmip, basename); //may eventually pass atype?

        pmip->INODE.i_atime = time(0L);
        pmip->dirty = 1;
        iput(pmip);
        printf("creat() -----End\n");
}

void kcreat(MINODE *pmip,const char *basename)
{
        printf("creat() ------\n");
        printf("creat() --- basename = %s\n", basename);
        int i = 0, inum = ialloc(pmip->dev);
        MINODE *mip = iget(pmip->dev, inum); //load INODE into and minode
        printf("creat() --- basename1 = %s\n", basename);

        //initialize mip->INODE as a DIR INODE
        mip->INODE.i_mode = 0x81A4; //mode is REG with permissions
        mip->INODE.i_uid = running->uid;
        mip->INODE.i_gid = running->gid;
        mip->INODE.i_size = 0; //no allocation needed
        printf("creat() --- basename2 = %s\n", basename);
        mip->INODE.i_links_count = 1;
        //should set up time
        mip->INODE.i_atime = mip->INODE.i_ctime = mip->INODE.i_mtime = time(0L);
        printf("creat() --- basename3 = %s\n", basename);
        mip->INODE.i_blocks = 0;
        mip->INODE.i_block[0] = 0;

        for (i=1; i < 15; i++) {
                mip->INODE.i_block[i] = 0;
        }

        printf("creat() --- basename4 = %s\n", basename);
        mip->dirty = 1;
        iput(mip); //write INODE back to disk
        printf("creat() --- basename5 = %s\n", basename);
        insert_dir_entry(pmip, inum, basename);
        printf("kmkdir() ------End\n");
}
