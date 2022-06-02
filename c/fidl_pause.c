/* Copyright 10/30/18 Washington University.  All Rights Reserved.
   fidl_pause.c  $Revision: 1.1 $ */
#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "fidl.h"
#include "subs_mask.h"
#include "dim_param2.h"
#include "write1.h"
#include "filetype.h"
#include "minmax.h"
#include "subs_util.h"
#include "monte_carlo_mult_comp.h"
static char rcsid[] = "$Header: /home/hannah/mcavoy/idl/clib/RCS/fidl_chunkstack.c,v 1.3 2016/11/14 20:51:55 mcavoy Exp $";
int main(int argc,char **argv)
{
char *mask=NULL,*strptr,string[MAXNAME],**pstr=NULL,*directory=NULL,*pidfile=NULL,*glmf=NULL,**files,*cifti_xmldata=NULL,
    *scratchdir=NULL,atlas[7]="",string2[MAXNAME],*strptr2;
int i,j,k,l,m,lccleanup=0,SunOS_Linux,npidf=0,proc=0,nfiles01=0,n_threshold_extent=0,*extent=NULL,npvalstr=0,
    *pid=NULL,*pidrun,pidcnt,pidval,filetype=IMG,volt;  //,lc_Z_uncorrected=0,lc_Z_monte_carlo=0;
size_t i1,*filesl;
float *temp_float,*temp_float2,center[3],mmppix[3];
double *threshold=NULL,*temp_double=NULL;
Files_Struct *pidf=NULL,*files01=NULL,*pvalstr;
Mask_Struct *ms=NULL;
Dim_Param2 *dp;
void *fp=NULL;
Interfile_header *ifh=NULL;
LinearModel *glm;
W1 *w1=NULL;
int64_t dims[3];
unsigned int ui,xmlsize=0;
Mcmc *mcmc=NULL;
Atlas_Param *ap=NULL;
if(argc<5){
    printf("fidl_pause Pause for pids.\n");
    printf("  -pid: Text files that contain the pid's.\n");
    exit(-1);
    }
for(i=1;i<argc;i++){
    if(!strcmp(argv[i],"-pid") && argc > i+1){
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++npidf;
        if(!(pidf=get_files(npidf,&argv[i+1])))exit(-1);
        i+=npidf;
        }
    }
print_version_number(rcsid,stdout);
if((SunOS_Linux=checkOS())==-1) exit(-1);
if(!npidf){
    printf("fidlError: fidl_pause npidf=%d\n",npidf);
    printf("fidlError: Need to specify the pid files with -pid\n");
    fflush(stdout);exit(-1);}
    }
if(!(pid=malloc(sizeof*pid*pidf->nfiles))){
    printf("fidlError: fidl_pause Unable to malloc pid\n");fflush(stdout);exit(-1);}
for(i=0;i<pidf->nfiles);i++){
    if(!(fp=fopen_sub(pidf->files[i],"r")))exit(-1);
    if((fscanf(fp,"%d",&pid[i]))==(int)EOF){
        printf("fidlError: fidl_pause %s is empty\n",pidf->files[i]);fflush(stdout);exit(-1);}
    fclose(fp);
    }
printf("pid=");for(i=0;i<proc;i++)printf("%d ",pid[i]);printf("\n");

if(!(pidfile=malloc(sizeof*pidfile*((scratchdir?strlen(scratchdir):0)+9)))){
    printf("fidlError: fidl_chunkstack Unable to malloc pidfile\n");fflush(stdout);exit(-1);}
pidfile[0]=0;
if(scratchdir){strcat(pidfile,scratchdir);strcat(pidfile,"/");}
strcat(pidfile,"pid.txt");

if(!(pidrun=malloc(sizeof*pidrun*proc))){
    printf("fidlError: fidl_chunkstack Unable to malloc pidrun\n");fflush(stdout);exit(-1);}
for(i=0;i<proc;i++)pidrun[i]=!npidf?1:0;
for(pidcnt=!npidf?0:proc;pidcnt>0;){
    for(i=0;i<proc;i++){
        if(!pidrun[i]){
            sprintf(string,"ps -o pid= -p %d > %s",pid[i],pidfile);
            if(system(string)==-1)printf("fidlError: fidl_chunkstack Unable to %s\n",string);
            if(!(fp=fopen_sub(pidfile,"r")))exit(-1);
            if((fscanf(fp,"%d",&pidval))==(int)EOF){pidrun[i]=1;pidcnt--;}
            }
        }
    //if(pidcnt)sleep(300);
    }
}
