/* Copyright 2/21/19 Washington University. All Rights Reserved.
   trend.h  $Revision: 1.1 $ */

#ifndef __TREND_H__
    #define __TREND_H__

    #ifdef __cplusplus
        extern "C" {
    #endif

    void trend(int nfiles,int tdim_total,int *tdim,int *valid_frms,double *slope,int *iframes);
    int _trend(int argc,char **argv);

    #ifdef __cplusplus
        }//extern
    #endif
#endif
