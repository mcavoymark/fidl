/* Copyright 4/6/11 Washington University.  All Rights Reserved.
   fidl_timestr.c  $Revision: 1.2 $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fidl.h>
main(int argc,char **argv)
{
char timestr[16];
printf("%s\n",make_timestr(timestr));
}
