/* Copyright 12/31/99 Washington University.  All Rights Reserved.
   inverse.c  $Revision: 12.87 $ 
   From: Numerical recipes in C*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "nrutil.h"

//START220115
#include "inverse.h"


#define NRANSI
#define TINY 1.0e-20;
int ludcmp(double **a, int n, int *indx, double *d);
void lubksb(double **a, int n, int *indx, double b[]);

double **inverse_old(double **A,int N);

//START220115
//int inverse(double **A,int N,double **Ainv);

#if 1
/********************************/
double **inverse_old(double **A,int N)
/********************************/
{
double   **A_inverse,**A_tmp,d,*col;

int	i,j,*indx;

/* Allocate output matrix. */
A_inverse = dmatrix(1,N,1,N);
A_tmp = dmatrix(1,N,1,N);
indx = ivector(1,N);
col  = dvector(1,N);

for(i=1;i<=N;i++) {
    for(j=1;j<=N;j++) A_tmp[i][j] = A[i][j];
    }

/* Perform LU decomposition. */
if(ludcmp(A_tmp,N,indx,&d)) {
    for(j=1;j<=N;j++) {
        for(i=1;i<=N;i++) col[i] = 0.;
        col[j] = 1.;
        lubksb(A_tmp,N,indx,col);
        for(i=1;i<=N;i++) A_inverse[i][j] = col[i];
        }
    }
else {
    free_dmatrix(A_inverse,1,N,1,N);
    A_inverse = (double **)NULL;
    }
free_ivector(indx,1,N);
free_dvector(col,1,N);
free_dmatrix(A_tmp,1,N,1,N);
return(A_inverse);
}
#endif


int inverse(double **A,int N,double **Ainv)
{
    double **A_tmp,d,*col;
    int i,j,*indx;

    A_tmp = dmatrix(1,N,1,N);
    indx = ivector(1,N);
    col  = dvector(1,N);

    for(i=1;i<=N;i++) for(j=1;j<=N;j++) A_tmp[i][j] = A[i][j];

    /* Perform LU decomposition. */
    if(ludcmp(A_tmp,N,indx,&d)) {
        for(j=1;j<=N;j++) {
            for(i=1;i<=N;i++) col[i] = 0.;
            col[j] = 1.;
            lubksb(A_tmp,N,indx,col);
            for(i=1;i<=N;i++) Ainv[i][j] = col[i];
            }
        }
    else {
        return 0;
        }
    free_dmatrix(A_tmp,1,N,1,N);
    free_ivector(indx,1,N);
    free_dvector(col,1,N);
    return 1;
}


/**************************************************/
int ludcmp(double **a, int n, int *indx, double *d)
/**************************************************/
{
	int i,imax,j,k,status=1;
	double big,dum,sum,temp;
	double *vv;

	vv=dvector(1,n);
	*d=1.0;
	for (i=1;i<=n;i++) {
		big=0.0;
		for (j=1;j<=n;j++)
			if ((temp=fabs(a[i][j])) > big) big=temp;
		if(big == 0.0) {
                    /*nrerror("Singular matrix in routine ludcmp");*/
                    status = 0;
                    break;
                    }
		vv[i]=1.0/big;
	}
        if(status) {
	    for (j=1;j<=n;j++) {
		    for (i=1;i<j;i++) {
			    sum=a[i][j];
			    for (k=1;k<i;k++) sum -= a[i][k]*a[k][j];
			    a[i][j]=sum;
		    }
		    big=0.0;
		    for (i=j;i<=n;i++) {
			    sum=a[i][j];
			    for (k=1;k<j;k++)
				    sum -= a[i][k]*a[k][j];
			    a[i][j]=sum;
			    if ( (dum=vv[i]*fabs(sum)) >= big) {
				    big=dum;
				    imax=i;
			    }
		    }
		    if (j != imax) {
			    for (k=1;k<=n;k++) {
				    dum=a[imax][k];
				    a[imax][k]=a[j][k];
				    a[j][k]=dum;
			    }
			    *d = -(*d);
			    vv[imax]=vv[j];
		    }
		    indx[j]=imax;
		    if (a[j][j] == 0.0) a[j][j]=TINY;
		    if (j != n) {
			    dum=1.0/(a[j][j]);
			    for (i=j+1;i<=n;i++) a[i][j] *= dum;
		    }
	    }
        }
	free_dvector(vv,1,n);
        return status;
}

/*************************************************/
void lubksb(double **a, int n, int *indx, double b[])
/*************************************************/

{
	int i,ii=0,ip,j;
	double sum;

	for (i=1;i<=n;i++) {
		ip=indx[i];
		sum=b[ip];
		b[ip]=b[i];
		if (ii)
			for (j=ii;j<=i-1;j++) sum -= a[i][j]*b[j];
		else if (sum) ii=i;
		b[i]=sum;
	}
	for (i=n;i>=1;i--) {
		sum=b[i];
		for (j=i+1;j<=n;j++) sum -= a[i][j]*b[j];
		b[i]=sum/a[i][i];
	}
}

/* (C) Copr. 1986-92 Numerical Recipes Software |oV'41.5.){2p49. */

#undef TINY
#undef NRANSI
