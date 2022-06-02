/* Copyright 8/5/15 Washington University.  All Rights Reserved.
   write_glm.h  $Revision: 1.2 $ */

#ifndef __WRITE_GLM_H__
    #define __WRITE_GLM_H__

    #ifdef __cplusplus
        extern "C" {
    #endif

    int write_glm(char *glm_file,LinearModel *glm,int what_to_write,int swapbytes);
    int write_glm_grand_mean(char *glm_file,LinearModel *glm,int lcseek,int swapbytes);

    #ifdef __cplusplus
        }//extern
    #endif
#endif
