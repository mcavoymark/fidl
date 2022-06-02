/* Copyright 5/5/10 Washington University.  All Rights Reserved.
   fidl_trend.c  $Revision: 1.2 $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>

static char rcsid[] = "$Header: /home/hannah/mcavoy/idl/clib/RCS/fidl_trend.c,v 1.2 2010/05/10 23:04:32 mcavoy Exp $";

main(int argc,char **argv)
{
char *strptr,*out_file="fidl_trend.dat"; 
int i,j,k,SunOS_Linux,nfiles,num_tc_files=0,num_exclude_frames=0,*exclude_framesi,*valid_frms,*iframes;
double *out;
Files_Struct *tc_files;
Dim_Param *dp;
FILE *fp;
print_version_number(rcsid,stderr);
if(argc < 3) {
    fprintf(stderr,"    -tc_files:            imgs or a single conc\n"); 
    fprintf(stderr,"    -exclude_frames:      Frames to exclude from the analysis. First frame is 1.\n");
    fprintf(stderr,"                          If not specified then all frames are included.\n");
    fprintf(stderr,"    -out:                 Output filename. Default is fidl_trend.dat\n");
    exit(-1);
    }
for(i=1;i<argc;i++) { 
    if(!strcmp(argv[i],"-tc_files") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_tc_files;
        strptr = strrchr(argv[i+1],'.');
        if(!strcmp(strptr,".conc")) {
            if(!(tc_files=read_conc(argv[i+1]))) return 0;
            }
        else if(!strcmp(strptr,".img")){
            if(!(tc_files=get_files(num_tc_files,&argv[i+1]))) exit(-1);
            }
        else {
            printf("Error: -tc_files not conc or img. Abort!\n");
            exit(-1);
            }
        i += num_tc_files;
        }
    if(!strcmp(argv[i],"-exclude_frames") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_exclude_frames;
        if(!(exclude_framesi=malloc(sizeof*exclude_framesi*num_exclude_frames))) {
            printf("Error: Unable to malloc exclude_framesi\n");
            exit(-1);
            }
        for(j=0;j<num_exclude_frames;j++) exclude_framesi[j] = atoi(argv[++i]) - 1;
        }
    if(!strcmp(argv[i],"-out") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        out_file = argv[++i];
    }
if((SunOS_Linux=checkOS())==-1) exit(-1);
if(!(dp=dim_param(tc_files->nfiles,tc_files->files,SunOS_Linux))) exit(-1);
if(!(valid_frms=malloc(sizeof*valid_frms*dp->tdim_total))) {
    printf("Error: Unable to malloc valid_frms\n");
    exit(-1);
    }
for(i=0;i<dp->tdim_total;i++) valid_frms[i]=1;
for(i=0;i<num_exclude_frames;i++) valid_frms[exclude_framesi[i]]=0;
if(!(out=malloc(sizeof*out*dp->tdim_total))) {
    printf("Error: Unable to malloc out\n");
    exit(-1);
    }
if(!(iframes=malloc(sizeof*iframes*dp->tdim_max))) {
    printf("Error: Unable to malloc iframes\n");
    exit(-1);
    }
trend(dp,valid_frms,out,iframes);
if(!(fp=fopen_sub(out_file,"w"))) return 0;
fprintf(fp,"trend\n");
for(i=0;i<dp->tdim_total;i++) fprintf(fp,"%f\n",out[i]);
fclose(fp);
printf("Output written to %s\n",out_file);
}
