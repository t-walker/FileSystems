#include "../filesystem.h"
#include <stdio.h>

// List the files in a directory
void getCommands(char* currentPath)
{
        char command[250];
        char* strs[100];
        int i = 0;
        printf("getCommands()---\n");

        while(1)
        {
                i=0;
                printf("TMFilesystem:%s$", currentPath);
                fgets(command, 250, stdin);
                parse(strip(command), " ", strs);

                //TODO: replace this with calling the commands
                while(i<100 && strs[i]!=NULL)
                {
                        printf("%s ", strs[i]);
                        i++;
                }

                if(strcmp(strs[0], "ls")==0)
                {
                        if(strs[1] != NULL)
                        {
                                ls(strs[1]);
                        }
                        else
                        {
                                ls(currentPath);
                        }
                }
                else if(strcmp(strs[0], "cd")==0)
                {
                        if(strs[1]!= NULL)
                        {
                                cd(strs[1]);
                        }
                }

                printf("\n");
                if(strcmp(strs[0], "quit") == 0)
                        break;
        }
        printf("getCommands()---finished\n");
}
