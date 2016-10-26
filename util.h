#include <string.h>

/*parse str on delim and return them in strs*/
int parse(const char* str,const char* delim, char *strs[100])
{
  int i = 0;
  char *token, *temp = strdup(str);
  if(debug) {printf("In parse str=%s\n",str);}
  token = strtok (temp, delim);
  while (token != NULL)
  {
    strs[i++] = token;
    token = strtok (NULL, delim);
  }
  strs[i] = NULL;
}
