/* Copyright 12/31/99 Washington University.  All Rights Reserved.
   mult_spm.c  $Revision: 12.87 $ */
/*********************************************************************

Function: mult_spm

Purpose: Correct for multiple comparisons Using Friston (1994) spatial
extent method..

Usage: Call from idl using call_external function

Arguments: 
    sigprob:  Signficance probability.
    actmask:  FIDL region file. First region has a value of 2. 
    n_reg_size: Array of detected region sizes.
    min_reg:  Minimum region size to be accepted.
    connectivity: Always face.

By: John Ollinger
************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <fidl.h>
#ifndef __sun__
    #include <stdint.h>
#endif

#define N_NGHBRS_FACE 6
#define N_NGHBRS_END_FACE 5
#define MAX_REG_SIZE 10  /* Maximum region size for which a new 
			    threshold is calculated.  All larger regions
			    use this threshold. */

int _mult_spm(int argc,char **argv)
{
    float *sigprob,*actmask,thresh_spm;
    int xdim,ydim,zdim,min_reg,num_sig_reg;
    sigprob = (float *)argv[0];
    actmask = (float *)argv[1];
    thresh_spm = *((float *)argv[3]);
    #ifdef __sun__
        min_reg = (int)argv[2];
        xdim = (int)argv[4];
        ydim = (int)argv[5];
        zdim = (int)argv[6];
    #else
        min_reg = (intptr_t)argv[2];
        xdim = (intptr_t)argv[4];
        ydim = (intptr_t)argv[5];
        zdim = (intptr_t)argv[6];
    #endif

num_sig_reg = mult_spm(sigprob,actmask,xdim,ydim,zdim,min_reg,thresh_spm);
return (int)num_sig_reg;
}


/*************************************************************************************************/
int mult_spm(float *sigprob,float *actmask,int xdim,int ydim,int zdim,int min_reg,float thresh_spm)
/*************************************************************************************************/
{

int     i,*n1,*n2,vol_length,num_sig_reg;

float   *thresh;

/*GETMEM(n1,1,int)*/
if(!(n1=(int*)d1(sizeof(int),1))) return -1;
n1[0] = min_reg;

/*GETMEM(n2,1,int)*/
if(!(n2=(int*)d1(sizeof(int),1))) return -1;
n2[0] = xdim*ydim*zdim; /*Allow any large region.*/

/*GETMEM(thresh,1,float)*/
if(!(thresh=(float*)d1(sizeof(float),1))) return -1;
thresh[0] = thresh_spm;

/*num_sig_reg = spatial_extent(sigprob,actmask,xdim,ydim,zdim,thresh,1,n1,n2);*/
num_sig_reg = spatial_extent(sigprob,actmask,xdim,ydim,zdim,thresh,1,n1,n2,0);


free(n1);
free(n2);
free(thresh);

return num_sig_reg;
}

