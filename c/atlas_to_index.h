/* Copyright 10/2/15 Washington University.  All Rights Reserved.
   atlas_to_index.h  $Revision: 1.3 $*/
/*source af3d_xyzindex*/

//START160621
#include "get_atlas_param.h"

void atlas_to_index(int ncoor,double **atlascoor,Atlas_Param *ap,int *xi,int *yi,int *zi,double *xd,double *yd,double *zd,int *index);
void atlas_to_index2(int ncoor,double *coor,Atlas_Param *ap,int *xi,int *yi,int *zi,double *xd,double *yd,double *zd,int *index);
