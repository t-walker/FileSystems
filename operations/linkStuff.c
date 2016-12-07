#include "../filesystem.h"

/********* Algorithm of link ********/
/*link(old_file, new_file)
{
    1. // verify old_file exists and is not DIR;
      oino = getino(&odev, old_file);
      omip = iget(odev, oino);
      check file type (cannot be DIR).
    2. // new_file must not exist yet:
      nion = getino(&ndev, new_file) must return 0;
      ndev of dirname(newfile) must be same as odev
    3.// creat entry in new_parent DIR with same ino
      pmip -> minode of dirname(new_file);
      enter_name(pmip, omip->ino, basename(new_file));
    4. omip->INODE.i_links_count++;
      omip->dirty = 1;
      iput(omip);
      iput(pmip);
  }*/

  void my_link(char *old_file, char *new_file)
  {
    int oinum, ninum, odev, ndev;
    MINODE *omip, *pmip;
    char dirname[256], basename[100];

    //validate old file path
    if(old_file[0] == '\0')
    {
      printf("No oldfile given\n");
      return;
    }

    if(new_file[0] == '\0')
    {
      printf("No newfile given\n");
      return;
    }

    //setup ips and devs
    if(old_file[0] == '/')
    {
      odev = root->dev;
    }
    else
    {
      odev = running->cwd->dev;
    }

    oinum = getino(old_file, odev);
    if(oinum == 0)
    {
      printf("Invalid oldfile\n");
      return;
    }

    //get old ip
    omip = iget(odev, oinum);

    //make sure it's a file or link
    if(!(S_ISREG(omip->INODE.i_mode)) && !(S_ISLNK(omip->INODE.i_mode)))
    {
      printf("Not file or link\n");
      iput(omip);
      return;
    }

    //get dirname and basename for new file
    strcpy(dirname, dname(new_file));
    strcpy(basename, bname(new_file));
    printf("link() ---dirname: %s\n", dirname);
    printf("link() --basename: %s\n", basename);

    //set up new file ip and dev
    if(dirname[0] == '/')
    {
      ndev = root->dev;
    }
    else
    {
      ndev = running ->cwd->dev;
    }

    //set up parent
    if(strcmp(dirname, ".")==0)
    {
      ninum = running ->cwd->ino;
    }
    else if (strcmp(dirname, "/")==0)
    {
      ninum = root-> ino;
    }
    else
    {
      ninum = getino(dirname, ndev);
    }

    //make sure new path is valid
    if(ninum == 0)
    {
      printf("Invalid new_file path\n");
      iput(omip);
      return;
    }

    //get the parent ip
    pmip = iget(ndev, ninum);

    //make sure it's a dir
    if(!(S_ISDIR(pmip->INODE.i_mode)))
    {
      printf("New file path not a directory\n");
      iput(omip);
      iput(pmip);
      return;
    }

    //mk sure they are the same device
    if(odev != ndev)
    {
      printf("Can't link on different devices\n");
      iput(omip);
      iput(pmip);
      return;
    }

    //add the new link
    insert_dir_entry(pmip, omip->ino, basename);
    omip->INODE.i_links_count++;
    omip->INODE.i_atime = pmip->INODE.i_atime = time(0L);
    pmip->dirty = 1;
    omip->dirty = 1;
    iput(omip);
    iput(pmip);

  }
/*********** Algorithm of unlink *************/
/*unlink(char *filename)
{
  1. get filenmae's minode:
    ino = getino(&dev, filename);
    mip = iget(dev, ino);
    check it's a REG or SLINK file
    2. // remove basename from parent DIR
      rm_child(pmip, mip->ino, basename);
      pmip->dirty = 1;
      iput(pmip);
    3. // decrement INODE's link_count
      mip->INODE.i_links_count--;
      if (mip->INODE.i_links_count > 0)
      {
        mip->dirty = 1; iput(mip);
      }
    4. if (!SLINK file) // assume:SLINK file has no data block
      truncate(mip);
      // deallocate all data blocks
      deallocate INODE;
      iput(mip);
  }*/
void my_unlink(char *filename)
{
  int i, inum, pinum, dev;
  MINODE *mip, *pmip;
  char dirname[256], basename[100];

  if(filename[0] == '\0')
  {
    printf("Filename does not exist\n");
    return;
  }

//get dev
  if(filename[0] == '/')
  {
    dev = root->dev;
  }
  else
  {
    dev = running->cwd->dev;
  }

//get ino
  inum = getino(filename, dev);
  if(inum == 0)
  {
    printf("pathname invalid\n");
    return;
  }

//get mip for filename
  mip = iget(dev, inum);

//check that it's not a dir (ie it is a file or link)
  if(S_ISDIR(mip->INODE.i_mode))
  {
    printf("directory, can't unlink\n");
    iput(mip);
    return;
  }

/////remove base name from parent dir ///
//split into parent and base
  strcpy(dirname, dname(filename));
  strcpy(basename, bname(filename));
  printf("unlink() ---dirname: %s\n", dirname);
  printf("unlink() --basename: %s\n", basename);

//Get the parent dir set up
  if(strcmp(dirname, ".")==0)
  {
    pinum = running->cwd->ino;
  }
  else if(strcmp(dirname, "/") ==0 /*&& 1 == strlen(dirname)*/)
  {
    pinum = root->ino;
  }
  else{
    pinum = getino(dirname, dev);
  }

//is inum valid
  if(pinum == 0)
  {
    printf("path doesn't exist\n");
    iput(mip);
    return;
  }

//get parent ip
  pmip = iget(dev, pinum);

//remove base from the directory
  delete_dir_entry(pmip, basename);

  pmip->INODE.i_atime = time(0L);
  pmip->INODE.i_mtime = time(0L);
  pmip->dirty = 1;

  iput(pmip);

  //decrement link count
    mip->INODE.i_links_count--;
  //remove the child's data by deallocating
  if(mip->INODE.i_links_count > 0)
  {
    mip->dirty = 1;
    iput(mip);
    return;
  }
  /*if (!SLINK file) // assume:SLINK file has no data block
    truncate(mip);*/
  idealloc(mip->dev, mip->ino);
  mip->dirty = 1;
  iput(mip);
}

