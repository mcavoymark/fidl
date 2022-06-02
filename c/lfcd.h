/* Copyright 4/23/21 Washington University.  All Rights Reserved.
   lfcd.h  $Revision: 1.1 $*/

#ifndef __LFCD_H__
    #define __LFCD_H__
    #include "spatialextent.h"
    class lfcd:public spatialextent{
        public:
        double *crushed,*work;
        int *lFCD;double *lFCDpearson;
    
        ~lfcd();
        lfcd(int xdim0,int ydim0,int zdim0,int nbrnidx0,int* brnidx0);
        void spatial_extent2lFCD(double thresh,double* mean,double* sd,float* tf,int tdim); 
        }; 
    double pearson(int vox1,int vox2,double* mean,double* sd,float* tf,int tdim,int vol);
#endif

#if 0
#ifndef __SPATIALEXTENTLFCD_H__
    #define __SPATIALEXTENTLFCD_H__

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


    class lFCD: public mask{

        public:
            double *mean,*sd;

            lFCD();
            ~lFCD();
            int lFCD0(char* file);

        };
#endif
