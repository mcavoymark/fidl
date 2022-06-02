/* Copyright 8/25/10 Washington University.  All Rights Reserved.
   cond_norm1.c  $Revision: 1.17 $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#ifndef __sun__
    #include <stdint.h>
#endif
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_errno.h>
#include "cond_norm1.h" 
int _cond_norm1(int argc,char **argv)
{
    double *Ainv;
    double *A = (double*)argv[0];
    #ifdef __sun__
        int m = (int)argv[1];
    #else
        int m = (intptr_t)argv[1];
    #endif
    double *cond = (double*)argv[2];

    /*START150204*/
    int ret;

    if(!(Ainv=malloc(sizeof*Ainv*m*m))) {
        printf("Error: Unable to malloc Ainv\n");
        return 0;
        }
    gsl_permutation *perm = gsl_permutation_alloc(m);

    /*cond_norm1(A,m,cond,Ainv,perm);*/
    /*START150204*/
    ret=cond_norm1(A,m,cond,Ainv,perm)?0:1;

    gsl_permutation_free(perm);
    free(Ainv);

    /*return 1;*/
    /*START150204*/
    return ret;
}
int cond_norm1(double *A,int m,double *cond,double *Ainv,gsl_permutation *perm)
{
    int signum,status;
    double Anorm1,Anorm1inv;

    #if 0
    int i,j,k;
    printf("cond_norm1 A\n"); 
    for(k=i=0;i<m;i++) {
        for(j=0;j<m;j++,k++) printf("%f ",A[k]);
        printf("\n");
        }
    #endif
    /*if(!writestack("ATAdidl2.4dfp.img",A,sizeof(double),(size_t)(m*m),0)) exit(-1);*/
    /*if(!writestack("ATAdc2.4dfp.img",A,sizeof(double),(size_t)(m*m),0)) exit(-1);*/
    /*exit(-1);*/
    /*for(i=0;i<m*m;i++)Ainv[i]=0.;*/


    Anorm1 = array_norm1(A,m);
    gsl_matrix_view gA = gsl_matrix_view_array(A,m,m);
    gsl_matrix_view gAinv = gsl_matrix_view_array(Ainv,m,m);
    gsl_linalg_LU_decomp(&gA.matrix,perm,&signum);
    gsl_set_error_handler_off();


    #if 0
    if((status=gsl_linalg_LU_invert(&gA.matrix,perm,&gAinv.matrix))) {
        printf("fidlInfo: %s  status=%d\n",gsl_strerror(status),status);
        return 1;
        }
    #endif
    //START150929
    if((status=gsl_linalg_LU_invert(&gA.matrix,perm,&gAinv.matrix))){
        printf("fidlInfo: cond_norm1 status=%d %s\n",status,gsl_strerror(status));
        return status;
        }

    #if 0
    printf("cond_norm1 Ainv\n"); 
    for(k=i=0;i<m;i++) {
        for(j=0;j<m;j++,k++) printf("%f ",Ainv[k]);
        printf("\n");
        }
    #endif
    /*if(!writestack("Ainvidl.4dfp.img",Ainv,sizeof(double),(size_t)(m*m),0)) exit(-1);*/
    /*if(!writestack("Ainvc.4dfp.img",Ainv,sizeof(double),(size_t)(m*m),0)) exit(-1);*/

    Anorm1inv = array_norm1(Ainv,m);
    *cond = Anorm1*Anorm1inv;
    /*printf("cond_norm1 Anorm1=%f Anorm1inv=%f *cond = %f\n",Anorm1,Anorm1inv,*cond);*/
    return 0;
}
double array_norm1(double *A,int m){
    double max=0.,sum;
    int i,j,j1;
    for(i=0;i<m;i++) {
        for(sum=0.,j1=i,j=0;j<m;j++,j1+=m) sum += fabs(A[j1]);
        if(sum>max) max = sum;
        } 
    return max;
    }
