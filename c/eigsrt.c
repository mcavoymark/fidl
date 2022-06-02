/*#include <stdio.h>
#include <stdlib.h>
#include <string.h>*/
#include <math.h>

/*#include <fidl.h>*/
/*START150323*/
#include "fidl.h"

/*void eigsrt(float d[], float **v, int n)*/
void eigsrt(double d[], double **v, int n)
{
	int k,j,i;
	/*float p;*/
	double p;

	for (i=1;i<n;i++) {
		p=d[k=i];
		for (j=i+1;j<=n;j++)
			if (d[j] >= p) p=d[k=j];
		if (k != i) {
			d[k]=d[i];
			d[i]=p;
			for (j=1;j<=n;j++) {
				p=v[j][i];
				v[j][i]=v[j][k];
				v[j][k]=p;
			}
		}
	}
}
/* (C) Copr. 1986-92 Numerical Recipes Software |oV'41.5.){2p49. */
