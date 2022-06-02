/*********************************************************************

Function: mult_comp

Purpose: Correct for multiple comparisons.

Usage: Call from idl using call_external function

Arguments: 
    sigprob:  Signficance probability.
    actmask:  Binary mask of activated regions.
    n_reg_size: Array of detected region sizes.
    min_reg:  Minimum region size to be accepted.
    connectivity: Always face.

By: John Ollinger

Date: 10/4/96

************************************************************************/

/*$Revision: 12.82 $*/


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <fidl.h>

#define MAXIT 1000
#define N_NGHBRS_FACE 6
#define N_NGHBRS_END_FACE 5
#define NUM_THRESH 5
#define MAX_REG_SIZE 10  /* Maximum region size for which a new 
			    threshold is calculated.  All larger regions
			    use this threshold. */

float 	mKappa,p_tilde;
int 	reg_size;

/************************************/
int _mult_comp_eval(int argc,char **argv)
/************************************/

{

int mult_comp_eval(float *sigprob,float *actmask,int xdim,int ydim,int zdim,int N,int min_reg,float thresh_max,float p_val,float *thresh,float *Prm,float *n_reg_size);

float	*sigprob,*actmask,thresh_max,p_val,*thresh,*Prm,*n_reg_size;

int 	num_sig_reg,xdim,ydim,zdim,N,min_reg;

sigprob    = (float *)argv[0];
actmask    = (float *)argv[1];
min_reg    = (int)argv[2];
thresh_max = *((float *)argv[3]);
xdim = (int)argv[4];
ydim = (int)argv[5];
zdim = (int)argv[6];
N = (int)argv[7];
p_val = *((float *)argv[8]);
thresh = (float *)argv[9];
Prm = (float *)argv[10];
n_reg_size = (float *)argv[11];
/*printf("%d %d %d %f %d %d %d %d %f %f\n",sigprob,actmask,min_reg,thresh_max,xdim,ydim,zdim,N,p_val,thresh_max);*/

num_sig_reg = mult_comp_eval(sigprob,actmask,xdim,ydim,zdim,N,min_reg,thresh_max,p_val,thresh,Prm,n_reg_size);
/*num_sig_reg = spatial_extent(sigprob,actmask,xdim,ydim,zdim,&thresh_max,1,&min_reg,&N);*/
/*printf("num_sig_reg=%d\n",num_sig_reg);*/

/*return((float)num_sig_reg);*/
return num_sig_reg;

}


/*********************************************************/
int mult_comp_eval(
	float *sigprob,
	float *actmask,
	int xdim,
	int ydim,
	int zdim,
	int N,
	int min_reg,
	float thresh_max,
	float p_val,
	float *thresh,
	float *Prm,
        float *n_reg_size)
