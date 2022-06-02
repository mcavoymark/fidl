/* Copyright 2/4/16 Washington University.  All Rights Reserved.
   fidl_chunkstack.c  $Revision: 1.3 $ */
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
    printf("fidl_chunkstack This program was written to assemble pieces of an image from separate files into a single image.\n");
    fprintf(stderr,"  -files01:          *.conc, *.img, *.nii or *.nii.gz.  Name includes 'PROC01'.\n");
    fprintf(stderr,"  -pid:              Text files that contain the pid's.\n");
    fprintf(stderr,"  -proc:             Number of processes. Should match the number of pid's.\n");
    fprintf(stderr,"  -mask:             If files are compressed, then files are uncompressed into the mask.\n");
    fprintf(stderr,"  -glm:              First glm file. Used to determine filetype and associated parameters for writing.\n");
    fprintf(stderr,"  -threshold_extent: Monte Carlo thresholds with spatial extent.\n");
    fprintf(stderr,"                     Ex. -threshold_extent \"3.75 18\" \"4 12\"\n");
    fprintf(stderr,"  -pval:             Associated p value for -threshold_extent. .05 is the default.\n");
    fprintf(stderr,"  -directory:        Output files will be placed here.\n");
    fprintf(stderr,"  -scratchdir:       Intermediate files are written here.\n");
    //fprintf(stderr,"  -output:           Output options: Z_uncorrected Z_monte_carlo\n");
    fprintf(stderr,"  -cleanup           Delete scratchdir.\n");
    exit(-1);
    }
