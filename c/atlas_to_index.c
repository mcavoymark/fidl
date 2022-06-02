/* Copyright 10/2/15 Washington University.  All Rights Reserved.
   atlas_to_index.c  $Revision: 1.3 $*/
/*source af3d_xyzindex*/

//START160621
//#define _POSIX_C_SOURCE 200112L
#include <math.h>

#include "atlas_to_index.h"
void atlas_to_index(int ncoor,double **atlascoor,Atlas_Param *ap,int *xi,int *yi,int *zi,double *xd,double *yd,double *zd,
    int *index){
    double td;
    int i;
    for(i=0;i<ncoor;i++) {
        td = (ap->center[0]-atlascoor[i][0])/ap->mmppix[0];
        xi[i] = (int)rint(td) - 1;
        xd[i] = td - 1.;
        td = (ap->center[1]+atlascoor[i][1])/ap->mmppix[1];
        yi[i] = (int)rint(td) - 1;
        yd[i] = td - 1.;
        td = (ap->center[2]+atlascoor[i][2])/ap->mmppix[2];
        zi[i] = ap->zdim - (int)rint(td) + 1;
        zd[i] = (double)ap->zdim - td + 1.;
        index[i] = (zi[i]-1)*ap->xdim*ap->ydim + yi[i]*ap->xdim + xi[i];
        }
    }
void atlas_to_index2(int ncoor,double *coor,Atlas_Param *ap,int *xi,int *yi,int *zi,double *xd,double *yd,double *zd,int *index){
    double td;
    int i;
    for(i=0;i<ncoor;i++) {
        td = (ap->center[0]-*coor++)/ap->mmppix[0];
        xi[i] = (int)rint(td) - 1;
        xd[i] = td - 1.;
        td = (ap->center[1]+*coor++)/ap->mmppix[1];
        yi[i] = (int)rint(td) - 1;
        yd[i] = td - 1.;
        td = (ap->center[2]+*coor++)/ap->mmppix[2];
        zi[i] = ap->zdim - (int)rint(td) + 1;
        zd[i] = (double)ap->zdim - td + 1.;
        index[i] = (zi[i]-1)*ap->xdim*ap->ydim + yi[i]*ap->xdim + xi[i];
        }
    }