/*Algorithm of
symlink(old_file, new_file)
{
1. check: old_file must exist and new_file not yet exist;
2. create new_file;
change new_file to SLINK type;
3. // assume length of old_file name <= 60 chars
store old_file name in newfile's INODE.i_block[ ] area.
mark new_file's minode dirty;
iput(new_file's minode);
4. mark new_file parent minode dirty;
put(new_file's parent minode);
}*/

void my_symlink(char *old_file, char *new_file)
{
  int oinum, ninum, odev, ndev;
  MINODE *omip, *pmip;
  char dirname[256], basename[100];

  //validate old file path
  if(old_file[0] == '\0')
  {
    printf("No oldfile given\n");
    return;
  }

  if(new_file[0] == '\0')
  {
    printf("No newfile given\n");
    return;
  }

  //setup ips and devs
  if(old_file[0] == '/')
  {
    odev = root->dev;
  }
  else
  {
    odev = running->cwd->dev;
  }

  oinum = getino(old_file, odev);
  if(oinum == 0)
  {
    printf("Invalid oldfile\n");
    return;
  }

  //get old ip
  omip = iget(odev, oinum);

  //make sure it's a file or link
  if(!(S_ISREG(omip->INODE.i_mode)) && !(S_ISLNK(omip->INODE.i_mode)))
  {
    printf("Not file or link\n");
    iput(omip);
    return;
  }
  //done with omip
  iput(omip);

  //get dirname and basename for new file
  strcpy(dirname, dname(new_file));
  strcpy(basename, bname(new_file));
  printf("link() ---dirname: %s\n", dirname);
  printf("link() --basename: %s\n", basename);

  //set up new file ip and dev
  if(dirname[0] == '/')
  {
    ndev = root->dev;
  }
  else
  {
    ndev = running ->cwd->dev;
  }

  //set up parent
  if(strcmp(dirname, ".")==0)
  {
    ninum = running ->cwd->ino;
  }
  else if (strcmp(dirname, "/")==0)
  {
    ninum = root-> ino;
  }
  else
  {
    ninum = getino(dirname, ndev);
  }

  //make sure new path is valid
  if(ninum == 0)
  {
    printf("Invalid new_file path\n");
    return;
  }

  //get the parent ip
  pmip = iget(ndev, ninum);

  //make sure it's a dir
  if(!(S_ISDIR(pmip->INODE.i_mode)))
  {
    printf("New file path not a directory\n");
    iput(pmip);
    return;
  }

  if(search(pmip, basename) != 0)
  {
          printf("%s already exists in %s\n", basename, dirname);
          iput(pmip);
          return;
  }

  symlinkimpl(pmip, basename, new_file);
  pmip->INODE.i_atime = time(0L);
  pmip->dirty = 1;
  iput(pmip);
}

void symlinkimpl(MINODE *pmip,const char *basename, const char *path)
{
        printf("symlinkimpl() ------\n");
        printf("symlinkimpl() --- basename = %s\n", basename);
        int i = 0, inum = ialloc(pmip->dev), pathlen = strlen(path);
        MINODE *mip = iget(pmip->dev, inum); //load INODE into and minode
        char mybuf[BLKSIZE], *temp = mybuf;
        //initialize mip->INODE as a DIR INODE
        mip->INODE.i_mode = 0xA000; //mode is REG with permissions
        mip->INODE.i_uid = running->uid;
        mip->INODE.i_gid = running->gid;
        mip->INODE.i_size = 0; //no allocation needed
        mip->INODE.i_links_count = 1;
        //should set up time
        mip->INODE.i_atime = mip->INODE.i_ctime = mip->INODE.i_mtime = time(0L);

        //copy the path name into the block
        get_block(mip->dev, mip->INODE.i_block[0], mybuf);
        i=0;
        while(temp < mybuf + pathlen)
        {
          *temp = path[i];
          i++;
          temp++;
        }
        put_block(mip->dev, mip->INODE.i_block[0], mybuf);

        mip->dirty = 1;
        iput(mip); //write INODE back to disk

        insert_dir_entry(pmip, inum, basename);
        printf("symlinkimpl() ------End\n");
}
