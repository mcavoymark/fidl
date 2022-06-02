/* Copyright 7/21/15 Washington University.  All Rights Reserved.
   cifti_getDimensions.h  $Revision: 1.2 $*/
#ifndef __CIFTI_GETDIMENSIONS_H__
    #define __CIFTI_GETDIMENSIONS_H__ 
    #ifdef __cplusplus
        extern "C" {
    #endif
    int cifti_getDimensions(char *file,int64_t *rowSize,int64_t *colSize);
    int _cifti_getDimensions(int argc,char **argv);
    #ifdef __cplusplus
        }//extern
    #endif
#endif
