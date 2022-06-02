/* Copyright 8/18/08 Washington University.  All Rights Reserved.
   fidl_collate.c  $Revision: 1.3 $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>

static char rcsid[] = "$Header: /home/hannah/mcavoy/idl/clib/RCS/fidl_collate.c,v 1.3 2009/02/25 20:16:16 mcavoy Exp $";

main(int argc,char **argv)
{
char *out="fidl_collate.txt",*scratchdir=NULL,string[MAXNAME];
int i,j,k,l,nfiles=0,nnames=0,nregions=0,nframes,size,SunOS_Linux,lccleanup=0;
float *temp_float,*fptr;
double *temp_double;
FILE *fp;
Files_Struct *files,*names,*regions;
Dim_Param *dp;

if(argc < 9) {
    fprintf(stderr,"    -files:      One for each subject.\n"); 
    fprintf(stderr,"    -names:      Factor level names.\n"); 
    fprintf(stderr,"    -regions:    Name of each region.\n"); 
    fprintf(stderr,"    -nframes:    Single number applied to all timecourses.\n"); 
    fprintf(stderr,"    -out:        Name of output file. Default is fidl_collate.txt\n"); 
    fprintf(stderr,"    -scratchdir: Location of precomputed scratchfiles. Include the backslash at the end.\n");
    fprintf(stderr,"    -clean_up    Delete scratch directory.\n");
    exit(-1);
    }
for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-files") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nfiles;
        if(!(files=get_files(nfiles,&argv[i+1]))) exit(-1);
        i += nfiles;
        }
    if(!strcmp(argv[i],"-names") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nnames;
        if(!(names=get_files(nnames,&argv[i+1]))) exit(-1);
        i += nnames;
        }
    if(!strcmp(argv[i],"-regions") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nregions;
        if(!(regions=get_files(nregions,&argv[i+1]))) exit(-1);
        i += nregions;
        }
    if(!strcmp(argv[i],"-nframes") && argc > i+1)
        nframes = atoi(argv[++i]);
    if(!strcmp(argv[i],"-out") && argc > i+1)
        out = argv[++i];
    if(!strcmp(argv[i],"-scratchdir") && argc > i+1)
        scratchdir = argv[++i];
    if(!strcmp(argv[i],"-clean_up"))
        lccleanup = 1;
    }
if((SunOS_Linux=checkOS())==-1) exit(-1);
if(!(dp=dim_param(nfiles,files->files,SunOS_Linux))) exit(-1);
if(dp->vol!=(size=nnames*nregions*nframes)) {
    printf("Error: dp->vol=%d size=%d Must be the same. Abort!\n",dp->vol,size);
    exit(-1);
    }
if(!(temp_double=malloc(sizeof*temp_double*dp->vol))) {
    printf("Error: Unable to malloc temp_double\n");
    exit(-1);
    }
for(i=0;i<dp->vol;i++) temp_double[i]=0.;
if(!(temp_float=malloc(sizeof*temp_float*nfiles*dp->vol))) {
    printf("Error: Unable to malloc temp_float\n");
    exit(-1);
    }
for(fptr=temp_float,i=0;i<nfiles;i++) {
    if(!readstack(files->files[i],(float*)fptr,sizeof(float),(size_t)dp->vol,SunOS_Linux)) exit(-1);
    for(j=0;j<dp->vol;j++) temp_double[j]+=(double)(*fptr++);
    }

/*for(j=0;j<dp->vol;j++) temp_double[j]/=(double)dp->vol;*/
/*START53 090225*/
for(j=0;j<dp->vol;j++) temp_double[j]/=(double)nfiles;

if(!(fp=fopen_sub(out,"w"))) exit(-1);
for(l=i=0;i<nnames;i++) {
    fprintf(fp,"%s",names->files[i]);
    for(j=0;j<nregions;j++) fprintf(fp,"\t%s",regions->files[j]);
    fprintf(fp,"\n");
    for(j=0;j<nframes;j++) {
        fprintf(fp,"%d",j+1);
        for(k=0;k<nregions;k++,l++) fprintf(fp,"\t%.4f",temp_double[l]);
        fprintf(fp,"\n");
        }
    fprintf(fp,"\n");
    }
fclose(fp);
printf("Averages written %s\n",out);
if(lccleanup&&scratchdir) {
    sprintf(string,"rm -rf %s",scratchdir);
    if(system(string) == -1) printf("Error: unable to %s\n",string);
    }
}
