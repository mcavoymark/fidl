/* Copyright 4/18/11 Washington University.  All Rights Reserved.
   sphere_nvox.c  $Revision: 1.8 $*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#ifndef __sun__
    #include <stdint.h>
#endif
#include "fidl.h"
int _sphere_nvox(int argc,char **argv){
    #ifdef __sun__
        int vol=(int)argv[0],xdim=(int)argv[1],ydim=(int)argv[2],zdim=(int)argv[3];
    #else
        int vol=(intptr_t)argv[0],xdim=(intptr_t)argv[1],ydim=(intptr_t)argv[2],zdim=(intptr_t)argv[3];
    #endif
    float diameter = *(float*)argv[4];
    float voxel_size = *(float*)argv[5];
    int nvox;
    float *temp_float;
    /*printf("vol=%d xdim=%d ydim=%d zdim=%d diameter=%f voxel_size=%f\n",vol,xdim,ydim,zdim,diameter,voxel_size); fflush(stdout);*/
    if(!(temp_float=malloc(sizeof*temp_float*vol))) {
        printf("Error: Unable to malloc temp_float\n");
        return 0;
        }
    nvox = sphere_nvox(vol,temp_float,xdim,ydim,zdim,diameter,voxel_size);
    free(temp_float);
    return nvox;
    }
int sphere_nvox(int vol,float *temp_float,int xdim,int ydim,int zdim,float diameter,float voxel_size){
    int i,x,y,zi,nvox;
    for(i=0;i<vol;i++) temp_float[i]=0.;
    x=xdim/2;y=ydim/2;zi=zdim/2;
    if(!(nvox=make_sphere_new((double)x,(double)y,(double)zi,temp_float,1.,xdim,ydim,zdim,diameter,voxel_size,(FILE*)NULL))) {
        printf("Error: nvox=%d Must be greater than zero.\n",nvox);
        return 0;
        }
    printf("%f mm diameter is a %d voxel sphere.\n",diameter,nvox); fflush(stdout);
    return nvox;
    }
int make_sphere_new(double x,double y,double z,float *temp_float,float value,int xdim,int ydim,int zdim,float diameter,
    float voxel_size,FILE *fp){ /*This one rounds to integers at the end.*/
    int i,j,k,xstart,xend,ystart,yend,zstart,zend,index,nvox=0;
    double radius;
    radius = (double)diameter/2./voxel_size;
    zstart = (int)rint(z-radius < 0. ? 0. : z-radius);
    zend = (int)rint(z+radius > (double)(zdim-1) ? (double)(zdim-1) : z+radius);
    ystart = (int)rint(y-radius < 0. ? 0. : y-radius);
    yend = (int)rint(y+radius > (double)(ydim-1) ? (double)(ydim-1) : y+radius);
    xstart = (int)rint(x-radius < 0. ? 0. : x-radius);
    xend = (int)rint(x+radius > (double)(xdim-1) ? (double)(xdim-1) : x+radius);
    for(k=zstart;k<=zend;k++)
        for(j=ystart;j<=yend;j++)
            for(i=xstart;i<=xend;i++)
                if(sqrt((k-z)*(k-z)+(y-j)*(y-j)+(x-i)*(x-i)) <= radius) {
                    index = (k-1)*xdim*ydim + j*xdim + i;
                    if(!temp_float[index]) {
                        temp_float[index] = value;
                        nvox++;
                        }
                    else {
                        /*printf("Information:     Voxel belongs to region %d (ifh code)\n",(int)temp_float[index]-2);*/
                        if(fp) fprintf(fp,"    Voxel belongs to region %d (ifh code)\n",(int)temp_float[index]-2);
                        }
                    }
    return nvox;
    }
int make_sphere(int x,int y,int z,float *temp_float,float value,int xdim,int ydim,int zdim,float diameter,float voxel_size,FILE *fp){ 
    /*This one rounds to integers right away.*/
    int i,j,k,radius_int,xstart,xend,ystart,yend,zstart,zend,index,nvox=0;
    float radius;
    radius = diameter/2/voxel_size;
    radius_int = (int)radius;
    zstart = z-radius_int < 0 ? 0 : z-radius_int;
    zend = z+radius_int > zdim-1 ? zdim-1 : z+radius_int;
    ystart = y-radius_int < 0 ? 0 : y-radius_int;
    yend = y+radius_int > ydim-1 ? ydim-1 : y+radius_int;
    xstart = x-radius_int < 0 ? 0 : x-radius_int;
    xend = x+radius_int > xdim-1 ? xdim-1 : x+radius_int;
    for(k=zstart;k<=zend;k++)
        for(j=ystart;j<=yend;j++)
            for(i=xstart;i<=xend;i++)
                if(sqrt((k-z)*(k-z)+(y-j)*(y-j)+(x-i)*(x-i)) <= radius) {
                    index = (k-1)*xdim*ydim + j*xdim + i;
                    if(!temp_float[index]) {
                        temp_float[index] = value;
                        nvox++;
                        }
                    else {
                        /*printf("Information:     Voxel belongs to region %d (ifh code)\n",(int)temp_float[index]-2);*/
                        if(fp) fprintf(fp,"    Voxel belongs to region %d (ifh code)\n",(int)temp_float[index]-2);
                        }
                    }
    return nvox;
    }
