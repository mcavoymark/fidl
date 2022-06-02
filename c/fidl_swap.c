/* Copyright 2/17/11 Washington University.  All Rights Reserved.
   fidl_swap.c  $Revision: 1.2 $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>

static char rcsid[] = "$Header: /home/hannah/mcavoy/idl/clib/RCS/fidl_swap.c,v 1.2 2011/02/18 16:38:42 mcavoy Exp $";

main(int argc,char **argv)
{
int i,j,nfiles=0,SunOS_Linux;
float *temp_float;
Dim_Param *dp;
Files_Struct *files;
Interfile_header *ifh=NULL;
if(argc < 3) {
    fprintf(stderr,"Usage: fidl_swap -files file1 file2 ...\n");
    fprintf(stderr,"    -files: Files to be byte swapped.\n");
    exit(-1);
    }
for(i=1;i<argc;i++) { 
    if(!strcmp(argv[i],"-files") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nfiles;
        if(!(files=get_files(nfiles,&argv[i+1]))) exit(-1);
        i += nfiles;
        }
    }
if(!nfiles) {
    printf("Error: No files found.\n");
    exit(-1);
    }
if((SunOS_Linux=checkOS())==-1) exit(-1);
if(!(dp=dim_param(files->nfiles,files->files,SunOS_Linux,1))) exit(-1);
if(!(temp_float=malloc(sizeof*temp_float*dp->lenvol_max))) {
    printf("Error: Unable to malloc temp_float\n");
    exit(-1);
    }
for(i=0;i<files->nfiles;i++) {
    if(!readstack(files->files[i],(float*)temp_float,sizeof(float),(size_t)dp->lenvol[i],SunOS_Linux)) exit(-1);
    if(!writestack(files->files[i],temp_float,sizeof(float),(size_t)dp->lenvol[i],dp->swapbytes[i]?0:1)) exit(-1);
    if(!(ifh=read_ifh(files->files[i],ifh))) exit(-1);
    /*ifh->bigendian = (int)fabs((double)ifh->bigendian-1.);*/
    ifh->bigendian = ifh->bigendian ? 0 : 1;
    min_and_max_init(&ifh->global_min,&ifh->global_max);
    min_and_max_floatstack(temp_float,dp->lenvol[i],&ifh->global_min,&ifh->global_max);
    if(!write_ifh(files->files[i],ifh,0)) exit(-1);
    free_ifh(ifh,1);
    printf("%s has been byteswapped.\n",files->files[i]); 
    }
}
