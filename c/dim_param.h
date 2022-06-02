/* Copyright 7/14/15 Washington University.  All Rights Reserved.
   dim_param.h  $Revision: 1.3 $*/
#ifndef __DIM_PARAM_H__
    #define __DIM_PARAM_H__
    typedef struct{
        size_t *byte;
        int nfiles,vol,xdim,ydim,zdim,orientation,tdim_max,tdim_min,tdim_total,*tdim,*bigendian,all_tdim_same,*swapbytes,
            *number_format,*lenvol,lenvol_max,*lccifti;
        float dxdy,dz;
        }Dim_Param;
    Dim_Param *dim_param(int nfiles,char **files,int SunOS_Linux,int novolcheck,int *xdim,int *tdim);
    void free_dim_param(Dim_Param *dp);
#endif
