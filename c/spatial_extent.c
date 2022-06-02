/* Copyright 12/31/99 Washington University.  All Rights Reserved.
   spatial_extent.c  $Revision: 1.12 $*/
/**********************************************************************************************
Given a threshold, measures the spatial extent of the activation.
Arguments: 
    sigprob:      Signficance probability.
    actmask:      FIDL region file. First region has a value of 2.
    n_reg_size:   Array of detected region sizes.
    connectivity: Always face.
**********************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#ifndef __sun__
    #include <stdint.h>
#endif
#include "fidl.h"
int _spatial_extent(int argc,char **argv)
{
    float *sigprob,*actmask,thresh;
    int xdim,ydim,zdim,num_thresh,extent,vol,p_or_z,num_sig_reg=0;
    sigprob = (float*)argv[0];
    actmask = (float*)argv[1];
    thresh = *(float*)argv[5]; 
    #ifdef __sun__
        xdim = (int)argv[2];
        ydim = (int)argv[3];
        zdim = (int)argv[4];
        num_thresh = (int)argv[6]; 
        extent = (int)argv[7];
        vol = (int)argv[8];
        p_or_z = (int)argv[9];
    #else
        xdim = (intptr_t)argv[2];
        ydim = (intptr_t)argv[3];
        zdim = (intptr_t)argv[4];
        num_thresh = (intptr_t)argv[6]; 
        extent = (intptr_t)argv[7];
        vol = (intptr_t)argv[8];
        p_or_z = (intptr_t)argv[9];
    #endif

    /*printf("xdim=%d ydim=%d zdim=%d thresh=%f num_thresh=%f extent=%d vol=%d p_or_z=%d\n",xdim,ydim,zdim,thresh,num_thresh,
        extent,vol,p_or_z);*/

    num_sig_reg=spatial_extent(sigprob,actmask,xdim,ydim,zdim,&thresh,num_thresh,&extent,&vol,1);
    return num_sig_reg;
}

/*********************************************************/
int spatial_extent(
	float *sigprob,
	float *actmask,
	int xdim,
	int ydim,
	int zdim,
	float *thresh,int num_thresh,
        int *n1,int *n2,int p_or_z)
