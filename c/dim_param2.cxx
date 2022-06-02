/* Copyright 1/10/19 Washington University.  All Rights Reserved.
   dim_param2.cxx  $Revision: 1.1 $*/
//#include <stdint.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <algorithm>

#include "filetype.h"
#include "dim_param2.h"
#include "shouldiswap.h"
#include "nifti_getDimensions.h"
#include "cifti_getDimensions.h"
#include "filetype.h"
#include "header.h"
#include "d2float.h"
#include "d2int.h"

//START190805
#include "checkOS.h"

//Dim_Param2 *dim_param2(size_t nfiles,char **files,int SunOS_Linux){
//START190801
Dim_Param2 *dim_param2(size_t nfiles,char **files){

    size_t i;
    
    //START210422
    //int lenvol;
    
    Dim_Param2 *dp;

    if(!(dp=(Dim_Param2*)malloc(sizeof*dp))){
        printf("fidlError: dim_param2 Unable to malloc dp\n");
        return NULL;
        }
    
    //START190801
    if((dp->SunOS_Linux=checkOS())==-1)return NULL;

    if(!(dp->filetype=(int*)malloc(sizeof*dp->filetype*nfiles))){
        printf("fidlError: dim_param2 Unable to malloc dp->filetype\n");
        return NULL;
        }
    for(i=0;i<nfiles;i++)if(!(dp->filetype[i]=get_filetype(files[i])))return NULL;
    if(!(dp->vol=(int*)malloc(sizeof*dp->vol*nfiles))){
        printf("fidlError: dim_param2 Unable to malloc dp->vol\n");
        return NULL;
        }
    if(!(dp->tdim=(int*)malloc(sizeof*dp->tdim*nfiles))){
        printf("fidlError: dim_param2 Unable to malloc dp->tdim\n");
        return NULL;
        }
    for(dp->filetypeall=dp->filetype[0],i=1;i<nfiles;i++)if(dp->filetype[i]!=dp->filetypeall){dp->filetypeall=-1;break;}
    if(!(dp->swapbytes=(int*)malloc(sizeof*dp->swapbytes*nfiles))){
        printf("fidlError: dim_param2 Unable to malloc dp->swapbytes\n");
        return NULL;
        }
    if(!(dp->bigendian=(int*)malloc(sizeof*dp->bigendian*nfiles))) {
        printf("fidlError: dim_param2 Unable to malloc dp->bigendian\n");
        return NULL;
        }
    if(!(dp->xdim=(int*)malloc(sizeof*dp->xdim*nfiles))){
        printf("fidlError: dim_param2 Unable to malloc dp->xdim\n");
        return NULL;
        }
    if(!(dp->ydim=(int*)malloc(sizeof*dp->ydim*nfiles))){
        printf("fidlError: dim_param2 Unable to malloc dp->ydim\n");
        return NULL;
        }
    if(!(dp->zdim=(int*)malloc(sizeof*dp->zdim*nfiles))){
        printf("fidlError: dim_param2 Unable to malloc dp->zdim\n");
        return NULL;
        }
    if(!(dp->dx=(float*)malloc(sizeof*dp->dx*nfiles))){
        printf("fidlError: dim_param2 Unable to malloc dp->dx\n");
        return NULL;
        }
    if(!(dp->dy=(float*)malloc(sizeof*dp->dy*nfiles))){
        printf("fidlError: dim_param2 Unable to malloc dp->dy\n");
        return NULL;
        }
    if(!(dp->dz=(float*)malloc(sizeof*dp->dz*nfiles))){
        printf("fidlError: dim_param2 Unable to malloc dp->dz\n");
        return NULL;
        }
    if(!(dp->orientation=(int*)malloc(sizeof*dp->orientation*nfiles))){
        printf("fidlError: dim_param2 Unable to malloc dp->orientation\n");
        return NULL;
        }

    //START190322
    if(!(dp->number_format=(int*)malloc(sizeof*dp->number_format*nfiles))){
        printf("fidlError: dim_param2 Unable to malloc dp->number_format\n");
        return NULL;
        }

    if(!(dp->centerf=d2float(nfiles,3)))return NULL;
    if(!(dp->mmppixf=d2float(nfiles,3)))return NULL;
    if(!(dp->c_orient=d2int(nfiles,3)))return NULL;
    header h0;
    for(dp->tdim_max=dp->lenvol_max=dp->tdim_total=0,i=0;i<nfiles;i++){


        if(!(dp->vol[i]=h0.header0(files[i])))return NULL;


        //h0.assign(dp->xdim[i],dp->ydim[i],dp->zdim[i],dp->tdim[i],dp->dx[i],dp->dy[i],dp->dz[i],dp->centerf[i],dp->mmppixf[i],
        //    dp->bigendian[i],dp->orientation[i],dp->c_orient[i]);
        //START190322
        //h0.assign(dp->xdim[i],dp->ydim[i],dp->zdim[i],dp->tdim[i],dp->dx[i],dp->dy[i],dp->dz[i],dp->centerf[i],dp->mmppixf[i],
        //    dp->bigendian[i],dp->orientation[i],dp->c_orient[i],dp->number_format[i]);
        //START210422
        dp->xdim[i]=h0.dim[0];dp->ydim[i]=h0.dim[1];dp->zdim[i]=h0.dim[2];dp->tdim[i]=h0.dim[3];
        dp->dx[i]=h0.voxel_size[0];dp->dy[i]=h0.voxel_size[1];dp->dz[i]=h0.voxel_size[2];
        for(int j=0;j<3;++j){dp->centerf[i][j]=h0.centerf[j];dp->mmppixf[i][j]=h0.mmppixf[j];dp->c_orient[i][j]=h0.c_orient[j];}
        dp->bigendian[i]=h0.bigendian;dp->orientation[i]=h0.orientation;dp->number_format[i]=h0.number_format; 

#if 0
            char *file;
            int dim[4],vol,filetype,bigendian,c_orient[3],number_format,orientation;
            int16_t datatype;
            int64_t dim64[4];
            float mmppixf[3],centerf[3],voxel_size[3];
#endif

        //if(dp->filetype[i]==(int)IMG)dp->swapbytes[i]=shouldiswap(SunOS_Linux,ifh->bigendian);
        //START190212
        //if(dp->filetype[i]==(int)IMG)dp->swapbytes[i]=shouldiswap(SunOS_Linux,dp->bigendian[i]);
        //START190805
        if(dp->filetype[i]==(int)IMG)dp->swapbytes[i]=shouldiswap(dp->SunOS_Linux,dp->bigendian[i]);

        //if((lenvol=dp->vol[i]*dp->tdim[i])>dp->lenvol_max)dp->lenvol_max=lenvol;
        //std::cout<<"here100 dp->vol["<<i<<"]="<<dp->vol[i]<<" dp->tdim["<<i<<"]="<<dp->tdim[i]<<" lenvol="<<lenvol<<std::endl;
        //if(dp->tdim[i]>dp->tdim_max)dp->tdim_max=dp->tdim[i];
        //START210422
        dp->lenvol_max=std::max(dp->lenvol_max,(unsigned long long int)dp->vol[i]*(unsigned long long int)dp->tdim[i]); 


        dp->tdim_total+=dp->tdim[i];
        }

    //START210422
    dp->tdim_max=*std::max_element(dp->tdim,dp->tdim+nfiles);

    for(dp->volall=dp->vol[0],i=1;i<nfiles;i++)if(dp->vol[i]!=dp->volall){dp->volall=-1;break;}
    for(dp->tdimall=dp->tdim[0],i=1;i<nfiles;i++)if(dp->tdim[i]!=dp->tdimall){dp->tdimall=-1;break;}
    if(dp->filetypeall==(int)IMG)
        for(dp->bigendianall=dp->bigendian[0],i=1;i<nfiles;i++)if(dp->bigendian[i]!=dp->bigendianall){dp->bigendianall=-1;break;}

    #if 0
    //START190910
    for(i=0;i<3;++i)dp->c_orientall[i]=dp->c_orient[0][i];
    for(i=1;i<nfiles;++i){
        if(dp->c_orient[i][0]!=dp->c_orientall[0]||dp->c_orient[i][1]!=dp->c_orientall[1]||dp->c_orient[i][2]!=dp->c_orientall[2]){
            for(int j=0;j<3;++j)dp->c_orientall[j]=-1;
            break;
            }
        }
    #endif
    //START190917
    for(i=0;i<3;++i)dp->c_orientall[i]=dp->c_orient[0][i];
    for(i=1;i<nfiles;++i){
        if(dp->c_orient[i][0]!=dp->c_orientall[0]||dp->c_orient[i][1]!=dp->c_orientall[1]||dp->c_orient[i][2]!=dp->c_orientall[2]){
            for(int j=0;j<3;++j)dp->c_orientall[j]=-1;
            break;
            }
        }
    dp->cf_flipall=dp->c_orientall[0]*100+dp->c_orientall[1]*10+dp->c_orientall[2];
 

    return dp;
    }
void free_dim_param2(Dim_Param2 *dp){
    if(dp->mmppixf)free_d2float(dp->mmppixf);
    if(dp->centerf)free_d2float(dp->centerf);
    if(dp->orientation)free(dp->orientation);
    if(dp->dz)free(dp->dz);
    if(dp->dy)free(dp->dy);
    if(dp->dx)free(dp->dx);
    if(dp->zdim)free(dp->zdim);
    if(dp->ydim)free(dp->ydim);
    if(dp->xdim)free(dp->xdim);
    if(dp->bigendian)free(dp->bigendian);
    if(dp->swapbytes)free(dp->swapbytes);
    if(dp->tdim)free(dp->tdim);
    if(dp->vol)free(dp->vol);
    if(dp->filetype)free(dp->filetype);
    free(dp);
    }
