#include "../filesystem.h"
#include <stdio.h>

// List the files in a directory
void getCommands(char* currentPath)
{
        char command[250];
        char* strs[100];
        int i = 0;
        int argc;
        printf("getCommands()---\n");

        while(1)
        {
                i=0;
                printf("TMFilesystem:");
                //pwd();
                printf("$ ");
                fgets(command, 250, stdin);
                argc = parse(strip(command), " ", strs);

                if(strcmp(strs[0], "ls")==0)
                {
                        ls(strs[1]);
                }
                else if(strcmp(strs[0], "cd")==0)
                {
                        if (argc == 2)
                        {
                                cd(strs[1]);
                        }
                        else if (argc == 1)
                        {
                                cd(NULL);
                        }
                }
                else if(strcmp(strs[0], "pwd")==0)
                {
                        pwd();
                }
                else if(strcmp(strs[0], "mkdir")==0)
                {
                        mk_dir(strs[1]);
                }
                else if(strcmp(strs[0], "chmod") == 0)
                {
                        if (argc == 3)
                        {
                                ch_mod(strs, argc);
                        }
                        else
                        {
                                printf("getCommands() -- Not enough arugments");
                        }
                }
                else if(strcmp(strs[0], "touch") == 0)
                {
                        if (argc >= 2)
                        {
                                touch(strs, argc);
                        }
                        else
                        {
                                printf("getCommands() -- Not enough arugments");
                        }
                }
                else if(strcmp(strs[0], "quit") == 0)
                {
                        quit();
                }
                //creat goes here
                else if(strcmp(strs[0], "rmdir")==0)
                {
                        rm_dir(strs[1]);
                }
                else if(strcmp(strs[0], "creat")==0)
                {
                        my_creat(strs[1]);
                }
                else if(strcmp(strs[0], "rm") == 0)
                {
                        my_rm(strs[1]);
                }
                else if(strcmp(strs[0], "stat") == 0)
                {
                        my_stat(strs[1]);
                }
                else if(strcmp(strs[0], "link") == 0)
                {
                        if (argc >= 3)
                        {
                                my_link(strs[1], strs[2]);
                        }
                }
                else if(strcmp(strs[0], "symlink") == 0)
                {
                        if (argc >= 3)
                        {
                                my_symlink(strs[1], strs[2]);
                        }
                }
                else if(strcmp(strs[0], "cp") == 0)
                {
                  if (argc >= 3)
                  {
                          my_cp(strs[1], strs[2]);
                  }
                }
                else if(strcmp(strs[0], "mv") == 0)
                {
                  if (argc >= 3)
                  {
                          my_mv(strs[1], strs[2]);
                  }
                }
                else if(strcmp(strs[0], "unlink") == 0)
                {
                        my_unlink(strs[1]);
                }
                else if(strcmp(strs[0], "cat") == 0)
                {
                        if (argc == 2)
                        {
                                cat(strs[1]);
                        }
                        else
                        {
                                printf("getCommands() -- Not enough arugments");

                        }
                }
                else if(strcmp(strs[0], "open") == 0)
                {
                        if (argc == 3)
                        {

                                int fd = my_open(strs[1], atoi(strs[2]));
                                printf("getCommands() -- open's fd is: %d", fd);
                        }
                        else
                        {
                                printf("getCommands() -- Not enough arugments");

                        }
                }
                else if(strcmp(strs[0], "close") == 0)
                {
                        if (argc == 2)
                        {
                                my_close(atoi(strs[1]));
                        }
                        else
                        {
                                printf("getCommands() -- Not enough arugments");

                        }
                }
                else if(strcmp(strs[0], "read") == 0)
                {
                        if (argc == 3)
                        {
                          int length = atoi(strs[2]);

                          printf("getCommands() -- length: %d", length);

                          char buffer[BLOCK_SIZE];
                          my_read(atoi(strs[1]), &buffer, length);
                          printf("Buffer contains: %s\0", buffer);
                        }
                        else
                        {
                                printf("getCommands() -- Not enough arugments");

                        }
                }
                printf("\n");
                if(strcmp(strs[0], "quit") == 0)
                        break;

        }
        printf("getCommands()---finished\n");
}
