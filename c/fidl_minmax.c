/* Copyright 1/10/05 Washington University.  All Rights Reserved.
   fidl_minmax.c  $Revision: 1.4 $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>

static char rcsid[] = "$Header: /home/hannah/mcavoy/idl/clib/RCS/fidl_minmax.c,v 1.4 2007/10/16 19:02:50 mcavoy Exp $";

main(int argc,char **argv)
{
char *minname=NULL,*maxname=NULL;
int i,j,nfiles=0,SunOS_Linux,swapbytes;
float *temp_float,*min,*max;
Dim_Param *dp;
Interfile_header *ifh;
Files_Struct *files;

if(argc < 5) {
    fprintf(stderr,"  -files: 4dfp files.\n");
    fprintf(stderr,"  -min:   Minimum value across files to be written to this file.\n");
    fprintf(stderr,"  -max:   Maximum value across files to be written to this file.\n");
    exit(-1);
    }
for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-files") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nfiles;
        if(!(files=get_files(nfiles,&argv[i+1]))) exit(-1);
        i += nfiles;
        }
    if(!strcmp(argv[i],"-min") && argc > i+1)
        minname = argv[++i];
    if(!strcmp(argv[i],"-max") && argc > i+1)
        maxname = argv[++i];
    }
if(!nfiles) {
    printf("Error: No files specified. Please use -files option.\n");
    exit(-1);
    }
if(!minname && !maxname) {
    printf("Error: Need to specify output names with either -min or -max or both.\n");
    exit(-1);
    }
if(!print_version_number(rcsid,stderr)) exit(-1);
if((SunOS_Linux=checkOS())==-1) exit(-1);
if(!(dp=dim_param(nfiles,files->files,SunOS_Linux))) exit(-1);

if(!(temp_float=malloc(sizeof*temp_float*dp->vol))) {
    printf("Error: Unable to malloc temp_float\n");
    exit(-1);
    }
if(!(min=malloc(sizeof*min*dp->vol))) {
    printf("Error: Unable to malloc min\n");
    exit(-1);
    }
if(!(max=malloc(sizeof*max*dp->vol))) {
    printf("Error: Unable to malloc max\n");
    exit(-1);
    }
for(i=0;i<dp->vol;i++) {
    min[i] = 1.e20;
    max[i] = -1.e20;
    }
for(i=0;i<nfiles;i++) {
    if(!readstack(files->files[i],(float*)temp_float,sizeof(float),(size_t)dp->vol,SunOS_Linux)) exit(-1);
    for(j=0;j<dp->vol;j++) {
        if(temp_float[j] > max[j]) max[j] = temp_float[j];
        if(temp_float[j] < min[j]) min[j] = temp_float[j];
        }
    }
if(!(ifh=read_ifh(files->files[0]))) exit(-1);
swapbytes=shouldiswap(SunOS_Linux,ifh->bigendian);
if(minname) {
    if(!writestack(minname,min,sizeof(float),(size_t)dp->vol,swapbytes)) exit(-1);
    if(!write_ifh(minname,ifh,(int)FALSE)) exit(-1);
    printf("Minimum written to %s\n",minname);
    }
if(maxname) {
    if(!writestack(maxname,max,sizeof(float),(size_t)dp->vol,swapbytes)) exit(-1);
    if(!write_ifh(maxname,ifh,(int)FALSE)) exit(-1);
    printf("Maximum written to %s\n",maxname);
    }
}
