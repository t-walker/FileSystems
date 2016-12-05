#include "../filesystem.h"
#include <stdio.h>

// List the files in a directory
void getCommands(char* currentPath)
{
        char command[250];
        char* strs[100];
        int i = 0;
        int argv;
        printf("getCommands()---\n");

        while(1)
        {
                i=0;
                printf("TMFilesystem:");
                pwd();
                printf("$ ");
                fgets(command, 250, stdin);
                argv = parse(strip(command), " ", strs);

                // //TODO: replace this with calling the commands
                // while(i<100 && strs[i]!=NULL)
                // {
                //         printf("%s ", strs[i]);
                //         i++;
                // }

                if(strcmp(strs[0], "ls")==0)
                {
                        ls(strs[1]);
                }
                else if(strcmp(strs[0], "cd")==0)
                {
                        cd(strs[1]);
                }
                else if(strcmp(strs[0], "pwd")==0)
                {
                        pwd();
                }
                else if(strcmp(strs[0], "mkdir")==0)
                {
                        printf("about to do mkdir %s\n",strs[1]);
                        mk_dir(strs[1]);
                }
                else if(strcmp(strs[0], "chmod") == 0)
                {
                  if (argv == 3)
                  {
                    ch_mod(strs, argv);
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
