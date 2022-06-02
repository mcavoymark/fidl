/* Copyright 7/13/15 Washington University.  All Rights Reserved.
   dim_param2.h  $Revision: 1.8 $*/
#ifndef __DIM_PARAM2_H__
    #define __DIM_PARAM2_H__
    typedef struct{
        int *filetype,filetypeall,*swapbytes,*vol,volall,*tdim,tdimall,tdim_total,tdim_max,*bigendian,bigendianall,
            *xdim,*ydim,*zdim,*orientation,**c_orient,c_orientall[3],cf_flipall,*number_format,SunOS_Linux; //lenvol_max

        //START210422
        unsigned long long int lenvol_max; 

        float *dx,*dy,*dz,**centerf,**mmppixf;
        }Dim_Param2;

    #ifdef __cplusplus
        extern "C" {
    #endif

    //Dim_Param2 *dim_param2(size_t nfiles,char **files,int SunOS_Linux);
    //START190801
    Dim_Param2 *dim_param2(size_t nfiles,char **files);

    void free_dim_param2(Dim_Param2 *dp);

    #ifdef __cplusplus
        }//extern
    #endif

#endif
