/* Copyright 4/29/21 Washington University.  All Rights Reserved.
   lfcd3.h  $Revision: 1.1 $*/

#ifndef __LFCD3_H__
    #define __LFCD3_H__
    //#include <cmath>

    #include "spatialextent2.h"

    class lfcd:public spatialextent{
        public:
        double *work; //*crushed
        int *crushed,*lFCD;double *lFCDpearson;
    
        ~lfcd();

        //lfcd(int xdim0,int ydim0,int zdim0,int nbrnidx0,int* brnidx0);
        //START210429
        lfcd(const mask& ms);

        //void spatial_extent2lFCD(double thresh,double* mean,double* sd,float* tf,int tdim); 
        //START210429
        void spatial_extent2lFCD(double thresh,double* mean,double* sd,float* tf,int tdim,int* maskidx); 

        }; 
    double pearson(int vox1,int vox2,double* mean,double* sd,float* tf,int tdim,int vol);

    //START210429
    /* upper triangular square matrix sans diagonal with dimension n, (i,j) to idx k */ 
    inline void ij_idx(int n,int i,int j,unsigned long long &k){
        k=((n*(n-1))/2)-((n-i)*((n-i)-1))/2+j-i-1;
        }

    #if 0
    //KEEP
    /* upper triangular square matrix sans diagonal with dimension n, idx k to (i,j) */ 
    inline void idx_ij(int n,unsigned long long k,int &i,int &j){
        i = n - 2 - floor(sqrt(-8*k + 4*n*(n-1)-7)/2.0 - 0.5); 
        j = k + i + 1 - n*(n-1)/2 + (n-i)*((n-i)-1)/2;
        }
    #endif

    template <typename T>
    void ascending(T& dFirst,T& dSecond){
        if(dFirst > dSecond) std::swap(dFirst,dSecond);
        } 

#endif
