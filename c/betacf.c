/*$Revision: 12.86 $*/

#if 0

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define ITMAX 100
#define EPS 3.0e-7

/******************/ 
double betacf(a,b,x)
/******************/ 
	double a,b,x;
{
        double qap,qam,qab,em,tem,d;
        double bz,bm=1.0,bp,bpp;
        double az=1.0,am=1.0,ap,app,aold;
        int m;
 
        qab=a+b;
        qap=a+1.0;
        qam=a-1.0;
        bz=1.0-qab*x/qap;
        for (m=1;m<=ITMAX;m++) {
                em=(double) m;
                tem=em+em;
                d=em*(b-em)*x/((qam+tem)*(a+tem));
                ap=az+d*am;
                bp=bz+d*bm;
                d = -(a+em)*(qab+em)*x/((qap+tem)*(a+tem));
                app=ap+d*az;
                bpp=bp+d*bz;
                aold=az;
                am=ap/bpp;
                bm=bp/bpp;
                az=app/bpp;
                bz=1.0;
                if (fabs(az-aold) < (EPS*fabs(az))) return az;
        }
/***        printf("a or b too big, or ITMAX too small in BETACF");***/
        return(-1.);
}
#endif

#if 1

#include <math.h>
#include <stdio.h>
#include <fidl.h>

#define MAXIT 100
#define EPS 3.0e-7
#define FPMIN 1.0e-30

double betacf(double a, double b, double x)
{
        /*void nrerror(char error_text[]);*/
        int m,m2;
        double aa,c,d,del,h,qab,qam,qap;

        qab=a+b;
        qap=a+1.0;
        qam=a-1.0;
        c=1.0;
        d=1.0-qab*x/qap;
        if (fabs(d) < (double)FPMIN) d=(double)FPMIN;
        d=1.0/d;
        h=d;
        for (m=1;m<=(int)MAXIT;m++) {
                m2=2*m;
                aa=m*(b-m)*x/((qam+m2)*(a+m2));
                d=1.0+aa*d;
                if (fabs(d) < (double)FPMIN) d=(double)FPMIN;
                c=1.0+aa/c;
                if (fabs(c) < (double)FPMIN) c=(double)FPMIN;
                d=1.0/d;
                h *= d*c;
                aa = -(a+m)*(qab+m)*x/((a+m2)*(qap+m2));
                d=1.0+aa*d;
                if (fabs(d) < (double)FPMIN) d=(double)FPMIN;
                c=1.0+aa/c;
                if (fabs(c) < (double)FPMIN) c=(double)FPMIN;
                d=1.0/d;
                del=d*c;
                h *= del;
                if (fabs(del-1.0) < (double)EPS) break;
        }
        /*if (m > MAXIT) nrerror("a or b too big, or MAXIT too small in betacf");*/
/****        if (m > MAXIT) printf("a or b too big, a: %f, b: %f\n",a,b);
        return h; ***/
        if (m > MAXIT)
            return (double)ERROR;
        else
            return h;
}
#undef MAXIT
#undef EPS
#undef FPMIN
/* (C) Copr. 1986-92 Numerical Recipes Software |oV'41.5.){2p49. */

#endif