/*********************************************************/
{
float	*tmpimg;

int	i,th,*off,*voxlist,n_srch=0,n_reg=0,vox,vox1,plndim,*reglist,
	n_nghbrs,*offsets,*offsets_pln0,num_sig_reg=0,
	*offsets_plnN,n_nghbrs_end,n_nghbrs_middle,vol;

plndim = xdim*ydim;
vol = xdim*ydim*zdim;
if(!(offsets=malloc(sizeof*offsets*6))) {
    printf("Unable to malloc offsets\n");
    return -1;
    }
if(!(offsets_pln0=malloc(sizeof*offsets_pln0*5))) {
    printf("Unable to malloc offsets_pln0\n");
    return -1;
    }
if(!(offsets_plnN=malloc(sizeof*offsets_plnN*5))) {
    printf("Unable to malloc offsets_plnN\n");
    return -1;
    }
if(!(tmpimg=malloc(sizeof*tmpimg*vol))) {
    printf("Unable to malloc tmpimg\n");
    return -1;
    }
if(!(voxlist=malloc(sizeof*voxlist*vol))) {
    printf("Unable to malloc voxlist\n");
    return -1;
    }
if(!(reglist=malloc(sizeof*reglist*vol))) {
    printf("Unable to malloc reglist\n");
    return -1;
    }

n_nghbrs_middle = (int)N_NGHBRS_FACE;
n_nghbrs_end = (int)N_NGHBRS_END_FACE;
offsets[0] = 1; offsets[1] = -1; offsets[2] = -xdim; 
offsets[3] = xdim; offsets[4] = plndim; offsets[5] = -plndim; 
offsets_pln0[0] = 1; offsets_pln0[1] = -1; offsets_pln0[2] = -xdim; 
offsets_pln0[3] = xdim; offsets_pln0[4] = plndim;
offsets_plnN[0] = 1; offsets_plnN[1] = -1; offsets_plnN[2] = -xdim; 
offsets_plnN[3] = xdim; offsets_plnN[4] = -plndim;

/* mult_comp num_thresh = (int)MAX_REG_SIZE
   mult_spm  num_thresh = 1 */

for(th=0;th<num_thresh;th++) {
    for(i=0;i<vol;i++) tmpimg[i] = sigprob[i];
    for(vox=xdim+1;vox<vol-xdim-2;vox++) {
        if(vox < xdim+1 || vox > vol-xdim-2) {
            /* Skip first and last row in volume. */
	    n_nghbrs = 0;
	    }
        else if(vox < plndim+xdim+1) {
	    n_nghbrs = n_nghbrs_end;
	    off = offsets_pln0;
	    }
        else if(vox > vol-(plndim+xdim)-2) {
	    n_nghbrs = n_nghbrs_end;
	    off = offsets_plnN;
	    }
        else {
	    n_nghbrs = n_nghbrs_middle;
	    off = offsets;
	    }
        /* mult_comp threshold = thresh[th]
           mult_spm  threshold = thresh_spm */
    
        if(!p_or_z) {
            if(tmpimg[vox] < thresh[th]) {
	        tmpimg[vox] = thresh[th];
                n_srch = n_reg = 0;
	        reglist[n_reg++] = vox; 
                for(i=0;i<n_nghbrs;i++) {
                    if(tmpimg[vox+off[i]] < thresh[th]) {
                        tmpimg[vox+off[i]] = thresh[th];
                        voxlist[n_srch++] = vox + off[i];
                        reglist[n_reg++] = vox+off[i];
                        }
                    }
                while(n_srch > 0) {
                    vox1 = voxlist[--n_srch];
                    if(vox1 < xdim+1 || vox1 > vol-xdim-2) {
                        /* Skip first and last row in volume. */
                        n_nghbrs = 0;
                        }
                    else if(vox1 < plndim+xdim+1) {
                        n_nghbrs = n_nghbrs_end;
                        off = offsets_pln0;
                        }
                    else if(vox1 > vol - (plndim+xdim)-2) {
                        n_nghbrs = n_nghbrs_end;
                        off = offsets_plnN;
                        }
                    else {
                        n_nghbrs = n_nghbrs_middle;
                        off = offsets;
                        }
                    for(i=0;i<n_nghbrs;i++) {
                        if(tmpimg[vox1+off[i]] < thresh[th]) {
                            tmpimg[vox1+off[i]] = thresh[th];
                            voxlist[n_srch++] = vox1 + off[i];
                            reglist[n_reg++] = vox1+off[i];
                            }
                        }
                    }
                if(n_reg >= n1[th] && n_reg <= n2[th]) {
                    if(num_thresh == 1)
                        for(i=0;i<n_reg;i++) actmask[reglist[i]] = num_sig_reg+2;
                    else
                        for(i=0;i<n_reg;i++) actmask[reglist[i]] = 1;
                    ++num_sig_reg;
                    }
                }
            }
        else {
            if(fabs(tmpimg[vox]) > thresh[th]) {
	        tmpimg[vox] = thresh[th];
                n_srch = n_reg = 0;
	        reglist[n_reg++] = vox; 
                for(i=0;i<n_nghbrs;i++) {
                    if(fabs(tmpimg[vox+off[i]]) > thresh[th]) {
                        tmpimg[vox+off[i]] = thresh[th];
                        voxlist[n_srch++] = vox + off[i];
                        reglist[n_reg++] = vox+off[i];
                        }
                    }
                while(n_srch > 0) {
                    vox1 = voxlist[--n_srch];
                    if(vox1 < xdim+1 || vox1 > vol-xdim-2) {
                        /* Skip first and last row in volume. */
                        n_nghbrs = 0;
                        }
                    else if(vox1 < plndim+xdim+1) {
                        n_nghbrs = n_nghbrs_end;
                        off = offsets_pln0;
                        }
                    else if(vox1 > vol - (plndim+xdim)-2) {
                        n_nghbrs = n_nghbrs_end;
                        off = offsets_plnN;
                        }
                    else {
                        n_nghbrs = n_nghbrs_middle;
                        off = offsets;
                        }
                    for(i=0;i<n_nghbrs;i++) {
                        if(fabs(tmpimg[vox1+off[i]]) > thresh[th]) {
                            tmpimg[vox1+off[i]] = thresh[th];
                            voxlist[n_srch++] = vox1 + off[i];
                            reglist[n_reg++] = vox1+off[i];
                            }
                        }
                    }
                if(n_reg >= n1[th] && n_reg <= n2[th]) {
                    if(num_thresh == 1)
                        for(i=0;i<n_reg;i++) actmask[reglist[i]] = num_sig_reg+2;
                    else
                        for(i=0;i<n_reg;i++) actmask[reglist[i]] = 1;
                    ++num_sig_reg;
                    }
                }
            }
        }
    } /*for(th=0;th<num_thresh;th++)*/

free(tmpimg);
free(voxlist);
free(reglist);
free(offsets);
free(offsets_pln0);
free(offsets_plnN);

return num_sig_reg;
}
