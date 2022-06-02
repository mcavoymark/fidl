/* Copyright 11/19/09 Washington University.  All Rights Reserved.
   fidl_checkglm.c  $Revision: 1.6 $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_linalg.h>
#include "fidl.h"
#include "cond_norm1.h"
#include "gsl_svd.h"
static char rcsid[] = "$Header: /home/hannah/mcavoy/idl/clib/RCS/fidl_checkglm.c,v 1.6 2016/04/13 17:02:49 mcavoy Exp $";
int main(int argc,char **argv)
{
char *glmfile=NULL;
int i,j,k,n,SunOS_Linux,ndep;
double **ATA,*temp_double,**ATAm1,*A,*V,*S,*work,cond,cond_norm2,td,condmax=10000.;
LinearModel *glm;
if(argc<3) {
    fprintf(stderr,"    -glm_file: glm\n");
    exit(-1);
    }
for(i=1;i<argc;i++) { 
    if(!strcmp(argv[i],"-glm_file") && argc > i+1) glmfile = argv[++i]; 
    }
print_version_number(rcsid,stdout);fflush(stdout);
if(!glmfile) {
    printf("Error: Need to specify glm file with -glm_file\n");
    exit(-1);
    }
if((SunOS_Linux=checkOS())==-1) exit(-1);
if(!(glm=read_glm(glmfile,1,SunOS_Linux))) {
    printf("Error: reading %s  Abort!\n",glmfile);
    exit(-1);
    }
printf("%s\n",glmfile);
printf("    degrees of freedom = %f\n",glm->ifh->glm_df<=0.01?(float)(glm->ifh->glm_Nrow-glm->ifh->glm_Mcol):glm->ifh->glm_df);
printf("    number of valid time points = %d\n    total number of estimated parameters = %d\n",glm->ifh->glm_Nrow,glm->ifh->glm_Mcol);
printf("    fwhm = %f\n",glm->ifh->glm_fwhm);
printf("    deleted frames (first frame is 1)\n    ");
for(n=i=0;i<glm->ifh->glm_tdim;i++) if(!(int)glm->valid_frms[i]) {printf("%d ",i+1);n++;}
if(!n) printf("    no deleted frames"); else printf("    %d deleted frames",n); printf("\n");
for(n=i=0;i<glm->ifh->glm_all_eff;i++) if(glm->ifh->glm_leffect_label[i]>n) n=glm->ifh->glm_leffect_label[i];
if(n<6)n=6;
n++;
printf("    %-*sframes\n",n,"effect"); 
for(i=0;i<glm->ifh->glm_all_eff;i++) printf("    %-*s%d\n",n,glm->ifh->glm_effect_label[i],glm->ifh->glm_effect_length[i]);
if(glm->ifh->glm_event_file) printf("    event file  %s\n",glm->ifh->glm_event_file);


/*START130222*/
gsl_permutation *perm = gsl_permutation_alloc(glm->ifh->glm_Mcol);
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
if(!(temp_double=malloc(sizeof*temp_double*glm->ifh->glm_Mcol*glm->ifh->glm_Mcol))) {
    printf("fidlError: Unable to malloc temp_double\n");
    exit(-1);
    }
if(!(ATA=d2double(glm->ifh->glm_Mcol,glm->ifh->glm_Mcol))) exit(-1);
gsl_matrix_view gATA = gsl_matrix_view_array(ATA[0],glm->ifh->glm_Mcol,glm->ifh->glm_Mcol);
if(!(ATAm1=d2double(glm->ifh->glm_Mcol,glm->ifh->glm_Mcol))) exit(-1);
if(!(A=malloc(sizeof*A*glm->ifh->glm_Nrow*glm->ifh->glm_Mcol))) {
    printf("fidlError: Unable to malloc A\n");
    exit(-1);
    }
for(k=i=0;i<glm->ifh->glm_Nrow;i++) for(j=0;j<glm->ifh->glm_Mcol;j++,k++) A[k] = (double)glm->AT[j][i];
gsl_matrix_view gA = gsl_matrix_view_array(A,glm->ifh->glm_Nrow,glm->ifh->glm_Mcol);
gsl_blas_dgemm(CblasTrans,CblasNoTrans,1.0,&gA.matrix,&gA.matrix,0.0,&gATA.matrix);
for(k=i=0;i<glm->ifh->glm_Mcol;i++) for(j=0;j<glm->ifh->glm_Mcol;j++,k++) temp_double[k] = ATA[i][j];
cond_norm1(temp_double,glm->ifh->glm_Mcol,&cond,ATAm1[0],perm);
printf("    Condition number norm1: %f\n",cond);
if(cond>condmax||cond==0.) {
    if(cond>condmax) printf("    Condition number greater than %f.\n",condmax);
    else printf("    Design matrix not invertible.\n");
    printf("    Computing singular value decomposition.\n");
    td=gsl_svd_golubreinsch(ATA[0],glm->ifh->glm_Mcol,glm->ifh->glm_Mcol,0.,V,S,ATAm1[0],&cond_norm2,&ndep,work);
    printf("    gsl_svd_golubreinsch cond_norm2=%f ndep=%d tol=%g\n",cond_norm2,ndep,td);
    }
printf("\n");
fflush(stdout);
}

#if 0
                        if(cond>condmax||cond==0.) {
                            printf("    i=%d subject %d\n",i,i+1); fflush(stdout);
                            if(cond>condmax) printf("    Condition number greater than %d.",condmax);
                            else printf("    Design matrix not invertible.");
                            printf(" Computing singular value decomposition.\n");

td=gsl_svd_golubreinsch(transformATAtransform,tcs->num_tc,tcs->num_tc,0.,V,Ssvd,ATAm1,&cond_norm2,&ndep,
                                worksvd);
                            printf("    gsl_svd_golubreinsch cond_norm2=%f ndep=%d tol=%g\n",cond_norm2,ndep,td);
                            }
#endif
