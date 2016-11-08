#!/bin/sh
gcc -m32 t1.c core.c \
  operations/mount_root.c \
  operations/cd.c \
  operations/ls.c \
