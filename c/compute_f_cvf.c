/* Copyright 12/31/99 Washington University.  All Rights Reserved.
   compute_f_cvf.c  $Revision: 1.14 $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>
#include <gsl/gsl_cdf.h>
int main(int argc,char **argv)
{
size_t i;
int ntests=0;
double p=0.,f=0.,dfn=0,dfd=0,p1,z;
if(argc < 7) {
    fprintf(stderr,"Usage: compute_f_cvf -p 0.05 -f 3.05 -dfn 6 -dfd 66 -ntests 210237\n");
    fprintf(stderr,"    Outputs p, f and z. -p and -f do not interact.\n");
    fprintf(stderr,"-p:      p value.\n");
    fprintf(stderr,"-f:      F statistic.\n");
    fprintf(stderr,"-dfn:    Numerator degrees of freedom.\n");
    fprintf(stderr,"-dfd:    Denominator degrees of freedom.\n");
    fprintf(stderr,"-ntests: Number of tests for bonferroni correction.\n");
    exit(-1);
    }
for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-p") && argc > i+1)
        p = atof(argv[++i]);
    if(!strcmp(argv[i],"-f") && argc > i+1)
        f = atof(argv[++i]);
    if(!strcmp(argv[i],"-dfn") && argc > i+1)
        dfn = atof(argv[++i]);
    if(!strcmp(argv[i],"-dfd") && argc > i+1)
        dfd = atof(argv[++i]);
    if(!strcmp(argv[i],"-ntests") && argc > i+1)
        ntests = atoi(argv[++i]);
    }
if(!p&&!f) {
    if(!p) fprintf(stderr,"-p has not been specified\n");
    if(!f) fprintf(stderr,"-f has not been specified\n");
    exit(-1);
    }
if(!dfn) {
    fprintf(stderr,"dfn has not been specified\n");
    exit(-1);
    }
if(!dfd) {
    fprintf(stderr,"dfd has not been specified\n");
    exit(-1);
    }
if(p) {
    printf("p = %g -> f = %f -> z = %f, 1 test\n",p,gsl_cdf_fdist_Qinv(p,dfn,dfd),gsl_cdf_ugaussian_Qinv(p/2.));
    if(ntests>1) printf("p = %g -> f = %f -> z = %f, %d tests (bonferroni)\n",p,gsl_cdf_fdist_Qinv(p/(double)ntests,dfn,dfd),
        gsl_cdf_ugaussian_Qinv(p/2./(double)ntests),ntests);
    }
if(f) {
    f_to_z(&f,&z,1,&dfn,&dfd);
    p1 = gsl_cdf_fdist_Q(f,dfn,dfd);
    printf("f = %f -> z = %f  p = %g, 1 test\n",f,z,p1);
    if(ntests>1) printf("f = %f -> z = %f  p = %g, %d tests (bonferroni)\n",f,gsl_cdf_ugaussian_Qinv(p1/2./(double)ntests),p1,ntests);
    }
}
