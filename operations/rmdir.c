#include "../filesystem.h"

/************ Algorithm of rmdir *************/
/* rmdir(char *pathname)
 { 1. get in-memory INODE of pathname:
      ino = getino(&de, pathanme);
      mip = iget(dev,ino);
2. verify INODE is a DIR (by INODE.i_mode field);
minode is not BUSY (refCount = 1);
DIR is empty (traverse data blocks for number of entries = 2);
3. /* get parent's ino and inode */
/*4. /* remove name from parent directory */
/*findname(pmip, ino, name); //find name from parent DIR rm_child(pmip, name);
5. /* deallocate its data blocks and inode */ //truncat(mip); // deallocate INODE's data blocks
/*6. deallocate INODE
idalloc(mip->dev, mip->ino); iput(mip);
   7. dec parent links_count by 1;
      mark parent dirty; iput(pmip);
 } 8. return 0 for SUCCESS.
*/

void rm_dir(char *pathname)
{
    printf("mkdir() -----\n");
    int i=0, dev = 0;
    char *strs[100], dirname[256], basename[100];
    int pinum = 0, inum = 0;
    MINODE *pmip = (MINODE*) malloc(sizeof(MINODE));
    MINODE *mip;

    //trying to remove the root
    if(pathname[0] == '/'&& strlen(pathname) == 1){
      printf("cannot remove root\n");
      return;
    }

    if(pathname[0] == '/')
    {
      dev = root->dev;
    }
    else
    {
      dev = running->cwd->dev;
    }

    //get path inum
    inum = getino(pathname, dev);
    if(inum == 0)
    {
      printf("%s is an invalid path\n", pathname);
      return;
    }

    mip = iget(dev, inum);

    //check who owns it
    if(mip->INODE.i_uid != running->uid && running->uid != 0)
    {
      printf("uid doesn't match\n");
      iput(mip);
      return;
    }

    //is it a r?
    if(!(S_ISDIR(mip->INODE.i_mode)))
    {
      printf("%s is not a directory\n", pathname);
      iput(mip);
      return;
    }

    //check if it is in use
    if(mip->refCount > 1)
    {
      printf("Still in use\n");
      iput(mip);
      return;
    }

    // has more than jsut . and ..
    if(mip->INODE.i_links_count > 2)
    {
        printf("Dir not empty\n");
        iput(mip);
        return;
    }

    //is it empty?
    if(mip->INODE.i_links_count == 2)
    {
      //check subsequent dirs
      if(!isEmpty(mip, mip->dev))
      {
        printf("Dir not empty\n");
        return;
      }
    }

    //free up the data
    for (i = 0; i < 12; i++)
    {
      if (mip->INODE.i_block[i] != 0)
      {
        bdealloc(mip->dev, mip->INODE.i_block[i]);
      }
    }
    idealloc(mip->dev, mip->ino);
    iput(mip);

    //fix the parent inode in memory
    strcpy(dirname, dname(pathname));
  	strcpy(basename, bname(pathname));
  	printf("mkdir() ---dirname: %s\n", dirname);
  	printf("mkdir() --basename: %s\n", basename);

  	//dirname must exist and is a DIR;
  	if(strcmp(dirname,".")==0) {
  	  pinum = running->cwd->ino;
  	}
  	else if(strcmp(dirname,"/") == 0){
  	  pinum = root->ino;
  	}
  	else{
  	  pinum = getino(dirname, dev);
  	}

  	if(pinum ==0){
  	  printf("&s does not exist\n", dirname);
  	  return;
  	}
  	pmip = iget(dev, pinum);

    delete_dir_entry(pmip, basename);

    //pmip->refCount --; //tenative
    pmip->INODE.i_links_count--;
    pmip->INODE.i_atime = pmip->INODE.i_mtime = time(0L);
    pmip->dirty = 1;

    iput(pmip);
}

