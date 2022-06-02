/* Copyright 11/19/18 Washington University.  All Rights Reserved.
   ellipsestack.c  $Revision: 1.1 $ */

#include "ellipsestack.h"

int ellipsestack(int N,int *x,int *y,int *z,float *stack,float *value,int xdim,int ydim,int zdim,float *xdia,float *ydia,
    float *zdia,float xvoxelsize,float yvoxelsize,float zvoxelsize)
{
    int i,j,k,l,xstart,xend,ystart,yend,zstart,zend,index,nvox=0; /*xradi,yradi,zradi*/
    float xrad,yrad,zrad,xrad2,yrad2,zrad2;
    for(l=0;l<N;l++) {
        xrad = xdia[l]/2./xvoxelsize;
        xrad2 = xrad*xrad;
        yrad = ydia[l]/2./yvoxelsize;
        yrad2 = yrad*yrad;
        zrad = zdia[l]/2./zvoxelsize;
        zrad2 = zrad*zrad;
        zstart = (int)rint(z[l]-zrad < 0. ? 0. : z[l]-zrad);
        zend = (int)rint(z[l]+zrad > (double)(zdim-1) ? (double)(zdim-1) : z[l]+zrad);
        ystart = (int)rint(y[l]-yrad < 0. ? 0. : y[l]-yrad);
        yend = (int)rint(y[l]+yrad > (double)(ydim-1) ? (double)(ydim-1) : y[l]+yrad);
        xstart = (int)rint(x[l]-xrad < 0. ? 0. : x[l]-xrad);
        xend = (int)rint(x[l]+xrad > (double)(xdim-1) ? (double)(xdim-1) : x[l]+xrad);
        for(k=zstart;k<=zend;k++) {
            for(j=ystart;j<=yend;j++) {
                for(i=xstart;i<=xend;i++) {
                    if(((float)((z[l]-k)*(z[l]-k))/zrad2+(float)((y[l]-j)*(y[l]-j))/yrad2+(float)((x[l]-i)*(x[l]-i))/xrad2) <= 1.){
                        index = (k-1)*xdim*ydim + j*xdim + i;
                        if(!stack[index]) {
                            stack[index] = value[l];
                            nvox++;
                            }

                        }
                    }
                }
            }
        }
    return nvox;
}
