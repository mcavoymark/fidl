/* Copyright 12/31/99 Washington University.  All Rights Reserved.
   errorfinv.c  $Revision: 12.81 $ */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/*************************************/
double _errorfinv(int argc,char **argv)
/*************************************/
{
double	*p,*z,p1;

int	i;

int	length;

p = (double *)argv[0];
z = (double *)argv[1];
length = (int)argv[2];


#if 0
for(i=0;i<length;i++) {
    if(p[i] <= -1) 
        z[i] = 0.;
    else if(p[i] < -0.7) {
        p1 = sqrt(-log((1.+p[i])/2));
        z[i] = -(((1.641345311*p1 + 3.429567803)*p1 
                - 1.624906493)*p1 - 1.970840454)/
               ((1.637067800*p1 + 3.543889200)*p1 + 1);
        z[i] -= (erff(z[i]) - p[i]) / (2./sqrt(M_PI) * exp(-z[i]*z[i]));
        z[i] -= (erff(z[i]) - p[i]) / (2./sqrt(M_PI) * exp(-z[i]*z[i]));
        }
    else if(fabs(p[i]) <= 0.7) {
	p1 = p[i]*p[i];
        z[i] = p[i]*(((-0.140543331*p1  +  0.914624893)*p1 
                 - 1.645349621)*p1 + 0.886226899)/ 
	       ((((0.012229801*p1 - 0.329097515)*p1 
		+  1.442710462)*p1 - 2.11837772)*p1 + 1);
        z[i] -= (erff(z[i]) - p[i]) / (2./sqrt(M_PI) * exp(-z[i]*z[i]));
        z[i] -= (erff(z[i]) - p[i]) / (2./sqrt(M_PI) * exp(-z[i]*z[i]));
        }
    else if(p[i] < 1.) {
        p1 = sqrt(-log((1.-p[i])/2));
        z[i] = (((1.641345311*p1 + 3.429567803)*p1 
                - 1.624906493)*p1 - 1.970840454)/
               ((1.637067800*p1 + 3.543889200)*p1 + 1);
        z[i] -= (erff(z[i]) - p[i]) / (2./sqrt(M_PI) * exp(-z[i]*z[i]));
        z[i] -= (erff(z[i]) - p[i]) / (2./sqrt(M_PI) * exp(-z[i]*z[i]));
        }
    else
        z[i] = 0.;

    }
#endif

for(i=0;i<length;i++) {
    if(p[i] <= -1) {
        z[i] = 0.;
        }
    else if(p[i] < -0.7) {
        p1 = sqrt(-log((1.+p[i])/2));
        z[i] = -(((1.641345311*p1 + 3.429567803)*p1
                - 1.624906493)*p1 - 1.970840454)/
               ((1.637067800*p1 + 3.543889200)*p1 + 1);
        z[i] -= (erf(z[i]) - p[i]) / (2./sqrt(M_PI) * exp(-z[i]*z[i]));
        z[i] -= (erf(z[i]) - p[i]) / (2./sqrt(M_PI) * exp(-z[i]*z[i]));
        }
    else if(fabs(p[i]) <= 0.7) {
        p1 = p[i]*p[i];
        z[i] = p[i]*(((-0.140543331*p1  +  0.914624893)*p1
                 - 1.645349621)*p1 + 0.886226899)/
               ((((0.012229801*p1 - 0.329097515)*p1
                +  1.442710462)*p1 - 2.11837772)*p1 + 1);
        z[i] -= (erf(z[i]) - p[i]) / (2./sqrt(M_PI) * exp(-z[i]*z[i]));
        z[i] -= (erf(z[i]) - p[i]) / (2./sqrt(M_PI) * exp(-z[i]*z[i]));
        }
    else if(p[i] < 1.) {
        p1 = sqrt(-log((1.-p[i])/2));
        z[i] = (((1.641345311*p1 + 3.429567803)*p1
                - 1.624906493)*p1 - 1.970840454)/
               ((1.637067800*p1 + 3.543889200)*p1 + 1);
        z[i] -= (erf(z[i]) - p[i]) / (2./sqrt(M_PI) * exp(-z[i]*z[i]));
        z[i] -= (erf(z[i]) - p[i]) / (2./sqrt(M_PI) * exp(-z[i]*z[i]));
        }
    else {
        z[i] = 0.;
        }
    }


}
