/* Copyright 7/28/16 Washington University.  All Rights Reserved.
   fdr_guts.h  $Revision: 1.2 $ */

#if 0
int fdr_guts(double *crushed,float *corrected,float *actmask,double thresh,Spatial_Extent2 *se,double **table,double q,int *clussize,
    float *min,float *max,size_t *p);
                          //allocate ms->lenbrain
#endif
#if 0
//START160809
int fdr_guts(double *crushed,float *corrected,float *actmask,double thresh,Spatial_Extent2 *se,double **table,double q,int *regstart,
    int *clussize,float *min,float *max,size_t *p);
                                        //allocate ms->lenbrain
#endif
//START160816
int fdr_guts(double *crushed,float *corrected,float *actmask,double thresh,Spatial_Extent2 *se,double **table,double q,float *regstart,
    int *clussize,float *min,float *max,size_t *p);
                                        //allocate ms->lenbrain
