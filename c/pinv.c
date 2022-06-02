/*-------------------------------------------------------------

Function:  pinv

Purpose: Compute generalized inverse of a matrix.

By: John Ollinger

Date: 9/17/98

---------------------------------------------------------------*/

/*$Revision: 12.85 $*/

#include        <stdio.h>
#include        <math.h>
#include        <fidl.h>
#include        <nrutil.h>

#ifndef __sun__
    #include <stdint.h>
#endif


int _pinv(int argc,char **argv)
{
    double *Ain,*pseudo_inverse,**A,**pinv_tmp,*epsilon;
    int i,j,joff,Nrow,Mcol;
    Ain = (double *)argv[0];
    pseudo_inverse = (double *)argv[1];
    #ifdef __sun__
        Nrow = (int)argv[2];
        Mcol = (int)argv[3];
    #else
        Nrow = (intptr_t)argv[2];
        Mcol = (intptr_t)argv[3];
    #endif
    epsilon = (double *)argv[4];

/* Copy into c-recipe format. */
A = dmatrix(1,Nrow,1,Mcol);
for(i=1,joff=0;i<=Nrow;i++,joff+=Mcol) {
    for(j=1;j<=Mcol;j++)
        A[i][j] = Ain[j+joff-1];
    }

/* Compute pseudo-inverse. */
pinv_tmp = pinv_old(A,Nrow,Mcol,*epsilon);

/* Return to same format. */
for(i=1,joff=0;i<=Nrow;i++,joff+=Mcol) {
    for(j=1;j<=Mcol;j++)
        pseudo_inverse[j+joff-1] = (double) pinv_tmp[i][j];
    }

free_dmatrix(A,1,Nrow,1,Mcol);
free_dmatrix(pinv_tmp,1,Nrow,1,Mcol);
return(0);
}





#if 1
/*****************************************************/
double **pinv_old(double **A,int Nrow,int Mcol,double epsilon)
/*****************************************************/
/* A: Matrix to be inverted.
   Nrow: Row dimension of A.
   Mcol: Column dimension of A.
   epsilon: Threshold below which singular values are assumed to lie in
            the null space.  Set to zero to use default. */
{
double	*W,**V,**U,**pseudo_inverse,Wmax;
int	i,j,joff,maxdim;

W   = dvector(1,Nrow);
U = dmatrix(1,Nrow,1,Mcol);
V = dmatrix(1,Nrow,1,Mcol);

/* Copy input matrix to keep from destroying it. */
for(i=1,joff=0;i<=Nrow;i++,joff+=Mcol) {
    for(j=1;j<=Mcol;j++)
        U[i][j] = A[i][j];
    }

/* Find singular value decomposition. */
svdcmp(U,Nrow,Mcol,W,V);

for(i=1,Wmax = 0.;i<=Mcol;i++) {
    if(W[i] > Wmax)
        Wmax = W[i];
    }

/* Compute epsilon. */
if(epsilon <= 0.) {
    if(Mcol > Nrow) maxdim = Mcol; else maxdim = Nrow;
    epsilon = (double)maxdim*pow((double).5,(double)48.)*Wmax;
    }

for(i=1;i<=Mcol;i++) {
    if(W[i] < epsilon)
        W[i] = 0.;
    else
        W[i] = 1./W[i];
    }

/* Form V*Winv. */
for(i=1;i<=Mcol;i++) {
    for(j=1;j<=Mcol;j++)
        V[i][j] *= W[j];
    }

/* Form V*Winv*UT */
pseudo_inverse = dmatrix_mult(V,U,Mcol,Mcol,Mcol,Nrow,TRANSPOSE_SECOND);

free_dmatrix(U,1,Nrow,1,Mcol);
free_dmatrix(V,1,Nrow,1,Mcol);
free_dvector(W,1,Nrow);
return(pseudo_inverse);
}
#endif

