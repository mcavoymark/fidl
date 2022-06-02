/* Copyright 9/20/16 Washington University.  All Rights Reserved.
   get_grand_mean_struct.h  $Revision: 1.2 $*/

#ifndef __GET_GRAND_MEAN_STRUCT_H__
    #define __GET_GRAND_MEAN_STRUCT_H__

    typedef struct{
        double *grand_mean,grand_mean_thresh,pct_chng_scl;
        }Grand_Mean_Struct;

    #ifdef __cplusplus
        extern "C" {
    #endif

    Grand_Mean_Struct *get_grand_mean(LinearModel *glm,char *filename,FILE *fp);
    void free_grand_mean(Grand_Mean_Struct *gms);

    #ifdef __cplusplus
        }//extern
    #endif
#endif
