/* Copyright 4/9/21 Washington University.  All Rights Reserved.
   spatial_extent2lFCD.cxx  $Revision: 1.1 $*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "fidl.h"
#include "spatial_extent2.h"
#include "spatial_extent2lFCD.h"

lFCD::lFCD(){
    mean=NULL;sd=NULL;
    }
lFCD::~lFCD(){
    if(mean) delete[] mean;
    if(sd) delete[] sd;
    }

int lFCD::lFCD0(char* file){
    stack fs;
    if(!(stack1(file)))return 0;
    mean=new double[ms.vol]();
    sd=new double[ms.vol]();

    for(k=i=0;i<dp->tdim[i1];++i,k+=dp->vol[i1]){
        for(j=0;j<ms.lenbrain;++j)mean[ms.brnidx[j]]+=(double)tf[k+ms.brnidx[j]];
        }
    for(i=0;i<ms.lenbrain;++i)mean[ms.brnidx[i]]/=(double)dp->tdim[i1];

    for(k=i=0;i<dp->tdim[i1];++i,k+=dp->vol[i1]){
        for(j=0;j<ms.lenbrain;++j)sd[ms.brnidx[j]]+=pow((double)tf[k+ms.brnidx[j]]-mean[ms.brnidx[j]],2.);
        }
    for(i=0;i<ms.lenbrain;++i)sd[ms.brnidx[i]]=sqrt(sd[ms.brnidx[i]]/(double)(dp->tdim[i1]-1));



    }


void spatial_extent2lFCD(double* crushed,double* work,double thresh,Spatial_Extent2* se,double* mean,double* sd,float* tf,int tdim,float* lFCD,float* lFCDpearson){                   
    int i,j,*off=NULL,n_reg,n_srch,vox,vox1,n_nghbrs;
    double td;
    if(thresh<=(double)UNSAMPLED_VOXEL)thresh=2.*(double)UNSAMPLED_VOXEL;

    for(j=0;j<se->vol;++j)lFCD[j]=lFCDpearson[j]=0.;

    for(se->nreg=se->nvox=j=0;j<se->nbrnidx;j++){
        for(i=0;i<se->vol;++i)crushed[i]=0.;

        vox = se->brnidx[j];
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

            crushed[vox+off[i]]=pearson(vox,vox+off[i],mean,sd,tdim,tf);

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

                crushed[vox1+off[i]]=pearson(vox,vox1+off[i],mean,sd,tdim,tf);

                if(fabs(crushed[vox1+off[i]]) >= thresh) {

                    #if 0
                    crushed[vox1+off[i]] = (double)UNSAMPLED_VOXEL; 
                    se->voxlist[n_srch++] = vox1+off[i];
                    se->reglist[se->nvox+n_reg++] = vox1+off[i];
                    #endif 
                    //START210409
                    work[n_reg++]=crushed[vox1+off[i]];
                    se->voxlist[n_srch++] = vox1+off[i];
                    crushed[vox1+off[i]] = (double)UNSAMPLED_VOXEL; 

                    }
                }
            }
        if(n_reg){
            lFCD[vox]=(float)nreg; 
            for(td=0.,i=0;i<n_reg;++i)td+=atanh(work[i]);
            lFCDpearson[vox]=tanh(td/(double)n_reg);
            }
        }
    }

double pearson(int vox1,int vox2,double* mean,double* sd,float* tf,int tdim,int vol){
    double rxy=0.;
    for(k=i=0;i<tdim;++i,k+=vol)rxy=(double)tf[k+vox1]*(double)td[k+vox2];
    rxy=(rxy-(double)tdim*mean[vox1]*mean[vox2])/((double)(tdim-1)*sd[vox1]*sd[vox2]); 
    return rxy;
    }
