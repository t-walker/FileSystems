#!/bin/sh
RED='\033[0;31m'
PURPLE='\033[1;35m'
CYAN='\033[1;36m'
NC='\033[0m' # No Color

echo "${CYAN}Starting compilation."
echo "${NC}Removing files."
rm filesystem
echo "Filesystem obj removed."
echo "Running gcc."
gcc -w -m32 t1.c core.c alloc_dealloc.c \
  operations/mount_root.c \
  operations/cd.c \
  operations/ls.c \
  operations/getCommands.c \
  operations/mkdir_creat.c \
  operations/pwd.c \
  operations/chmod.c \
  -o filesystem -lm
if [ -f "filesystem" ]
then
  echo "${PURPLE}Filesystem compiled successfully.${NC}"
else
  echo "${RED}Filesystem failed to compile.${NC}"
fi
