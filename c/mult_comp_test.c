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


/*$Revision: 12.80 $*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAXIT 1000
#define N_NGHBRS_FACE 6
#define N_NGHBRS_END_FACE 5
#define NUM_THRESH 5
#define MAX_REG_SIZE 10  /* Maximum region size for which a new 
			    threshold is calculated.  All larger regions
			    use this threshold. */
#define MIN_LRG_REG_SIZE 6 /* Minimum regions size for which large-region
			    approximation holds. */

float 	mKappa,p_tilde;
int 	reg_size;

/*****************************************/
float _mult_comp_test(int argc,char **argv)
/*****************************************/

{

int mult_comp_test(float *sigprob,float *actmask,int xdim,int ydim,int zdim,int N,int min_reg,int thresh_max);

float	*sigprob,*actmask,thresh_max,*p_val1,p_val,*thresh,*Prm,*n_reg_size;

int	xdim,ydim,zdim,N,min_reg;

sigprob    = (float *)argv[0];
actmask    = (float *)argv[1];
min_reg    = (int)argv[2];
thresh_max = *((float *)argv[3]);
xdim = (int)argv[4];
ydim = (int)argv[5];
zdim = (int)argv[6];
N = (int)argv[7];
p_val1 = (float *)argv[8];
p_val = *p_val1;
thresh = (float *)argv[9];
Prm = (float *)argv[10];
n_reg_size = (float *)argv[11];
/*printf("%d %d %d %f %d %d %d %d\n",sigprob,actmask,min_reg,thresh_max,xdim,ydim,zdim,N);*/

mult_comp_test(sigprob,actmask,xdim,ydim,zdim,N,min_reg,thresh_max);
}

