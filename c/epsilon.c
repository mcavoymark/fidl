/*****************************
epsilon.c

Library version of epsilon.pro
*****************************/

/*$Revision: 1.1 $*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>
#include "nrutil.h"


/**************************************************************/
/*double epsilon(LinearModel *glm,int nconditions,int *index)*/
double epsilon(LinearModel *glm,int num_tc,int *tc)
/**************************************************************/
{
int i,j;/*,k,m,n;*/
double numerator=0,denominator=0,sub;

#if 0
for(i=0;i<nconditions;i++)
    for(j=0;j<glm->period;j++) {
        k = index[i]*glm->period+j;	
	numerator += glm->ATAm1[k+1][k+1];
	}
numerator *= numerator;

for(i=0;i<nconditions;i++) {
    k = index[i]*glm->period;
    for(j=0;j<nconditions;j++) {
	m = index[j]*glm->period;
	for(sub=n=0;n<glm->period;n++)
	    sub += glm->ATAm1[k+n+1][m+n+1]*glm->ATAm1[k+n+1][m+n+1];
        denominator += i!=j ? 0.5*sub : sub;
	}
    }
denominator *= nconditions*glm->period;
#endif


for(i=0;i<num_tc;i++)
    numerator += glm->ATAm1[tc[i]][tc[i]];
numerator *= numerator;

for(i=0;i<num_tc;i++) {
    for(j=0;j<num_tc;j++) {
        sub += glm->ATAm1[tc[i]][tc[j]]*glm->ATAm1[tc[i]][tc[j]];
        if(i!=j) sub *= 0.5;
        denominator += i!=j ? 0.5*sub : sub;
        }
    }
denominator *= num_tc;


return numerator/denominator;
}
