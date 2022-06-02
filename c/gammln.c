/*$Revision: 12.84 $*/

#include <math.h>

/*p.207 Fortran(1992): Approximation is good everywhere in the half complex plane Re z>0.
                       So the transformation code as given below is unnecessary. However, if you're going to
                       integrate the gamma fn numerically then the transformation is necessary.
		       This routine and the unix lgamma were cross validated with MATLAB.*/ 
                           #if 0
				double lngamma(double xx)
				{
    				    double yy=xx,scl=1,gammln(double x);

    				    if(yy < 1) {
        				yy = xx + 1;
        				scl = xx; 
        				}
    				    return gammln(yy)/scl;
				}
                            #endif


/**********************/
double gammln(double xx)
/**********************/
{
        double x,y,tmp,ser;
        static double cof[6]={76.18009172947146,-86.50532032941677,
                24.01409824083091,-1.231739572450155,
                0.1208650973866179e-2,-0.5395239384953e-5};
        int j;

        y=x=xx;
        tmp=x+5.5;
        tmp -= (x+0.5)*log(tmp);
        ser=1.000000000190015;
        for (j=0;j<=5;j++) ser += cof[j]/++y;
        return -tmp+log(2.5066282746310005*ser/x);
}
/* (C) Copr. 1986-92 Numerical Recipes Software |oV'41.5.){2p49. */