int mult_comp_test(float *sigprob,float *actmask,int xdim,int ydim,int zdim,int N,int min_reg,int thresh_max)
{

float	*tmpimg,*thresh,*Prm,x;

int	i,*off,*voxlist,*reg_list,n_srch,n_reg,vox,vox1,plndim,
	*reglist,th,n1,n2,n_nghbrs,*offsets,*offsets_pln0,
	*offsets_plnN,n_lower[MAX_REG_SIZE],
	n_nghbrs_end,n_nghbrs_middle,vol_length;

short 	num_sig_reg;	/* Number of significant regions detected. */

float   *n_reg_size,p_val,*p_val1;

void 	calc_thresh_test();

/*min_reg = 1;*/

plndim = xdim*ydim;
vol_length = xdim*ydim*zdim;

n_nghbrs_middle = N_NGHBRS_FACE;
n_nghbrs_end = N_NGHBRS_END_FACE;

offsets = (int *)malloc(6*sizeof(int));
offsets_pln0 = (int *)malloc(5*sizeof(int));
offsets_plnN = (int *)malloc(5*sizeof(int));
offsets[0] = 1; offsets[1] = -1; offsets[2] = -xdim; 
offsets[3] = xdim; offsets[4] = plndim; offsets[5] = -plndim; 

offsets_pln0[0] = 1; offsets_pln0[1] = -1; offsets_pln0[2] = -xdim; 
offsets_pln0[3] = xdim; offsets_pln0[4] = plndim;

offsets_plnN[0] = 1; offsets_plnN[1] = -1; offsets_plnN[2] = -xdim; 
offsets_plnN[3] = xdim; offsets_plnN[4] = -plndim;

calc_thresh_test(thresh,Prm,p_val,N,min_reg,xdim,ydim,zdim);

for(i=0;i<MAX_REG_SIZE;i++) {
    if(thresh[i] > thresh_max) 
	thresh[i] = thresh_max;
    }

for(i=0;i<MAX_REG_SIZE;i++) {
    if(i < NUM_THRESH)
 	n_lower[i] = i + 1;
    else 
        n_lower[i] = NUM_THRESH;
    if(n_lower[i] < min_reg)
        n_lower[i] = min_reg;
    }

tmpimg = (float *)malloc(vol_length*sizeof(float));
voxlist = (int *)malloc(vol_length*sizeof(int));
reglist = (int *)malloc(vol_length*sizeof(int));
for(i=0;i<vol_length;i++) {
    voxlist[i] = 0;
    reglist[i] = 0;
    }
num_sig_reg = 0;
n_reg = 0;
n_srch = 0;

for(th=0;th<MAX_REG_SIZE;th++) {
    for(i=0;i<vol_length;i++)
        tmpimg[i] = sigprob[i];
    n1 = n_lower[th];
    if(n1 < min_reg)
  	n1 = min_reg;
    if(th < MAX_REG_SIZE-1)
        n2 = (int)(p_tilde/thresh[th]);
    else
	n2 = vol_length; /* Allow any large region. */
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
        if(tmpimg[vox] < thresh[th]) {
	    n_srch = 0;
	    tmpimg[vox] = thresh[th];
	    reglist[0] = vox; 
	    n_reg = 1;
	    for(i=0;i<n_nghbrs;i++) {
	        if(tmpimg[vox+off[i]] < thresh[th]) {
	    	    tmpimg[vox+off[i]] = thresh[th];
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
	            if(tmpimg[vox1+off[i]] < thresh[th]) {
	    	        tmpimg[vox1+off[i]] = thresh[th];
	    	        voxlist[n_srch] = vox1 + off[i];
	    	        ++n_srch; /* Increment list length. */
		        reglist[n_reg] = vox1+off[i];
	 	        ++n_reg;
	    	        }
	            }
		}  /* End of while loop. */
	     if(n_reg >= n1 && n_reg <= n2) {
		if(n_reg < 100) 
		    ++n_reg_size[n_reg-1];
	        for(i=0;i<n_reg;i++) {
	            actmask[reglist[i]] = 1;	
	     	    }
	        ++num_sig_reg;
/*printf("num_sig_reg: %d, n_reg_size: %f\n",num_sig_reg,n_reg_size[n_reg-1]);*/
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
free(thresh);

return(num_sig_reg);

}


/******************************************/
void calc_thresh_test(q,Prm,p,N,m_min,xdim,ydim,zdim)
/******************************************/

float	*q,
	*Prm,
	p;
int	N,
	m_min,
	xdim,
	ydim,
	zdim;

{


int	i,j,k,m;

double	kappa[MAX_REG_SIZE],Kappa[MAX_REG_SIZE],xi,Sp,sum,x,Spm1,sumPrE,
	p_tilde0;

double polynomial_test(),rtsec_test();

/*kappa[0] = N;
for(i=1;i<10;i++)
  kappa[i] = kappa[i-1]*(i*N_NGHBRS_FACE - 2*(i-1));*/
Kappa[0] = N;
kappa[0] = 1.;
for(m=1;m<MAX_REG_SIZE;m++) {
    kappa[m] = m*N_NGHBRS_FACE - 2*(m-1);
    Kappa[m] = Kappa[m-1]*kappa[m];
    }
for(m=1;m<MAX_REG_SIZE;m++)
    Kappa[m] /= m+1;

Sp = 3.;
p_tilde = p/Sp/2.;
do {
    q[0] = p_tilde/Kappa[0];
    for(m=1;m<MAX_REG_SIZE;m++) {
        reg_size = m + 1;
	mKappa = (double)(reg_size)*Kappa[m];
        q[m] = (float)rtsec_test((double)q[m-1],(double)p,(double)1.e-9);
/*printf("m: %d, p: %f, p_tilde: %f, Sp: %f, mKappa: %g, Kappa: %g, q: %g\n",m,p,p_tilde,Sp,mKappa,Kappa[m],q[m]);*/
/*printf("m: %d, Sp: %f, q: %g, Prm: %g\n",m+1,Sp,q[m],Prm[m]);*/
	}

    for(m=0;m<MAX_REG_SIZE;m++) {
        Prm[m] = (m+1)*Kappa[m]*pow((double)q[m],(double)(m+1));
/*printf("Prm[%d]: %g\n",m,Prm[m]);*/
        }
    for(m=m_min-1,sumPrE=0;m<MAX_REG_SIZE;m++)
        sumPrE += Prm[m];
    p_tilde = p*Prm[0]/(2*sumPrE);
    p_tilde0 = p_tilde;
    if(p_tilde > p/2) 
        p_tilde = p/2;
    for(m=0;m<MAX_REG_SIZE;m++) {
        Prm[m] /= sumPrE;
/*printf("Prm[%d]: %g\n",m,Prm[m]);*/
        }
    Spm1 = Sp;
    Sp = p/(2*p_tilde0);
/*printf("Sp: %f, Spm1: %f sumPrE: %f, p_tilde0: %f, p_tilde: %f, m_min: %d\n",Sp,Spm1,sumPrE,p_tilde0,p_tilde,m_min);*/

    } while ((float)fabs((double)(Sp-Spm1))/Sp > .01);

/*for(i=0;i<10;i++) 
    printf("q[%d]: %g, Pr[M=%d]: %g, Sp: %f\n",i+1,q[i],i+1,Prm[i],Sp);*/

}

/*****************/
double polynomial_test(q)
/*****************/

double q;

{

double val;

val = mKappa*pow((double)q,(double)reg_size) + reg_size*q - p_tilde;

return(val);

}

/**********************/
double rtsec_test(x1,x2,xacc)
/**********************/

double x1,x2,xacc;
{
        int j;
        double fl,f,dx,swap,xl,rts;
        fl=polynomial_test(x1);
        f=polynomial_test(x2);
        if (fabs(fl) < fabs(f)) {
                rts=x1;
                xl=x2;
                swap=fl;
                fl=f;
                f=swap;
        } else {
                xl=x1;
                rts=x2;
        }
        for (j=1;j<=MAXIT;j++) {
                dx=(xl-rts)*f/(f-fl);
                xl=rts;
                fl=f;
                rts += dx;
                f=polynomial_test(rts);
                if (fabs(dx) < xacc*rts || f == 0.0) return rts;
        }
        printf("Maximum number of iterations exceeded in RTSEC\n");
}
 
