/* Copyright 7/30/15 Washington University.  All Rights Reserved.
   nifti_gethdr.h  $Revision: 1.2 $*/

#ifndef __NIFTI_GETHDR_H__
    #define __NIFTI_GETHDR_H__

    //START190924
    #if 0
    struct NiftiS{
        int c_orient[3];
        int16_t datatype; 
        int64_t dims[4];
        float *center,*mmppix;
        };
    #endif
    #if 0
    typedef struct{
        int c_orient[3];
        int16_t datatype; 
        int64_t dims[4];
        float center[3],mmppix[3];
        }NiftiS;
    #endif
    //START211019
    typedef struct{
        int c_orient[3];
        int16_t datatype;
        int64_t dims[4];
        float center[3],mmppix[3];
        char datatypestr[100];
        }NiftiS;
 

    #ifdef __cplusplus
        extern "C" {
    #endif

    //int nifti_gethdr(char *file,int64_t *xdim,int64_t *ydim,int64_t *zdim,int64_t *tdim,float *center,float *mmppix,int *c_orient);
    //START190924
    //NiftiS* nifti_gethdr(char* file);
    int nifti_gethdr(char* file,NiftiS *niftis);

    //void nifti_delhdr(NiftiS* niftiS){delete[] niftis};
    //void nifti_delhdr(NiftiS* niftiS);

    int _nifti_gethdr(int argc,char **argv);
    #ifdef __cplusplus
        }//extern
    #endif
#endif