/*********************************************************/
{

float	*tmpimg,x;

int	i,*off,*voxlist,*reg_list,n_srch,n_reg,vox,vox1,plndim,
	*reglist,th,n1,n2,n_nghbrs,*offsets,*offsets_pln0,
	*offsets_plnN,n_lower[MAX_REG_SIZE],
	n_nghbrs_end,n_nghbrs_middle,vol_length;

int 	num_sig_reg;	/* Number of significant regions detected. */

void 	calc_thresh();

/*printf("%d %d %d %f %d %d %d %d %f %f\n",sigprob,actmask,min_reg,thresh_max,xdim,ydim,zdim,N,p_val,thresh_max);*/

plndim = xdim*ydim;
vol_length = xdim*ydim*zdim;

n_nghbrs_middle = (int)N_NGHBRS_FACE;
n_nghbrs_end = (int)N_NGHBRS_END_FACE;

GETMEM(offsets,6,int)
GETMEM(offsets_pln0,5,int)
GETMEM(offsets_plnN,5,int)
offsets[0] = 1; offsets[1] = -1; offsets[2] = -xdim; 
offsets[3] = xdim; offsets[4] = plndim; offsets[5] = -plndim; 

offsets_pln0[0] = 1; offsets_pln0[1] = -1; offsets_pln0[2] = -xdim; 
offsets_pln0[3] = xdim; offsets_pln0[4] = plndim;

offsets_plnN[0] = 1; offsets_plnN[1] = -1; offsets_plnN[2] = -xdim; 
offsets_plnN[3] = xdim; offsets_plnN[4] = -plndim;

/*calc_thresh(thresh,Prm,p_val,N,min_reg,xdim,ydim,zdim);*/

for(i=0;i<1;i++) {
    thresh[i] = thresh_max;
    }

for(i=0;i<1;i++) {
    if(i<(int)NUM_THRESH)
 	n_lower[i] = i + 1;
    else 
        n_lower[i] = (int)NUM_THRESH;
    if(n_lower[i] < min_reg)
        n_lower[i] = min_reg;
    }

GETMEM(tmpimg,vol_length,float)
GETMEM(voxlist,vol_length,int)
GETMEM(reglist,vol_length,int)
for(i=0;i<vol_length;i++) {
    voxlist[i] = 0;
    reglist[i] = 0;
    }
num_sig_reg = 0;
n_reg = 0;
n_srch = 0;

for(th=0;th<1;th++) {
    for(i=0;i<vol_length;i++)
        tmpimg[i] = sigprob[i];
    n1 = n_lower[th];
    if(n1 < min_reg)
  	n1 = min_reg;
    n2 = vol_length; /* Allow any large region. */
/***fprintf(stdout,"n1: %d, n2: %d, thresh: %f\n",n1,n2,thresh[th]);
fflush(stdout);***/
    for(vox=xdim+1;vox<vol_length-xdim-2;vox++) {
	if(vox < xdim+1 || vox > vol_length-xdim-2) {
	    /* Skip first and last row in volume. */
	    n_nghbrs = 0;
	    }
	else if(vox < plndim+xdim+1) {
	    n_nghbrs = n_nghbrs_end;
	    off = offsets_pln0;
	    }
	else if(vox > vol_length-(plndim+xdim)-2) {
	    n_nghbrs = n_nghbrs_end;
	    off = offsets_plnN;
	    }
	else {
	    n_nghbrs = n_nghbrs_middle;
	    off = offsets;
	    }
/***        if(tmpimg[vox] < thresh[th]) {***/
        if(tmpimg[vox] > 0.) {
	    n_srch = 0;
	    tmpimg[vox] = 0.;
	    reglist[0] = vox; 
	    n_reg = 1;
	    for(i=0;i<n_nghbrs;i++) {
	        if(tmpimg[vox+off[i]] > 0) {
	    	    tmpimg[vox+off[i]] = 0.;
	    	    voxlist[n_srch] = vox + off[i];
	    	    ++n_srch;
		    reglist[n_reg] = vox+off[i];
	 	    ++n_reg;
	    	    }
	        }
	    while(n_srch > 0) {
		--n_srch;
		vox1 = voxlist[n_srch];
		if(vox1 < xdim+1 || vox1 > vol_length-xdim-2) {
		    /* Skip first and last row in volume. */
		    n_nghbrs = 0;
		    }
		else if(vox1 < plndim+xdim+1) {
		    n_nghbrs = n_nghbrs_end;
		    off = offsets_pln0;
		    }
		else if(vox1 > vol_length - (plndim+xdim)-2) {
		    n_nghbrs = n_nghbrs_end;
		    off = offsets_plnN;
		    }
		else {
		    n_nghbrs = n_nghbrs_middle;
		    off = offsets;
		    }
	        for(i=0;i<n_nghbrs;i++) {
	            if(tmpimg[vox1+off[i]] > 0.) {
	    	        tmpimg[vox1+off[i]] = 0.;
	    	        voxlist[n_srch] = vox1 + off[i];
	    	        ++n_srch; /* Increment list length. */
		        reglist[n_reg] = vox1+off[i];
	 	        ++n_reg;
	    	        }
	            }
		}  /* End of while loop. */
/*fprintf(stdout,"nreg: %d\n",n_reg);
fflush(stdout);*/
	     if(n_reg >= n1 && n_reg <= n2) {
		if(n_reg < 100) 
		    n_reg_size[n_reg-1] = n_reg_size[n_reg-1] + 1;
	        for(i=0;i<n_reg;i++) {
	            actmask[reglist[i]] = 1;	
	     	    }
	        ++num_sig_reg;
/***fprintf(stdout,"num_sig_reg: %d, n_reg_size: %f\n",num_sig_reg,n_reg_size[n_reg-1]);
fflush(stdout);***/
	        }
	    } /* End of processing for single voxel above threshold. */
	}
    }

free(tmpimg);
free(voxlist);
free(reglist);
free(offsets);
free(offsets_pln0);
free(offsets_plnN);

return(num_sig_reg);

}
 
