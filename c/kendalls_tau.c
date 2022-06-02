/*********************************************************************
c
c subroutine: kendalls_tau
c
c Purpose: Front-end for nrecipe kendl2 routine to compute kendalls tau
test.
c
c************************************************************************/

/*$Revision: 12.80 $*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <nrutil.h>

void kendalls_tau(float *image,float *behav,float *zstat,int lenimg,int nbehav);
float kendl1(float *data1,float *data2,int n);

/*****************************/
int _kendalls_tau(argc,argv)
/*****************************/

int	argc;
char	*argv[];

{

float	*image,*behav,*zstat,max;

int	i,lenimg,nbehav;

image = (float *)argv[0];
behav = (float *)argv[1];
zstat = (float *)argv[2];
lenimg = (int)argv[3];
nbehav = (int)argv[4];

--behav;  /* Correct for indexing-from-one by numerical recipes routines.*/

kendalls_tau(image,behav,zstat,lenimg,nbehav);
for(i=0,max=0;i<lenimg;i++) {
    if(zstat[i] > max)
        max = zstat[i];
    }

return(0);

}

/***************************************************************************/
void kendalls_tau(float *image,float *behav,float *zstat,int lenimg,int nbehav)
/***************************************************************************/

{

float	*img_data,*tau;

int	i,j,k;

img_data = vector(1,nbehav);

for(i=0;i<lenimg;i++) {
    for(j=1,k=i;j<=nbehav;j++,k+=lenimg) {
        img_data[j] = image[k];
        }
    zstat[i] = kendl1(behav,img_data,nbehav);
    }

free_vector(img_data,1,nbehav);

}

 
/*******************************************/
float kendl1(float *data1,float *data2,int n)
/*******************************************/

/* Lightly modified from numerical recipes in C. */

{

int n2=0,n1=0,k,j;
int is=0;
float svar,aa,a2,a1,zstat,tau;

for (j=1;j<n;j++) {
    for (k=(j+1);k<=n;k++) {
        a1=data1[j]-data1[k];
        a2=data2[j]-data2[k];
        aa=a1*a2;
        if (aa) {
            ++n1;
            ++n2;
            aa > 0.0 ? ++is : --is;
            } 
        else {
            if (a1) ++n1;
            if (a2) ++n2;
            }
        }
    }
tau=is/(sqrt((double) n1)*sqrt((double) n2));
svar=(4.0*n+10.0)/(9.0*n*(n-1.0));
zstat=(tau)/sqrt(svar);

return(zstat);

}
