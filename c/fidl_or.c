/* Copyright 4/20/11 Washington University.  All Rights Reserved.
   fidl_or.c  $Revision: 1.2 $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>
main(int argc,char **argv)
{
char *out="fidl_or.4dfp.img";
int i,j,nfiles=0,vol,lenvol,SunOS_Linux,nvox;
float *temp_float,*and;
double *temp_double;
Dim_Param *dp;
Files_Struct *files;
Interfile_header *ifh;

if(argc < 3) {
    fprintf(stderr,"Usage: fid_or -files $FILES -out fidl_or.4dfp.img\n");
    fprintf(stderr,"        -files: Files to OR.\n");
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
if(!(dp=dim_param(nfiles,files->files,SunOS_Linux,0))) exit(-1);
for(i=0;i<files->nfiles;i++) {
     if(dp->number_format[i]!=(int)FLOAT_IF && dp->number_format[i]!=(int)DOUBLE_IF) {
        printf("Error: Need to add code to OR this type of stack.\n");
        exit(-1);
        }
    }
lenvol=dp->vol*dp->tdim_max;
if(!(temp_float=malloc(sizeof*temp_float*lenvol))) {
    printf("Error: Unable to malloc temp_float\n");
    exit(-1);
    }
if(!(temp_double=malloc(sizeof*temp_double*lenvol))) {
    printf("Error: Unable to malloc temp_double\n");
    exit(-1);
    }
if(!(and=malloc(sizeof*and*lenvol))) {
    printf("Error: Unable to malloc and\n");
    exit(-1);
    }
for(i=0;i<lenvol;i++) and[i]=0.;
for(i=0;i<files->nfiles;i++) {
    vol=dp->vol*dp->tdim[i];
    if(dp->number_format[i]==(int)FLOAT_IF) {
        if(!readstack(files->files[i],(float*)temp_float,sizeof(float),(size_t)vol,SunOS_Linux)) exit(-1); 
        for(j=0;j<vol;j++) temp_double[j] = temp_float[j]!=(float)UNSAMPLED_VOXEL?(double)temp_float[j]:(double)UNSAMPLED_VOXEL;
        }
    else {
        if(!readstack(files->files[i],(double*)temp_double,sizeof(double),(size_t)vol,SunOS_Linux)) exit(-1); 
        }
    for(j=0;j<vol;j++) if(fabs(temp_double[j])>(double)UNSAMPLED_VOXEL && !isnan(temp_double[j])) and[j]=1;
    }
for(i=0;i<lenvol;i++) if(and[i]) nvox++;
printf("Information: number of voxels = %d\n",nvox);

if(!(ifh=init_ifh(4,dp->xdim,dp->ydim,dp->zdim,dp->tdim_max,dp->dxdy,dp->dxdy,dp->dz,dp->bigendian[0]))) exit(-1);
min_and_max_init(&ifh->global_min,&ifh->global_max);
min_and_max_floatstack(and,lenvol,&ifh->global_min,&ifh->global_max);
if(dp->number_format[0]==(int)FLOAT_IF) {
    if(!writestack(out,and,sizeof(float),(size_t)lenvol,dp->swapbytes[0])) exit(-1); 
    }
else {
    if(!writestack(out,and,sizeof(double),(size_t)lenvol,dp->swapbytes[0])) exit(-1); 
    }
if(!write_ifh(out,ifh,0)) exit(-1);
printf("OR written to %s\n",out);
fflush(stdout);
}
