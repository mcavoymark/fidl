/* Copyright 12/31/99 Washington University.  All Rights Reserved.
   $Revision: 12.87 $*/
/*********************************************************************
Function: mult_comp

Purpose: Correct for multiple comparisons.

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

#define MAXIT 1000
#define NUM_THRESH 5
#define MAX_REG_SIZE 10  /* Maximum region size for which a new 
			    threshold is calculated.  All larger regions
			    use this threshold. */

float 	mKappa,p_tilde;
int 	reg_size;

int _mult_comp(int argc,char **argv)
{
    float *sigprob,*actmask,thresh_max,p_val,*thresh,*Prm,*n_reg_size;
    int num_sig_reg,xdim,ydim,zdim,N,min_reg;
    sigprob = (float *)argv[0];
    actmask = (float *)argv[1];
    thresh_max = *(float *)argv[3];
    p_val = *((float *)argv[8]);
    thresh = (float *)argv[9];
    Prm = (float *)argv[10];
    n_reg_size = (float *)argv[11];
    #ifdef __sun__
        min_reg = (int)argv[2];
        xdim = (int)argv[4];
        ydim = (int)argv[5];
        zdim = (int)argv[6];
        N = (int)argv[7];
    #else
        min_reg = (intptr_t)argv[2];
        xdim = (intptr_t)argv[4];
        ydim = (intptr_t)argv[5];
        zdim = (intptr_t)argv[6];
        N = (intptr_t)argv[7];
    #endif

/*printf("%d %d %d %f %d %d %d %d %f %f\n",sigprob,actmask,min_reg,thresh_max,xdim,ydim,zdim,N,p_val,thresh_max);*/

num_sig_reg = mult_comp(sigprob,actmask,xdim,ydim,zdim,N,min_reg,thresh_max,p_val,thresh,Prm);

return (float)num_sig_reg;
}


/*********************************************************/
int mult_comp(
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
	float *Prm)
/*********************************************************/
{
void calc_thresh(float *q,float *Prm,float p,int N,int m_min,int xdim,int ydim,int zdim);

int	i,*n1,*n2,lenvol,num_sig_reg;

float   *temp_float,*threshold;

calc_thresh(thresh,Prm,p_val,N,min_reg,xdim,ydim,zdim);
lenvol = xdim*ydim*zdim;

#if 0
GETMEM(n1,(int)MAX_REG_SIZE,int)
GETMEM(n2,(int)MAX_REG_SIZE,int)
for(i=0;i<(int)MAX_REG_SIZE;i++) {
    if(thresh[i] > thresh_max) thresh[i] = thresh_max;
    n1[i] = i<(int)NUM_THRESH ? i+1 : (int)NUM_THRESH;
    if(n1[i] < min_reg) n1[i] = min_reg;
    n2[i] = i<(int)MAX_REG_SIZE-1 ? (int)(p_tilde/thresh[i]) : lenvol; /*Allow any large region.*/
    }
num_sig_reg = spatial_extent(sigprob,actmask,xdim,ydim,zdim,thresh,(int)MAX_REG_SIZE,n1,n2);
GETMEM_0(temp_float,lenvol,float);
GETMEM(threshold,1,float);
threshold[0] = 2;
n1[0] = 1;
n2[0] = lenvol;
for(i=0;i<lenvol;i++) if(actmask[i] < 1.) actmask[i] = 2;
num_sig_reg = spatial_extent(actmask,temp_float,xdim,ydim,zdim,threshold,1,n1,n2);
for(i=0;i<lenvol;i++) actmask[i] = temp_float[i];
free(temp_float);
free(threshold);
free(n1);
free(n2);
return num_sig_reg;
#endif

if(!(n1=(int*)d1(sizeof(int),(size_t)MAX_REG_SIZE))) return -1;
if(!(n2=(int*)d1(sizeof(int),(size_t)MAX_REG_SIZE))) return -1;
for(i=0;i<(int)MAX_REG_SIZE;i++) {
    if(thresh[i] > thresh_max) thresh[i] = thresh_max;
    n1[i] = i<(int)NUM_THRESH ? i+1 : (int)NUM_THRESH;
    if(n1[i] < min_reg) n1[i] = min_reg;
    n2[i] = i<(int)MAX_REG_SIZE-1 ? (int)(p_tilde/thresh[i]) : lenvol; /*Allow any large region.*/
    }
if((num_sig_reg=spatial_extent(sigprob,actmask,xdim,ydim,zdim,thresh,(int)MAX_REG_SIZE,n1,n2,0))==-1) return -1;
if(!(temp_float=(float*)d1(sizeof(float),(size_t)lenvol))) return -1;
if(!(threshold=(float*)d1(sizeof(float),1))) return -1;
threshold[0] = 2;
n1[0] = 1;
n2[0] = lenvol;
for(i=0;i<lenvol;i++) if(actmask[i] < 1.) actmask[i] = 2;
if((num_sig_reg=spatial_extent(actmask,temp_float,xdim,ydim,zdim,threshold,1,n1,n2,0))==-1) return -1;
for(i=0;i<lenvol;i++) actmask[i] = temp_float[i];
free(temp_float);
free(threshold);
free(n1);
free(n2);
return num_sig_reg;
}


