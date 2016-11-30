#include "../filesystem.h"

// List the files in a directory
void mkdir (char *pathname)
{
        printf("mkdir() -----\n");
	
        printf("mkdir() -----End\n");
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

/* Algorithm of creat() ***
creat(char* fileName)
{
    this is similar to mkdir except
        1. the INODE.i_mode field is set to REG file type, permission bits set to 0644 for rw-r--r--, and
	2. no data block is allocated for it, so the file size is 0
	3. Do not increment parent INODE's link count
}*/
