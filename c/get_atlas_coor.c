/* Copyright 5/18/05 Washington University.  All Rights Reserved.
   get_atlas_coor.c  $Revision: 1.9 $*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "fidl.h"
#include "get_atlas_coor.h"
void print_atlas_coor(int index,Atlas_Param *ap){
    double x,y,z,coor[3];
    col_row_slice(1,&index,&x,&y,&z,ap);
    get_atlas_coor(1,&x,&y,&z,(double)ap->zdim,ap->center,ap->mmppix,coor);
    printf("    atlas coordinates: %d %d %d\n",(int)rint(coor[0]),(int)rint(coor[1]),(int)rint(coor[2]));
    }

void get_atlas_coor(int n,double *col,double *row,double *slice,double zdim,double *center,double *mmppix,double *atlas_coor){
    /* slice+1             col                 row    [fidl coordinates = (col,row)]
       z_fidl              x_fidl              y_fidl
       fidl_coordinates[2] fidl_coordinates[0] fidl_coordinates[1] */
    #if 0
    fidl_coordinates[0] = col;
    fidl_coordinates[1] = row;
    fidl_coordinates[2] = slice+1;
    #endif

    int i,j;

    /*printf("center = %f %f %f\n",center[0],center[1],center[2]);
    printf("mmppix = %f %f %f\n",mmppix[0],mmppix[1],mmppix[2]);*/


    #if 0
    for(j=i=0;i<n;i++) {
        /*printf("col[%d]=%f row[%d]=%f slice[%d]=%f\n",i,col[i],i,row[i],i,slice[i]);*/
        atlas_coor[j++] = (int)rint(center[0] - mmppix[0]*(col[i]+1.));
        atlas_coor[j++] = (int)rint(mmppix[1]*(row[i]+1.) - center[1]);
        atlas_coor[j++] = (int)rint(mmppix[2]*(zdim-slice[i]) - center[2]);
        }
    #endif

    for(j=i=0;i<n;i++) {
        /*printf("col[%d]=%f row[%d]=%f slice[%d]=%f\n",i,col[i],i,row[i],i,slice[i]);*/
        atlas_coor[j++] = center[0] - mmppix[0]*(col[i]+1.);
        atlas_coor[j++] = mmppix[1]*(row[i]+1.) - center[1];
        atlas_coor[j++] = mmppix[2]*(zdim-slice[i]) - center[2];
        }

    #if 0
    for(j=i=0;i<n;i++) {
        printf("%d %d %d",atlas_coor[j++],atlas_coor[j++],atlas_coor[j++]);
        printf("\n");
        }
    #endif
    }

//START190124
void get_atlas_coorf(int n,double *col,double *row,double *slice,double zdim,double *center,double *mmppix,float *atlas_coor){
    /* slice+1             col                 row    [fidl coordinates = (col,row)]
       z_fidl              x_fidl              y_fidl
       fidl_coordinates[2] fidl_coordinates[0] fidl_coordinates[1] */

    int i,j;

    for(j=i=0;i<n;i++) {
        atlas_coor[j++] = (float)(center[0] - mmppix[0]*(col[i]+1.));
        atlas_coor[j++] = (float)(mmppix[1]*(row[i]+1.) - center[1]);
        atlas_coor[j++] = (float)(mmppix[2]*(zdim-slice[i]) - center[2]);
        }
    }

//START190508
void get_atlas_coorff(int n,double *col,double *row,double *slice,double zdim,float *center,float *mmppix,float *atlas_coor){
    /* slice+1             col                 row    [fidl coordinates = (col,row)]
       z_fidl              x_fidl              y_fidl
       fidl_coordinates[2] fidl_coordinates[0] fidl_coordinates[1] */

    int i,j;

    for(j=i=0;i<n;i++) {
        atlas_coor[j++] = (float)(center[0] - mmppix[0]*(col[i]+1.));
        atlas_coor[j++] = (float)(mmppix[1]*(row[i]+1.) - center[1]);
        atlas_coor[j++] = (float)(mmppix[2]*(zdim-slice[i]) - center[2]);
        }
    }


void col_row_slice(int n,int *index,double *dcol,double *drow,double *dslice,Atlas_Param *ap){
    int i,slice,col_row,col,row; /*Must be integers.*/
    for(i=0;i<n;i++) {
        slice = index[i]/ap->area;
        col_row = index[i]-slice*ap->area;
        row = col_row/ap->xdim;
        col = col_row - row*ap->xdim;
        dcol[i] = (double)col;
        drow[i] = (double)row;
        dslice[i] = (double)slice;
        }
    }

//START190128
void col_row_slice2(int n,int *index,double *dcol,double *drow,double *dslice,int xdim,int ydim){
    int i,slice,col_row,col,row,area=xdim*ydim; /*Must be integers.*/
    for(i=0;i<n;i++) {
        slice = index[i]/area;
        col_row = index[i]-slice*area;
        row = col_row/xdim;
        col = col_row - row*xdim;
        dcol[i] = (double)col;
        drow[i] = (double)row;
        dslice[i] = (double)slice;
        }
    }

