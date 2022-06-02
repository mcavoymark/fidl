/* Copyright 3/23/15 Washington University.  All Rights Reserved.
   monte_carlo_mult_comp.h  $Revision: 1.5 $ */
#include "spatial_extent2.h"
#include "center_of_mass2.h"
#include "assign_region_names2.h"
#include "get_atlas_param.h"

#ifndef __MONTE_CARLO_MULT_COMP_H__
    #define __MONTE_CARLO_MULT_COMP_H__

    typedef struct {
        int lenatlas,*roi;
        float *actmask,*atlas_zstat,*atlas_image;
        double *temp_double;
        Spatial_Extent2 *se;
        center_of_mass2struct *com;
        assign_region_names2struct *arn;
        } Mcmc;

    #ifdef __cplusplus
        extern "C" {
    #endif

    Mcmc *monte_carlo_mult_comp_init(int dim1,int dim2,int dim3,int nbrnidx,int *brnidx);
    int monte_carlo_mult_comp(double *zstat,double *threshold,int *extent,int n_threshold_extent,char *string,
        Interfile_header *ifh,int mode,double *f_or_tstat,char *string2,int swapbytes,char **pstr,double *avg,char *avgstr,
        Mcmc *mcmc,Atlas_Param *ap);

    #ifdef __cplusplus
        }//extern
    #endif
#endif
