/* Copyright 3/24/15 Washington University.  All Rights Reserved.
   fidl_glmcond.c  $Revision: 1.3 $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_linalg.h>
#include "fidl.h"
#include "cond_norm1.h"
#include "gsl_svd.h"
int main(int argc,char **argv)
{
char *glmf=NULL;
int i,j,k,SunOS_Linux,ndep;
double condmax=10000.,cond,cond_norm2,td,*A,*ATA,*ATAm1,*temp_double,*V,*S,*work;
LinearModel *glm;
if(argc<3) {
    fprintf(stderr,"Usage: fidl_glmcond -glm filename\n");
    fprintf(stderr,"    -glm: glm file\n");
    exit(-1);
    }
for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-glm") && argc > i+1)
        glmf = argv[++i];
    }
if((SunOS_Linux=checkOS())==-1) exit(-1);
if(!(glm=read_glm(glmf,1,SunOS_Linux))) {
    printf("fidlError: reading %s  Abort!\n",glmf);
    exit(-1);
    }
if(!(A=malloc(sizeof*A*glm->ifh->glm_Nrow*glm->ifh->glm_Mcol))) {
    printf("fidlError: Unable to malloc A\n");
    exit(-1);
    }
if(!(ATA=malloc(sizeof*ATA*glm->ifh->glm_Mcol*glm->ifh->glm_Mcol))) {
    printf("fidlError: Unable to malloc ATA\n");
    exit(-1);
    }
if(!(ATAm1=malloc(sizeof*ATAm1*glm->ifh->glm_Mcol*glm->ifh->glm_Mcol))) {
    printf("fidlError: Unable to malloc ATAm1\n");
    exit(-1);
    }
if(!(temp_double=malloc(sizeof*temp_double*glm->ifh->glm_Mcol*glm->ifh->glm_Mcol))) {
    printf("fidlError: Unable to malloc temp_double\n");
    exit(-1);
    }
if(!(V=malloc(sizeof*V*glm->ifh->glm_Mcol*glm->ifh->glm_Mcol))) {
    printf("fidlError: Unable to malloc V\n");
    exit(-1);
    }
if(!(S=malloc(sizeof*S*glm->ifh->glm_Mcol))) {
    printf("fidlError: Unable to malloc S\n");
    exit(-1);
    }
if(!(work=malloc(sizeof*work*glm->ifh->glm_Mcol))) {
    printf("fidlError: Unable to malloc work\n");
    exit(-1);
    }
gsl_permutation *perm = gsl_permutation_alloc(glm->ifh->glm_Mcol);
for(k=i=0;i<glm->ifh->glm_Nrow;i++) for(j=0;j<glm->ifh->glm_Mcol;j++,k++) A[k] = (double)glm->AT[j][i];
gsl_matrix_view gA = gsl_matrix_view_array(A,glm->ifh->glm_Nrow,glm->ifh->glm_Mcol);
gsl_matrix_view gATA = gsl_matrix_view_array(ATA,glm->ifh->glm_Mcol,glm->ifh->glm_Mcol);
gsl_blas_dgemm(CblasTrans,CblasNoTrans,1.0,&gA.matrix,&gA.matrix,0.0,&gATA.matrix);
for(i=0;i<glm->ifh->glm_Mcol*glm->ifh->glm_Mcol;i++) temp_double[i] = ATA[i];
if(cond_norm1(temp_double,glm->ifh->glm_Mcol,&cond,ATAm1,perm))cond=0.;
printf("%s\n    Condition number norm1: %f\n",glmf,cond);
if(cond>condmax||cond==0.) {
    if(cond>condmax) printf("    Condition number greater than %f.\n",condmax); else printf("    Design matrix not invertible.\n");
    }
printf("%s\n",(cond>condmax||cond==0.)?"    Computing singular value decomposition.":
    "    Checking for dependencies with a singular value decomposition.");
for(i=0;i<glm->ifh->glm_Mcol*glm->ifh->glm_Mcol;i++) temp_double[i] = ATA[i];
td=gsl_svd_golubreinsch(temp_double,glm->ifh->glm_Mcol,glm->ifh->glm_Mcol,0.,V,S,ATAm1,&cond_norm2,&ndep,work);
printf("    gsl_svd_golubreinsch cond_norm2=%f ndep=%d tol=%g\n",cond_norm2,ndep,td);
}
