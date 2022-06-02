/* Copyright 7/5/12 Washington University.  All Rights Reserved.
   fidl_zeromean.c  $Revision: 1.3 $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>

static char rcsid[] = "$Header: /home/hannah/mcavoy/idl/clib/RCS/fidl_zeromean.c,v 1.3 2012/08/09 20:00:14 mcavoy Exp $";

main(int argc,char **argv)
{
char filename[MAXNAME],*strptr;
int i,j,k,l,m,i1,num_tc_files=0,*roi=NULL,lc_names_only=0,swapbytes=0,SunOS_Linux;
float *temp_float;
double m1; 
Interfile_header *ifh;
Dim_Param *dp;
Files_Struct *tc_files;

if(argc<2) {
    fprintf(stderr,"  -file: imgs or a conc. Zero mean not through conc.\n");
    exit(-1);
    }
for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-file") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_tc_files;
        if(num_tc_files) {
            strptr = strrchr(argv[i+1],'.');
            if(!strcmp(strptr,".conc")) {
                if(num_tc_files>1) {printf("fidlError: Only set up to handle a single conc. Abort!\n");fflush(stdout);exit(-1);}
                if(!(tc_files=read_conc(argv[++i]))) exit(-1);
                }
            else if(!strcmp(strptr,".img")){
                if(!(tc_files=get_files(num_tc_files,&argv[i+1]))) exit(-1);
                i += num_tc_files;
                }
            else {
                printf("Error: -file not conc or img. Abort!\n");fflush(stdout);exit(-1);
                }
            }
        }
    }
if((SunOS_Linux=checkOS())==-1) exit(-1);
if(!num_tc_files) {
    printf("Error: No timecourse files. Abort!\n");
    exit(-1);
    }
if(!(dp=dim_param(tc_files->nfiles,tc_files->files,SunOS_Linux,0))) exit(-1);

if(!(temp_float=malloc(sizeof*temp_float*dp->vol*dp->tdim_max))) {
    printf("Error: Unable to malloc temp_float\n");
    exit(-1);
    }
for(m=0;m<tc_files->nfiles;m++) {
    if(!readstack(tc_files->files[m],(float*)temp_float,sizeof(float),(size_t)dp->lenvol[m],SunOS_Linux)) exit(-1);
    for(j=0;j<dp->vol;j++) {
        for(i1=j,m1=0.,l=0;l<dp->tdim[m];l++,i1+=dp->vol) m1+=(double)temp_float[i1]; 
        m1/=(double)dp->tdim[m];
        for(i1=j,l=0;l<dp->tdim[m];l++,i1+=dp->vol) temp_float[i1] = (float)((double)temp_float[i1]-m1);
        }
    strcpy(filename,tc_files->files[m]);
    if(!(strptr=get_tail_sans_ext(filename))) exit(-1);
    strcat(filename,"_zeromean.4dfp.img");
    if(!writestack(filename,temp_float,sizeof(float),(size_t)dp->lenvol[m],dp->swapbytes[m])) exit(-1);
    if(!(ifh=read_ifh(tc_files->files[m],ifh))) return 0;
    min_and_max(temp_float,dp->lenvol[m],&ifh->global_min,&ifh->global_max);
    if(!write_ifh(filename,ifh,0)) exit(-1);
    printf("Output written to %s\n",filename);fflush(stdout);

    /*CHECK*/
    #if 0
    for(j=0;j<dp->vol;j++) {
        for(i1=j,m1=0.,l=0;l<dp->tdim[m];l++,i1+=dp->vol) m1+=(double)temp_float[i1];
        temp_float[j] = m1;
        }
    strcpy(filename,tc_files->files[m]);
    if(!(strptr=get_tail_sans_ext(filename))) exit(-1);
    strcat(filename,"_zerocheck.4dfp.img");
    if(!writestack(filename,temp_float,sizeof(float),(size_t)dp->vol,dp->swapbytes[m])) exit(-1);
    min_and_max(temp_float,dp->vol,&ifh->global_min,&ifh->global_max);
    ifh->dim4 = 1;
    if(!write_ifh(filename,ifh,0)) exit(-1);
    printf("Output written to %s\n",filename);fflush(stdout);
    #endif

    free_ifh(ifh,0);
    }
exit(0);
}