for(i=1;i<argc;i++){
    if(!strcmp(argv[i],"-files01") && argc > i+1){
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nfiles01;
        if(!nfiles01){
            printf("fidlError: nfiles01=%d Need to provide *.conc, *.img, *.nii or *.nii.gz after -files01\n",nfiles01);
            fflush(stdout);exit(-1);
            }
        strptr = strrchr(argv[i+1],'.');
        if(!strcmp(strptr,".conc")){
            if(!(files01=read_conc(nfiles01,&argv[i+1]))) exit(-1);
            }
        else if(!strcmp(strptr,".img")||!strcmp(strptr,".nii")||!strcmp(strptr-4,".nii.gz")){
            if(!(files01=get_files(nfiles01,&argv[i+1]))) exit(-1);
            }
        else{
            printf("fidlError: -files01 not *.conc, *.img, *.nii or *.nii.gz. Abort!\n");fflush(stdout);exit(-1);
            }
        i+=nfiles01;
        }
    if(!strcmp(argv[i],"-pid") && argc > i+1){
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++npidf;
        if(!(pidf=get_files(npidf,&argv[i+1])))exit(-1);
        i+=npidf;
        }
    if(!strcmp(argv[i],"-proc") && argc > i+1)
        proc=(int)strtol(argv[++i],NULL,10);
    if(!strcmp(argv[i],"-mask") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        mask=argv[++i];
    if(!strcmp(argv[i],"-glm") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        glmf=argv[++i];
    if(!strcmp(argv[i],"-threshold_extent") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++n_threshold_extent;
        if(!(threshold=malloc(sizeof*threshold*n_threshold_extent))) {
            printf("fidlError: Unable to malloc threshold\n");
            exit(-1);
            }
        if(!(extent=malloc(sizeof*extent*n_threshold_extent))) {
            printf("fidlError: Unable to malloc extent\n");
            exit(-1);
            }
        for(j=0;j<n_threshold_extent;j++) sscanf(argv[++i],"%lf %d",&threshold[j],&extent[j]);
        }
    if(!strcmp(argv[i],"-pval") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++npvalstr;
        if(!(pvalstr=get_files(npvalstr,&argv[i+1]))) exit(-1);
        pstr = pvalstr->files;
        i += npvalstr;
        }
    if(!strcmp(argv[i],"-directory") && argc > i+1) {
        i1=strlen(argv[i+1]);
        if(argv[i+1][i1-1]!='/')i1++;
        if(!(directory=malloc(sizeof*directory*i1))) {
            printf("fidlError: chunkstack Unable to malloc directory\n");
            exit(-1);
            }
        strncpy(directory,argv[++i],i1);
        if(directory[i1-1]=='/')directory[i1-1]=0;
        }
    if(!strcmp(argv[i],"-scratchdir") && argc > i+1) {
        i1=strlen(argv[i+1]);
        if(argv[i+1][i1-1]!='/')i1++;
        if(!(scratchdir=malloc(sizeof*scratchdir*i1))) {
            printf("fidlError: fidl_chunkstack Unable to malloc scratchdir\n");
            exit(-1);
            }
        strncpy(scratchdir,argv[++i],i1);
        if(scratchdir[i1-1]=='/')scratchdir[i1-1]=0;
        }
    //if(!strcmp(argv[i],"-output") && argc > i+1){
    //    for(j=1;i+j<argc && strchr(argv[i+j],'-') != argv[i+j]; j++){
    //        if(!strcmp(argv[i+j],"Z_uncorrected")) lc_Z_uncorrected = 1;
    //        else if(!strcmp(argv[i+j],"Z_monte_carlo")) lc_Z_monte_carlo = 1;
    //        }
    //    i+=lc_Z_uncorrected+lc_Z_monte_carlo;
    //    }
    if(!strcmp(argv[i],"-cleanup"))
        lccleanup=1;
    }
print_version_number(rcsid,stdout);
if((SunOS_Linux=checkOS())==-1) exit(-1);
if(!nfiles01){printf("fidlError: No -files01. Abort!\n");fflush(stdout);exit(-1);}

printf("nfiles01=%d files01->nfiles=%zd proc=%d\n",nfiles01,files01->nfiles,proc);fflush(stdout);

if(!npidf){if(!proc){
    printf("fidlError: fidl_chunkstack proc=%d npidf=%d\n",proc,npidf);
    printf("fidlError: Need to either specify the number of processes with -proc or provide the pid files with -pid\n");
    fflush(stdout);exit(-1);}
    }
else{
    if(!proc){proc=(int)pidf->nfiles;}else if(proc!=(int)pidf->nfiles){
        printf("fidlError: fidl_chunkstack proc=%d pidf->nfiles=%zd Must be equal.\n",proc,pidf->nfiles);fflush(stdout);exit(-1);}
    if(!(pid=malloc(sizeof*pid*pidf->nfiles))){
        printf("fidlError: fidl_chunkstack Unable to malloc pid\n");fflush(stdout);exit(-1);}
    for(i=0;i<proc;i++){
        if(!(fp=fopen_sub(pidf->files[i],"r")))exit(-1);
        if((fscanf(fp,"%d",&pid[i]))==(int)EOF){
            printf("fidlError: fidl_chunkstack %s is empty\n",pidf->files[i]);fflush(stdout);exit(-1);}
        fclose(fp);
        }
    printf("pid=");for(i=0;i<proc;i++)printf("%d ",pid[i]);printf("\n");

    #if 0
    if(!(pidfile=malloc(sizeof*pidfile*(strlen(scratchdir)+9)))){
        printf("fidlError: fidl_chunkstack Unable to malloc pidfile\n");fflush(stdout);exit(-1);}
    sprintf(pidfile,"%s/pid.txt",scratchdir);
    #endif
    //START160324
    if(!(pidfile=malloc(sizeof*pidfile*((scratchdir?strlen(scratchdir):0)+9)))){
        printf("fidlError: fidl_chunkstack Unable to malloc pidfile\n");fflush(stdout);exit(-1);}
    pidfile[0]=0;
    if(scratchdir){strcat(pidfile,scratchdir);strcat(pidfile,"/");}
    strcat(pidfile,"pid.txt");

    }
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
    if(pidcnt)sleep(300);
    }
if(!(filesl=malloc(sizeof*filesl*files01->nfiles*(size_t)proc))){
    printf("fidlError: Unable to malloc filesl\n");fflush(stdout);exit(-1);}
for(k=i=0;i<(int)files01->nfiles;i++)for(j=0;j<proc;j++,k++)filesl[k]=files01->strlen_files[i];
if(!(files=d2charvar(files01->nfiles*(size_t)proc,filesl)))exit(-1);
for(k=i=0;i<(int)files01->nfiles;i++){
    strncpy(string,files01->files[i],files01->strlen_files[i]-16);
    string[files01->strlen_files[i]-16]=0;
    for(j=0;j<proc;j++,k++)sprintf(files[k],"%sPROC%s%d.4dfp.img",string,j<9?"0":"",j+1);
    } 
if(!(dp=dim_param2(files01->nfiles*(size_t)proc,files,SunOS_Linux)))exit(-1);
if(!(ms=get_mask_struct(mask,0,(int*)NULL,SunOS_Linux,(LinearModel*)NULL,0)))exit(-1);
for(volt=i=0;i<proc;i++)volt+=dp->vol[i];
if(volt<ms->lenbrain){printf("fidlError: fidl_chunkstack volt=%d ms->lenbrain=%d volt must be equal or greater\n",volt,ms->lenbrain);
    fflush(stdout);exit(-1);}
if(!(temp_float=malloc(sizeof*temp_float*ms->lenbrain))){
    printf("fidlError: Unable to malloc temp_float\n");fflush(stdout);exit(-1);}
if(!(temp_float2=malloc(sizeof*temp_float2*ms->lenvol))){
    printf("fidlError: Unable to malloc temp_float2\n");fflush(stdout);exit(-1);}
for(i=0;i<ms->lenvol;i++)temp_float2[i]=0.;
if(n_threshold_extent){if(!(temp_double=malloc(sizeof*temp_double*ms->lenbrain))){
    printf("fidlError: Unable to malloc temp_double\n");fflush(stdout);exit(-1);}}

//if(!(ifh=init_ifh(4,ms->xdim,ms->ydim,ms->zdim,1,ms->voxel_size_1,ms->voxel_size_2,ms->voxel_size_3,0)))exit(-1);
//START161103
if(!(ifh=init_ifh(4,ms->xdim,ms->ydim,ms->zdim,1,ms->voxel_size_1,ms->voxel_size_2,ms->voxel_size_3,0,ms->center,ms->mmppix)))exit(-1);

if(glmf){
    if(!(glm=read_glm(glmf,(int)SMALL,SunOS_Linux))){
        printf("fidlError: fidl_chunkstack reading %s  Abort!\n",glmf);fflush(stdout);exit(-1);}
    filetype=get_filetype2(glm->ifh->glm_cifti_xmlsize,glm->ifh->glm_boldtype);
    if(filetype==(int)NIFTI){
        dims[0]=glm->ifh->glm_xdim;dims[1]=glm->ifh->glm_ydim;dims[2]=glm->ifh->glm_zdim;
        center[0]=glm->ifh->center[0];center[1]=glm->ifh->center[1];center[2]=glm->ifh->center[2];
        mmppix[0]=glm->ifh->mmppix[0];mmppix[1]=glm->ifh->mmppix[1];mmppix[2]=glm->ifh->mmppix[2];
        }
    else if(filetype==(int)CIFTI){
        xmlsize=glm->ifh->glm_cifti_xmlsize;
        if(!(cifti_xmldata=malloc(sizeof*cifti_xmldata*xmlsize))){
            printf("fidlError: fidl_chunkstack Unable to malloc cifti_xmldata\n");fflush(stdout);exit(-1);}
        for(ui=0;ui<xmlsize;ui++)cifti_xmldata[ui]=glm->cifti_xmldata[ui];
        }
    }
if(n_threshold_extent){
    if(!(mcmc=monte_carlo_mult_comp_init(ifh->dim1,ifh->dim2,ifh->dim3,ms->lenbrain,ms->brnidx)))exit(-1);
    get_atlas(ms->lenvol,atlas);
    if(!(ap=get_atlas_param(atlas,ifh)))exit(-1);
    }
if(!(w1=malloc(sizeof*w1))){
    printf("fidlError: fidl_chunkstack Unable to malloc w1\n");fflush(stdout);exit(-1);}
w1->cifti_xmldata=cifti_xmldata;
w1->xmlsize=xmlsize;
w1->filetype=filetype;
w1->how_many=ms->lenvol;
w1->swapbytes=0;
w1->dims=dims;
w1->temp_float=temp_float2;
w1->center=center;
w1->mmppix=mmppix;
w1->temp_double=NULL;
w1->ifh=ifh;
w1->lenbrain=ms->lenbrain;
w1->dontminmax=1;
for(k=i=0;i<(int)files01->nfiles;i++){
    min_and_max_init(&ifh->global_min,&ifh->global_max);
    for(m=j=0;j<proc;j++,k++){
        printf("Reading %s\n",files[k]);
        if(!readstack(files[k],(float*)temp_float,sizeof(float),(size_t)dp->vol[k],SunOS_Linux,dp->bigendian[k]))exit(-1);
        for(l=0;l<dp->vol[k];l++,m++)if(m<ms->lenbrain)temp_float2[ms->brnidx[m]]=temp_float[l];
        min_and_max_floatstack(temp_float,dp->vol[k],&ifh->global_min,&ifh->global_max);
        }

    //START161104
    for(j=0;j<ms->lenbrain;j++)if(temp_float2[ms->brnidx[j]]==(float)UNSAMPLED_VOXEL)temp_float2[ms->brnidx[j]]=0.;

    strptr=get_tail_sans_ext(files01->files[i]);
    *(strptr+strlen(strptr)-6)=0;
    sprintf(string2,"%s/%s%s",directory,strptr,Fileext[filetype]);
    if(!write1(string2,w1))exit(-1);
    printf("Output written to %s\n",string2);fflush(stdout);
    if(n_threshold_extent&&filetype==(int)IMG){
        for(j=0;j<ms->lenbrain;j++)temp_double[j]=temp_float2[ms->brnidx[j]]==(float)UNSAMPLED_VOXEL?(double)UNSAMPLED_VOXEL:
            (double)temp_float2[ms->brnidx[j]];
        strptr2=strtok_r(strptr,"_zstat",&strptr);
        sprintf(string,"%s/%s",directory,strptr2);
        if(!monte_carlo_mult_comp(temp_double,threshold,extent,n_threshold_extent,string2,ifh,0,temp_double,string,
            0,pstr,(double*)NULL,(char*)NULL,mcmc,ap))exit(-1);
        }
    }
if(lccleanup){
    if(scratchdir){
        sprintf(string,"rm -rf %s",scratchdir);
        if(system(string)==-1)printf("fidlError: fidl_chunkstack Unable to %s\n",string);
        }
    else{
        sprintf(string,"rm -rf %s",pidfile);
        if(system(string)==-1)printf("fidlError: fidl_chunkstack Unable to %s\n",string);
        }
    }

}
