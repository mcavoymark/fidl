/*-------------------------------------------------------------

Function:  svd

Purpose: Compute svd of a hemodynamic response. statistic.

By: John Ollinger

Date: 1/26/98

---------------------------------------------------------------*/

/*$Revision: 12.80 $*/

#include        <stdio.h>
#include        <math.h>
#include        <fcntl.h>

static float at,bt,ct;
#define PYTHAG(a,b) ((at=fabs(a)) > (bt=fabs(b)) ? \
(ct=bt/at,at*sqrt(1.0+ct*ct)) : (bt ? (ct=at/bt,bt*sqrt(1.0+ct*ct)): 0.0))

static float maxarg1,maxarg2;
#define MAX(a,b) (maxarg1=(a),maxarg2=(b),(maxarg1) > (maxarg2) ?\
	(maxarg1) : (maxarg2))
#define SIGN(a,b) ((b) >= 0.0 ? fabs(a) : -fabs(a))
#define ABS(x) (if(x < 0) x = -x;)

void svdcmp(float **a,int m,int n,float *w,float **v);
float 	*vector();
float 	**matrix();
void	nrerror();
void	free_vector(),free_matrix();


/******************************/
float _svd(int argc,char **argv)
/******************************/

{
int svd(float *hrfs,float *var,double *ATAm1,float *corr,float *Z,float *eigvec,int xdim,int ydim,int tdim,int eff0,int N);

float *hrfs,*var,*corr,*Z,*eigvec;
double *ATAm1;
int xdim,ydim,tdim,eff0,N;

hrfs   =  (float *)argv[0];
var    =  (float *)argv[1];
ATAm1 =  (double *)argv[2];
corr =  (float *)argv[3];
Z      =  (float *)argv[4];
eigvec =  (float *)argv[5];
xdim   = (int)argv[6];
ydim   = (int)argv[7];
tdim  = (int)argv[8];
eff0  = (int)argv[9];
N = (int)argv[10];
 
svd(hrfs,var,ATAm1,corr,Z,eigvec,xdim,ydim,tdim,eff0,N);

}


int svd(float *hrfs,float *var,double *ATAm1,float *corr,float *Z,float *eigvec,int xdim,int ydim,int tdim,int eff0,int N)

{

float	*U,V,*w,max,*ww,**R,**S,**VV,sum,*emax,eigvar,*b,corr1,corrvar;

int	i,j,k,kk,ioff,koff,m,moff,toff,joff,imax;

/*printf("xdim: %d, ydim: %d, tdim: %d, eff0: %d, N: %d\n",xdim,ydim,tdim,eff0,N);*/

ww   = vector(1,tdim);
b   = vector(1,tdim);
emax   = vector(1,tdim);
R = matrix(1,tdim,1,tdim);
S = matrix(1,tdim,1,tdim);
VV = matrix(1,tdim,1,tdim);

for(i=0,joff=0;i<xdim;i++,joff+=xdim) {
    for(j=0;j<ydim;j++) {
        for(k=0,koff=0,sum=0.;k<tdim;k++,koff+=xdim*ydim) {
            b[k+1] = hrfs[j+joff+koff];
            sum += b[k+1];
            }
        for(k=1;k<=tdim;k++)
            b[k] -= sum;
        for(k=0,toff=eff0*N+eff0;k<tdim;k++,toff+=N) {
            for(m=0;m<tdim;m++) {
                R[k+1][m+1] = b[k+1]*b[m+1] + var[j+joff]*ATAm1[m+toff];
                S[k+1][m+1] = R[k+1][m+1];
                }
            }
        svdcmp(R,tdim,tdim,ww,VV);
        for(max=0.,k=1;k<=tdim;k++) {
            if(ww[k] > max) {
                max = ww[k];
                imax = k;
                for(k=1;k<=tdim;k++)
                    emax[k] = R[k][imax]; 
                }
            }
        for(k=1,sum=0;k<=tdim;k++)
            sum += emax[k]; 
        sum /= tdim;
        for(k=1;k<=tdim;k++)
            emax[k] -= sum;
        for(k=1,sum=0;k<=tdim;k++)
            sum += emax[k]*emax[k]; 
        for(k=1;k<=tdim;k++)
            emax[k] /= sum;
        for(k=1,corr1=0.,corrvar=0.,toff=0,moff=eff0*N+eff0;k<=tdim;
 				    k++,toff+=xdim*ydim,moff+=N) {
            corr1 += b[k]*emax[k];
            eigvec[j+joff+toff] = emax[k];
            for(m=1;m<=tdim;m++)
                corrvar += emax[k]*emax[m]*ATAm1[m-1+moff];
            }
/****        if(corr1 < 0.) 
            corr1 = -corr1;***/
        corr[j+joff] = corr1;
corr[j+joff] = max;
        corrvar *= var[j+joff];
        if(corrvar > 0.)
            Z[j+joff] = corr1/sqrt((double)corrvar);
        else
            Z[j+joff] = 0.;
/*if(i ==32 && j == 32)
 printf("max: %f, corr: %f, corrvar: %f, sum: %f\n",max,corr1,corrvar,sum);*/
	}
    }

free_vector(ww,1,tdim);
free_vector(b,1,tdim);
free_vector(emax,1,tdim);
free_matrix(R,1,tdim,1,tdim);
free_matrix(S,1,tdim,1,tdim);
free_matrix(VV,1,tdim,1,tdim);

return(NULL);

}
