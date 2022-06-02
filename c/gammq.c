#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "fidl.h"

double gammq(double a, double x)
{
	void gcf(double *gammcf, double a, double x, double *gln);
	void gser(double *gamser, double a, double x, double *gln);
	void nrerror(char error_text[]);
	double gamser,gammcf,gln;

	/*if (x < 0.0 || a <= 0.0) nrerror("Invalid arguments in routine gammq");*/
	/*if(x < 0.0 || a <= 0.0) {
            printf("x=%f a=%f\n",x,a);
            nrerror("Invalid arguments in routine gammq");
            }*/
	if(x < 0.0 || a <= 0.0) {
            printf("Error: gammq x<0.0||a<=0.0  x=%f a=%f  Setting to %g\n",x,a,(double)UNSAMPLED_VOXEL);
            /*nrerror("Invalid arguments in routine gammq");*/
            return (double)UNSAMPLED_VOXEL;
            }


	if (x < (a+1.0)) {
		gser(&gamser,a,x,&gln);
		return 1.0-gamser;
	} else {
		gcf(&gammcf,a,x,&gln);
		return gammcf;
	}
}
/* (C) Copr. 1986-92 Numerical Recipes Software |oV'41.5.){2p49. */
