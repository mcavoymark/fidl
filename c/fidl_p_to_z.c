/* Copyright 11/3/10 Washington University.  All Rights Reserved.
   fidl_p_to_z.c  $Revision: 1.4 $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <gsl/gsl_cdf.h>
//#include "fidl.h"
int main(int argc,char **argv)
{
//size_t i;
int i,ntests=0;
double p=0.,z=0.,p1; /* zbon,sigprob */
if (argc < 3) {
    fprintf(stderr,"Usage: fidl_p_to_z -p 0.05 -z 3.00 -ntests 210237\n");
    fprintf(stderr,"     -p:      Significance probability to be converted to Z statistic.\n");
    fprintf(stderr,"     -z:      Z statistic to be converted to be converted to significance probability.\n");
    fprintf(stderr,"     -ntests: Number of tests for bonferroni correction.\n");
    exit(-1);
    }
for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-p") && argc > i+1)
        p = atof(argv[++i]);
    if(!strcmp(argv[i],"-z") && argc > i+1)
        z = atof(argv[++i]);
    if(!strcmp(argv[i],"-ntests") && argc > i+1)
        ntests = atoi(argv[++i]);
    }
if(!p&&!z) {
    if(!p) printf("fidlError: -p has not been specified\n");
    if(!z) printf("fidlError: -z has not been specified\n");
    exit(-1);
    }
if(p) {
    #if 0
    printf("pval = %g  z = %f",p,p_to_z(p));
    if(ntests==0) {
        printf("\n");
        }
    else {
        printf("  bonferroni z = %f\n",p_to_z(p/(double)ntests));
        }
    #endif

    printf("p = %g -> z = %.2f, 1 test",p,gsl_cdf_ugaussian_Qinv(p));
    if(ntests>1) printf("; z = %.2f, %d tests (bonferroni)",gsl_cdf_ugaussian_Qinv(p/(double)ntests),ntests);
    printf("\n");
    }
if(z) {
    #if 0
    z_to_sigprob(&z,&sigprob,1);
    printf("zstat = %f  p = %g",z,sigprob);
    if(ntests==0) {
        printf("\n");
        }
    else {
        printf("  bonferroni z = %f\n",p_to_z(sigprob/(double)ntests));
        }
    #endif

    #if 0
    printf("z = %.2f -> p = %g, 1 test",z,p1=gsl_cdf_ugaussian_Q(z));
    if(ntests>1) printf("; z = %.2f, %d tests (bonferroni)",gsl_cdf_ugaussian_Qinv(p1/(double)ntests),ntests);
    #endif
    //START150918
    printf("z = %.2f -> p = %g, 1 test",z,p1=z<0.?gsl_cdf_ugaussian_P(z):gsl_cdf_ugaussian_Q(z));
    if(ntests>1) printf("; z = %.2f, %d tests (bonferroni)",z<0.?gsl_cdf_ugaussian_Pinv(p1/(double)ntests):
        gsl_cdf_ugaussian_Qinv(p1/(double)ntests),ntests);

    printf("\n");
    }
printf("\n");
}

#if 0
if(p) {
    f1 = gsl_cdf_fdist_Qinv(p,dfn,dfd);
    printf("p = %g -> f = %f  z = %f, 1 test\n",p,f1,gsl_cdf_ugaussian_Qinv(p));
    if(ntests>1) printf("p = %g -> f = %f  z = %f, %d tests (bonferroni)\n",p,gsl_cdf_fdist_Qinv(p/(double)ntests,dfn,dfd),
        gsl_cdf_ugaussian_Qinv(p/(double)ntests),ntests);
    }
#endif

#if 0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>
main(int argc,char **argv)
{
int i,nvox=0;
double pval=0.,zstat=0.,z,zbon,sigprob;
if (argc < 3) {
    fprintf(stderr,"Usage: fidl_p_to_z -pval 0.05 -nvox 210237\n");
    fprintf(stderr,"Usage: fidl_p_to_z -zstat 3.00 -nvox 210237\n");
    fprintf(stderr,"     -pval: Significance probability to be converted to Z statistic.\n");
    fprintf(stderr,"     -zstat: Z statistic to be converted to be converted to significance probability.\n");
    fprintf(stderr,"     -nvox: Number of voxels for bonferroni correction.\n");
    exit(-1);
    }
for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-pval") && argc > i+1)
        pval = atof(argv[++i]);
    if(!strcmp(argv[i],"-zstat") && argc > i+1)
        zstat = atof(argv[++i]);
    if(!strcmp(argv[i],"-nvox") && argc > i+1)
        nvox = atoi(argv[++i]);
    }
if(!pval&&!zstat) {
    if(!pval) fprintf(stderr,"pval has not been specified\n");
    if(!zstat) fprintf(stderr,"zstat has not been specified\n");
    exit(-1);
    }
if(pval>0.) {
    z=p_to_z(pval);
    printf("pval = %g  z = %f",pval,z);
    if(nvox==0) {
        printf("\n");
        }
    else {
        zbon=p_to_z(pval/(double)nvox);
        printf("  bonferroni z = %f\n",zbon);
        }
    }
if(zstat>0.) {
    z_to_sigprob(&zstat,&sigprob,1);
    printf("zstat = %f  p = %g",zstat,sigprob);
    if(nvox==0) {
        printf("\n");
        }
    else {
        zbon=p_to_z(sigprob/(double)nvox);
        printf("  bonferroni z = %f\n",zbon);
        }
    }
}
#endif