//returns 0 for false 1 for true
int isEmpty(MINODE *mip, int dev)
{
  char mybuf[BLKSIZE], temp;
  DIR *dp;
  char *cp;
  int i;

  for(i = 0; i < 12; i++)
  {
    if(!mip->INODE.i_block[i] == 0)
    {
      get_block(dev, mip->INODE.i_block[i], mybuf);
      dp = (DIR*) mybuf;
      cp = dp;

      //traverse through the block to see if it has any files or dirs that aren't . and ..
      while (cp< mybuf + BLKSIZE)
      {
        temp = dp->name[dp->name_len];
        dp->name[dp->name_len] = 0;
        if(strcmp(dp->name, ".")== 0 || strcmp(dp->name, "..")==0)
        {
          dp->name[dp->name_len] = temp;

          cp += dp->rec_len;
          dp = (DIR *)cp;
        }
        else {return 0;} //isn't empty

      }
    }
    else {return 1;} //is empty
  }
}

/************* Algorithm of Delete_dir_entry (name) *************/
/*(1). search DIR's data block(s) for entry by name;
(2). if (entry is the only entry in block)
  clear entry’s inode number to 0;
  else{
    (3). if (entry is last entry in block)
              add entry's rec_len to predecessor entry's rec_len;
    (4).  else{ // entry in middle of block
        add entry's rec_len to last entry's rec_len;
        move all trailing entries left to overlay deleted entry;
      }
    }
    (5). write block back to disk;*/
void delete_dir_entry(MINODE *pmip, char *basename)
{
  int i = 0, j = 0, prev_len = 0, ideal_len = 0, lastLen = 0, add_len = 0;
  int length = 0;
  char mybuf[BLKSIZE], *cp, temp, *tp, *t2p;
  DIR *dp;

  for(i = 0; i < 12; i++)
  {
    if( pmip->INODE.i_block[i] == 0)
    {
      return;
    }

    get_block(pmip->dev, pmip->INODE.i_block[i], mybuf);
    dp = (DIR*) mybuf;
    cp = mybuf;

    while(cp < mybuf + BLKSIZE)
    {
      j = i;
      ideal_len = 4 * ((8 + dp->name_len + 3) / 4);

      temp = dp->name[dp->name_len];
      dp->name[dp->name_len] = 0;

      //have we found the name?
      if(strcmp(basename, dp->name) == 0)// dp name might need a null char appended
      {
        //only entry
        if(dp->rec_len == BLKSIZE)
        {
          //deallocate it so that we can remove
          bdealloc(pmip->dev, pmip->INODE.i_block[j]);
          pmip->INODE.i_block[j]=0;
          pmip->INODE.i_size -= BLKSIZE;

          //move data within direct blocks
          while(j+1 < 12 && pmip->INODE.i_block[j+1]!=0)
          {
            pmip->INODE.i_block[j] = pmip->INODE.i_block[j+1];
            j++;
          }
          return;
        }
        else if(dp->rec_len != ideal_len) //last item
        {
          lastLen = dp->rec_len;
          cp -= prev_len;
          dp = (DIR *) cp;
          dp->rec_len += lastLen;

          put_block(pmip->dev, pmip->INODE.i_block[i], mybuf); //Write bac
          return;
        }
        else{ //dp->rec_len == ideal_length
          add_len = dp->rec_len;
          tp = cp + dp->rec_len;
          length = mybuf + BLOCK_SIZE - tp;

          t2p = cp;

          //Got to the last entry
          while(cp + dp->rec_len < mybuf + BLOCK_SIZE)
          {
            cp += dp->rec_len;
            dp = (DIR *) cp;
          }

          dp->rec_len += add_len;

          memcpy(t2p, tp, length);

          put_block(pmip->dev, pmip->INODE.i_block[i], mybuf);

          return;
        }
      }
      //reset final dp position
      dp->name[dp->name_len] = temp;

      prev_len = dp->rec_len;
      cp += dp->rec_len;
      dp = (DIR *) cp;
    }
  }
}