#if 0
double **pinv(double **A,int Nrow,int Mcol,double epsilon,double **Ainv)
/* A: Matrix to be inverted.
   Nrow: Row dimension of A.
   Mcol: Column dimension of A.
   epsilon: Threshold below which singular values are assumed to lie in
            the null space.  Set to zero to use default. */
{
double  *W,**V,**U,**pseudo_inverse,Wmax;
int     i,j,joff,maxdim;

W = dvector(1,Nrow);
U = dmatrix(1,Nrow,1,Mcol);
V = dmatrix(1,Nrow,1,Mcol);

/* Copy input matrix to keep from destroying it. */
for(i=1,joff=0;i<=Nrow;i++,joff+=Mcol) {
    for(j=1;j<=Mcol;j++)
        U[i][j] = A[i][j];
    }

/* Find singular value decomposition. */
svdcmp(U,Nrow,Mcol,W,V);

for(i=1,Wmax = 0.;i<=Mcol;i++) {
    if(W[i] > Wmax)
        Wmax = W[i];
    }

/* Compute epsilon. */
if(epsilon <= 0.) {
    if(Mcol > Nrow) maxdim = Mcol; else maxdim = Nrow;
    epsilon = (double)maxdim*pow((double).5,(double)48.)*Wmax;
    }

for(i=1;i<=Mcol;i++) {
    if(W[i] < epsilon)
        W[i] = 0.;
    else
        W[i] = 1./W[i];
    }

/* Form V*Winv. */
for(i=1;i<=Mcol;i++) {
    for(j=1;j<=Mcol;j++)
        V[i][j] *= W[j];
    }

/* Form V*Winv*UT */
if(!Ainv) {
    pseudo_inverse = dmatrix_mult(V,U,Mcol,Mcol,Mcol,Nrow,TRANSPOSE_SECOND);
    }
else {
    dmatrix_mult_nomem(V,U,Mcol,Mcol,Mcol,Nrow,(int)TRANSPOSE_SECOND,Ainv);
    pseudo_inverse = Ainv;
    }

free_dvector(W,1,Nrow);
free_dmatrix(U,1,Nrow,1,Mcol);
free_dmatrix(V,1,Nrow,1,Mcol);
return pseudo_inverse;
}
#endif

void pinv(double **A,int Nrow,int Mcol,double epsilon,double **Ainv)
/* A: Matrix to be inverted.
   Nrow: Row dimension of A.
   Mcol: Column dimension of A.
   epsilon: Threshold below which singular values are assumed to lie in
            the null space.  Set to zero to use default. */
{
double  *W,**V,**U,Wmax;
int     i,j,joff,maxdim,rtn=1;

W = dvector(1,Nrow);
U = dmatrix(1,Nrow,1,Mcol);
V = dmatrix(1,Nrow,1,Mcol);

/* Copy input matrix to keep from destroying it. */
for(i=1,joff=0;i<=Nrow;i++,joff+=Mcol) {
    for(j=1;j<=Mcol;j++)
        U[i][j] = A[i][j];
    }

/* Find singular value decomposition. */
svdcmp(U,Nrow,Mcol,W,V);

for(i=1,Wmax = 0.;i<=Mcol;i++) {
    if(W[i] > Wmax)
        Wmax = W[i];
    }

/* Compute epsilon. */
if(epsilon <= 0.) {
    if(Mcol > Nrow) maxdim = Mcol; else maxdim = Nrow;
    epsilon = (double)maxdim*pow((double).5,(double)48.)*Wmax;
    }

for(i=1;i<=Mcol;i++) {
    if(W[i] < epsilon)
        W[i] = 0.;
    else
        W[i] = 1./W[i];
    }

/* Form V*Winv. */
for(i=1;i<=Mcol;i++) {
    for(j=1;j<=Mcol;j++)
        V[i][j] *= W[j];
    }

/* Form V*Winv*UT */
dmatrix_mult_nomem(V,U,Mcol,Mcol,Mcol,Nrow,(int)TRANSPOSE_SECOND,Ainv);

free_dvector(W,1,Nrow);
free_dmatrix(U,1,Nrow,1,Mcol);
free_dmatrix(V,1,Nrow,1,Mcol);
}

