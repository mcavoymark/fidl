/* Copyright 4/29/21 Washington University.  All Rights Reserved.
   lfcd3.cxx  $Revision: 1.1 $*/

#include <cstddef>
#include <iostream>
#include <cmath>
#include <unordered_map>
#include "constants.h"
#include "lfcd3.h"

lfcd::~lfcd(){

    std::cout<<"here100"<<std::endl;

    if(crushed) delete[] crushed;

    std::cout<<"here101"<<std::endl;

    if(work) delete[] work; 

    std::cout<<"here102"<<std::endl;

    if(lFCD) delete[] lFCD;

    std::cout<<"here103"<<std::endl;

    if(lFCDpearson) delete[] lFCDpearson;

    std::cout<<"here104"<<std::endl;
    }

lfcd::lfcd(const mask& ms):spatialextent(ms){

    std::cout<<"lfcd::lfcd"<<std::endl;

    crushed=new int[ms.vol];
    work=new double[ms.lenbrain];
    lFCD=new int[ms.lenbrain]();
    lFCDpearson=new double[ms.lenbrain]();
    }

void lfcd::spatial_extent2lFCD(double thresh,double* mean,double* sd,float* tf,int tdim,int* maskidx){                   
    int i,*off=NULL,n_reg,n_srch,vox,vox1,n_nghbrs;
    size_t i1,j1;
    double td;

    int i2,j2; 
    unsigned long long k2; 
    std::unordered_map<unsigned long long,double> cc;

    for(j1=0;j1<brnidx.size();++j1){
        lFCD[j1]=0;lFCDpearson[j1]=0.;

        for(i=0;i<vol;++i)crushed[i]=1;
        for(i1=0;i1<brnidx.size();++i1)crushed[brnidx[i1]]=0;

        vox = brnidx[j1];

        //printf("vox=%d\n",vox);
        //std::cout<<"brnidx["<<j1<<"]="<<brnidx[j1]<<" brnidx.size()="<<brnidx.size()<<std::endl; 

        if(vox < xdim+1 || vox > vol-xdim-2) {
            /* Skip first and last row in volume. */
            n_nghbrs = 0;
            }
        else if(vox < plndim+xdim+1) {
            n_nghbrs = n_nghbrs_end;
            off = offsets_pln0;
            }
        else if(vox > vol-(plndim+xdim)-2) {
           n_nghbrs = n_nghbrs_end;
           off = offsets_plnN;
           }
        else {
           n_nghbrs = n_nghbrs_middle;
           off = offsets;
           }
        n_srch = n_reg = 0;

        crushed[vox] = 1;

        for(i=0;i<n_nghbrs;i++){
            if(!crushed[vox+off[i]]){

                i2=(int)j1;j2=maskidx[vox+off[i]];

                //START210503
                //std::cout<<"i2="<<i2<<" j2="<<j2<<std::endl;
                if(j2<0){
                    std::cout<<"j2="<<j2<<std::endl;
                    exit(-1);
                    }

                ascending(i2,j2);    
                ij_idx((int)brnidx.size(),i2,j2,k2);
                //std::cout<<"ascending i2="<<i2<<" j2="<<j2<<" k2="<<k2<<std::endl;

                //auto it=cc.find(k2);
                //if(it==cc.end())cc[k2]=pearson(vox,vox+off[i],mean,sd,tf,tdim,vol);
                //START210503
                if(!cc.count(k2))cc[k2]=pearson(vox,vox+off[i],mean,sd,tf,tdim,vol);

                if(std::abs(cc[k2]) >= thresh){
                    work[n_reg++]=cc[k2];
                    voxlist[n_srch++] = vox+off[i];
                    }
                crushed[vox+off[i]] = 1; 
                }
            }
 
        while(n_srch > 0){
            vox1 = voxlist[--n_srch];
            if(vox1 < xdim+1 || vox1 > vol-xdim-2) {
                /* Skip first and last row in volume. */
                n_nghbrs = 0;
                }
            else if(vox1 < plndim+xdim+1) {
                n_nghbrs = n_nghbrs_end;
                off = offsets_pln0;
                }
            else if(vox1 > vol - (plndim+xdim)-2) {
                n_nghbrs = n_nghbrs_end;
                off = offsets_plnN;
                }
            else {
                n_nghbrs = n_nghbrs_middle;
                off = offsets;
                }
            for(i=0;i<n_nghbrs;++i) {
                if(!crushed[vox1+off[i]]){

                    i2=(int)j1;j2=maskidx[vox1+off[i]];
                
                    //START210503
                    //std::cout<<"i2="<<i2<<" j2="<<j2<<std::endl;
                    if(j2<0){
                        std::cout<<"j2="<<j2<<std::endl;
                        exit(-1);
                        }

                    ascending(i2,j2);    
                    ij_idx((int)brnidx.size(),i2,j2,k2);
                    //std::cout<<"ascending i2="<<i2<<" j2="<<j2<<" k2="<<k2<<std::endl;

                    //auto it=cc.find(k2);
                    //if(it==cc.end())cc[k2]=pearson(vox,vox1+off[i],mean,sd,tf,tdim,vol);
                    //START210503
                    if(!cc.count(k2))cc[k2]=pearson(vox,vox1+off[i],mean,sd,tf,tdim,vol);

                    if(std::abs(cc[k2]) >= thresh) {
                        work[n_reg++]=cc[k2];
                        voxlist[n_srch++] = vox1+off[i];
                        }
                    crushed[vox1+off[i]] = 1; 

                    }
                }
            }
        if(n_reg){
            lFCD[j1]=n_reg; 
            for(td=0.,i=0;i<n_reg;++i)td+=atanh(work[i]);
            lFCDpearson[j1]=tanh(td/(double)n_reg);

            if(isnan(td)){
                std::cout<<"j1="<<j1<< "n_reg="<<n_reg<<std::endl;
                std::cout<<"work=";for(i=0;i<n_reg;++i)std::cout<<" "<<work[i];std::cout<<std::endl;
                std::cout<<"atanh(work)=";for(i=0;i<n_reg;++i)std::cout<<" "<<atanh(work[i]);std::cout<<std::endl;
                }
            }
        }
    cc.clear();

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
#if 1
//START210422
double pearson(int vox1,int vox2,double* mean,double* sd,float* tf,int tdim,int vol){

    double rxy=0.,rxy1;
    for(int i=0;i<tdim;++i,tf+=vol)rxy+=(double)*(tf+vox1)*(double)*(tf+vox2);

    //if(isnan(rxy)){printf("rxy=%f\n",rxy);fflush(stdout);}

    rxy1=(rxy-(double)tdim*mean[vox1]*mean[vox2])/((double)(tdim-1)*sd[vox1]*sd[vox2]); 

    if(isnan(rxy1)){
        //printf("rxy1=%f rxy=%f mean[%d]=%f mean[%d]=%f sd[%d]=%f sd[%d]=%f\n",rxy1,rxy,vox1,mean[vox1],vox2,mean[vox2],vox1,sd[vox1],vox2,sd[vox2]);fflush(stdout);
        printf("rxy1=%f rxy=%f sd[%d]=%f sd[%d]=%f\n",rxy1,rxy,vox1,sd[vox1],vox2,sd[vox2]);fflush(stdout);
        exit(-1);
        }

    return rxy1;
    }
#endif
