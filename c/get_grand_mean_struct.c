/* Copyright 9/20/16 Washington University.  All Rights Reserved.
   get_grand_mean_struct.c  $Revision: 1.2 $*/
#include "fidl.h"
#include "get_grand_mean_struct.h"

#if 0
Grand_Mean_Struct *get_grand_mean_struct(float *glm_grand_mean,int vol,char *filename,FILE *fp){
    int i;
    double mean;
    Grand_Mean_Struct *gms;
    if(!(gms=malloc(sizeof*gms))) {
        printf("Error: Unable to malloc gms\n");
        return NULL;
        }
    if(!(gms->grand_mean=malloc(sizeof*gms->grand_mean*vol))) {
        printf("Error: Unable to malloc gms->grand_mean\n");
        return NULL;
        }
    for(mean=0.,i=0;i<vol;i++) if(glm_grand_mean[i] <= (float)GRAND_MEAN_THRESH) ++mean;
    if(mean > .9*vol) {
        printf("fidlInfo: %s\n",filename);
        printf("fidlInfo:    More than 90%% of voxels in grand mean less than %f. Must not be unprocessed image data.\n",
            (float)GRAND_MEAN_THRESH);
        if(fp) {
            fprintf(fp,"fidlInfo: %s\n",filename);
            fprintf(fp,"fidlInfo:    More than 90%% of voxels in grand mean less than %f. Must not be unprocessed image data.\n",
               (float)GRAND_MEAN_THRESH);
            }
        for(i=0;i<vol;i++) gms->grand_mean[i] = 1;
        gms->grand_mean_thresh = 0;
        gms->pct_chng_scl = 1;
        gms->nvox_ltgm = 0;
        }
    else {
        gms->grand_mean_thresh = (double)GRAND_MEAN_THRESH;
        gms->pct_chng_scl = 100;
        for(gms->nvox_ltgm=i=0;i<vol;i++) {
            if((gms->grand_mean[i] = (double)glm_grand_mean[i]) < gms->grand_mean_thresh) gms->nvox_ltgm++;
            }
        if(!(gms->index_nvox_ltgm=malloc(sizeof*gms->index_nvox_ltgm*gms->nvox_ltgm))) {
            printf("Error: Unable to malloc gms->index_nvox_ltgm\n");
            return NULL;
            }
        for(gms->nvox_ltgm=i=0;i<vol;i++) {
            if(gms->grand_mean[i] < gms->grand_mean_thresh) gms->index_nvox_ltgm[gms->nvox_ltgm++] = i;
            }
        }
    return gms;
    }
void free_grand_mean_struct(Grand_Mean_Struct *gms){
    free(gms->grand_mean);
    if(gms->nvox_ltgm)free(gms->index_nvox_ltgm);
    free(gms);
    }
#endif
#if 0
//START160920
Grand_Mean_Struct *get_grand_mean(LinearModel *glm,char *filename,FILE *fp){
    int i,vol;
    double mean;
    Grand_Mean_Struct *gms;
    vol=glm->ifh->glm_xdim*glm->ifh->glm_ydim*glm->ifh->glm_zdim; 
    //vol0=glm->ifh_glm_masked?glm->nmaski:vol;
    if(!(gms=malloc(sizeof*gms))){
        printf("fidlError: Unable to malloc gms\n");
        return NULL;
        }
    if(!(gms->grand_mean=malloc(sizeof*gms->grand_mean*vol))){
        printf("fidlError: Unable to malloc gms->grand_mean\n");
        return NULL;
        }
    if(glm->ifh->glm_masked)for(i=0;i<vol;i++)gms->grand_mean[i]=0.;

    //for(mean=0.,i=0;i<vol0;i++)if(glm->grand_mean[i]<=(float)GRAND_MEAN_THRESH)++mean;
    for(mean=0.,i=0;i<glm->nmaski;i++)if(glm->grand_mean[i]<=(float)GRAND_MEAN_THRESH)++mean;

    //if(mean>.9*vol0){
    if(mean>(.9*(double)glm->nmaski)){

        printf("fidlInfo: %s\n",filename);
        printf("fidlInfo:    More than 90%% of voxels in grand mean less than %f. Must not be unprocessed image data.\n",
            (float)GRAND_MEAN_THRESH);
        if(fp){
            fprintf(fp,"fidlInfo: %s\n",filename);
            fprintf(fp,"fidlInfo:    More than 90%% of voxels in grand mean less than %f. Must not be unprocessed image data.\n",
                (float)GRAND_MEAN_THRESH);
            }
        for(i=0;i<vol;i++)gms->grand_mean[i]=1;
        gms->grand_mean_thresh=0;
        gms->pct_chng_scl=1;
        }
    else{
        gms->grand_mean_thresh=(double)GRAND_MEAN_THRESH;
        gms->pct_chng_scl=100.;
        for(i=0;i<glm->nmaski;i++)gms->grand_mean[glm->maski[i]]=(double)glm->grand_mean[i];
        }
    return gms;
    }
#endif
//START160921
//Grand_Mean_Struct *get_grand_mean(LinearModel *glm,char *filename,FILE *fp){
//START190918
Grand_Mean_Struct *get_grand_mean(LinearModel *glm,char *filename,_IO_FILE* fp){
    int i;
    double mean;
    Grand_Mean_Struct *gms;
    if(!(gms=malloc(sizeof*gms))){
        printf("fidlError: Unable to malloc gms\n");
        return NULL;
        }
    if(!(gms->grand_mean=malloc(sizeof*gms->grand_mean*glm->nmaski))){
        printf("fidlError: Unable to malloc gms->grand_mean\n");
        return NULL;
        }
    for(mean=0.,i=0;i<glm->nmaski;i++)if(glm->grand_mean[i]<=(float)GRAND_MEAN_THRESH)++mean;

    //for(i=0;i<glm->nmaski;i++)printf("%f ",glm->grand_mean[i]);printf("\n");
    //printf("here100 glm->nmaski=%d mean=%f\n",glm->nmaski,mean);

    if(mean>(.9*(double)glm->nmaski)){
        printf("fidlInfo: %s\n",filename);
        printf("fidlInfo:    More than 90%% of voxels in grand mean less than %f. Must not be unprocessed image data.\n",
            (float)GRAND_MEAN_THRESH);
        if(fp){
            fprintf(fp,"fidlInfo: %s\n",filename);
            fprintf(fp,"fidlInfo:    More than 90%% of voxels in grand mean less than %f. Must not be unprocessed image data.\n",
               (float)GRAND_MEAN_THRESH);
            }
        for(i=0;i<glm->nmaski;i++)gms->grand_mean[i]=1;
        gms->grand_mean_thresh=0;
        gms->pct_chng_scl=1;
        }
    else{
        gms->grand_mean_thresh=(double)GRAND_MEAN_THRESH;
        gms->pct_chng_scl=100.;
        for(i=0;i<glm->nmaski;i++)gms->grand_mean[i]=(double)glm->grand_mean[i];
        }
    return gms;
    }





void free_grand_mean(Grand_Mean_Struct *gms){
    free(gms->grand_mean);
    free(gms);
    }
