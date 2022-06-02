/* Copyright 4/8/21 Washington University.  All Rights Reserved.
   spatial_extent2lFCD.c  $Revision: 1.1 $*/
#include <stdlib.h>
#include <math.h>
#include "fidl.h"
#include "spatial_extent2.h"
#include "spatial_extent2lFCD.h"

//void spatial_extent2lFCD(double* crushed,double* work,double thresh,Spatial_Extent2* se,double* mean,double* sd,float* tf,int tdim,float* lFCD,float* lFCDpearson){                   
void spatial_extent2lFCD(double* crushed,double* work,double thresh,Spatial_Extent2* se,double* mean,double* sd,float* tf,int tdim,int* lFCD,double* lFCDpearson){                   
    int i,j,*off=NULL,n_reg,n_srch,vox,vox1,n_nghbrs;
    double td;
    if(thresh<=(double)UNSAMPLED_VOXEL)thresh=2.*(double)UNSAMPLED_VOXEL;

    //for(j=0;j<se->vol;++j)lFCD[j]=lFCDpearson[j]=0.;
    for(j=0;j<se->vol;++j){lFCD[j]=0;lFCDpearson[j]=0.;}

    for(se->nreg=se->nvox=j=0;j<se->nbrnidx;++j){
        for(i=0;i<se->vol;++i)crushed[i]=0.;
        vox = se->brnidx[j];

        printf("vox=%d\n",vox);

        if(vox < se->xdim+1 || vox > se->vol-se->xdim-2) {
            /* Skip first and last row in volume. */
            n_nghbrs = 0;
            }
        else if(vox < se->plndim+se->xdim+1) {
            n_nghbrs = se->n_nghbrs_end;
            off = se->offsets_pln0;
            }
        else if(vox > se->vol-(se->plndim+se->xdim)-2) {
           n_nghbrs = se->n_nghbrs_end;
           off = se->offsets_plnN;
           }
        else {
           n_nghbrs = se->n_nghbrs_middle;
           off = se->offsets;
           }
        n_srch = n_reg = 0;
        crushed[vox] = (double)UNSAMPLED_VOXEL;

	//se->reglist[se->nvox+n_reg++] = vox; 

        for(i=0;i<n_nghbrs;i++){

            crushed[vox+off[i]]=pearson(vox,vox+off[i],mean,sd,tf,tdim,se->vol);

            if(fabs(crushed[vox+off[i]]) >= thresh){

                #if 0
                crushed[vox+off[i]] = (double)UNSAMPLED_VOXEL; 
                se->voxlist[n_srch++] = vox+off[i];
                se->reglist[se->nvox+n_reg++] = vox+off[i];
                #endif
                //START210409
                work[n_reg++]=crushed[vox+off[i]];
                se->voxlist[n_srch++] = vox+off[i];
                crushed[vox+off[i]] = (double)UNSAMPLED_VOXEL; 

                }
            }


        while(n_srch > 0){
            vox1 = se->voxlist[--n_srch];
            if(vox1 < se->xdim+1 || vox1 > se->vol-se->xdim-2) {
                /* Skip first and last row in volume. */
                n_nghbrs = 0;
                }
            else if(vox1 < se->plndim+se->xdim+1) {
                n_nghbrs = se->n_nghbrs_end;
                off = se->offsets_pln0;
                }
            else if(vox1 > se->vol - (se->plndim+se->xdim)-2) {
                n_nghbrs = se->n_nghbrs_end;
                off = se->offsets_plnN;
                }
            else {
                n_nghbrs = se->n_nghbrs_middle;
                off = se->offsets;
                }
            for(i=0;i<n_nghbrs;++i) {
                crushed[vox1+off[i]]=pearson(vox,vox1+off[i],mean,sd,tf,tdim,se->vol);

                printf("here57 crushed[%d]=%f\n",vox1+off[i],crushed[vox1+off[i]]);fflush(stdout);

                if(fabs(crushed[vox1+off[i]]) >= thresh) {

                    printf("here58 n_reg=%d n_srch=%d\n",n_reg,n_srch);fflush(stdout);

                    #if 0
                    crushed[vox1+off[i]] = (double)UNSAMPLED_VOXEL; 
                    se->voxlist[n_srch++] = vox1+off[i];
                    se->reglist[se->nvox+n_reg++] = vox1+off[i];
                    #endif 
                    //START210409
                    work[n_reg++]=crushed[vox1+off[i]];
                    se->voxlist[n_srch++] = vox1+off[i];
                    crushed[vox1+off[i]] = (double)UNSAMPLED_VOXEL; 

                    printf("here59\n");fflush(stdout);

                    }
                }

            printf("here60\n");fflush(stdout);

            }

        printf("here61\n");fflush(stdout);

        if(n_reg){
            //lFCD[vox]=(float)n_reg; 
            lFCD[j]=n_reg; 
            for(td=0.,i=0;i<n_reg;++i)td+=atanh(work[i]);
            //lFCDpearson[vox]=(float)tanh(td/(double)n_reg);
            lFCDpearson[j]=tanh(td/(double)n_reg);
            }

        printf("here62\n");fflush(stdout);

        }
    }

#if 0
double pearson(int vox1,int vox2,double* mean,double* sd,float* tf,int tdim,int vol){
    double rxy=0.;
    int i,k;
    for(k=i=0;i<tdim;++i,k+=vol)rxy+=(double)tf[k+vox1]*(double)tf[k+vox2];
    rxy=(rxy-(double)tdim*mean[vox1]*mean[vox2])/((double)(tdim-1)*sd[vox1]*sd[vox2]); 
    return rxy;
    }
#endif
//START210422
double pearson(int vox1,int vox2,double* mean,double* sd,float* tf,int tdim,int vol){

    double rxy=0.,rxy1;
    for(int i=0;i<tdim;++i,tf+=vol)rxy+=(double)*(tf+vox1)*(double)*(tf+vox2);

    //if(isnan(rxy)){printf("rxy=%f\n",rxy);fflush(stdout);}

    rxy1=(rxy-(double)tdim*mean[vox1]*mean[vox2])/((double)(tdim-1)*sd[vox1]*sd[vox2]); 

    if(isnan(rxy1)){
        printf("rxy1=%f rxy=%f mean[%d]=%f mean[%d]=%f sd[%d]=%f sd[%d]=%f\n",rxy1,rxy,vox1,mean[vox1],vox2,mean[vox2],vox1,sd[vox1],vox2,sd[vox2]);fflush(stdout);
        exit(-1);
        }

    return rxy1;
    }
