/*-------------------------------------------------------------

Function:  kolmo_smrnov

Purpose: Compute Kolmogorov-Smirnov statistic using Numerical recipes
         in C.  Called from IDL.

By: John Ollinger

Date: 6/14/96

---------------------------------------------------------------*/

/*$Revision: 12.80 $*/

#include        <stdio.h>
#include        <math.h>
#include        <fcntl.h>

float _kolmo_smrnov(argc,argv)

int	argc;
char	*argv[];

{



float	*dist1,*dist2,*D,*prob,*x,*y,xx,
	*vector();
void	free_vector(),free_ivector();

int	i,j,k,ioff,joff,koff,xdim,ydim,tdim1,tdim2,ii;

void	kstwo();

dist1  =  (float *)argv[0];
dist2  =  (float *)argv[1];
D      =  (float *)argv[2];
prob   =  (float *)argv[3];
xdim   = (int)argv[4];
ydim   = (int)argv[5];
tdim1  = (int)argv[6];
tdim2  = (int)argv[7];
--D;
--prob;

x = vector(1,tdim1);
y = vector(1,tdim2);

for(i=0;i<xdim;i++) {
    for(j=0,ioff=0;j<ydim;j++,ioff+=xdim) {
	for(k=0,koff=i+ioff;k<tdim1;k++,koff+=xdim*ydim)
	    x[k+1] = dist1[koff];
	for(k=0,koff=i+ioff;k<tdim2;k++,koff+=xdim*ydim)
	    y[k+1] = dist2[koff];
 	kstwo(x,tdim1,y,tdim2,&D[i+ioff],&prob[i+ioff]);
	}
    }

free_vector(x,1,tdim1);
free_vector(y,1,tdim2);

return(0.);

}


void kstwo(data1,n1,data2,n2,d,prob)
float data1[],data2[],*d,*prob;
int n1,n2;
{
	int j1=1,j2=1,i;
	float en1,en2,fn1=0.0,fn2=0.0,dt,d1,d2;
	void sort();
	float probks();

	en1=n1;
	en2=n2;
	*d=0.0;
	sort(n1,data1);
	sort(n2,data2);
	while (j1 <= n1 && j2 <= n2) {
		if ((d1=data1[j1]) <= (d2=data2[j2])) {
			fn1=(j1++)/en1;
		}
		if (d2 <= d1) {
			fn2=(j2++)/en2;
		}
		if ((dt=fabs(fn2-fn1)) > *d) *d=dt;
	}
	*prob=probks(sqrt(en1*en2/(en1+en2))*(*d));
}


void sort(n,ra)
int n;
float ra[];
{
	int l,j,ir,i;
	float rra;

	l=(n >> 1)+1;
	ir=n;
	for (;;) {
		if (l > 1)
			rra=ra[--l];
		else {
			rra=ra[ir];
			ra[ir]=ra[1];
			if (--ir == 1) {
				ra[1]=rra;
				return;
			}
		}
		i=l;
		j=l << 1;
		while (j <= ir) {
			if (j < ir && ra[j] < ra[j+1]) ++j;
			if (rra < ra[j]) {
				ra[i]=ra[j];
				j += (i=j);
			}
			else j=ir+1;
		}
		ra[i]=rra;
	}
}
#include <math.h>

#define EPS1 0.001
#define EPS2 1.0e-8

float probks(alam)
float alam;
{
	int j;
	float a2,fac=2.0,sum=0.0,term,termbf=0.0;

	a2 = -2.0*alam*alam;
	for (j=1;j<=100;j++) {
		term=fac*exp(a2*j*j);
		sum += term;
		if (fabs(term) <= EPS1*termbf || fabs(term) < EPS2*sum) return sum;
		fac = -fac;
		termbf=fabs(term);
	}
	return 0.0;
}
