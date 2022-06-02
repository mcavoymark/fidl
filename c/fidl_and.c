/* Copyright 12/31/99 Washington University.  All Rights Reserved.
   fidl_and.c  $Revision: 1.5 $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "fidl.h"
#include "dim_param2.h"
#include "subs_util.h"
#include "minmax.h"
int main(int argc,char **argv)
{
char *out="fidl_and.4dfp.img";
int i,j,nfiles=0,vol,SunOS_Linux,nvox; //lenvol
float *temp_float,*and;
//double *temp_double;

//Dim_Param *dp;
//START151002
Dim_Param2 *dp;

Files_Struct *files=NULL;
Interfile_header *ifh;
if(argc < 3) {
    fprintf(stderr,"Usage: fid_and -files $FILES -out fid_and.4dfp.img\n");
    fprintf(stderr,"        -files: Files to AND.\n");
    fprintf(stderr,"        -out:   Output image name.\n");
    exit(-1);
    }
for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-files") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nfiles;
        if(!(files=get_files(nfiles,&argv[i+1]))) exit(-1);
        i += nfiles;
        }
    if(!strcmp(argv[i],"-out") && argc > i+1)
        out = argv[++i];
    }
if(!nfiles) {
    printf("Error: Need to specify -files\n");
    exit(-1);
    }
if((SunOS_Linux=checkOS())==-1) exit(-1);

#if 0
if(!(dp=dim_param(nfiles,files->files,SunOS_Linux))) exit(-1);
for(i=0;i<files->nfiles;i++) {
     if(dp->number_format[i]!=(int)FLOAT_IF && dp->number_format[i]!=(int)DOUBLE_IF) {
        printf("Error: Need to add code to average this type of stack.\n");
        exit(-1);
        }
    }
lenvol=dp->vol*dp->tdim_max;
#endif
/*START151002*/
if(!(dp=dim_param2(files->nfiles,files->files,SunOS_Linux)))exit(-1);
if(dp->volall==-1){printf("fidlError: fidl_and All files must be the same size. Abort!\n");fflush(stdout);exit(-1);}

if(!(temp_float=malloc(sizeof*temp_float*dp->lenvol_max))) {
    printf("Error: Unable to malloc temp_float\n");
    exit(-1);
    }

//START151002
#if 0
if(!(temp_double=malloc(sizeof*temp_double*dp->lenvol_max))) {
    printf("Error: Unable to malloc temp_double\n");
    exit(-1);
    }
#endif

if(!(and=malloc(sizeof*and*dp->lenvol_max))) {
    printf("Error: Unable to malloc and\n");
    exit(-1);
    }
for(i=0;i<dp->lenvol_max;i++) and[i]=0.;

//for(i=0;i<files->nfiles;i++) {
//START151002
for(i=0;i<(int)files->nfiles;i++) {

    #if 0
    vol=dp->vol*dp->tdim[i];
    if(dp->number_format[i]==(int)FLOAT_IF) {
        if(!readstack(files->files[i],(float*)temp_float,sizeof(float),(size_t)vol,SunOS_Linux)) exit(-1); 
        for(j=0;j<vol;j++) temp_double[j] = temp_float[j]!=(float)UNSAMPLED_VOXEL?(double)temp_float[j]:(double)UNSAMPLED_VOXEL;
        }
    else {
        if(!readstack(files->files[i],(double*)temp_double,sizeof(double),(size_t)vol,SunOS_Linux)) exit(-1); 
        }
    for(j=0;j<vol;j++) temp_double[j] = temp_float[j]!=(float)UNSAMPLED_VOXEL?(double)temp_float[j]:(double)UNSAMPLED_VOXEL;
    for(j=0;j<vol;j++) if(fabs(temp_double[j])>(double)UNSAMPLED_VOXEL && !isnan(temp_double[j])) and[j]++;
    #endif
    /*START151002*/
    vol=dp->vol[i]*dp->tdim[i];
    if(!readstack(files->files[i],(float*)temp_float,sizeof(float),(size_t)vol,SunOS_Linux,dp->bigendian[i]))exit(-1); 
    for(j=0;j<vol;j++) if(fabs(temp_float[j])>(float)UNSAMPLED_VOXEL && !isnan(temp_float[j]))and[j]++;

    }

//for(i=0;i<dp->lenvol_max;i++)if((and[i]=(int)and[i]==files->nfiles?1.:0.))nvox++;
//START151002
for(nvox=i=0;i<dp->lenvol_max;i++)if((and[i]=(int)and[i]==(int)files->nfiles?1.:0.))nvox++;
    
printf("fidlInfo: number of voxels = %d\n",nvox);

//if(!(ifh=init_ifh(4,dp->xdim,dp->ydim,dp->zdim,dp->tdim_max,dp->dxdy,dp->dxdy,dp->dz,dp->bigendian[0]))) exit(-1);
//START151002
//if(!(ifh=init_ifh(4,dp->xdim,dp->ydim,dp->zdim,dp->tdim_max,dp->dxdy,dp->dxdy,dp->dz,SunOS_Linux?0:1))) exit(-1);
if(!(ifh=read_ifh(files->files[0],(Interfile_header*)NULL)))exit(-1);
ifh->dim4=dp->tdim_max;
ifh->bigendian=SunOS_Linux?0:1;

min_and_max_init(&ifh->global_min,&ifh->global_max);

//min_and_max_floatstack(and,lenvol,&ifh->global_min,&ifh->global_max);
//START151002
min_and_max_floatstack(and,dp->lenvol_max,&ifh->global_min,&ifh->global_max);

#if 0
if(dp->number_format[0]==(int)FLOAT_IF) {
    if(!writestack(out,and,sizeof(float),(size_t)lenvol,dp->swapbytes[0])) exit(-1); 
    }
else {
    if(!writestack(out,and,sizeof(double),(size_t)lenvol,dp->swapbytes[0])) exit(-1); 
    }
#endif
/*START151002*/
if(!writestack(out,and,sizeof(float),(size_t)dp->lenvol_max,0)) exit(-1); 


if(!write_ifh(out,ifh,0)) exit(-1);
printf("AND written to %s\n",out);
fflush(stdout);
}
