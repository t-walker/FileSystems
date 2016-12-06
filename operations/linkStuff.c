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
      //omip = root;
      odev = root->dev;
    }
    else
    {
      //omip = running->cwd;
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
    printf("creat() ---dirname: %s\n", dirname);
    printf("creat() --basename: %s\n", basename);

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
