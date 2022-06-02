/* Copyright 6/15/16 Washington University.  All Rights Reserved.
   fidl_flipglm.c  $Revision: 1.2 $ */
//#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fidl.h"
#include "subs_mask.h"
#include "get_atlas_param.h"
#include "get_atlas_coor.h"
#include "atlas_to_index.h"
#include "shouldiswap.h"
#include "write_glm.h"
#include "subs_util.h"
static char rcsid[] = "$Header: /home/hannah/mcavoy/idl/clib/RCS/fidl_flipglm.c,v 1.2 2016/06/21 18:15:55 mcavoy Exp $";
int main(int argc,char **argv)
{
char *glmf=NULL,*out=NULL,string[MAXNAME],*strptr,atlas[7]="";
int i,j,k,vol,SunOS_Linux,swapbytes,*xi,*yi,*zi,*index;
long startb;
float *temp_float,*temp_float2;
double *col,*row,*slice,*coor,*xd,*yd,*zd,*temp_double;
LinearModel *glm;
FILE *fp,*op;
Mask_Struct *ms;
Atlas_Param *ap;
print_version_number(rcsid,stdout);
if(argc<5){
    printf("  -glm: GLM to be flipped.\n");
    printf("  -out: Flipped GLM output name. Default is to append _FLIP and write to working directory.\n");
    exit(-1);
    }
for(i=1;i<argc;i++){
    if(!strcmp(argv[i],"-glm") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        glmf = argv[++i];
    if(!strcmp(argv[i],"-out") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        out = argv[++i];
    }
if((SunOS_Linux=checkOS())==-1)exit(-1);
if(!glmf){printf("fidlError: fidl_flipglm No -glm specified.\n");fflush(stdout);exit(-1);}
if(!out){
    strcpy(string,glmf);
    strptr=get_tail_sans_ext(string);
    if(!(out=malloc(sizeof*out*(strlen(strptr)+10)))) {
        printf("fidlError: fidl_flipglm Unable to malloc out\n");
        exit(-1);
        }
    sprintf(out,"%s_FLIP.glm",strptr);
    }
if(!strcmp(glmf,out)){
    printf("fidlError: fidl_flipglm -glmf %s -out %s Must be different. This is a very desirable safety feature.\n",glmf,out);
    exit(-1);
    }
if(!(glm=read_glm(glmf,0,SunOS_Linux))){
    printf("fidlError: fidl_flipglm reading %s  Abort!\n",glmf);
    exit(-1);
    }
startb=find_b(glm);
vol=glm->ifh->glm_xdim*glm->ifh->glm_ydim*glm->ifh->glm_zdim;
if(!(ms=get_mask_struct(glmf,vol,(int*)NULL,SunOS_Linux,glm,vol)))exit(-1);
//get_atlas(vol,atlas);
if(!(ap=get_atlas_param(atlas,glm->ifh)))exit(-1); //atlas not used
if(!(coor=malloc(sizeof*coor*ms->lenbrain*3))){
    printf("fidlError: fidl_flipglm Unable to malloc coor\n");
    exit(-1);
    }
if(!(col=malloc(sizeof*col*ms->lenbrain))){
    printf("fidlError: fidl_flipglm Unable to malloc col\n");
    exit(-1);
    }
if(!(row=malloc(sizeof*row*ms->lenbrain))){
    printf("fidlError: fidl_flipglm Unable to malloc row\n");
    exit(-1);
    }
if(!(slice=malloc(sizeof*slice*ms->lenbrain))){
    printf("fidlError: fidl_flipglm Unable to malloc slice\n");
    exit(-1);
    }
if(!(xi=malloc(sizeof*xi*ms->lenbrain))){
    printf("fidlError: fidl_flipglm Unable to malloc xi\n");
    exit(-1);
    }
if(!(yi=malloc(sizeof*yi*ms->lenbrain))){
    printf("fidlError: fidl_flipglm Unable to malloc yi\n");
    exit(-1);
    }
if(!(zi=malloc(sizeof*zi*ms->lenbrain))){
    printf("fidlError: fidl_flipglm Unable to malloc zi\n");
    exit(-1);
    }
if(!(xd=malloc(sizeof*xd*ms->lenbrain))){
    printf("fidlError: fidl_flipglm Unable to malloc xd\n");
    exit(-1);
    }
if(!(yd=malloc(sizeof*yd*ms->lenbrain))){
    printf("fidlError: fidl_flipglm Unable to malloc yd\n");
    exit(-1);
    }
if(!(zd=malloc(sizeof*zd*ms->lenbrain))){
    printf("fidlError: fidl_flipglm Unable to malloc zd\n");
    exit(-1);
    }
if(!(index=malloc(sizeof*index*ms->lenbrain))){
    printf("fidlError: fidl_flipglm Unable to malloc index\n");
    exit(-1);
    }
col_row_slice(ms->lenbrain,ms->brnidx,col,row,slice,ap);
get_atlas_coor(ms->lenbrain,col,row,slice,(double)ap->zdim,ap->center,ap->mmppix,coor);
for(j=i=0;i<ms->lenbrain;i++,j+=3)coor[j]*= -1.;
atlas_to_index2(ms->lenbrain,coor,ap,xi,yi,zi,xd,yd,zd,index);

//printf("index=");for(i=0;i<ms->lenbrain;i++)printf("%d ",index[i]);printf("\n");

if(!(temp_float=malloc(sizeof*temp_float*vol))){
    printf("fidlError: fidl_flipglm Unable to malloc temp_float\n");
    exit(-1);
    }
for(i=0;i<vol;i++)temp_float[i]=0.;
for(i=0;i<ms->lenbrain;i++)temp_float[i]=(float)UNSAMPLED_VOXEL;
if(!(temp_float2=malloc(sizeof*temp_float2*vol))){
    printf("fidlError: fidl_flipglm Unable to malloc temp_float2\n");
    exit(-1);
    }
if(glm->ATAm1vox){
    for(k=0,i=glm->ifh->glm_Mcol;i>=1;i--)k+=i;
    for(i=0;i<k;i++){
        for(j=0;j<ms->lenbrain;j++)temp_float[index[j]]=glm->ATAm1vox[i][ms->brnidx[j]];
        for(j=0;j<vol;j++)glm->ATAm1vox[i][j]=0.;
        for(j=0;j<ms->lenbrain;j++)glm->ATAm1vox[i][index[j]]=temp_float[index[j]];
        }
    }
if(glm->df){
    for(i=0;i<ms->lenbrain;i++)temp_float[index[i]]=glm->df[ms->brnidx[i]];
    for(i=0;i<vol;i++)glm->df[i]=0.;
    for(i=0;i<ms->lenbrain;i++)glm->df[index[i]]=temp_float[index[i]];
    }
if(glm->sd){
    for(i=0;i<ms->lenbrain;i++)temp_float[index[i]]=glm->sd[ms->brnidx[i]];
    for(i=0;i<vol;i++)glm->sd[i]=0.;
    for(i=0;i<ms->lenbrain;i++)glm->sd[index[i]]=temp_float[index[i]];
    }
if(glm->var){
    if(!(temp_double=malloc(sizeof*temp_double*vol))){
        printf("fidlError: fidl_flipglm Unable to malloc temp_double\n");
        exit(-1);
        }
    for(i=0;i<vol;i++)temp_double[i]=0.;
    for(i=0;i<ms->lenbrain;i++)temp_double[i]=(double)UNSAMPLED_VOXEL;
    for(i=0;i<ms->lenbrain;i++)temp_double[index[i]]=glm->var[ms->brnidx[i]];
    for(i=0;i<vol;i++)glm->var[i]=0.;
    for(i=0;i<ms->lenbrain;i++)glm->var[index[i]]=temp_double[index[i]];
    }
if(glm->ifh->glm_nF){
    if(glm->fstat){
        for(k=i=0;i<glm->ifh->glm_nF;i++,k+=vol){
            for(j=0;j<ms->lenbrain;j++)temp_float[index[j]]=glm->fstat[k+ms->brnidx[j]];
            for(j=0;j<vol;j++)glm->fstat[k+j]=0.;
            for(j=0;j<ms->lenbrain;j++)glm->fstat[k+index[j]]=temp_float[index[j]];
            }
        }
    for(k=i=0;i<glm->ifh->glm_nF;i++,k+=vol){
        for(j=0;j<ms->lenbrain;j++)temp_float[index[j]]=glm->fzstat[k+ms->brnidx[j]];
        for(j=0;j<vol;j++)glm->fzstat[k+j]=0.;
        for(j=0;j<ms->lenbrain;j++)glm->fzstat[k+index[j]]=temp_float[index[j]];
        }
    }
if(glm->grand_mean){
    for(i=0;i<ms->lenbrain;i++)temp_float[index[i]]=glm->grand_mean[ms->brnidx[i]];
    for(i=0;i<vol;i++)glm->grand_mean[i]=0.;
    for(i=0;i<ms->lenbrain;i++)glm->grand_mean[index[i]]=temp_float[index[i]];
    }
swapbytes=shouldiswap(SunOS_Linux,glm->ifh->bigendian);
if(!write_glm(out,glm,(int)WRITE_GLM_THRU_FZSTAT,vol,swapbytes)) {
    printf("fidlError: fidl_flipglm Writing WRITE_GLM_THRU_FZSTAT to %s\n",out);
    exit(-1);
    }
if(!(op=fopen_sub(out,"a")))exit(-1);
if(!(fwrite_sub(glm->grand_mean,sizeof(float),(size_t)vol,op,swapbytes))){
    printf("fidlError: fidl_flipglm Could not write grand_mean to %s.\n",out);
    exit(-1);
    }
if(!(fp=fopen_sub(glmf,"r")))exit(-1);
if(fseek(fp,startb,SEEK_SET)) {
    printf("fidlError: fidl_flipglm Occured while seeking to %ld in %s.\n",startb,glmf);
    exit(-1);
    }
for(i=0;i<glm->ifh->glm_Mcol;i++) {
    if(!fread_sub(temp_float2,sizeof*temp_float2,(size_t)vol,fp,swapbytes,glmf)){
        printf("fidlError: fidl_flipglm Reading parameter estimates from %s.\n",glmf);
        exit(-1);
        }
    for(j=0;j<ms->lenbrain;j++)temp_float[index[j]]=temp_float2[ms->brnidx[j]];
    if(!(fwrite_sub(temp_float,sizeof*temp_float,(size_t)vol,op,swapbytes))) {
        printf("fidlError: fidl_flipglm Could not write b to %s. eff=%d\n",out,i);
        exit(-1);
        }
    }
fclose(fp);fclose(op);
printf("Flipped glm written to %s\n",out);
}
