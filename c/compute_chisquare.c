/* Copyright 5/14/03 Washington University.  All Rights Reserved.
   compute_chisquare.c  $Revision: 1.2 $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>

double x_cvf(double p,double df);

main(int argc,char **argv)
{
int i;

double p_val=0,df=0;

if(argc < 5) {
    fprintf(stderr,"Usage: compute_chisquare -p_val 0.001 -df 6\n");
    fprintf(stderr,"        -p_val: Desired p value.\n");
    fprintf(stderr,"        -df:    Degrees of freedom.\n");
    fprintf(stderr,"                For the loglikelihood statistic, the df is the number of timepoints.\n");
    exit(-1);
    }

for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-p_val") && argc > i+1)
        p_val = atof(argv[++i]);
    if(!strcmp(argv[i],"-df") && argc > i+1)
        df = atof(argv[++i]);
    }
if(!p_val) {
    fprintf(stderr,"p_val has not been specified\n");
    exit(-1);
    }
if(!df) {
    fprintf(stderr,"df has not been specified\n");
    exit(-1);
    }

printf("chisquare = %f\n",x_cvf(p_val,df));
}
    

double x_cvf(double p,double df)
{
    int i;
    double up,below,sigprob,del=1.0e-6,mid;

    if(p<0.0 || p>1.0) {
        fprintf(stderr,"p must be in the interval [0.0, 1.0]\n");
        return -1;
        }
    df /= 2.;
    up = 300.0;

    sigprob = gammq(df,up);
    for(below=0.0;(sigprob = gammq(df,up)),sigprob>p;) {
        below = up;
        up *= 2.0;
        }
    mid = below + (up - below)*(1-p);

    for(i=0;i<100 && fabs(up-below)>del*mid;i++) {
        if(i) {
            if(sigprob < p)
                up = mid;
            else
                below = mid;
            /*printf("z=%e p=%e up=%f low=%f mid=%f\n",1.0-sigprob,1.0-p,up,below,mid);*/
            mid = (up + below)/2.;
            }
        /*printf("count=%d up=%f below=%f mid=%f\n",i+1,up,below,mid);*/
        sigprob = gammq(df,mid);
        }
    return mid*2.;
}