/**************************************************************************************/
void calc_thresh(float *q,float *Prm,float p,int N,int m_min,int xdim,int ydim,int zdim)
/**************************************************************************************/
{
double polynomial(double q),rtsec(double x1,double x2,double xacc);

int	i,j,k,m;

double	kappa[MAX_REG_SIZE],Kappa[MAX_REG_SIZE],xi,Sp,sum,x,Spm1,sumPrE,p_tilde0;


/*kappa[0] = N;
for(i=1;i<10;i++)
  kappa[i] = kappa[i-1]*(i*N_NGHBRS_FACE - 2*(i-1));*/
Kappa[0] = N;
kappa[0] = 1.;
for(m=1;m<(int)MAX_REG_SIZE;m++) {
    kappa[m] = m*N_NGHBRS_FACE - 2*(m-1);
    Kappa[m] = Kappa[m-1]*kappa[m];
    }
for(m=1;m<(int)MAX_REG_SIZE;m++)
    Kappa[m] /= m+1;

Sp = 3.;
/*p_tilde = p/Sp/2.; This is hard coded for a two sided test.*/
p_tilde = p/Sp;
do {
    q[0] = p_tilde/Kappa[0];
    for(m=1;m<(int)MAX_REG_SIZE;m++) {
        reg_size = m + 1;
	mKappa = (double)(reg_size)*Kappa[m];
        q[m] = (float)rtsec((double)q[m-1],(double)p,(double)1.e-9);
/*printf("m: %d, p: %f, p_tilde: %f, Sp: %f, mKappa: %g, Kappa: %g, q: %g\n",m,p,p_tilde,Sp,mKappa,Kappa[m],q[m]);*/
/*printf("m: %d, Sp: %f, q: %g, Prm: %g\n",m+1,Sp,q[m],Prm[m]);*/
	}

    for(m=0;m<(int)MAX_REG_SIZE;m++) {
        Prm[m] = (m+1)*Kappa[m]*pow((double)q[m],(double)(m+1));
/*printf("Prm[%d]: %g\n",m,Prm[m]);*/
        }
    for(m=m_min-1,sumPrE=0;m<(int)MAX_REG_SIZE;m++)
        sumPrE += Prm[m];
    p_tilde = p*Prm[0]/(2*sumPrE);
    p_tilde0 = p_tilde;
    if(p_tilde > p/2) 
        p_tilde = p/2;
    for(m=0;m<(int)MAX_REG_SIZE;m++) {
        Prm[m] /= sumPrE;
/*printf("Prm[%d]: %g\n",m,Prm[m]);*/
        }
    Spm1 = Sp;
    Sp = p/(2*p_tilde0);
/*printf("Sp: %f, Spm1: %f sumPrE: %f, p_tilde0: %f, p_tilde: %f, m_min: %d\n",Sp,Spm1,sumPrE,p_tilde0,p_tilde,m_min);*/

    } while ((float)fabs((double)(Sp-Spm1))/Sp > .01);

}

/*************************/
double polynomial(double q)
/*************************/
{
double val;

val = mKappa*pow((double)q,(double)reg_size) + reg_size*q - p_tilde;

return val;
}

/*******************************************/
double rtsec(double x1,double x2,double xacc)
/*******************************************/
{
int j;
double fl,f,dx,swap,xl,rts;
fl=polynomial(x1);
f=polynomial(x2);
if (fabs(fl) < fabs(f)) {
    rts=x1;
    xl=x2;
    swap=fl;
    fl=f;
    f=swap;
    } 
else {
    xl=x1;
    rts=x2;
    }
for (j=1;j<=MAXIT;j++) {
    dx=(xl-rts)*f/(f-fl);
    xl=rts;
    fl=f;
    rts += dx;
    f=polynomial(rts);
    if (fabs(dx) < xacc*rts || f == 0.0) return rts;
    }
printf("Maximum number of iterations exceeded in RTSEC\n");
}
 
