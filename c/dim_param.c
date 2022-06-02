/* Copyright 7/14/15 Washington University.  All Rights Reserved.
   dim_param.c  $Revision: 1.3 $*/
#include <stdlib.h>
#include "dim_param.h"
#include "shouldiswap.h"
Dim_Param *dim_param(int nfiles,char **files,int SunOS_Linux,int novolcheck,int *xdim,int *tdim){
    size_t i;
    int vol,dim4;
    Dim_Param *dp;
    Interfile_header *ifh=NULL;
    if(!(dp=malloc(sizeof*dp))) {
        printf("fidlError: Unable to malloc dp\n");
        return NULL;
        }
    if(!(dp->tdim=malloc(sizeof*dp->tdim*nfiles))) {
        printf("fidlError: Unable to malloc dp->tdim\n");
        return NULL;
        }
    if(!xdim) {
        if(!(dp->bigendian=malloc(sizeof*dp->bigendian*nfiles))) {
            printf("fidlError: Unable to malloc dp->bigendian\n");
            return NULL;
            }
        if(!(dp->swapbytes=malloc(sizeof*dp->swapbytes*nfiles))) {
            printf("fidlError: Unable to malloc dp->swapbytes\n");
            return NULL;
            }
        if(!(dp->number_format=malloc(sizeof*dp->number_format*nfiles))) {
            printf("fidlError: Unable to malloc dp->number_format\n");
            return NULL;
            }
        if(!(dp->byte=malloc(sizeof*dp->byte*nfiles))) {
            printf("fidlError: Unable to malloc dp->byte\n");
            return NULL;
            }
        }
    else {
        dp->bigendian=NULL;
        dp->swapbytes=NULL;
        dp->number_format=NULL;
        dp->byte=NULL;
        }
    if(!(dp->lenvol=malloc(sizeof*dp->lenvol*nfiles))) {
        printf("fidlError: Unable to malloc dp->lenvol\n");
        return NULL;
        }
    dp->nfiles = nfiles;
    for(dp->all_tdim_same=1,dp->tdim_min=10000,dp->lenvol_max=dp->tdim_max=dp->tdim_total=0,i=0;i<(size_t)nfiles && dp;i++) {
        if(!xdim) {
            if(!(ifh=read_ifh(files[i],ifh))) return NULL; 
            dp->bigendian[i] = ifh->bigendian;
            dp->swapbytes[i] = shouldiswap(SunOS_Linux,ifh->bigendian);
            dp->number_format[i] = ifh->number_format;
            if(ifh->number_format==(int)FLOAT_IF) dp->byte[i]=sizeof(float);
            else if(ifh->number_format==(int)DOUBLE_IF) dp->byte[i]=sizeof(double);
            else {printf("fidlError: Unknown number format for %s in dim_param\n",files[i]);return NULL;}
            }
        dim4 = !tdim ? ifh->dim4 : tdim[i];
        dp->tdim[i] = dim4 > 1 ? dim4 : 1;
        if(dim4 > dp->tdim_max) dp->tdim_max = dim4;
        if(dim4 < dp->tdim_min) dp->tdim_min = dim4;
        vol = !xdim ? ifh->dim1*ifh->dim2*ifh->dim3 : xdim[i];
        if((dp->lenvol[i]=vol*dim4)>dp->lenvol_max) dp->lenvol_max = dp->lenvol[i];
        dp->tdim_total += dp->tdim[i];
        if(!i) {
            dp->vol = vol;
            dp->xdim = !xdim ? ifh->dim1 : vol;
            dp->ydim = !xdim ? ifh->dim2 : 1;
            dp->zdim = !xdim ? ifh->dim3 : 1;
            if(!xdim) {
                dp->dxdy = ifh->voxel_size_1;
                dp->dz = ifh->voxel_size_3;
                dp->orientation = ifh->orientation;
                }
            }
        else if(!novolcheck&&(vol!=dp->vol)) {
            printf("fidlError: %s has a vol of %d. It needs to to have a vol of %d.\n",files[i],vol,dp->vol);
            return NULL;
            }
        if(dp->tdim[i]!=dp->tdim[0]) dp->all_tdim_same=0;
        if(!xdim) free_ifh(ifh,1);
        }
    if(ifh)free(ifh);
    return dp;
    }
void free_dim_param(Dim_Param *dp){
    if(dp->lenvol)free(dp->lenvol);
    if(dp->byte)free(dp->byte);
    if(dp->number_format)free(dp->number_format);
    if(dp->swapbytes)free(dp->swapbytes);
    if(dp->bigendian)free(dp->bigendian);
    if(dp->tdim)free(dp->tdim);
    free(dp);
    }
