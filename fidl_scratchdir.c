/* Copyright 5/22/12 Washington University.  All Rights Reserved.
   fidl_scratchdir.c  $Revision: 1.5 $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*START150423*/
/*#include "fidl.h"*/

/*START150424*/
#include "make_scratchdir.h"

main(int argc,char **argv)
{
char *label="SCRATCH",*scratchdir=NULL;
int i,nopath=0;
if(argc<1) {
    fprintf(stderr," -nopath: No path.\n");
    fprintf(stderr," -label: Default label is SCRATCH.\n");
    exit(-1);
    }
for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-nopath")) nopath = 1;
    if(!strcmp(argv[i],"-label") && argc > i+1) label = argv[++i];
    }
if(!(scratchdir=make_scratchdir(nopath,label))) exit(-1);
printf("%s\n",scratchdir);
}
