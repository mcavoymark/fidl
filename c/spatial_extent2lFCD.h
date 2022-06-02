/* Copyright 4/9/21 Washington University.  All Rights Reserved.
   spatial_extent2lFCD.h  $Revision: 1.1 $*/

#ifndef __SPATIAL_EXTENT2LFCD_H__
    #define __SPATIAL_EXTENT2LFCD_H__

    #ifdef __cplusplus
        extern "C" {
    #endif

    //void spatial_extent2lFCD(double* crushed,double* work,double thresh,Spatial_Extent2* se,double* mean,double* sd,float* tf,int tdim,float* lFCD,float* lFCDpearson);
    void spatial_extent2lFCD(double* crushed,double* work,double thresh,Spatial_Extent2* se,double* mean,double* sd,float* tf,int tdim,int* lFCD,double* lFCDpearson);

    double pearson(int vox1,int vox2,double* mean,double* sd,float* tf,int tdim,int vol);

    #ifdef __cplusplus
        }//extern
    #endif
#endif


#if 0
    class lFCD: public mask{

        public:
            double *mean,*sd;

            lFCD();
            ~lFCD();
            int lFCD0(char* file);

        };
#endif
