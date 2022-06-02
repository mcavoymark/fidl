/* Copyright 7/29/07 Washington University.  All Rights Reserved.
   meschach_pinv.c  $Revision: 1.4 $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <fidl.h>
/*#include "matrix2.h"*/

void meschach_pinv(MAT *A,MAT *Ainv)
{
    static MAT *Acopy,*U,*V;
    static VEC *d;
    int i,j,maxdim;
    Acopy=m_get(A->m,A->n);
    U=m_get(A->m,A->m);
    V=m_get(A->n,A->n);
    maxdim = max(A->m,A->n);
    d=v_get(maxdim);
    m_copy(A,Acopy);
    svd(Acopy,U,V,d);
    for(i=0;i<maxdim;i++) for(j=0;j<maxdim;j++) V->me[i][j]/=d->ve[j];
    Ainv=mmtr_mlt(V,U,Ainv);
    m_free(Acopy);
    m_free(U);
    m_free(V);
    v_free(d);
}
