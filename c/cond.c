/* Copyright 12/31/99 Washington University.  All Rights Reserved.
   cond.c  $Revision: 12.87 $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>
#include <nrutil.h>

double array_norm(double **A,int N);

double inverse_cover(double **A,double **Ainv,int N,double threshold,char *str)
{
    double cond;

    cond = condition_number(A,N,Ainv);
    if(cond >= threshold || cond==(double)UNSAMPLED_VOXEL) {

        /*printf("voxel=%d threshold = %f Larger than condition number. Computing pinv.\n",voxeli,threshold);*/
        printf("%s threshold = %f Condition number is bigger. Computing pinv.\n",str,threshold);

        pinv(A,N,N,(double)0.,Ainv);
        }
    return cond;
}




double condition_number(double **A,int N,double **Ainv)
{
    double condition;

    inverse(A,N,Ainv);
    condition = array_norm(A,N)*array_norm(Ainv,N);
    return condition;
}
double array_norm(double **A,int N)
{
    double max=0,sum;
    int i,j;

    for(j=1;j<=N;j++) {
        for(i=1,sum=0.;i<=N;i++) sum += (double)fabs(A[j][i]);
        if(sum > max) max = sum;
        }
    return max;
}

#if 1
/****************************************/
double	condition_number_old(double **A,int N)
/****************************************/
{
    double **Ainv,condition;

    Ainv = inverse_old(A,N);
    condition = array_norm(A,N)*array_norm(Ainv,N);
    free_dmatrix(Ainv,1,N,1,N);
    
    return condition;
}
#endif
