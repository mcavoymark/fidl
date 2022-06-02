/* Copyright 5/2/05 Washington University.  All Rights Reserved.
   pca.c  $Revision: 1.4 $ */

/*#include <stdio.h>
#include <stdlib.h>
#include <string.h>*/
#include <math.h>

/*#include <fidl.h>*/
/*START150323*/
#include "fidl.h"

#include "nrutil.h"

void tred2(double **a,int n,double d[],double e[]);
int tqli(double d[],double e[],int n,double **z);
void eigsrt(double d[],double **v,int n);
double pythag(double a,double b);




#if 0
PC *pca(double *x,int nr,int nc)
/* x[row 1,row 2,...,row nr] Each row has nc values. */
{
    PC *pc;
    double *e;
    int i,j,ii,jj,kk;
    
    if(!(pc=malloc(sizeof*pc))) {
        printf("Error: Unable to malloc pc\n");
        return NULL;
        }

    /*KEEP THIS*/
    #if 0
    /* xx' */
    pc->EIGVEC = dmatrix(1,nr,1,nr);
    pc->EIGVAL = dvector(1,nr);
    pc->n = nr;
    e = dvector(1,nr);
    for(ii=0,i=1;i<=nr;i++,ii++) {
        for(jj=ii,j=i;j<=nr;j++,jj++) {
            pc->EIGVEC[i][j] = 0.;
            for(kk=0;kk<nc;kk++) pc->EIGVEC[i][j] += x[ii*nc+kk]*x[jj*nc+kk];
            pc->EIGVEC[j][i] = pc->EIGVEC[i][j]; 
            }
        }
    #endif

    /* x'x */
    pc->EIGVEC = dmatrix(1,nc,1,nc);
    pc->EIGVAL = dvector(1,nc);
    pc->n = nc;
    e = dvector(1,nc);
    for(ii=0,i=1;i<=nc;i++,ii++) {
        for(jj=ii,j=i;j<=nc;j++,jj++) {
            pc->EIGVEC[i][j] = 0.;
            for(kk=0;kk<nr;kk++) pc->EIGVEC[i][j] += x[kk*nc+ii]*x[kk*nc+jj];
            pc->EIGVEC[j][i] = pc->EIGVEC[i][j]; 
            }
        }



    tred2(pc->EIGVEC,pc->n,pc->EIGVAL,e);
    if(!tqli(pc->EIGVAL,e,pc->n,pc->EIGVEC)) return NULL;
    eigsrt(pc->EIGVAL,pc->EIGVEC,pc->n);
    free_dvector(e,1,pc->n);
    return pc;
}
#endif

PC *pca(double *x,int nr,int nc)
/* x[row 1,row 2,...,row nr] Each row has nc values. */
{
    PC *pc;
    int i,j,ii,jj,kk;

    /* x'x */
    if(!(pc=allocate_PC(nc))) return NULL;

    for(ii=0,i=1;i<=nc;i++,ii++) {
        for(jj=ii,j=i;j<=nc;j++,jj++) {
            pc->EIGVEC[i][j] = 0.;
            for(kk=0;kk<nr;kk++) pc->EIGVEC[i][j] += x[kk*nc+ii]*x[kk*nc+jj];
            pc->EIGVEC[j][i] = pc->EIGVEC[i][j];
            }
        }
    if(!eig(pc)) return NULL;
    return pc;
}

int eig(PC *pc)
{
    double *e;

    e = dvector(1,pc->n);
    tred2(pc->EIGVEC,pc->n,pc->EIGVAL,e);
    if(!tqli(pc->EIGVAL,e,pc->n,pc->EIGVEC)) return 0;
    eigsrt(pc->EIGVAL,pc->EIGVEC,pc->n);
    free_dvector(e,1,pc->n);
    return 1;
}

PC *allocate_PC(int nc)
{
    PC *pc;

    if(!(pc=malloc(sizeof*pc))) {
        printf("Error: Unable to malloc pc\n");
        return NULL;
        }
    pc->EIGVEC = dmatrix(1,nc,1,nc);
    pc->EIGVAL = dvector(1,nc);
    pc->n = nc;
    return pc;
}

void free_pc(PC *pc)
{
    free_dmatrix(pc->EIGVEC,1,pc->n,1,pc->n);
    free_dvector(pc->EIGVAL,1,pc->n);
    free(pc);
}
