/* Copyright 9/18/19 Washington University.  All Rights Reserved.
   utilities2.h  $Revision: 1.1 $*/
#ifndef __UTILITIES2_H__
    #define __UTILITIES2_H__

    #ifdef __cplusplus
        extern "C" {
    #endif

    int autocorr_guts(char *infile,char *outfile,double *r,int vol,int tdim);
                                                                             /*set to -1 for crosscor*/
    int crosscorr_guts(char *infile,char *outfile,double *r,int vol,int tdim,int maxlik_unbias,double *instack);
    int crosscov_guts2(char *outfile,double *r,int vol1,double *in1,int vol2,double *in2,int tdim,int nlags,int maxlik_unbias);
    int crosscov_guts3(char *outfile,double *r,int vol1,double *in1,int vol2,double *in2,int tdim,int nlags,int maxlik_unbias,
        double *sd1,double *sd2); /*if sd1 sd2 not null, then crosscorr is computed*/
                                                                        /* neg don't normalize */
    int cov_guts(char *infile,char *outfile,double *r,int vol,int tdim,int maxlik_unbias);
    int corr_guts(char *infile,char *outfile,double *r,double *den,int vol,int tdim);
                                                                        /*neg don't normalize*/
    int var_guts(char *infile,char *outfile,double *r,int vol,int tdim,int maxlik_unbias);

    #ifdef __cplusplus
        }//extern
    #endif
#endif




