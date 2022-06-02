/* Copyright 12/6/02 Washington University.  All Rights Reserved.
   monte_carlo_mult_comp.c  $Revision: 1.32 $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fidl.h"
#include "monte_carlo_mult_comp.h"
#include "subs_util.h"
#include "minmax.h"

//START190403
#include "filetype.h"

Mcmc *monte_carlo_mult_comp_init(int dim1,int dim2,int dim3,int nbrnidx,int *brnidx){
    int i;
    Mcmc *mcmc;
    if(!(mcmc=malloc(sizeof*mcmc))) {
        printf("Error: Unable to malloc mcmc\n"); fflush(stdout);
        return NULL;
        }
    mcmc->lenatlas = dim1*dim2*dim3;
    if(!(mcmc->atlas_zstat=malloc(sizeof*mcmc->atlas_zstat*mcmc->lenatlas))) {
        printf("Error: Unable to malloc mcmc->atlas_zstat\n"); fflush(stdout);
        return NULL;
        }
    if(!(mcmc->atlas_image=malloc(sizeof*mcmc->atlas_image*mcmc->lenatlas))) {
        printf("Error: Unable to malloc mcmc->atlas_image\n"); fflush(stdout);
        return NULL;
        }
    if(!(mcmc->actmask=malloc(sizeof*mcmc->actmask*mcmc->lenatlas))) {
        printf("Error: Unable to malloc mcmc->actmask\n"); fflush(stdout);
        return NULL;
        }
    if(!(mcmc->temp_double=malloc(sizeof*mcmc->temp_double*mcmc->lenatlas))) {
        printf("Error: Unable to malloc mcmc->temp_double\n"); fflush(stdout);
        return NULL;
        }
    if(!(mcmc->roi=malloc(sizeof*mcmc->roi*nbrnidx))) {
        printf("Error: Unable to malloc mcmc->roi\n"); fflush(stdout);
        return NULL;
        }
    for(i=0;i<nbrnidx;i++) mcmc->roi[i] = i;
    if(!(mcmc->se=spatial_extent2init(dim1,dim2,dim3,nbrnidx,brnidx))) exit(-1);
    if(!(mcmc->com=center_of_mass2init(nbrnidx,nbrnidx))) exit(-1);
    if(!(mcmc->arn=assign_region_names2init(nbrnidx))) exit(-1);
    return mcmc;
    }
int monte_carlo_mult_comp(double *zstat,double *threshold,int *extent,int n_threshold_extent,char *string,Interfile_header *ifh,
    int mode,double *f_or_tstat,char *string2,int swapbytes,char **pstr,double *avg,char *avgstr,Mcmc *mcmc,Atlas_Param *ap){
/*mode = 0, print z; mode = 1, print t or f; mode = 2, print both*/
    char filename[MAXNAME],*dpstr=".05",*ext;
    size_t i,j;
    int lccom=0,lcpeak=0;
    for(j=0;j<(size_t)mcmc->lenatlas;j++) mcmc->atlas_zstat[j] = mcmc->atlas_image[j] = 0.;

    //printf("here91 mcmc->lenatlas=%d mode=%d string=%s\n",mcmc->lenatlas,mode,string);fflush(stdout);

    #if 0
    if(mode == 0 || mode == 2) *strstr(string,".4dfp.img") = 0;
    if(mode == 1 || mode == 2) *strstr(string2,".4dfp.img") = 0;
    if(avg) *strstr(avgstr,".4dfp.img") = 0;
    #endif
    //START190403
    if(mode == 0 || mode == 2){
        if(!(i=get_filetype(string)))return 0;
        ext=getFileext(i);
        *strstr(string,ext) = 0;
        }
    if(mode == 1 || mode == 2){
        if(!(i=get_filetype(string2)))return 0;
        ext=getFileext(i);
        *strstr(string2,ext) = 0;
        }
    if(avg){
        if(!(i=get_filetype(avgstr)))return 0;
        ext=getFileext(i);
        *strstr(avgstr,ext) = 0;
        }

    for(i=0;i<(size_t)n_threshold_extent;i++) {
        for(j=0;j<(size_t)mcmc->lenatlas;j++) mcmc->actmask[j] = 0.;
        for(j=0;j<(size_t)mcmc->lenatlas;j++) mcmc->temp_double[j] = 0.;
        for(j=0;j<(size_t)mcmc->se->nbrnidx;j++) mcmc->temp_double[mcmc->se->brnidx[j]] = zstat[j];
        spatial_extent2(mcmc->temp_double,mcmc->actmask,threshold[i],extent[i],
            mcmc->lenatlas,1,mcmc->se);
        printf("mcmc->se->nreg = %d\n",mcmc->se->nreg); fflush(stdout);
        for(j=0;j<(size_t)mcmc->se->nbrnidx;j++) mcmc->atlas_zstat[mcmc->se->brnidx[j]] = 
            !mcmc->actmask[mcmc->se->brnidx[j]] && zstat[j]!=(double)UNSAMPLED_VOXEL ? 0. : (float)zstat[j];
        if(mode == 0 || mode == 2) {
            sprintf(filename,"%s_mcomp_monte_carlo_z%.2fn%dp%s_all.4dfp.img",string,threshold[i],extent[i],pstr?pstr[i]:dpstr);
            if(!writestack(filename,mcmc->atlas_zstat,sizeof(float),(size_t)mcmc->lenatlas,swapbytes)) return 0;
            if(!write_ifh(filename,ifh,0)) return 0;
            printf("Output written to %s\n",filename);
            fflush(stdout);
            }
        if(mode == 1 || mode == 2) {
            for(j=0;j<(size_t)mcmc->se->nbrnidx;j++) mcmc->atlas_image[mcmc->se->brnidx[j]] = 
                !mcmc->actmask[mcmc->se->brnidx[j]]&&zstat[j]!=(double)UNSAMPLED_VOXEL ? 0. : (float)f_or_tstat[j];
            sprintf(filename,"%s_mcomp_monte_carlo_z%.2fn%dp%s_all.4dfp.img",string2,threshold[i],extent[i],pstr?pstr[i]:dpstr);
            if(!writestack(filename,mcmc->atlas_image,sizeof(float),(size_t)mcmc->lenatlas,swapbytes)) return 0;
            if(!write_ifh(filename,ifh,0)) return 0;
            fprintf(stdout,"Output written to %s\n",filename);
            fflush(stdout);
            if(mode==1) mcmc->atlas_zstat = mcmc->atlas_image; 
            }
        if(mcmc->se->nreg) {
            center_of_mass2(mcmc->atlas_zstat,mcmc->com,mcmc->se->nvox,mcmc->se->reglist,mcmc->se->nreg,mcmc->se->regsize,ap);
            if(!assign_region_names2(mcmc->se->nreg,mcmc->arn,ifh,mcmc->se->regsize,!lccom?(double*)NULL:mcmc->com->coor,
                mcmc->com->peakcoor,!lcpeak?(float*)NULL:mcmc->com->peakval,(char**)NULL)) return 0;
            sprintf(filename,"%s_mcomp_monte_carlo_z%.2fn%dp%s_all_regions.4dfp.img",string,threshold[i],extent[i],
                pstr?pstr[i]:dpstr);
            if(!writestack(filename,mcmc->actmask,sizeof(float),(size_t)mcmc->lenatlas,swapbytes)) return 0;
            min_and_max(mcmc->actmask,mcmc->lenatlas,&ifh->global_min,&ifh->global_max);
            if(!write_ifh(filename,ifh,0)) exit(-1);
            free_ifhregnames(ifh);
            printf("Region file written to %s\n",filename); fflush(stdout);
            }
        if(avg) {
            for(j=0;j<(size_t)mcmc->se->nbrnidx;j++) mcmc->atlas_image[mcmc->se->brnidx[j]] = 
                !mcmc->actmask[mcmc->se->brnidx[j]]&&zstat[j]!=(double)UNSAMPLED_VOXEL ? 0. : (float)avg[j];
            sprintf(filename,"%s_mcomp_monte_carlo_z%.2fn%dp%s_all.4dfp.img",avgstr,threshold[i],extent[i],pstr?pstr[i]:dpstr);
            if(!writestack(filename,mcmc->atlas_image,sizeof(float),(size_t)mcmc->lenatlas,swapbytes)) return 0;
            if(!write_ifh(filename,ifh,0)) exit(-1);
            printf("Output written to %s\n",filename);
            fflush(stdout);
            }
        }
    return 1;
}
