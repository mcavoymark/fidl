/* Copyright 5/23/16 Washington University.  All Rights Reserved.
   fdr_mult_comp.c  $Revision: 1.1 $ */

int fdr_mult_comp(double *zstat,double *threshold,int n_threshold,char *string,Interfile_header *ifh,
    int mode,double
// mode bit 0, print z
//      bit 1, print reg
//      bit 2, print t or f













#if 0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fidl.h"
#include "monte_carlo_mult_comp.h"
#include "subs_util.h"
int fdr_mult_comp(double *zstat,double *threshold,int n_threshold,char *string,Interfile_header *ifh,
    int mode,double *f_or_tstat,char *string2,int swapbytes,char **pstr,double *avg,char *avgstr,Mcmc *mcmc,Atlas_Param *ap){
/*mode = 0, print z; mode = 1, print t or f; mode = 2, print both*/
    char filename[MAXNAME],*dpstr=".05";
    size_t i,j;
    int lccom=0,lcpeak=0;
    for(j=0;j<(size_t)mcmc->lenatlas;j++) mcmc->atlas_zstat[j] = mcmc->atlas_image[j] = 0.;
    if(mode == 0 || mode == 2) *strstr(string,".4dfp.img") = 0;
    if(mode == 1 || mode == 2) *strstr(string2,".4dfp.img") = 0;
    if(avg) *strstr(avgstr,".4dfp.img") = 0;
    for(i=0;i<(size_t)n_threshold;i++) {
        for(j=0;j<(size_t)mcmc->lenatlas;j++) mcmc->actmask[j] = 0.;
        for(j=0;j<(size_t)mcmc->lenatlas;j++) mcmc->temp_double[j] = 0.;
        for(j=0;j<(size_t)mcmc->se->nbrnidx;j++) mcmc->temp_double[mcmc->se->brnidx[j]] = zstat[j];

        //spatial_extent2(mcmc->temp_double,mcmc->actmask,threshold[i],extent[i],mcmc->lenatlas,1,mcmc->se);
        //START160523
        spatial_extent2(mcmc->temp_double,mcmc->actmask,threshold[i],1,mcmc->lenatlas,1,mcmc->se);


#if 0
if((nreg=spatial_extent2(temp_double2,(float*)NULL,thresh[j],1,ms->lenvol,1,se)))
#endif


        printf("mcmc->se->nreg = %d\n",mcmc->se->nreg); fflush(stdout);
        for(j=0;j<(size_t)mcmc->se->nbrnidx;j++) mcmc->atlas_zstat[mcmc->se->brnidx[j]] = 
            !mcmc->actmask[mcmc->se->brnidx[j]] && zstat[j]!=(double)UNSAMPLED_VOXEL ? 0. : (float)zstat[j];
        if(mode == 0 || mode == 2) {
            sprintf(filename,"%s_mcomp_monte_carlo_z%.2fn%dp%s_all.4dfp.img",string,threshold[i],extent[i],pstr?pstr[i]:dpstr);
            if(!writestack(filename,mcmc->atlas_zstat,sizeof(float),(size_t)mcmc->lenatlas,swapbytes)) return 0;
            if(!write_ifh(filename,ifh,(int)FALSE)) return 0;
            printf("Output written to %s\n",filename);
            fflush(stdout);
            }
        if(mode == 1 || mode == 2) {
            for(j=0;j<(size_t)mcmc->se->nbrnidx;j++) mcmc->atlas_image[mcmc->se->brnidx[j]] = 
                !mcmc->actmask[mcmc->se->brnidx[j]]&&zstat[j]!=(double)UNSAMPLED_VOXEL ? 0. : (float)f_or_tstat[j];
            sprintf(filename,"%s_mcomp_monte_carlo_z%.2fn%dp%s_all.4dfp.img",string2,threshold[i],extent[i],pstr?pstr[i]:dpstr);
            if(!writestack(filename,mcmc->atlas_image,sizeof(float),(size_t)mcmc->lenatlas,swapbytes)) return 0;
            if(!write_ifh(filename,ifh,(int)FALSE)) return 0;
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
            if(!write_ifh(filename,ifh,(int)FALSE)) exit(-1);
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
#endif
