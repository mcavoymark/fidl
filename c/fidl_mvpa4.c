/* Copyright 9/13/13 Washington University.  All Rights Reserved.
   fidl_mvpa4.c  $Revision: 1.4 $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <math.h>
#include <float.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_sort_int.h>
#include "fidl.h"
#include "anova_header_new.h"
#include "get_atlas_param.h"
#include "subs_mask.h"
#include "dim_param2.h"
#include "subs_util.h"
#include "d2double.h"
#include "cond_norm1.h"
#include "gsl_svd.h"
#include "get_atlas_coor.h"
#include "minmax.h"
#ifdef __sun__
    #include <nan.h>
#endif
static char rcsid[] = "$Header: /home/hannah/mcavoy/idl/clib/RCS/fidl_mvpa4.c,v 1.4 2017/02/21 20:36:55 mcavoy Exp $";
#define MAXINC 2
#define MAXIT 5
#define TOL 0.1 
#define NR_FOLD 5
#include "svm.h"
#ifndef M_LN2
    #define M_LN2 0.693147180559945309417
#endif
typedef struct{
    double **v,**s,**sinv,*V,*S,*work,*vec,*tdvm,*wts;
    int **colsumn,**colsumalltrialsn,*ilistidx,*havei,**sumtdim,nwhichtrials,*whichtrials,nilistidx,*nonzero1,*have,nihave,*ihave,
        *whichclass,*imaxv,*correctbyclass;
    size_t *whichtrialsnonzero,*whichvoxel;
    }Gutsv;
typedef struct {
    double **colsum,**colsumalltrials,**mean_colsum,**mean_multicolsum,*halfmean1plusmean2,*mean1minusmean2;
    } Ldav;
typedef struct{
    double **X,*Az,*y,*wnew;
    int sumtdimmax,*N,*Iwv;
    }Lrv;
typedef struct {
    double *y,*wnew;
    double *feature_max,*feature_min,*feature_dif,lower,upper,dif;
    int cross_validation,sumtdimmax,*Iwv,*N;
    struct svm_parameter param;
    struct svm_problem *prob;
    struct svm_node *x_space,*x;
    } Svmv;
typedef struct{
    int *which,*list,*listi;
    size_t *iwhich;
    }Loo;
typedef struct {
   double *trial,**class,**pc;
    } Pc;
typedef struct {
   double ***wts;
   size_t ***wtsn;
    } Wts;
Gutsv *allocGutsv(int num_datafiles,int nclass,int ncoor);
Ldav *allocLdav(int nclass,int ncoor);
Lrv *allocLrv(int num_datafiles,int ncoor);
Svmv *allocSvmv(int num_datafiles,int ncoor);
void initSvmv(double svmscalel,double svmscaleu,double svmtrainc,Svmv *gsvm);
Svmgroupclassesv *allocSvmgroupclassesv(int nclass,int num_datafiles);
struct svm_model *allocModel(int nclass,int num_datafiles,struct svm_parameter *param);
struct svm_problem *allocsubprob(int num_datafiles);
Svmtrainv *allocSvmtrainv(Svmv *gsvm,int nclass,int num_datafiles);
Svmbinarysvcprobabilityv *allocSvmbinarysvcprobabilityv(Svmv *gsvm,int nclass,int num_datafiles);
Svmsvrprobabilityv *allocSvmsvrprobabilityv(int nclass,int num_datafiles,Svmv *gsvm); 
Svmcrossvalidationv *allocSvmcrossvalidationv(int nclass,int num_datafiles,Svmv *gsvm);
Svmpredictvaluesv *allocSvmpredictvaluesv(int nclass,int num_datafiles);
Svmpredictv *allocSvmpredictv(int nclass,int num_datafiles,int svm_type); 
Multiclassprobabilityv *allocMulticlassprobabilityv(int nclass); 
Svmpredictprobabilityv *allocSvmpredictprobabilityv(int nclass,int num_datafiles,struct svm_parameter param);
Loo *allocLoo(int num_datafiles,int n);
Loo *allocLootrial(int num_datafiles);
Pc *allocPc(int num_regions,int num_region_files,int nclass,int n);
Wts *allocWts(int nclass0,int ncoor_,int num_regions);
int ldaguts(Gutsv *g,Ldav *glda,Loo *loo,Wts *w,int nclass,int nclass0,int n_,int nlist_,size_t *correctby_,
    float *temp_float,double *temp_double,size_t i,double lambdalda); 
int lrguts(Gutsv *g,Lrv *glr,Loo *loo,Wts *w,int nclass,int nclass0,int n_,int nlist_,size_t *correctby_,
    float *temp_float,double *temp_double,int i,int ncoor,double lambdalr); 
int svmguts(Gutsv *g,Svmv *gsvm,Svmtrainv *svmtrainv,Svmbinarysvcprobabilityv *svmbinarysvcprobabilityv,
    Svmsvrprobabilityv *svmsvrprobabilityv,Svmpredictv *svmpredictv,Loo *loo,int nclass,int n_,
    int nlist_,size_t *correctby_,float *temp_float,int i); 
int printreg(char *f,char *which,Regions_By_File *rbf,Pc *pc,int class_lnptri,int nclass,char **ad_lnptr,
    double *xx,double *yy,double *zz,Atlas_Param *ap,double *coor1,int nclass0,double ***_wts_,int n_); 
int printimg(Mask_Struct *ms,float *temp_float,double *_pctrial_,char *f,Interfile_header *ifh,int class_lnptri,int ncoor,
    char **ad_lnptr,double ***_wts_,int nclass0);
void printfp(FILE *fp,int nclass,char *region_names_ptr,Gutsv *g);
int main(int argc,char **argv){
char string[MAXNAME],*outfile=NULL,*maskfile=NULL,*driverf=NULL,ldaftrial[MAXNAME],ldafrun[MAXNAME],ldafsession[MAXNAME],
    ldafsubject[MAXNAME],lrftrial[MAXNAME],lrfrun[MAXNAME],lrfsession[MAXNAME],lrfsubject[MAXNAME],*cleanup=NULL,
    svmftrial[MAXNAME],svmfrun[MAXNAME],svmfsession[MAXNAME],svmfsubject[MAXNAME],atlas[7]="";
const char *error_msg;
int i,j,k,l,m,ii,ll,num_region_files=0,*roi=NULL,SunOS_Linux,correct,xdim,ydim,zdim,x,y,zi,x1,y1,zi1,area=0,
    col_row,ncoor=0,*coor=NULL,index,nonzero,modulo=1000,lccompressed=0,nidx,*idx,lctrial=0,classi=-1,
    *trialsperclass,class_lnptri=0,num_regions=0,*listidx,lcrun=0,runi=-1,run_lnptri,lcsession=0,
    sessioni=-1,session_lnptri,lcsubject=0,subjecti=-1,subject_lnptri,lclda=0,lclr=0,lcsvm=0,nsession=0,nsubject=0,nclass,nclass0,
    nrun=0,nlistrun,nlistsession,nlistsubject,nlisttrial;
size_t *correctby_=NULL;
    //nsession,nsubject,nclass,nclass0,nrun,nlistrun,nlistsession,nlistsubject,nlisttrial
float *temp_float,*temp_float2,diameter,tf;
double td,*temp_double,*xx=NULL,*yy=NULL,*zz=NULL,*coor1=NULL,lambdalda=0.,lambdalr=0.,svmtrainc=1.,svmscalel=-1.,svmscaleu=1.;
FILE *ldafptrial=NULL,*ldafprun=NULL,*ldafpsession=NULL,*ldafpsubject=NULL,*lrfptrial=NULL,*lrfprun=NULL,*lrfpsession=NULL,
    *lrfpsubject=NULL,*svmfptrial=NULL,*svmfprun=NULL,*svmfpsession=NULL,*svmfpsubject=NULL; 
Files_Struct *region_files=NULL;
Dim_Param2 *dp;
Regions **reg;
Regions_By_File *rbf=NULL;
Memory_Map *mm;
Mask_Struct *ms=NULL;
Interfile_header *ifh=NULL;
AnovaDesign *ad;
Atlas_Param *ap=NULL;
Gutsv *g;
Ldav *glda=NULL;
Lrv *glr=NULL;
Svmv *gsvm=NULL;
Svmtrainv *svmtrainv=NULL;
Svmbinarysvcprobabilityv *svmbinarysvcprobabilityv=NULL;
Svmsvrprobabilityv *svmsvrprobabilityv=NULL;
Svmpredictv *svmpredictv=NULL;

//START170203
#if 0
Files_Struct *files;
FS *fs;
Multiclassprobabilityv *multiclassprobabilityv;
#endif

Loo *trial=NULL,*run=NULL,*subject=NULL,*session=NULL;
Pc *pcldatrial=NULL,*pclrtrial=NULL,*pcsvmtrial=NULL,*pcldarun=NULL,*pclrrun=NULL,*pcsvmrun=NULL,*pcldasession=NULL,*pclrsession=NULL,
    *pcsvmsession=NULL,*pcldasubject=NULL,*pclrsubject=NULL,*pcsvmsubject=NULL;
Wts *wldatrial=NULL,*wlrtrial=NULL,*wldarun=NULL,*wlrrun=NULL,*wldasession=NULL,*wlrsession=NULL,*wldasubject=NULL,*wlrsubject=NULL; 
    //*wsvmrun,*wsvmsession,*wsvmtrial,*wsvmsubject
if(argc<6) {
    fprintf(stderr,"    -out:                 Output files will have this root. Extension is ignored. Default is fidl_mvpa\n");
    fprintf(stderr,"Analysis can be performed on regions\n");
    fprintf(stderr,"    -region_file:         4dfp file(s) that specifies regions of interest.\n");
    fprintf(stderr,"                          Analysis is over voxels in each region.\n");
    fprintf(stderr,"    -regions_of_interest: First region is one.\n");
    fprintf(stderr,"or voxels within a mask\n");
    fprintf(stderr,"    -diameter:            Diameter (mm) of sphere for searchlight.\n");
    fprintf(stderr,"    -driver:              Driving file.\n");
    fprintf(stderr,"    -mask:                Only mask voxels are included in the computation.\n");
    fprintf(stderr,"    -trial                Leave one out by trial.\n");
    fprintf(stderr,"    -run                  Leave one out by run.\n");
    fprintf(stderr,"    -session              Leave one out by session.\n");
    fprintf(stderr,"    -subject              Leave one out by session.\n");
    fprintf(stderr,"    -lda:                 Linear discriminant analysis with regularization. 0 for no regularization.\n");
    fprintf(stderr,"    -lr:                  Logistic regression with regularization. 0 for no regularization.\n");

    fprintf(stderr,"    -svmtrainc:          svm-train -c cost :set the parameter C of C-SVC, epsilon-SVR, and nu-SVR (default 1)\n");
    fprintf(stderr,"    -svmscalel:          svm-scale -l lower :x scaling lower limit (default -1)\n");
    fprintf(stderr,"    -svmscaleu:          svm-scale -u upper :x scaling upper limit (default +1)\n");

    fprintf(stderr,"    -cleanup:             Remove this directory at completion.\n");
    fprintf(stderr,"    -xdim:                Used if no mask or region file.\n");
    fprintf(stderr,"    -ydim:                Used if no mask or region file.\n");
    fprintf(stderr,"    -zdim:                Used if no mask or region file.\n");
    exit(-1);
    }
for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-out") && argc > i+1)
        outfile = argv[++i];
    if(!strcmp(argv[i],"-region_file") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_region_files;
        if(!(region_files=get_files(num_region_files,&argv[i+1]))) exit(-1);
        i += num_region_files;
        }
    if(!strcmp(argv[i],"-regions_of_interest") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_regions;
        if(!(roi=malloc(sizeof*roi*num_regions))) {
            printf("Error: Unable to malloc roi\n");
            exit(-1);
            }
        for(j=0;j<num_regions;j++) roi[j] = atoi(argv[++i]) - 1;
        }
    if(!strcmp(argv[i],"-mask") && argc > i+1)
        maskfile = argv[++i];
    if(!strcmp(argv[i],"-diameter") && argc > i+1)
        diameter = atof(argv[++i]);
    if(!strcmp(argv[i],"-driver") && argc > i+1)
        driverf = argv[++i];
    if(!strcmp(argv[i],"-trial"))
        lctrial = 1;
    if(!strcmp(argv[i],"-run"))
        lcrun = 1;
    if(!strcmp(argv[i],"-session"))
        lcsession = 1;
    if(!strcmp(argv[i],"-subject"))
        lcsubject = 1;
    if(!strcmp(argv[i],"-lda") && argc > i+1) {
        lclda = 1;
        lambdalda = atof(argv[++i]);
        }
    if(!strcmp(argv[i],"-lr") && argc > i+1) {
        lclr = 1;
        lambdalr = atof(argv[++i]);
        }
    if(!strcmp(argv[i],"-svmtrainc") && argc > i+1) {
        lcsvm = 1;
        svmtrainc = atof(argv[++i]);
        }
    if(!strcmp(argv[i],"-svmscalel") && argc > i+1) {
        lcsvm = 1;
        svmscalel = atof(argv[++i]); 
        }
    if(!strcmp(argv[i],"-svmscaleu") && argc > i+1) {
        lcsvm = 1;
        svmscaleu = atof(argv[++i]); 
        }
    if(!strcmp(argv[i],"-cleanup") && argc > i+1)
        cleanup = argv[++i];
    if(!strcmp(argv[i],"-xdim") && argc > i+1)
        xdim = atoi(argv[++i]);
    if(!strcmp(argv[i],"-ydim") && argc > i+1)
        ydim = atoi(argv[++i]);
    if(!strcmp(argv[i],"-zdim") && argc > i+1)
        zdim = atoi(argv[++i]);
    }
print_version_number(rcsid,stdout);
if((SunOS_Linux=checkOS())==-1) exit(-1);
printf("SunOS_Linux=%d\n",SunOS_Linux);
gsl_set_error_handler_off();
if(!driverf) {
    printf("fidlError: Provide driver file with -driver\n");
    exit(-1);
    }
if(num_region_files&&maskfile) {
    printf("fidlError: Just one. Specify either -region_file or -mask\n");
    exit(-1);
    }
if(!lclda&&!lclr&&!lcsvm) {
    printf("fidlError: Need to specify '-lda 0.0' and/or '-lr 0.0' (for no regulariazion) and/or '-svmtrainc 1.0'\n");
    exit(-1);
    }
if(outfile) {
    if(!get_tail_sans_ext(outfile)) exit(-1);
    }
else {
    if(!(outfile=malloc(sizeof*outfile*10))) {
        printf("fidlError: Unable to malloc outfile\n");
        exit(-1);
        }
    strcpy(outfile,"fidl_mvpa"); 
    }        
if(lclda) {
    if(lctrial) sprintf(ldaftrial,"%s_trial_lda%g%s",outfile,lambdalda,num_region_files?".txt":".4dfp.img");
    if(lcrun) sprintf(ldafrun,"%s_run_lda%g%s",outfile,lambdalda,num_region_files?".txt":".4dfp.img");
    if(lcsession) sprintf(ldafsession,"%s_session_lda%g%s",outfile,lambdalda,num_region_files?".txt":".4dfp.img");
    if(lcsubject) sprintf(ldafsubject,"%s_subject_lda%g%s",outfile,lambdalda,num_region_files?".txt":".4dfp.img");
    }
if(lclr) {
    if(lctrial) sprintf(lrftrial,"%s_trial_lr%g%s",outfile,lambdalr,num_region_files?".txt":".4dfp.img");
    if(lcrun) sprintf(lrfrun,"%s_run_lr%g%s",outfile,lambdalr,num_region_files?".txt":".4dfp.img");
    if(lcsession) sprintf(lrfsession,"%s_session_lr%g%s",outfile,lambdalr,num_region_files?".txt":".4dfp.img");
    if(lcsubject) sprintf(lrfsubject,"%s_subject_lr%g%s",outfile,lambdalr,num_region_files?".txt":".4dfp.img");
    }
if(lcsvm) {
    if(lctrial) sprintf(svmftrial,"%s_trial_svmC%g%s",outfile,svmtrainc,num_region_files?".txt":".4dfp.img");
    if(lcrun) sprintf(svmfrun,"%s_run_C%g%s",outfile,svmtrainc,num_region_files?".txt":".4dfp.img");
    if(lcsession) sprintf(svmfsession,"%s_session_svmC%g%s",outfile,svmtrainc,num_region_files?".txt":".4dfp.img");
    if(lcsubject) sprintf(svmfsubject,"%s_subject_svmC%g%s",outfile,svmtrainc,num_region_files?".txt":".4dfp.img");
    }

//if(!(ad=read_driver2(driverf,1,3,1))) exit(-1);
//START170207
if(!(ad=read_driver2(driverf,1,3,1,'_',1))) exit(-1);

printf("ad->num_factors=%d ",ad->num_factors); for(i=0;i<ad->num_factors;i++) printf("%s ",ad->fnptr[i]); printf("\n");
printf("ad->Perlman_levels=");for(i=0;i<ad->num_factors;i++) printf("%d ",ad->Perlman_levels[i]); printf("\n");
for(j=i=0;i<ad->num_factors;j+=ad->Perlman_levels[i++]) {
    if(!strcasecmp(ad->fnptr[i],"class")) {classi=i;class_lnptri=j;}
    if(!strcasecmp(ad->fnptr[i],"run")) {runi=i;run_lnptri=j;}
    if(!strcasecmp(ad->fnptr[i],"session")) {sessioni=i;session_lnptri=j;}
    if(!strcasecmp(ad->fnptr[i],"subject")) {subjecti=i;subject_lnptri=j;}
    }
printf("classi=%d runi=%d sessioni=%d subjecti=%d lclda=%d lclr=%d lambdalda=%f lambdalr=%f\n",classi,runi,sessioni,subjecti,lclda,
    lclr,lambdalda,lambdalr);


if(classi==-1) {
    printf("fidlError: class must be labeled at the top of %s\n",driverf);
    fflush(stdout);exit(-1);
    }
nclass = ad->Perlman_levels[classi];
nclass0 = nclass==2 ? 1 : nclass;
if(lcrun) {
    if(runi==-1) {
        printf("fidlError: run must be labeled at the top of %s\n",driverf);
        fflush(stdout);exit(-1);
        }
    if((nrun=ad->Perlman_levels[runi])==1) {
        printf("fidlError: You have just a single run in %s\n",driverf);
        fflush(stdout);exit(-1);
        }
    }
if(lcsession) {
    if(sessioni==-1) {
        printf("fidlError: session must be labeled at the top of %s\n",driverf);
        fflush(stdout);exit(-1);
        }
    if((nsession=ad->Perlman_levels[sessioni])==1) {
        printf("fidlError: You have just a single session in %s\n",driverf);
        fflush(stdout);exit(-1);
        }
    }
if(lcsubject) {
    if(subjecti==-1) {
        printf("fidlError: subject must be labeled at the top of %s\n",driverf);
        fflush(stdout);exit(-1);
        }
    if((nsubject=ad->Perlman_levels[subjecti])==1) {
        printf("fidlError: You have just a single subject in %s\n",driverf);
        fflush(stdout);exit(-1);
        }
    }

//if(!(dp=dim_param(ad->num_datafiles,ad->datafiles,SunOS_Linux,0))) exit(-1);
//START170202
if(!(dp=dim_param2(ad->num_datafiles,ad->datafiles,SunOS_Linux)))exit(-1);
if(!dp->volall==-1){printf("fidlError: fidl_mvpa4 All files must have the same volume.\n");fflush(stdout);exit(-1);}

if(num_region_files) {

    #if 0
    if(!check_dimensions(num_region_files,region_files->files,dp->vol)) {
        lccompressed = 1;
        printf("    Error negated. Setting lccompressed. dp->vol=%d lccompressed=%d\n",dp->vol,lccompressed);fflush(stdout);
        }
    #endif
    //START170202
    if(!check_dimensions(num_region_files,region_files->files,dp->volall)){
        lccompressed=1;
        printf("    Error negated. Setting lccompressed. dp->volall=%d lccompressed=%d\n",dp->volall,lccompressed);fflush(stdout);
        }

    if(!(reg=malloc(sizeof*reg*num_region_files))) {
        printf("fidlError: Unable to malloc reg\n");
        exit(-1);
        }
    for(m=0;m<num_region_files;m++) {
        if(!(reg[m] = extract_regions(region_files->files[m],0,0,(float*)NULL,0,SunOS_Linux,(char**)NULL))) exit(-1);
        }
    if(!num_regions) {
        for(m=0;m<num_region_files;m++) num_regions += reg[m]->nregions;
        if(!(roi=malloc(sizeof*roi*num_regions))) {
            printf("Error: Unable to malloc roi\n");
            exit(-1);
            }
        for(j=0;j<num_regions;j++) roi[j] = j;
        }
    if(!(rbf=find_regions_by_file_cover(num_region_files,num_regions,reg,roi))) exit(-1);
    ncoor = rbf->nvoxels_region_max;

    //START170209
    get_atlas(reg[0]->vol,atlas);
    if(!atlas[0]){if(!(ifh=read_ifh(region_files->files[0],(Interfile_header*)NULL)))exit(-1);}
    if(!(ap=get_atlas_param(atlas,atlas[0]?NULL:ifh))) exit(-1);

    }
else {

    //if(!(ms=get_mask_struct(maskfile,dp->vol,(int*)NULL,SunOS_Linux,(LinearModel*)NULL,dp->vol))) exit(-1);
    //START170202
    if(!(ms=get_mask_struct(maskfile,dp->volall,(int*)NULL,SunOS_Linux,(LinearModel*)NULL,dp->volall)))exit(-1);

    printf("ms->lenvol=%d ms->lenbrain=%d\n",ms->lenvol,ms->lenbrain);fflush(stdout);

    #if 0
    if(ms->lenvol!=dp->vol) {
        if(ms->lenbrain!=dp->vol) {
            printf("ms->lenbrain=%d dp->vol=%d Must be equal.\n",ms->lenbrain,dp->vol); 
            exit(-1);
            }
        lccompressed = 1;
        printf("ms->lenvol=%d dp->vol=%d lccompressed=%d dp->dxdy=%f\n",ms->lenvol,dp->vol,lccompressed,dp->dxdy);
        }
    #endif
    //START170202
    if(ms->lenvol!=dp->volall) {
        if(ms->lenbrain!=dp->volall) {
            printf("ms->lenbrain=%d dp->volall=%d Must be equal.\n",ms->lenbrain,dp->volall);
            exit(-1);
            }
        lccompressed = 1;
        printf("ms->lenvol=%d dp->volall=%d lccompressed=%d dp->dxdy[0]=%f\n",ms->lenvol,dp->volall,lccompressed,dp->dxdy[0]);
        }


    }

#if 0
atlas = get_atlas(num_region_files?reg[0]->vol:ms->lenvol);
if(!(ap=get_atlas_param(atlas,(Interfile_header*)NULL))) exit(-1);
#endif
#if 0
//START170209
get_atlas(num_region_files?reg[0]->vol:ms->lenvol,atlas);
if(!atlas[0]){if(!(ifh=read_ifh(num_region_files?region_files->files[0]:mask_file,(Interfile_header*)NULL)))exit(-1);}
if(!(ap=get_atlas_param(atlas,atlas[0]?NULL:ifh))) exit(-1);
#endif




/*printf("atlas=%d lctrial=%d lcrun=%d lccompressed=%d classi=%d nclass=%d nclass0=%d lccompressed=%d\n",atlas,lctrial,lcrun,
    lccompressed,classi,nclass,nclass0,lccompressed);*/
if(!(temp_float2=malloc(sizeof*temp_float2*(num_region_files?rbf->nvoxels_region_max:ms->lenvol)))) {
    printf("Error: Unable to malloc temp_float2\n");
    exit(-1);
    }
if(!num_region_files) {
    if(maskfile){xdim=ms->xdim;ydim=ms->ydim;zdim=ms->zdim;}

    //printf("diameter=%f dp->dxdy=%f dp->dz=%f xdim=%d ydim=%d zdim=%d\n",diameter,dp->dxdy,dp->dz,xdim,ydim,zdim);fflush(stdout);
    //START170202
    printf("diameter=%f dp->dxdy[0]=%f dp->dz[0]=%f xdim=%d ydim=%d zdim=%d\n",diameter,dp->dxdy[0],dp->dz[0],xdim,ydim,zdim);
    fflush(stdout);

    for(i=0;i<ms->lenvol;i++) temp_float2[i]=0.;
    x=(int)rint((double)xdim/2.);y=(int)rint((double)ydim/2.);zi=(int)rint((double)zdim/2.);tf=1.;

    #if 0
    if(!(ncoor=ellipsestack(1,&x,&y,&zi,temp_float2,&tf,xdim,ydim,zdim,&diameter,&diameter,&diameter,dp->dxdy,dp->dxdy,dp->dz)))
        exit(-1);
    #endif
    //START170202
    if(!(ncoor=ellipsestack(1,&x,&y,&zi,temp_float2,&tf,xdim,ydim,zdim,&diameter,&diameter,&diameter,dp->dxdy[0],dp->dxdy[0],
        dp->dz[0])))exit(-1);

    #if 0
    sprintf(string,"sphere%.fmmdia%dvoxels.4dfp.img",diameter,ncoor);
    if(!writestack(string,temp_float2,sizeof*temp_float2,(size_t)ms->lenvol,0)) exit(-1);
    //if(!(ifh=init_ifh(4,xdim,ydim,zdim,1,dp->dxdy,dp->dxdy,dp->dz,SunOS_Linux?0:1))) exit(-1);
    //START170202
    if(!(ifh=init_ifh(4,xdim,ydim,zdim,1,dp->dxdy[0],dp->dxdy[0],dp->dz[0],SunOS_Linux?0:1))) exit(-1);
    ifh->global_min=0.; ifh->global_max=1;
    if(!write_ifh(string,ifh,0)) exit(-1);
    printf("Sphere written to %s\n",string);fflush(stdout);
    exit(-1);
    #endif
    if(!(coor=malloc(sizeof*coor*ncoor*3))) {
        printf("Error: Unable to malloc coor\n");
        exit(-1);
        }
    area = xdim*ydim;
    for(j=i=0;i<ms->lenvol;i++) {
        if(temp_float2[i]>0.) {
            zi1 = i/area;
            col_row = i-zi1*area;
            y1 = col_row/xdim;
            x1 = col_row - y1*xdim;
            coor[j++]=x1-x;
            coor[j++]=y1-y;
            coor[j++]=zi1+1-zi;
            /*index = zi1*area + y1*xdim + x1;
            printf("%d %d %d %f i=%d index=%d\n",coor[j-3],coor[j-2],coor[j-1],temp_float2[i],i,index);*/
            }
        }

    num_regions = ms->lenbrain;
    }
if(!(idx=malloc(sizeof*idx*ncoor))) {
    printf("fidlError: Unable to malloc idx\n");
    exit(-1);
    }
if(!(temp_float=malloc(sizeof*temp_float*ad->num_datafiles*ncoor))) {
    printf("fidlError: Unable to malloc temp_float\n");
    exit(-1);
    }
if(!(temp_double=malloc(sizeof*temp_double*(ncoor+1)*(ncoor+1)))) {
    printf("Error: Unable to malloc temp_double\n");
    exit(-1);
    }
if(!(trialsperclass=malloc(sizeof*trialsperclass*nclass))) {
    printf("Error: Unable to malloc trialsperclass\n");
    exit(-1);
    }
if(!(listidx=malloc(sizeof*listidx*ncoor))) {
    printf("Error: Unable to malloc listidx\n");
    exit(-1);
    }

if(!(g=allocGutsv(ad->num_datafiles,nclass,ncoor))) {
    printf("fidlError: allocation failure allocGutsv\n");
    exit(-1);
    }
if(lclda)
    if(!(glda=allocLdav(nclass,ncoor))) {
        printf("fidlError: allocation failure allocLdav\n");
        exit(-1);
        }
if(lclr)
    if(!(glr=allocLrv(ad->num_datafiles,ncoor))) {
        printf("fidlError: allocation failure allocLrv\n");
        exit(-1);
        }


if(lcsvm) {
    if(!(gsvm=allocSvmv(ad->num_datafiles,ncoor))) {
        printf("fidlError: allocation failure allocSvmv\n");
        exit(-1);
        }
    initSvmv(svmscalel,svmscaleu,svmtrainc,gsvm);
    if((error_msg=svm_check_parameter(gsvm->prob,&gsvm->param))){printf("fidlERROR: %s\n",error_msg);fflush(stdout);exit(-1);}
    if(!(svmtrainv=allocSvmtrainv(gsvm,nclass,ad->num_datafiles))) {
        printf("fidlError: allocation failure allocSvmtrainv\n");
        exit(-1);
        }
    if(gsvm->param.probability)
        if(!(svmbinarysvcprobabilityv=allocSvmbinarysvcprobabilityv(gsvm,nclass,ad->num_datafiles))) {
            printf("fidlError: allocation failure allocSvmbinarysvcprobabilityv\n");
            exit(-1);
            }
    if(gsvm->param.svm_type == ONE_CLASS || gsvm->param.svm_type == EPSILON_SVR || gsvm->param.svm_type == NU_SVR)

        //if(!(svmsvrprobabilityv=allocSvmsvrprobabilityv(nclass,ad->num_datafiles,ncoor,gsvm))) {
        //START170203
        if(!(svmsvrprobabilityv=allocSvmsvrprobabilityv(nclass,ad->num_datafiles,gsvm))) {

            printf("fidlError: allocation failure allocSvmsvrprobabilityv\n");
            exit(-1);
            }
    if(!(svmpredictv=allocSvmpredictv(nclass,ad->num_datafiles,gsvm->param.svm_type))) {
        printf("fidlError: allocation failure allocSvmpredictv\n");
        exit(-1);
        }
    }
if(lctrial) {
    if(!(trial=allocLootrial(ad->num_datafiles))) {
        printf("fidlError: allocation failure allocLootrial\n");
        exit(-1);
        }
    if(lclda) {
        if(!(pcldatrial=allocPc(num_regions,num_region_files,nclass,0))) {
            printf("fidlError: allocation failure allocPc for pcldatrial\n");
            exit(-1);
            }
        if(!(wldatrial=allocWts(nclass0,ncoor,num_regions))) {
            printf("fidlError: allocation failure allocWts for wldatrial\n");
            exit(-1);
            }
        }
    if(lclr) {
        if(!(pclrtrial=allocPc(num_regions,num_region_files,nclass,0))) {
            printf("fidlError: allocation failure allocPc for pclrtrial\n");
            exit(-1);
            }
        if(!(wlrtrial=allocWts(nclass0,ncoor+1,num_regions))) {
            printf("fidlError: allocation failure allocWts for wlrtrial\n");
            exit(-1);
            }
        }
    if(lcsvm) {
        if(!(pcsvmtrial=allocPc(num_regions,num_region_files,nclass,0))) {
            printf("fidlError: allocation failure allocPc for pcsvmtrial\n");
            exit(-1);
            }
        /*START140225*/
        #if 0
        if(!(wsvmtrial=allocWts(nclass0,ncoor,num_regions))) {
            printf("fidlError: allocation failure allocWts for wsvmtrial\n");
            exit(-1);
            }
        #endif
        }
    }
if(lcrun) {
    if(!(run=allocLoo(ad->num_datafiles,nrun))) {
        printf("fidlError: allocation failure allocLoo for run\n");
        exit(-1);
        }
    if(lclda) {
        if(!(pcldarun=allocPc(num_regions,num_region_files,nclass,nrun))) {
            printf("fidlError: allocation failure allocPc for pcldarun\n");
            exit(-1);
            }
        if(!(wldarun=allocWts(nclass0,ncoor,num_regions))) {
            printf("fidlError: allocation failure allocWts for wldarun\n");
            exit(-1);
            }
        }
    if(lclr) {
        if(!(pclrrun=allocPc(num_regions,num_region_files,nclass,nrun))) {
            printf("fidlError: allocation failure allocPc for pclrrun\n");
            exit(-1);
            }
        if(!(wlrrun=allocWts(nclass0,ncoor+1,num_regions))) {
            printf("fidlError: allocation failure allocWts for wlrrun\n");
            exit(-1);
            }
        }
    if(lcsvm) {
        if(!(pcsvmrun=allocPc(num_regions,num_region_files,nclass,nrun))) {
            printf("fidlError: allocation failure allocPc for pcsvmrun\n");
            exit(-1);
            }
        /*START140225*/
        #if 0
        if(!(wsvmrun=allocWts(nclass0,ncoor,num_regions))) {
            printf("fidlError: allocation failure allocWts for wsvmrun\n");
            exit(-1);
            }
        #endif
        }
    }
if(lcsession) {
    if(!(session=allocLoo(ad->num_datafiles,nsession))) {
        printf("fidlError: allocation failure allocLoo for session\n");
        exit(-1);
        }
    if(lclda) {
        if(!(pcldasession=allocPc(num_regions,num_region_files,nclass,nsession))) {
            printf("fidlError: allocation failure allocPc for pcldasession\n");
            exit(-1);
            }
        if(!(wldasession=allocWts(nclass0,ncoor,num_regions))) {
            printf("fidlError: allocation failure allocWts for wldasession\n");
            exit(-1);
            }
        }
    if(lclr) {
        if(!(pclrsession=allocPc(num_regions,num_region_files,nclass,nsession))) {
            printf("fidlError: allocation failure allocPc for pclrsession\n");
            exit(-1);
            }
        if(!(wlrsession=allocWts(nclass0,ncoor+1,num_regions))) {
            printf("fidlError: allocation failure allocWts for wlrsession\n");
            exit(-1);
            }
        }
    if(lcsvm) {
        if(!(pcsvmsession=allocPc(num_regions,num_region_files,nclass,nsession))) {
            printf("fidlError: allocation failure allocPc for pcsvmsession\n");
            exit(-1);
            }
        /*START140225*/
        #if 0
        if(!(wsvmsession=allocWts(nclass0,ncoor,num_regions))) {
            printf("fidlError: allocation failure allocWts for wsvmsession\n");
            exit(-1);
            }
        #endif
        }
    }
if(lcsubject) {
    if(!(subject=allocLoo(ad->num_datafiles,nsubject))) {
        printf("fidlError: allocation failure allocLoo for subject\n");
        exit(-1);
        }
    if(lclda) {
        if(!(pcldasubject=allocPc(num_regions,num_region_files,nclass,nsubject))) {
            printf("fidlError: allocation failure allocPc for pcldasubject\n");
            exit(-1);
            }
        if(!(wldasubject=allocWts(nclass0,ncoor,num_regions))) {
            printf("fidlError: allocation failure allocWts for wldasubject\n");
            exit(-1);
            }
        }
    if(lclr) {
        if(!(pclrsubject=allocPc(num_regions,num_region_files,nclass,nsubject))) {
            printf("fidlError: allocation failure allocPc for pclrsubject\n");
            exit(-1);
            }
        if(!(wlrsubject=allocWts(nclass0,ncoor+1,num_regions))) {
            printf("fidlError: allocation failure allocWts for wlrsubject\n");
            exit(-1);
            }
        }
    if(lcsvm) {
        if(!(pcsvmsubject=allocPc(num_regions,num_region_files,nclass,nsubject))) {
            printf("fidlError: allocation failure allocPc for pcsvmsubject\n");
            exit(-1);
            }
        /*START140225*/
        #if 0
        if(!(wsvmsubject=allocWts(nclass0,ncoor,num_regions))) {
            printf("fidlError: allocation failure allocWts for wsvmsubject\n");
            exit(-1);
            }
        #endif
        }
    }
if(num_region_files) {
    i=0;
    if(lcrun) i=nrun;
    if(lcsession) if(nsession>i)i=nsession;
    if(lcsubject) if(nsubject>i)i=nsubject;
    if(i) if(!(correctby_=malloc(sizeof*correctby_*i))) {
        printf("fidlError: Unable to malloc correctby_\n");
        exit(-1);
        }
    if(!(xx=malloc(sizeof*xx*rbf->nvoxels))) {
        printf("fidlError: Unable to malloc xx\n");
        exit(-1);
        }
    if(!(yy=malloc(sizeof*yy*rbf->nvoxels))) {
        printf("fidlError: Unable to malloc yy\n");
        exit(-1);
        }
    if(!(zz=malloc(sizeof*zz*rbf->nvoxels))) {
        printf("fidlError: Unable to malloc zz\n");
        exit(-1);
        }
    if(!(coor1=malloc(sizeof*coor1*rbf->nvoxels*3))) {
        printf("Error: Unable to malloc coor1\n");
        exit(-1);
        }
    if(lctrial) {
        if(lclda) {
            if(!(ldafptrial=fopen_sub(ldaftrial,"w"))) exit(-1);
            fprintf(ldafptrial,"classes\n-------\n");
            for(j=class_lnptri,i=0;i<nclass;i++,j++) fprintf(ldafptrial,"%d\t%s\n",i+1,ad->lnptr[j]);
            fprintf(ldafptrial,"\n");
            }
        if(lclr) {
            if(!(lrfptrial=fopen_sub(lrftrial,"w"))) exit(-1);
            fprintf(lrfptrial,"classes\n-------\n");
            for(j=class_lnptri,i=0;i<nclass;i++,j++) fprintf(lrfptrial,"%d\t%s\n",i+1,ad->lnptr[j]);
            fprintf(lrfptrial,"\n");
            }
        if(lcsvm) {
            if(!(svmfptrial=fopen_sub(svmftrial,"w"))) exit(-1);
            fprintf(svmfptrial,"classes\n-------\n");
            for(j=class_lnptri,i=0;i<nclass;i++,j++) fprintf(svmfptrial,"%d\t%s\n",i+1,ad->lnptr[j]);
            fprintf(svmfptrial,"\n");
            }
        }
    if(lcrun) {
        if(lclda) {
            if(!(ldafprun=fopen_sub(ldafrun,"w"))) exit(-1);
            fprintf(ldafprun,"classes\n-------\n");
            for(j=class_lnptri,i=0;i<nclass;i++,j++) fprintf(ldafprun,"%d\t%s\n",i+1,ad->lnptr[j]);
            fprintf(ldafprun,"\n");
            }
        if(lclr) {
            if(!(lrfprun=fopen_sub(lrfrun,"w"))) exit(-1);
            fprintf(lrfprun,"classes\n-------\n");
            for(j=class_lnptri,i=0;i<nclass;i++,j++) fprintf(lrfprun,"%d\t%s\n",i+1,ad->lnptr[j]);
            fprintf(lrfprun,"\n");
            }
        if(lcsvm) {
            if(!(svmfprun=fopen_sub(svmfrun,"w"))) exit(-1);
            fprintf(svmfprun,"classes\n-------\n");
            for(j=class_lnptri,i=0;i<nclass;i++,j++) fprintf(svmfprun,"%d\t%s\n",i+1,ad->lnptr[j]);
            fprintf(svmfprun,"\n");
            }
        }
    if(lcsession) {
        if(lclda) {
            if(!(ldafpsession=fopen_sub(ldafsession,"w"))) exit(-1);
            fprintf(ldafpsession,"classes\n-------\n");
            for(j=class_lnptri,i=0;i<nclass;i++,j++) fprintf(ldafpsession,"%d\t%s\n",i+1,ad->lnptr[j]);
            fprintf(ldafpsession,"\n");
            }
        if(lclr) {
            if(!(lrfpsession=fopen_sub(lrfsession,"w"))) exit(-1);
            fprintf(lrfpsession,"classes\n-------\n");
            for(j=class_lnptri,i=0;i<nclass;i++,j++) fprintf(lrfpsession,"%d\t%s\n",i+1,ad->lnptr[j]);
            fprintf(lrfpsession,"\n");
            }
        if(lcsvm) {
            if(!(svmfpsession=fopen_sub(svmfsession,"w"))) exit(-1);
            fprintf(svmfpsession,"classes\n-------\n");
            for(j=class_lnptri,i=0;i<nclass;i++,j++) fprintf(svmfpsession,"%d\t%s\n",i+1,ad->lnptr[j]);
            fprintf(svmfpsession,"\n");
            }
        }
    if(lcsubject) {
        if(lclda) {
            if(!(ldafpsubject=fopen_sub(ldafsubject,"w"))) exit(-1);
            fprintf(ldafpsubject,"classes\n-------\n");
            for(j=class_lnptri,i=0;i<nclass;i++,j++) fprintf(ldafpsubject,"%d\t%s\n",i+1,ad->lnptr[j]);
            fprintf(ldafpsubject,"\n");
            }
        if(lclr) {
            if(!(lrfpsubject=fopen_sub(lrfsubject,"w"))) exit(-1);
            fprintf(lrfpsubject,"classes\n-------\n");
            for(j=class_lnptri,i=0;i<nclass;i++,j++) fprintf(lrfpsubject,"%d\t%s\n",i+1,ad->lnptr[j]);
            fprintf(lrfpsubject,"\n");
            }
        if(lcsvm) {
            if(!(svmfpsubject=fopen_sub(svmfsubject,"w"))) exit(-1);
            fprintf(svmfpsubject,"classes\n-------\n");
            for(j=class_lnptri,i=0;i<nclass;i++,j++) fprintf(svmfpsubject,"%d\t%s\n",i+1,ad->lnptr[j]);
            fprintf(svmfpsubject,"\n");
            }
        }
    }

//printf("atlas=%s lctrial=%d lcrun=%d lccompressed=%d classi=%d nclass=%d nclass0=%d lccompressed=%d num_regions=%d\n",atlas,
//    lctrial,lcrun,lccompressed,classi,nclass,nclass0,lccompressed,num_regions);
//START170209
printf("lctrial=%d lcrun=%d lccompressed=%d classi=%d nclass=%d nclass0=%d lccompressed=%d num_regions=%d\n",
    lctrial,lcrun,lccompressed,classi,nclass,nclass0,lccompressed,num_regions);

for(ii=i=0;i<num_regions;i++) {
//START170210
//for(ii=i=2517;i<2518;i++) {

    if(num_region_files) {
        printf("Processing region %s\n    Checking data\n",rbf->region_names_ptr[i]);
        for(nidx=l=0;l<rbf->nvoxels_region[i];l++,ii++) idx[nidx++] = lccompressed ? ii : rbf->indices[ii];
        }
    else {
        if(!(i%modulo)) printf("Processing voxel %d\n",i);
        zi = ms->brnidx[i]/area;
        col_row = ms->brnidx[i]-zi*area;
        y = col_row/xdim;
        x = col_row - y*xdim;
        for(nidx=ll=l=0;l<ncoor;l++) {
            x1=x+coor[ll++];
            y1=y+coor[ll++];
            zi1=zi+coor[ll++];
            index = zi1*area + y1*xdim + x1;
            if(index>=0&&index<ms->lenvol) {
                /*printf("l=%d ms->maskidx[%d]=%d\n",l,index,ms->maskidx[index]);fflush(stdout);*/
                if(ms->maskidx[index]>=0) idx[nidx++] = lccompressed ? ms->maskidx[index] : index;
                }
            }
        }
    //printf("i=%d nidx=%d idx\n",i,nidx);for(j=0;j<nidx;j++)printf("%d ",idx[j]);printf("\n");
    for(j=0;j<nclass;j++) trialsperclass[j]=0;
    for(j=0;j<nidx;j++) listidx[j]=0;
    if(lcrun) for(j=0;j<nrun;j++) run->list[j]=0;
    if(lcsession) for(j=0;j<nsession;j++) session->list[j]=0;
    if(lcsubject) for(j=0;j<nsubject;j++) subject->list[j]=0;
    for(g->nwhichtrials=nonzero=j=0;j<ad->num_datafiles;j++) {
        g->whichtrialsnonzero[g->nwhichtrials]=nonzero;

        //if(!(mm=map_disk(ad->datafiles[j],dp->vol,0,sizeof(float)))) exit(-1);
        //START170202
        if(!(mm=map_disk(ad->datafiles[j],dp->vol[j],0,sizeof(float)))) exit(-1);

        for(k=0;k<nidx;k++) temp_float2[k] = mm->ptr[idx[k]];
        if(!unmap_disk(mm)) exit(-1);
        if(dp->swapbytes[j]) swap_bytes((unsigned char *)temp_float2,sizeof(float),(size_t)nidx);
        for(g->nonzero1[j]=k=0;k<nidx;k++) {
            td=(double)(tf=temp_float2[k]);
            if(tf==0.) {
                /*printf("Error: tf=%f is zero.\n",tf);*/
                }
            else if(tf==(float)UNSAMPLED_VOXEL) {
                /*printf("Error: tf=%f UNSAMPLED_VOXEL detected.\n",tf);*/
                }
            #ifdef __sun__
                else if(IsNANorINF(td)) {
                    /*printf("Error: tf=%f NANorINF detected.\n",tf);*/
                    }
            #else
                else if(isnan(td)) {
                    /*printf("Error: tf=%f isnan detected.\n",tf);*/
                    }
                else if(isinf(td)) {
                    /*printf("Error: tf=%f isinf detected.\n",tf);*/
                    }
            #endif
            else {
                temp_float[nonzero]=tf;
                g->whichvoxel[nonzero++]=k;
                g->nonzero1[j]++;
                listidx[k]++;
                }
            }
        if(g->nonzero1[j]>1) {
            g->whichclass[g->nwhichtrials]=(size_t)ad->levelall[j][classi];
            if(lcrun) {
                run->which[g->nwhichtrials]=ad->levelall[j][runi];
                run->list[ad->levelall[j][runi]]++;
                }
            if(lcsession) {
                session->which[g->nwhichtrials]=ad->levelall[j][sessioni];
                session->list[ad->levelall[j][sessioni]]++;
                }
            if(lcsubject) {
                subject->which[g->nwhichtrials]=ad->levelall[j][subjecti];
                subject->list[ad->levelall[j][subjecti]]++;
                }
            g->whichtrials[g->nwhichtrials++]=j; 
            trialsperclass[ad->levelall[j][classi]]++;
            }
        else if(g->nonzero1[j]==1)nonzero--;
        }
    if(!nonzero||!g->nwhichtrials) continue;
    nlisttrial=nlistsession=nlistrun=nlistsubject=0;
    if(lcrun){for(j=0;j<nrun;j++)if(run->list[j])run->listi[nlistrun++]=j;}
    if(lcsession){for(j=0;j<nsession;j++)if(session->list[j])session->listi[nlistsession++]=j;}
    if(lcsubject){for(j=0;j<nsubject;j++)if(subject->list[j])subject->listi[nlistsubject++]=j;}
    if(lctrial){
        nlisttrial=g->nwhichtrials;
        for(j=0;j<g->nwhichtrials;j++)trial->listi[j]=0;
        trial->list[0]=1;
        for(j=0;j<g->nwhichtrials;j++) trial->iwhich[j]=j;
        }
    for(g->nilistidx=j=0;j<nidx;j++) g->ilistidx[j] = listidx[j] ? g->nilistidx++ : -1;
    if(lclda) {
        for(j=0;j<nclass;j++)for(k=0;k<g->nilistidx;k++)
            glda->colsumalltrials[j][k]=glda->mean_colsum[j][k]=glda->mean_multicolsum[j][k]=0.;
        for(l=j=0;j<g->nwhichtrials;j++)
            for(k=0;k<g->nonzero1[g->whichtrials[j]];k++,l++)
                glda->colsumalltrials[g->whichclass[j]][g->ilistidx[g->whichvoxel[l]]]+=(double)temp_float[l];
        }
    for(j=0;j<nclass;j++)for(k=0;k<g->nilistidx;k++)g->colsumalltrialsn[j][k]=0;
    for(l=j=0;j<g->nwhichtrials;j++)for(k=0;k<g->nonzero1[g->whichtrials[j]];k++,l++)
        g->colsumalltrialsn[g->whichclass[j]][g->ilistidx[g->whichvoxel[l]]]++; 
    if(lctrial) {
        if(lclda) {
            correct=ldaguts(g,glda,trial,wldatrial,nclass,nclass0,0,nlisttrial,(size_t*)NULL,temp_float,temp_double,i,lambdalda);
            pcldatrial->trial[i]=(double)correct/(double)(g->nwhichtrials)*100.;
            if(num_region_files) {
                for(j=0;j<nclass;j++) pcldatrial->class[i][j]=(double)g->correctbyclass[j]/(double)trialsperclass[j]*100.;
                printf("    lda trial correct=%d percent correct=%.2f\n",correct,pcldatrial->trial[i]);fflush(stdout);
                printfp(ldafptrial,nclass,rbf->region_names_ptr[i],g);
                }
            }
        if(lclr) {
            correct=lrguts(g,glr,trial,wlrtrial,nclass,nclass0,0,nlisttrial,(size_t*)NULL,temp_float,temp_double,i,ncoor,lambdalr);
            pclrtrial->trial[i]=(double)correct/(double)(g->nwhichtrials)*100.;
            if(num_region_files) {
                for(j=0;j<nclass;j++) pclrtrial->class[i][j]=(double)g->correctbyclass[j]/(double)trialsperclass[j]*100.;
                printf("    lr trial  correct=%d percent correct=%.2f\n",correct,pclrtrial->trial[i]);fflush(stdout);
                printfp(lrfptrial,nclass,rbf->region_names_ptr[i],g);
                }
            }
        if(lcsvm) {
            correct=svmguts(g,gsvm,svmtrainv,svmbinarysvcprobabilityv,svmsvrprobabilityv,svmpredictv,trial,nclass,
                0,nlisttrial,(size_t*)NULL,temp_float,i);
            pcsvmtrial->trial[i]=(double)correct/(double)(g->nwhichtrials)*100.;
            if(num_region_files) {
                for(j=0;j<nclass;j++) pcsvmtrial->class[i][j]=(double)g->correctbyclass[j]/(double)trialsperclass[j]*100.;
                printf("    svm trial  correct=%d percent correct=%.2f\n",correct,pcsvmtrial->trial[i]);fflush(stdout);
                printfp(svmfptrial,nclass,rbf->region_names_ptr[i],g);
                }
            }
        }
    if(nlistrun>1) {
        if(lclda) {
            correct=ldaguts(g,glda,run,wldarun,nclass,nclass0,nrun,nlistrun,correctby_,temp_float,temp_double,i,lambdalda);
            pcldarun->trial[i]=(double)correct/(double)(g->nwhichtrials)*100.;
            if(num_region_files) {
                for(j=0;j<nlistrun;j++) 
                    pcldarun->pc[i][run->listi[j]]=(double)correctby_[run->listi[j]]/(double)(run->list[run->listi[j]])*100.;
                for(j=0;j<nclass;j++) pcldarun->class[i][j]=(double)g->correctbyclass[j]/(double)trialsperclass[j]*100.;
                printf("    lda run   correct=%d percent correct=%.2f\n",correct,pcldarun->trial[i]);fflush(stdout);
                printfp(ldafprun,nclass,rbf->region_names_ptr[i],g);
                }
            }
        if(lclr) {
            correct=lrguts(g,glr,run,wlrrun,nclass,nclass0,nrun,nlistrun,correctby_,temp_float,temp_double,i,ncoor,lambdalr);
            pclrrun->trial[i]=(double)correct/(double)(g->nwhichtrials)*100.;
            if(num_region_files) {
                for(j=0;j<nlistrun;j++) 
                    pclrrun->pc[i][run->listi[j]]=(double)correctby_[run->listi[j]]/(double)(run->list[run->listi[j]])*100.;
                for(j=0;j<nclass;j++) pclrrun->class[i][j]=(double)g->correctbyclass[j]/(double)trialsperclass[j]*100.;
                printf("    lr run    correct=%d percent correct=%.2f\n",correct,pclrrun->trial[i]);fflush(stdout); 
                printfp(lrfprun,nclass,rbf->region_names_ptr[i],g);
                }
            }
        if(lcsvm) {
            correct=svmguts(g,gsvm,svmtrainv,svmbinarysvcprobabilityv,svmsvrprobabilityv,svmpredictv,run,nclass,
                0,nlistrun,(size_t*)NULL,temp_float,i);
            pcsvmrun->trial[i]=(double)correct/(double)(g->nwhichtrials)*100.;
            if(num_region_files) {
                for(j=0;j<nlistrun;j++)
                    pcsvmrun->pc[i][run->listi[j]]=(double)correctby_[run->listi[j]]/(double)(run->list[run->listi[j]])*100.;
                for(j=0;j<nclass;j++) pcsvmrun->class[i][j]=(double)g->correctbyclass[j]/(double)trialsperclass[j]*100.;
                printf("    svm run   correct=%d percent correct=%.2f\n",correct,pcsvmrun->trial[i]);fflush(stdout);
                printfp(svmfprun,nclass,rbf->region_names_ptr[i],g);
                }
            }
        }
    if(nlistsession>1) {
        if(lclda) {
            correct=ldaguts(g,glda,session,wldasession,nclass,nclass0,nsession,nlistsession,correctby_,temp_float,temp_double,i,
                lambdalda);
            pcldasession->trial[i]=(double)correct/(double)(g->nwhichtrials)*100.;
            if(num_region_files) {
                for(j=0;j<nlistsession;j++) pcldasession->pc[i][session->listi[j]]=
                    (double)correctby_[session->listi[j]]/(double)(session->list[session->listi[j]])*100.;
                for(j=0;j<nclass;j++) pcldasession->class[i][j]=(double)g->correctbyclass[j]/(double)trialsperclass[j]*100.;
                printf("    lda session    correct=%d percent correct=%.2f\n",correct,pcldasession->trial[i]);fflush(stdout);
                printfp(ldafpsession,nclass,rbf->region_names_ptr[i],g);
                }
            }
        if(lclr) {
            correct=lrguts(g,glr,session,wlrsession,nclass,nclass0,nsession,nlistsession,correctby_,temp_float,temp_double,i,ncoor,
                lambdalr);
            pclrsession->trial[i]=(double)correct/(double)(g->nwhichtrials)*100.;
            if(num_region_files) {
                for(j=0;j<nlistsession;j++) pclrsession->pc[i][session->listi[j]]=
                    (double)correctby_[session->listi[j]]/(double)(session->list[session->listi[j]])*100.;
                for(j=0;j<nclass;j++) pclrsession->class[i][j]=(double)g->correctbyclass[j]/(double)trialsperclass[j]*100.;
                printf("    lr session    correct=%d percent correct=%.2f\n",correct,pclrsession->trial[i]);fflush(stdout);
                printfp(lrfpsession,nclass,rbf->region_names_ptr[i],g);
                }
            }
        if(lcsvm) {
            correct=svmguts(g,gsvm,svmtrainv,svmbinarysvcprobabilityv,svmsvrprobabilityv,svmpredictv,session,nclass,
                0,nlistsession,(size_t*)NULL,temp_float,i);
            pcsvmsession->trial[i]=(double)correct/(double)(g->nwhichtrials)*100.;
            if(num_region_files) {
                for(j=0;j<nlistsession;j++) pcsvmsession->pc[i][session->listi[j]]=
                    (double)correctby_[session->listi[j]]/(double)(session->list[session->listi[j]])*100.;
                for(j=0;j<nclass;j++) pcsvmsession->class[i][j]=(double)g->correctbyclass[j]/(double)trialsperclass[j]*100.;
                printf("    svm session   correct=%d percent correct=%.2f\n",correct,pcsvmsession->trial[i]);fflush(stdout);
                printfp(svmfpsession,nclass,rbf->region_names_ptr[i],g);
                }
            }
        }
    if(nlistsubject>1) {
        if(lclda) {
            correct=ldaguts(g,glda,subject,wldasubject,nclass,nclass0,nsubject,nlistsubject,correctby_,temp_float,temp_double,i,
                lambdalda);
            pcldasubject->trial[i]=(double)correct/(double)(g->nwhichtrials)*100.;
            if(num_region_files) {
                for(j=0;j<nlistsubject;j++) pcldasubject->pc[i][subject->listi[j]]=
                    (double)correctby_[subject->listi[j]]/(double)(subject->list[subject->listi[j]])*100.;
                for(j=0;j<nclass;j++) pcldasubject->class[i][j]=(double)g->correctbyclass[j]/(double)trialsperclass[j]*100.;
                printf("    lda subject    correct=%d percent correct=%.2f\n",correct,pcldasubject->trial[i]);fflush(stdout);
                printfp(ldafpsubject,nclass,rbf->region_names_ptr[i],g);
                }
            }
        if(lclr) {
            correct=lrguts(g,glr,subject,wlrsubject,nclass,nclass0,nsubject,nlistsubject,correctby_,temp_float,temp_double,i,ncoor,
                lambdalr);
            pclrsubject->trial[i]=(double)correct/(double)(g->nwhichtrials)*100.;
            if(num_region_files) {
                for(j=0;j<nlistsubject;j++) pclrsubject->pc[i][subject->listi[j]]=
                    (double)correctby_[subject->listi[j]]/(double)(subject->list[subject->listi[j]])*100.;
                for(j=0;j<nclass;j++) pclrsubject->class[i][j]=(double)g->correctbyclass[j]/(double)trialsperclass[j]*100.;
                printf("    lr subject    correct=%d percent correct=%.2f\n",correct,pclrsubject->trial[i]);fflush(stdout);
                printfp(lrfpsubject,nclass,rbf->region_names_ptr[i],g);
                }
            }
        if(lcsvm) {
            correct=svmguts(g,gsvm,svmtrainv,svmbinarysvcprobabilityv,svmsvrprobabilityv,svmpredictv,subject,nclass,
                0,nlistsubject,(size_t*)NULL,temp_float,i);
            pcsvmsubject->trial[i]=(double)correct/(double)(g->nwhichtrials)*100.;
            if(num_region_files) {
                for(j=0;j<nlistsubject;j++) pcsvmsubject->pc[i][subject->listi[j]]=
                    (double)correctby_[subject->listi[j]]/(double)(subject->list[subject->listi[j]])*100.;
                for(j=0;j<nclass;j++) pcsvmsubject->class[i][j]=(double)g->correctbyclass[j]/(double)trialsperclass[j]*100.;
                printf("    svm subject   correct=%d percent correct=%.2f\n",correct,pcsvmsubject->trial[i]);fflush(stdout);
                printfp(svmfpsubject,nclass,rbf->region_names_ptr[i],g);
                }
            }
        }
    fflush(stdout);
    }
if(!num_region_files) {
    if(temp_float) free(temp_float);
    if(!(temp_float=malloc(sizeof*temp_float*ms->lenvol))) {
        printf("Error: Unable to malloc temp_float\n");
        exit(-1);
        }

    #if 0
    if(!(ifh=init_ifh(4,ap->xdim,ap->ydim,ap->zdim,1,ap->voxel_size[0],ap->voxel_size[1],ap->voxel_size[2],SunOS_Linux?0:1)))exit(-1);
    ifh->mmppix[0] = ap->mmppix[0];
    ifh->mmppix[1] = ap->mmppix[1];
    ifh->mmppix[2] = ap->mmppix[2];
    ifh->center[0] = ap->center[0];
    ifh->center[1] = ap->center[1];
    ifh->center[2] = ap->center[2];
    if(!atlas) {ifh->dim1=xdim;ifh->dim2=ydim;ifh->dim3=zdim;}
    #endif
    //START170209
    if(!(ifh=read_ifh(maskfile?maskfile:ad->datafiles[0],NULL)))exit(-1);
    ifh->bigendian=SunOS_Linux?0:1;
    
    }
if(lctrial) {
    if(lclda) for(i=0;i<num_regions;i++)for(j=0;j<nclass0;j++)
        for(k=0;k<ncoor;k++)if(wldatrial->wtsn[j][k][i])wldatrial->wts[j][k][i]/=(double)wldatrial->wtsn[j][k][i];
    if(lclr) for(i=0;i<num_regions;i++)for(j=0;j<nclass0;j++)
        for(k=0;k<ncoor;k++)if(wlrtrial->wtsn[j][k][i])wlrtrial->wts[j][k][i]/=(double)wlrtrial->wtsn[j][k][i];
    if(num_region_files) {
        if(lclda) {
            fflush(ldafptrial);fclose(ldafptrial);
            printf("Output written to %s\n",ldaftrial);
            if(!printreg(ldaftrial,"trial",rbf,pcldatrial,class_lnptri,nclass,ad->lnptr,xx,yy,zz,ap,coor1,nclass0,wldatrial->wts,0))
                printf("fidlError: wallace00 printing region results in printreg\n");
            }
        if(lclr) {
            fflush(lrfptrial);fclose(lrfptrial);
            printf("Output written to %s\n",lrftrial);
            if(!printreg(lrftrial,"trial",rbf,pclrtrial,class_lnptri,nclass,ad->lnptr,xx,yy,zz,ap,coor1,nclass0,wlrtrial->wts,0))
                printf("fidlError: wallace01 printing region results in printreg\n");
            }
        if(lcsvm) {
            fflush(svmfptrial);fclose(svmfptrial);
            printf("Output written to %s\n",svmftrial);
            if(!printreg(svmftrial,"trial",rbf,pcsvmtrial,class_lnptri,nclass,ad->lnptr,xx,yy,zz,ap,coor1,nclass0,(double***)NULL,0))
                printf("fidlError: wallace02 printing region results in printreg\n");
            }
        }
    else {
        if(lclda) if(!printimg(ms,temp_float,pcldatrial->trial,ldaftrial,ifh,class_lnptri,ncoor,ad->lnptr,wldatrial->wts,nclass0))
            printf("fidlError: wallace03 printing region results in printimg\n");
        if(lclr) if(!printimg(ms,temp_float,pclrtrial->trial,lrftrial,ifh,class_lnptri,ncoor,ad->lnptr,wlrtrial->wts,nclass0))
            printf("fidlError: wallace04 printing region results in printimg\n");
        if(lcsvm) if(!printimg(ms,temp_float,pcsvmtrial->trial,svmftrial,ifh,class_lnptri,ncoor,ad->lnptr,(double***)NULL,nclass0))
            printf("fidlError: wallace05 printing region results in printimg\n");
        }
    }
if(lcrun) {
    if(lclda) for(i=0;i<num_regions;i++)for(j=0;j<nclass0;j++)
        for(k=0;k<ncoor;k++)if(wldarun->wtsn[j][k][i])wldarun->wts[j][k][i]/=(double)wldarun->wtsn[j][k][i];
    if(lclr) for(i=0;i<num_regions;i++)for(j=0;j<nclass0;j++)
        for(k=0;k<ncoor;k++)if(wlrrun->wtsn[j][k][i])wlrrun->wts[j][k][i]/=(double)wlrrun->wtsn[j][k][i];
    if(num_region_files) {
        if(lclda) {
            fflush(ldafprun);fclose(ldafprun);
            printf("Output written to %s\n",ldafrun);
            if(!printreg(ldafrun,"run",rbf,pcldarun,class_lnptri,nclass,ad->lnptr,xx,yy,zz,ap,coor1,nclass0,wldarun->wts,nrun))
                printf("fidlError: wallace07 printing region results in printreg");
            }
        if(lclr) {
            fflush(lrfprun);fclose(lrfprun);
            printf("Output written to %s\n",lrfrun);
            if(!printreg(lrfrun,"run",rbf,pclrrun,class_lnptri,nclass,ad->lnptr,xx,yy,zz,ap,coor1,nclass0,wlrrun->wts,nrun))
                printf("fidlError: wallace08 printing region results in printreg");
            }

        if(lcsvm) {
            fflush(svmfprun);fclose(svmfprun);
            printf("Output written to %s\n",svmfrun);
            if(!printreg(svmfrun,"run",rbf,pcsvmrun,class_lnptri,nclass,ad->lnptr,xx,yy,zz,ap,coor1,nclass0,(double***)NULL,nrun))
                printf("fidlError: wallace09 printing region results in printreg");
            }
        }
    else {
        if(lclda) if(!printimg(ms,temp_float,pcldarun->trial,ldafrun,ifh,class_lnptri,ncoor,ad->lnptr,wldarun->wts,nclass0))
            printf("fidlError: wallace10 printing region results in printimg");
        if(lclr) if(!printimg(ms,temp_float,pclrrun->trial,lrfrun,ifh,class_lnptri,ncoor,ad->lnptr,wlrrun->wts,nclass0))
            printf("fidlError: wallace11 printing region results in printimg");
        if(lcsvm) if(!printimg(ms,temp_float,pcsvmrun->trial,svmfrun,ifh,class_lnptri,ncoor,ad->lnptr,(double***)NULL,nclass0))
            printf("fidlError: wallace12 printing region results in printimg");
        }
    }
if(lcsession) {
    if(num_region_files) {
        if(lclda) {
            fflush(ldafpsession);fclose(ldafpsession);
            printf("Output written to %s\n",ldafsession);
            if(!printreg(ldafsession,"session",rbf,pcldasession,class_lnptri,nclass,ad->lnptr,xx,yy,zz,ap,coor1,nclass0,
                wldasession->wts,nsession)) printf("fidlError: wallace13 printing region results in printreg\n");
            }
        if(lclr) {
            fflush(lrfpsession);fclose(lrfpsession);
            printf("Output written to %s\n",lrfsession);
            if(!printreg(lrfsession,"session",rbf,pclrsession,class_lnptri,nclass,ad->lnptr,xx,yy,zz,ap,coor1,nclass0,
                wlrsession->wts,nsession))printf("fidlError: wallace14 printing region results in printreg\n");
            }
        if(lcsvm) {
            fflush(svmfpsession);fclose(svmfpsession);
            printf("Output written to %s\n",svmfsession);
            if(!printreg(svmfsession,"session",rbf,pclrsession,class_lnptri,nclass,ad->lnptr,xx,yy,zz,ap,coor1,nclass0,
                (double***)NULL,nsession))printf("fidlError: wallace15 printing region results in printreg\n");
            }
        }
    else {
        if(lclda) if(!printimg(ms,temp_float,pcldasession->trial,ldafsession,ifh,class_lnptri,ncoor,ad->lnptr,wldasession->wts,
            nclass0)) printf("fidlError: wallace16 printing region results in printimg\n");
        if(lclr) if(!printimg(ms,temp_float,pclrsession->trial,lrfsession,ifh,class_lnptri,ncoor,ad->lnptr,wlrsession->wts,nclass0))
            printf("fidlError: wallace17 printing region results in printimg\n");
        if(lcsvm) if(!printimg(ms,temp_float,pcsvmsession->trial,svmfsession,ifh,class_lnptri,ncoor,ad->lnptr,(double***)NULL,
            nclass0)) printf("fidlError: wallace18 printing region results in printimg\n");
        }
    }
if(lcsubject) {
    if(num_region_files) {
        if(lclda) {
            fflush(ldafpsubject);fclose(ldafpsubject);
            printf("Output written to %s\n",ldafsubject);
            if(!printreg(ldafsubject,"subject",rbf,pcldasubject,class_lnptri,nclass,ad->lnptr,xx,yy,zz,ap,coor1,nclass0,
                wldasubject->wts,nsubject)) printf("fidlError: wallace19 printing region results in printreg\n");
            }
        if(lclr) {
            fflush(lrfpsubject);fclose(lrfpsubject);
            printf("Output written to %s\n",lrfsubject);
            if(!printreg(lrfsubject,"subject",rbf,pclrsubject,class_lnptri,nclass,ad->lnptr,xx,yy,zz,ap,coor1,nclass0,
                wlrsubject->wts,nsubject))printf("fidlError: wallace20 printing region results in printreg\n");
            }
        if(lclr) {
            fflush(svmfpsubject);fclose(svmfpsubject);
            printf("Output written to %s\n",svmfsubject);
            if(!printreg(svmfsubject,"subject",rbf,pcsvmsubject,class_lnptri,nclass,ad->lnptr,xx,yy,zz,ap,coor1,nclass0,
                (double***)NULL,nsubject))printf("fidlError: wallace21 printing region results in printreg\n");
            }
        }
    else {
        if(lclda) if(!printimg(ms,temp_float,pcldasubject->trial,ldafsubject,ifh,class_lnptri,ncoor,ad->lnptr,wldasubject->wts,
            nclass0)) printf("fidlError: wallace22 printing region results in printimg\n");
        if(lclr) if(!printimg(ms,temp_float,pclrsubject->trial,lrfsubject,ifh,class_lnptri,ncoor,ad->lnptr,wlrsubject->wts,nclass0))
            printf("fidlError: wallace23 printing region results in printimg\n");

        #if 0
        if(lcsvm) if(!printimg(ms,temp_float,pcsvmsubject->trial,svmfsubject,ifh,class_lnptri,ncoor,ad->lnptr,wsvmsubject->wts,
            nclass0)) printf("fidlError: wallace24 printing region results in printimg\n");
        #endif
        //START170207
        if(lcsvm)if(!printimg(ms,temp_float,pcsvmsubject->trial,svmfsubject,ifh,class_lnptri,ncoor,ad->lnptr,(double***)NULL,
            nclass0)) printf("fidlError: wallace24 printing region results in printimg\n");

        }
    }
if(cleanup) {
    sprintf(string,"rm -rf %s",cleanup);
    if(system(string) == -1) printf("fidlError: unable to %s\n",string);
    }
fflush(stdout);exit(0);
}

//Gutsv *allocGutsv(int num_datafiles,size_t nclass,int ncoor) {
//START170203
Gutsv *allocGutsv(int num_datafiles,int nclass,int ncoor) {

    Gutsv *g;
    if(!(g=malloc(sizeof*g))) {
        printf("fidlError: Unable to malloc g\n");
        return NULL;
        }
    if(!(g->v=d2double(num_datafiles,nclass))) {
        printf("fidlError: Unable to malloc g->v\n");
        return NULL;
        }
    if(!(g->s=malloc(sizeof*g->s*(ncoor+1)))) {
        printf("Error: Unable to malloc g->s\n");
        return NULL;
        }
    if(!(g->s[0]=malloc(sizeof*g->s[0]*(ncoor+1)*(ncoor+1)))) {
        printf("Error: Unable to malloc g->s[0]\n");
        return NULL;
        }
    if(!(g->sinv=malloc(sizeof*g->sinv*(ncoor+1)))) {
        printf("Error: Unable to malloc g->sinv\n");
        return NULL;
        }
    if(!(g->sinv[0]=malloc(sizeof*g->sinv[0]*(ncoor+1)*(ncoor+1)))) {
        printf("Error: Unable to malloc g->sinv[0]\n");
        return NULL;
        }

    /*START130905*/
    if(!(g->V=malloc(sizeof*g->V*(ncoor+1)*(ncoor+1)))) {
        printf("Error: Unable to malloc g->V\n");
        return 0;
        }
    if(!(g->S=malloc(sizeof*g->S*(ncoor+1)))) {
        printf("Error: Unable to malloc g->S\n");
        return 0;
        }
    if(!(g->work=malloc(sizeof*g->work*(ncoor+1)))) {
        printf("Error: Unable to malloc g->work\n");
        return 0;
        }

    if(!(g->vec=malloc(sizeof*g->vec*(ncoor+1)))) {
        printf("Error: Unable to malloc g->vec\n");
        return NULL;
        }
    if(!(g->tdvm=malloc(sizeof*g->tdvm*(ncoor+1)))) {
        printf("Error: Unable to malloc g->tdvm\n");
        return NULL;
        }
    if(!(g->wts=malloc(sizeof*g->wts*(ncoor+1)))) {
        printf("fidlError: Unable to malloc g->wts\n");
        return NULL;
        }
    if(!(g->colsumn=d2int(nclass,ncoor))) {
        printf("fidlError: Unable to malloc g->colsumn\n");
        return NULL;
        }
    if(!(g->colsumalltrialsn=d2int(nclass,ncoor))) {
        printf("fidlError: Unable to malloc g->colsumalltrialsn\n");
        return NULL;
        }
    if(!(g->sumtdim=d2int(nclass,ncoor))) {
        printf("fidlError: Unable to malloc g->sumtdim\n");
        return NULL;
        }
    if(!(g->ilistidx=malloc(sizeof*g->ilistidx*ncoor))) {
        printf("Error: Unable to malloc g->ilistidx\n");
        return NULL;
        }
    if(!(g->havei=malloc(sizeof*g->havei*ncoor))) {
        printf("Error: Unable to malloc g->havei\n");
        return NULL;
        }
    if(!(g->whichvoxel=malloc(sizeof*g->whichvoxel*num_datafiles*ncoor))) {
        printf("fidlError: Unable to malloc g->whichvoxel\n");
        return NULL;
        }
    if(!(g->whichtrials=malloc(sizeof*g->whichtrials*num_datafiles))) {
        printf("fidlError: Unable to malloc g->whichtrials\n");
        return NULL;
        }
    if(!(g->whichtrialsnonzero=malloc(sizeof*g->whichtrialsnonzero*num_datafiles))) {
        printf("fidlError: Unable to malloc g->whichtrialsnonzero\n");
        return NULL;
        }
    if(!(g->nonzero1=malloc(sizeof*g->nonzero1*num_datafiles))) {
        printf("fidlError: Unable to malloc g->nonzero1\n");
        return NULL;
        }
    if(!(g->correctbyclass=malloc(sizeof*g->correctbyclass*nclass))) {
        printf("Error: Unable to malloc g->correctbyclass\n");
        return NULL;
        }
    if(!(g->whichclass=malloc(sizeof*g->whichclass*num_datafiles))) {
        printf("fidlError: Unable to malloc g->whichclass\n");
        return NULL;
        }
    if(!(g->imaxv=malloc(sizeof*g->imaxv*num_datafiles))) {
        printf("Error: Unable to malloc g->imaxv\n");
        return NULL;
        }
    if(!(g->have=malloc(sizeof*g->have*ncoor))) {
        printf("Error: Unable to malloc g->have\n");
        return NULL;
        }
    if(!(g->ihave=malloc(sizeof*g->ihave*ncoor))) {
        printf("Error: Unable to malloc g->ihave\n");
        return NULL;
        }
    return g;
    }

//Ldav *allocLdav(size_t nclass,int ncoor) {
//START170203
Ldav *allocLdav(int nclass,int ncoor){

    Ldav *glda;
    if(!(glda=malloc(sizeof*glda))) {
        printf("fidlError: Unable to malloc glda\n");
        return NULL;
        }
    if(!(glda->colsum=d2double(nclass,ncoor))) {
        printf("fidlError: Unable to malloc glda->colsum\n");
        return NULL;
        }
    if(!(glda->colsumalltrials=d2double(nclass,ncoor))) {
        printf("fidlError: Unable to malloc glda->colsumalltrials\n");
        return NULL;
        }
    if(!(glda->mean_colsum=d2double(nclass,ncoor))) {
        printf("fidlError: Unable to malloc glda->mean_colsum\n");
        return NULL;
        }
    if(!(glda->mean_multicolsum=d2double(nclass,ncoor))) {
        printf("fidlError: Unable to malloc glda->mean_multicolsum\n");
        return NULL;
        }
    if(!(glda->halfmean1plusmean2=malloc(sizeof*glda->halfmean1plusmean2*ncoor))) {
        printf("Error: Unable to malloc glda->halfmean1plusmean2\n");
        return NULL;
        }
    if(!(glda->mean1minusmean2=malloc(sizeof*glda->mean1minusmean2*ncoor))) {
        printf("Error: Unable to malloc glda->mean1minusmean2\n");
        return NULL;
        }
    return glda;
    }

//Lrv *allocLrv(int num_datafiles,size_t nclass0,int ncoor) {
//START170203
Lrv *allocLrv(int num_datafiles,int ncoor) {

    Lrv *glr;
    if(!(glr=malloc(sizeof*glr))) {
        printf("fidlError: Unable to malloc glr\n");
        return NULL;
        }
    if(!(glr->X=d2double(num_datafiles,ncoor+1))) {
        printf("fidlError: Unable to malloc glr->X\n");
        return NULL;
        }
    if(!(glr->Az=malloc(sizeof*glr->Az*num_datafiles))) {
        printf("fidlError: Unable to malloc glr->Az\n");
        return NULL;
        }
    if(!(glr->y=malloc(sizeof*glr->y*num_datafiles))) {
        printf("fidlError: Unable to malloc glr->y\n");
        return NULL;
        }
    if(!(glr->Iwv=malloc(sizeof*glr->Iwv*num_datafiles*ncoor))) {
        printf("fidlError: Unable to malloc glr->Iwv\n");
        return NULL;
        }
    if(!(glr->N=malloc(sizeof*glr->N*num_datafiles))) {
        printf("fidlError: Unable to malloc glr->N\n");
        return NULL;
        }
    if(!(glr->wnew=malloc(sizeof*glr->wnew*(ncoor+1)))) {
        printf("fidlError: Unable to malloc glr->wnew\n");
        exit(-1);
        }
    return glr;
    }
struct svm_problem *allocsubprob(int num_datafiles) {
    struct svm_problem *subprob;
    if(!(subprob=malloc(sizeof*subprob))) {
        printf("fidlError: Unable to malloc subprob\n");
        return NULL;
        }
    if(!(subprob->y=malloc(sizeof*subprob->y*num_datafiles))) {
        printf("fidlError: Unable to malloc subprob->y\n");
        return NULL;
        }
    if(!(subprob->x=malloc(sizeof*subprob->x*num_datafiles))) {
        printf("fidlError: Unable to malloc subprob->x\n");
        return NULL;
        }

    /*START131220*/
    #if 1
    if(!(subprob->n=malloc(sizeof*subprob->n*num_datafiles))) {
        printf("fidlError: Unable to malloc subprob->n\n");
        return NULL;
        }
    #endif

    return subprob;
    }

//Svmv *allocSvmv(int num_datafiles,size_t nclass0,int ncoor) {
//START170203
Svmv *allocSvmv(int num_datafiles,int ncoor) {

    Svmv *gsvm;
    if(!(gsvm=malloc(sizeof*gsvm))) {
        printf("fidlError: Unable to malloc gsvm\n");
        return NULL;
        }
    if(!(gsvm->prob=allocsubprob(num_datafiles))) {
        printf("fidlError: Unable to malloc gsvm->prob\n");
        return NULL;
        }
    if(!(gsvm->x_space=malloc(sizeof*gsvm->x_space*num_datafiles*(ncoor+1)))) {
        printf("fidlError: Unable to malloc gsvm->x_space\n");
        exit(-1);
        }
    if(!(gsvm->feature_max=malloc(sizeof*gsvm->feature_max*ncoor))) {
        printf("fidlError: Unable to malloc gsvm->feature_max\n");
        exit(-1);
        }
    if(!(gsvm->feature_min=malloc(sizeof*gsvm->feature_min*ncoor))) {
        printf("fidlError: Unable to malloc gsvm->feature_min\n");
        exit(-1);
        }
    if(!(gsvm->feature_dif=malloc(sizeof*gsvm->feature_dif*ncoor))) {
        printf("fidlError: Unable to malloc gsvm->feature_dif\n");
        exit(-1);
        }
    if(!(gsvm->N=malloc(sizeof*gsvm->N*num_datafiles))) {
        printf("fidlError: Unable to malloc gsvm->N\n");
        return NULL;
        }
    if(!(gsvm->x=malloc(sizeof*gsvm->x*ncoor))) {
        printf("fidlError: Unable to malloc gsvm->x\n");
        exit(-1);
        }
    return gsvm;
    }
void initSvmv(double svmscalel,double svmscaleu,double svmtrainc,Svmv *gsvm) {
    gsvm->lower = svmscalel;
    gsvm->upper = svmscaleu;
    gsvm->dif = svmscaleu-svmscalel;
    gsvm->param.svm_type = C_SVC;
    gsvm->param.kernel_type = LINEAR;
    gsvm->param.degree = 3;
    gsvm->param.gamma = 0;
    gsvm->param.coef0 = 0;
    gsvm->param.nu = 0.5;
    gsvm->param.cache_size = 100;
    gsvm->param.C = svmtrainc;
    gsvm->param.eps = 1e-3;
    gsvm->param.p = 0.1;
    gsvm->param.shrinking = 1;
    gsvm->param.probability = 0;
    gsvm->param.nr_weight = 0;
    gsvm->param.weight_label = NULL;
    gsvm->param.weight = NULL;
    gsvm->cross_validation = 0;


#if 0
-d degree : set degree in kernel function (default 3)
-g gamma : set gamma in kernel function (default 1/num_features)
-r coef0 : set coef0 in kernel function (default 0)
-c cost : set the parameter C of C-SVC, epsilon-SVR, and nu-SVR (default 1)
-n nu : set the parameter nu of nu-SVC, one-class SVM, and nu-SVR (default 0.5)
-p epsilon : set the epsilon in loss function of epsilon-SVR (default 0.1)
-m cachesize : set cache memory size in MB (default 100)
-e epsilon : set tolerance of termination criterion (default 0.001)
-h shrinking : whether to use the shrinking heuristics, 0 or 1 (default 1)
-b probability_estimates : whether to train a SVC or SVR model for probability estimates, 0 or 1 (default 0)
-wi weight : set the parameter C of class i to weight*C, for C-SVC (default 1)
-v n: n-fold cross validation mode
-q : quiet mode (no outputs)
#endif

    }

//Svmgroupclassesv *allocSvmgroupclassesv(size_t nclass,int num_datafiles) {
//START170203
Svmgroupclassesv *allocSvmgroupclassesv(int nclass,int num_datafiles) {

    Svmgroupclassesv *svmgroupclassesv;
    if(!(svmgroupclassesv=malloc(sizeof*svmgroupclassesv))) {
        printf("fidlError: Unable to malloc svmgroupclassesv\n");
        return NULL;
        }
    if(!(svmgroupclassesv->label=malloc(sizeof*svmgroupclassesv->label*nclass))) {
        printf("fidlError: Unable to malloc svmgroupclassesv->label\n");
        return NULL;
        }
    if(!(svmgroupclassesv->count=malloc(sizeof*svmgroupclassesv->count*nclass))) {
        printf("fidlError: Unable to malloc svmgroupclassesv->count\n");
        return NULL;
        }
    if(!(svmgroupclassesv->data_label=malloc(sizeof*svmgroupclassesv->data_label*num_datafiles))) {
        printf("fidlError: Unable to malloc svmgroupclassesv->data_label\n");
        return NULL;
        }
    if(!(svmgroupclassesv->start=malloc(sizeof*svmgroupclassesv->start*nclass))) {
        printf("fidlError: Unable to malloc svmgroupclassesv->start\n");
        return NULL;
        }
    svmgroupclassesv->nclass=nclass;
    return svmgroupclassesv;
    }

//struct svm_model *allocModel(size_t nclass,int num_datafiles,struct svm_parameter *param) {
//START170203
struct svm_model *allocModel(int nclass,int num_datafiles,struct svm_parameter *param) {

    struct svm_model *model;
    if(!(model=malloc(sizeof*model))) {
        printf("fidlError: Unable to malloc model\n");
        return NULL;
        }
    if(!(model->label=malloc(sizeof*model->label*nclass))) {
        printf("fidlError: Unable to malloc model->label\n");
        return NULL;
        }
    if(!(model->rho=malloc(sizeof*model->rho*nclass*(nclass-1)/2))) {
        printf("fidlError: Unable to malloc model->rho\n");
        return NULL;
        }
    if(!param->probability) {
        model->probA=model->probB=NULL;
        }
    else {
        if(!(model->probA=malloc(sizeof*model->probA*nclass*(nclass-1)/2))) {
            printf("fidlError: Unable to malloc model->probA\n");
            return NULL;
            }
        if(!(model->probB=malloc(sizeof*model->probB*nclass*(nclass-1)/2))) {
            printf("fidlError: Unable to malloc model->probB\n");
            return NULL;
            }
        }
    if(param->svm_type == ONE_CLASS || param->svm_type == EPSILON_SVR || param->svm_type == NU_SVR) {
        if(!(model->sv_coef=malloc(sizeof*model->sv_coef))) {
            printf("fidlError: Unable to malloc model->sv_coef\n");
            return NULL;
            }
        if(!(model->rho=malloc(sizeof*model->rho))) {
            printf("fidlError: Unable to malloc model->rho\n");
            return NULL;
            }
        }
    if(!(model->nSV=malloc(sizeof*model->nSV*nclass))) {
        printf("fidlError: Unable to malloc model->nSV\n");
        return NULL;
        }
    if(!(model->SV=malloc(sizeof*model->SV*num_datafiles))) {
        printf("fidlError: Unable to malloc model->SV\n");
        return NULL;
        }
    if(!(model->sv_indices=malloc(sizeof*model->sv_indices*num_datafiles))) {
        printf("fidlError: Unable to malloc model->sv_indices\n");
        return NULL;
        }
    if(!(model->sv_coef=malloc(sizeof*model->sv_coef*(nclass-1)))) {
        printf("fidlError: Unable to malloc model->sv_coef\n");
        return NULL;
        }
    if(!(model->sv_coef[0]=malloc(sizeof*model->sv_coef[0]*(nclass-1)*num_datafiles))) {
        printf("fidlError: Unable to malloc model->sv_coef[0]\n");
        return NULL;
        }
    return model;
    }

//Svmcrossvalidationv *allocSvmcrossvalidationv(size_t nclass,int num_datafiles,int ncoor,Svmv *gsvm) {
//START170203
Svmcrossvalidationv *allocSvmcrossvalidationv(int nclass,int num_datafiles,Svmv *gsvm) {

    Svmcrossvalidationv *svmcrossvalidationv;
    if(!(svmcrossvalidationv=malloc(sizeof*svmcrossvalidationv))) {
        printf("fidlError: Unable to malloc svmcrossvalidationv\n");
        return NULL;
        }
    if(!(svmcrossvalidationv->perm=malloc(sizeof*svmcrossvalidationv->perm*num_datafiles))) {
        printf("fidlError: Unable to malloc svmcrossvalidationv->perm\n");
        return NULL;
        }
    if(!(svmcrossvalidationv->fold_start=malloc(sizeof*svmcrossvalidationv->fold_start*(NR_FOLD+1)))) {
        printf("fidlError: Unable to malloc svmcrossvalidationv->fold_start\n");
        return NULL;
        }
    if((gsvm->param.svm_type == C_SVC || gsvm->param.svm_type == NU_SVC) && NR_FOLD < num_datafiles) {
        if(!(svmcrossvalidationv->svmgroupclassesv=allocSvmgroupclassesv(nclass,num_datafiles))) return NULL;
        if(!(svmcrossvalidationv->fold_count=malloc(sizeof*svmcrossvalidationv->fold_count*NR_FOLD))) {
            printf("fidlError: Unable to malloc svmcrossvalidationv->fold_count\n");
            return NULL;
            }
        if(!(svmcrossvalidationv->index=malloc(sizeof*svmcrossvalidationv->index*num_datafiles))) {
            printf("fidlError: Unable to malloc svmcrossvalidationv->index\n");
            return NULL;
            }
        }
    if(!(svmcrossvalidationv->subprob=allocsubprob(num_datafiles))) {
        printf("fidlError: Unable to malloc svmcrossvalidationv->sub_prob\n");
        return NULL;
        }
    if(!(svmcrossvalidationv->svmtrainv=allocSvmtrainv(gsvm,nclass,num_datafiles))) {
        printf("fidlError: Unable to allocate svmcrossvalidationv->svmtrainv\n");
        return NULL;
        }
    if(gsvm->param.probability) {
        if(!(svmcrossvalidationv->svmbinarysvcprobabilityv=allocSvmbinarysvcprobabilityv(gsvm,nclass,num_datafiles))) {
            printf("fidlError: Unable to allocate svmcrossvalidationv->svmbinarysvcprobabilityv\n");
            return NULL;
            }
        }
    if(gsvm->param.probability && (gsvm->param.svm_type == C_SVC || gsvm->param.svm_type == NU_SVC)) {
        if(!(svmcrossvalidationv->prob_estimates=malloc(sizeof*svmcrossvalidationv->prob_estimates*nclass))) {
            printf("fidlError: Unable to malloc svmcrossvalidationv->prob_estimates\n");
            return NULL;
            }
        }
    else if(!(svmcrossvalidationv->svmpredictv=allocSvmpredictv(nclass,num_datafiles,gsvm->param.svm_type))) {
        printf("fidlError: Unable to malloc svmcrossvalidationv->svmpredictv\n");
        return NULL;
        }
    return svmcrossvalidationv;
    }
Svmsvrprobabilityv *allocSvmsvrprobabilityv(int nclass,int num_datafiles,Svmv *gsvm) {
    Svmsvrprobabilityv *svmsvrprobabilityv=NULL;
    if(!(svmsvrprobabilityv->ymv=malloc(sizeof*svmsvrprobabilityv->ymv*num_datafiles))) {
        printf("fidlError: Unable to malloc svmsvrprobabilityv->ymv\n");
        return NULL;
        }
    /*svmsvrprobabilityv->nr_fold=NR_FOLD;*/
    if(!(svmsvrprobabilityv->svmcrossvalidationv=allocSvmcrossvalidationv(nclass,num_datafiles,gsvm))) {
        printf("fidlError: Unable to alloc svmsvrprobabilityv->svmcrossvalidationv\n");
        return NULL;
        }
    return svmsvrprobabilityv;
    }

//Svmtrainv *allocSvmtrainv(Svmv *gsvm,size_t nclass,int num_datafiles) {
//    size_t i;
//START170203
Svmtrainv *allocSvmtrainv(Svmv *gsvm,int nclass,int num_datafiles) {
    int i;

    Svmtrainv *svmtrainv;
    if(!(svmtrainv=malloc(sizeof*svmtrainv))) {
        printf("fidlError: Unable to malloc svmtrainv\n");
        return NULL;
        }
    if(!(svmtrainv->model=allocModel(nclass,num_datafiles,&gsvm->param))) {
        printf("fidlError: Unable to allocate svmtrainv->model\n");
        return NULL;
        }
    if(!(svmtrainv->svmgroupclassesv=allocSvmgroupclassesv(nclass,num_datafiles))) return NULL;
    if(!(svmtrainv->perm=malloc(sizeof*svmtrainv->perm*num_datafiles))) {
        printf("fidlError: Unable to malloc svmtrainv->perm\n");
        return NULL;
        }
    if(!(svmtrainv->x=malloc(sizeof*svmtrainv->x*num_datafiles))) {
        printf("fidlError: Unable to malloc svmtrainv->x\n");
        return NULL;
        }
    if(!(svmtrainv->weighted_C=malloc(sizeof*svmtrainv->weighted_C*nclass))) {
        printf("fidlError: Unable to malloc svmtrainv->weighted_C\n");
        return NULL;
        }
    if(!(svmtrainv->nonzero=malloc(sizeof*svmtrainv->nonzero*num_datafiles))) {
        printf("fidlError: Unable to malloc svmtrainv->nonzero\n");
        return NULL;
        }
    if(!(svmtrainv->f=malloc(sizeof*svmtrainv->f*nclass*(nclass-1)/2))) {
        printf("fidlError: Unable to malloc svmtrainv->f\n");
        return NULL;
        }
    for(i=0;i<nclass*(nclass-1)/2;i++) {
        if(!(svmtrainv->f[i].alpha=malloc(sizeof*svmtrainv->f[i].alpha*num_datafiles))) {
            printf("fidlError: Unable to malloc svmtrainv->f[i].alpha\n");
            return NULL;
            }
        }
    if(!gsvm->param.probability) {
        svmtrainv->probA=svmtrainv->probB=NULL;
        }
    else {
        if(!(svmtrainv->probA=malloc(sizeof*svmtrainv->probA*nclass*(nclass-1)/2))) {
            printf("fidlError: Unable to malloc svmtrainv->probA\n");
            return NULL;
            }
        if(!(svmtrainv->probB=malloc(sizeof*svmtrainv->probB*nclass*(nclass-1)/2))) {
            printf("fidlError: Unable to malloc svmtrainv->probB\n");
            return NULL;
            }
        }
    if(!(svmtrainv->nz_count=malloc(sizeof*svmtrainv->nz_count*nclass))) {
        printf("fidlError: Unable to malloc svmtrainv->nz_count\n");
        return NULL;
        }
    if(!(svmtrainv->nz_start=malloc(sizeof*svmtrainv->nz_start*nclass))) {
        printf("fidlError: Unable to malloc svmtrainv->nz_start\n");
        return NULL;
        }
    if(!(svmtrainv->sub_prob=allocsubprob(num_datafiles))) {
        printf("fidlError: Unable to malloc svmtrainv->sub_prob\n");
        return NULL;
        }
    return svmtrainv;
    }

//Svmbinarysvcprobabilityv *allocSvmbinarysvcprobabilityv(Svmv *gsvm,size_t nclass,int num_datafiles) {
//START170203
Svmbinarysvcprobabilityv *allocSvmbinarysvcprobabilityv(Svmv *gsvm,int nclass,int num_datafiles) {

    Svmbinarysvcprobabilityv *svmbinarysvcprobabilityv;
    if(!(svmbinarysvcprobabilityv=malloc(sizeof*svmbinarysvcprobabilityv))) {
        printf("fidlError: Unable to malloc svmbinarysvcprobabilityv\n");
        return NULL;
        }
    if(!(svmbinarysvcprobabilityv->perm=malloc(sizeof*svmbinarysvcprobabilityv->perm*num_datafiles))) {
        printf("fidlError: Unable to malloc svmbinarysvcprobabilityv->perm\n");
        return NULL;
        }
    if(!(svmbinarysvcprobabilityv->dec_values=malloc(sizeof*svmbinarysvcprobabilityv->dec_values*num_datafiles))) {
        printf("fidlError: Unable to malloc svmbinarysvcprobabilityv->dec_values\n");
        return NULL;
        }
    if(!(svmbinarysvcprobabilityv->subprob=allocsubprob(num_datafiles))) {
        printf("fidlError: Unable to malloc svmbinarysvcprobabilityv->subprob\n");
        return NULL;
        }
    if(!(svmbinarysvcprobabilityv->svmtrainv=allocSvmtrainv(gsvm,nclass,num_datafiles))) {
        printf("fidlError: allocation failure svmbinarysvcprobabilityv->allocSvmtrainv\n");
        return NULL;
        }
    if(!(svmbinarysvcprobabilityv->subparam.weight_label=malloc(sizeof*svmbinarysvcprobabilityv->subparam.weight_label*2))) {
        printf("fidlError: Unable to malloc svmbinarysvcprobabilityv->subparam.weight_label\n");
        exit(-1);
        }
    if(!(svmbinarysvcprobabilityv->subparam.weight=malloc(sizeof*svmbinarysvcprobabilityv->subparam.weight*2))) {
        printf("fidlError: Unable to malloc svmbinarysvcprobabilityv->subparam.weight\n");
        exit(-1);
        }
    return svmbinarysvcprobabilityv;
    }

//Svmpredictvaluesv *allocSvmpredictvaluesv(size_t nclass,int num_datafiles) {
//START170203
Svmpredictvaluesv *allocSvmpredictvaluesv(int nclass,int num_datafiles) {

    Svmpredictvaluesv *svmpredictvaluesv;
    if(!(svmpredictvaluesv=malloc(sizeof*svmpredictvaluesv))) {
        printf("fidlError: Unable to malloc svmpredictvaluesv\n");
        return NULL;
        }
    if(!(svmpredictvaluesv->kvalue=malloc(sizeof*svmpredictvaluesv->kvalue*num_datafiles))) {
        printf("fidlError: Unable to malloc svmpredictvaluesv->kvalue\n");
        return NULL;
        }
    if(!(svmpredictvaluesv->start=malloc(sizeof*svmpredictvaluesv->start*nclass))) {
        printf("fidlError: Unable to malloc svmpredictvaluesv->start\n");
        return NULL;
        }
    if(!(svmpredictvaluesv->vote=malloc(sizeof*svmpredictvaluesv->vote*nclass))) {
        printf("fidlError: Unable to malloc svmpredictvaluesv->start\n");
        return NULL;
        }
    return svmpredictvaluesv;
    }

//Svmpredictv *allocSvmpredictv(size_t nclass,int num_datafiles,int svm_type) {
//START170203
Svmpredictv *allocSvmpredictv(int nclass,int num_datafiles,int svm_type) {

    Svmpredictv *svmpredictv;
    if(!(svmpredictv=malloc(sizeof*svmpredictv))) {
        printf("fidlError: Unable to malloc svmpredictv\n");
        return NULL;
        }
    if(!(svmpredictv->dec_values=malloc(sizeof*svmpredictv->dec_values*
        ((svm_type==ONE_CLASS||svm_type==EPSILON_SVR||svm_type==NU_SVR)?1:(nclass*(nclass-1)/2))))) {
        printf("fidlError: Unable to malloc svmpredictv->dec_values\n");
        return NULL;
        }
    if(!(svmpredictv->svmpredictvaluesv=allocSvmpredictvaluesv(nclass,num_datafiles))) {
        printf("fidlError: Unable to alloc svmpredictv->svmpredictvaluesv\n");
        return NULL;
        } 
    return svmpredictv;
    }

//Multiclassprobabilityv *allocMulticlassprobabilityv(size_t nclass) {
//START170203
Multiclassprobabilityv *allocMulticlassprobabilityv(int nclass) {

    Multiclassprobabilityv *multiclassprobabilityv;
    if(!(multiclassprobabilityv=malloc(sizeof*multiclassprobabilityv))) {
        printf("fidlError: Unable to malloc multiclassprobabilityv\n");
        return NULL;
        }
    if(!(multiclassprobabilityv->Q=d2double(nclass,nclass))) {
        printf("fidlError: Unable to malloc multiclassprobabilityv->Q\n");
        return NULL;
        }
    if(!(multiclassprobabilityv->Qp=malloc(sizeof*multiclassprobabilityv->Qp*nclass))) {
        printf("fidlError: Unable to malloc multiclassprobabilityv->Qp\n");
        return NULL;
        }
    return multiclassprobabilityv;
    }

//Svmpredictprobabilityv *allocSvmpredictprobabilityv(size_t nclass,int num_datafiles,int ncoor,struct svm_parameter param) {
//START170203
Svmpredictprobabilityv *allocSvmpredictprobabilityv(int nclass,int num_datafiles,struct svm_parameter param) {

    Svmpredictprobabilityv *svmpredictprobabilityv;
    if(!(svmpredictprobabilityv=malloc(sizeof*svmpredictprobabilityv))) {
        printf("fidlError: Unable to malloc svmpredictprobabilityv\n");
        return NULL;
        }
    if((param.svm_type == C_SVC || param.svm_type == NU_SVC) && param.probability) {
        if(!(svmpredictprobabilityv->dec_values=malloc(sizeof*svmpredictprobabilityv->dec_values*nclass*(nclass-1)/2))) {
            printf("fidlError: Unable to malloc svmpredictprobabilityv->dec_values\n");
            return NULL;
            }
        if(!(svmpredictprobabilityv->pairwise_prob=d2double(nclass,nclass))) {
            printf("fidlError: Unable to malloc svmpredictprobabilityv->pairwise_prob\n");
            return NULL;
            }
        if(!(svmpredictprobabilityv->svmpredictvaluesv=allocSvmpredictvaluesv(nclass,num_datafiles))) {
            printf("fidlError: Unable to alloc svmpredictprobabilityv->svmpredictvaluesv\n");
            return NULL;
            } 
        if(!(svmpredictprobabilityv->multiclassprobabilityv=allocMulticlassprobabilityv(nclass))) {
            printf("fidlError: Unable to alloc svmpredictprobabilityv->multiclassprobabilityv\n");
            return NULL;
            } 
        }
    else if(!(svmpredictprobabilityv->svmpredictv=allocSvmpredictv(nclass,num_datafiles,param.svm_type))) {
        printf("fidlError: Unable to allocate svmpredictprobabilityv->svmpredictv\n");
        return NULL; 
        }
    return svmpredictprobabilityv;
    }

Loo *allocLootrial(int num_datafiles) {
    Loo *loo;
    if(!(loo=malloc(sizeof*loo))) {
        printf("fidlError: Unable to malloc loo\n");
        return NULL;
        }
    loo->which=NULL;
    if(!(loo->iwhich=malloc(sizeof*loo->iwhich*num_datafiles))) {
        printf("fidlError: Unable to malloc loo->iwhich\n");
        return NULL;
        }
    if(!(loo->list=malloc(sizeof*loo->list))) {
        printf("fidlError: Unable to malloc loo->list\n");
        return NULL;
        }
    if(!(loo->listi=malloc(sizeof*loo->listi*num_datafiles))) {
        printf("fidlError: Unable to malloc loo->listi\n");
        return NULL;
        }
    return loo;
    }

//Loo *allocLoo(int num_datafiles,size_t n) {
//START170202
Loo *allocLoo(int num_datafiles,int n) {

    Loo *loo;
    if(!(loo=malloc(sizeof*loo))) {
        printf("fidlError: Unable to malloc loo\n");
        return NULL;
        }
    if(!(loo->which=malloc(sizeof*loo->which*num_datafiles))) {
        printf("fidlError: Unable to malloc loo->which\n");
        return NULL;
        }
    if(!(loo->iwhich=malloc(sizeof*loo->iwhich*num_datafiles))) {
        printf("fidlError: Unable to malloc loo->iwhich\n");
        return NULL;
        }
    if(!(loo->list=malloc(sizeof*loo->list*n))) {
        printf("fidlError: Unable to malloc loo->list\n");
        return NULL;
        }
    if(!(loo->listi=malloc(sizeof*loo->listi*n))) {
        printf("fidlError: Unable to malloc loo->listi\n");
        return NULL;
        }
    return loo;
    }

//Pc *allocPc(int num_regions,int num_region_files,size_t nclass,size_t n) {
//    size_t i;
//START170203
Pc *allocPc(int num_regions,int num_region_files,int nclass,int n) {
    int i;

    Pc *pc;
    if(!(pc=malloc(sizeof*pc))) {
        printf("fidlError: Unable to malloc pc\n");
        return NULL;
        }
    if(!(pc->trial=malloc(sizeof*pc->trial*num_regions))) {
        printf("fidlError: Unable to malloc pc->trial\n");
        return NULL;
        }
    for(i=0;i<num_regions;i++)pc->trial[i]=0.;
    if(num_region_files) {
        if(!(pc->class=d2double(num_regions,nclass))) {
            printf("fidlError: Unable to malloc pc->class\n");
            return NULL;
            }
        if(n) if(!(pc->pc=d2double(num_regions,n))) {
            printf("fidlError: Unable to malloc pc->pc\n");
            return NULL;
            }
        }
    return pc;
    }

//Wts *allocWts(size_t nclass0,int ncoor_,int num_regions) {
//START170203
Wts *allocWts(int nclass0,int ncoor_,int num_regions) {

    Wts *w;
    if(!(w=malloc(sizeof*w))) {
        printf("fidlError: Unable to malloc w\n");
        return NULL;
        }
    if(!(w->wts=d3double(nclass0,ncoor_,num_regions))) return NULL;
    if(!(w->wtsn=d3size_t(nclass0,ncoor_,num_regions))) return NULL;
    return w;
    }

//int ldaguts(Gutsv *g,Ldav *glda,Loo *loo,Wts *w,size_t nclass,size_t nclass0,size_t n_,size_t nlist_,size_t *correctby_,
//    float *temp_float,double *temp_double,size_t i,double lambda) {
//START170203
int ldaguts(Gutsv *g,Ldav *glda,Loo *loo,Wts *w,int nclass,int nclass0,int n_,int nlist_,size_t *correctby_,
    float *temp_float,double *temp_double,size_t i,double lambda) {

    int j,k,l,m,n,kk,j1,k1,iwv,correct,ndep;  
    size_t l1,m1,wc0,iwr,iwt;
    double cond_norm2,max,*wtsptr,*dptr,cond,condmax=10000.;
    for(j=0;j<nclass;j++)g->correctbyclass[j]=0;
    if(correctby_) for(j=0;j<n_;j++)correctby_[j]=0;
    if(n_) gsl_sort_int_index(loo->iwhich,loo->which,(size_t)1,(size_t)g->nwhichtrials);
    for(correct=0,j1=j=0;j<nlist_;j1+=loo->list[loo->listi[j++]]) {
        for(k=0;k<nclass;k++)
            for(l=0;l<g->nilistidx;l++){glda->colsum[k][l]=glda->colsumalltrials[k][l];g->colsumn[k][l]=g->colsumalltrialsn[k][l];}
        for(k1=j1,k=0;k<loo->list[loo->listi[j]];k++,k1++) {
            iwt = g->whichtrials[iwr=loo->iwhich[k1]];
            for(kk=g->whichtrialsnonzero[iwr],l=0;l<g->nonzero1[iwt];l++,kk++) {
                iwv=g->ilistidx[g->whichvoxel[kk]];
                glda->colsum[g->whichclass[iwr]][iwv] -= (double)temp_float[kk];
                g->colsumn[g->whichclass[iwr]][iwv]--;
                }
            }
        for(k=0;k<g->nilistidx;k++){g->have[k]=0;g->havei[k]=-1;}
        for(k=0;k<nclass;k++)for(l=0;l<g->nilistidx;l++)if(g->colsumn[k][l])g->have[l]++;
        for(g->nihave=k=0;k<g->nilistidx;k++)if(g->have[k]==nclass){g->ihave[g->nihave]=k;g->havei[k]=g->nihave++;}
        /*classifiers eg 1 vs 234, 2 vs 134, ... */
        for(k=0;k<nclass;k++) {
            for(l=0;l<g->nihave;l++) 
                {glda->mean_colsum[k][g->ihave[l]]=glda->mean_multicolsum[k][g->ihave[l]]=0.;g->sumtdim[k][g->ihave[l]]=0;}
            for(l=0;l<nclass;l++) {
                if(k==l) for(m=0;m<g->nihave;m++) 
                    glda->mean_colsum[k][g->ihave[m]]=glda->colsum[l][g->ihave[m]]/(double)g->colsumn[l][g->ihave[m]];
                else for(m=0;m<g->nihave;m++){glda->mean_multicolsum[k][g->ihave[m]]+=glda->colsum[l][g->ihave[m]];
                    g->sumtdim[k][g->ihave[m]]+=g->colsumn[l][g->ihave[m]];}
                }
            for(l=0;l<g->nihave;l++) glda->mean_multicolsum[k][g->ihave[l]]/=(double)g->sumtdim[k][g->ihave[l]];
            }
        gsl_permutation *perm = gsl_permutation_alloc(g->nihave); 
        gsl_vector_view gvec = gsl_vector_view_array(g->vec,g->nihave);
        gsl_matrix_view gs = gsl_matrix_view_array(g->s[0],g->nihave,g->nihave);
        gsl_matrix_view gsinv = gsl_matrix_view_array(g->sinv[0],g->nihave,g->nihave);
        for(k=1;k<g->nihave;k++) {g->s[k]=g->s[k-1]+g->nihave;g->sinv[k]=g->sinv[k-1]+g->nihave;}

        //START170203
        //gsl_vector_view ghalfmean1plusmean2 = gsl_vector_view_array(glda->halfmean1plusmean2,g->nihave);

        gsl_vector_view gmean1minusmean2 = gsl_vector_view_array(glda->mean1minusmean2,g->nihave);
        gsl_vector_view gtdvm = gsl_vector_view_array(g->tdvm,g->nihave);
        for(k=0;k<nclass0;k++) {
            for(dptr=g->s[0],l=0;l<g->nihave*g->nihave;l++) *dptr++=0.;
            for(l1=l=0;l<nlist_;l1+=loo->list[loo->listi[l++]]) {
                if(l!=j) {
                    for(m1=l1,m=0;m<loo->list[loo->listi[l]];m++,m1++) {
                        iwr = loo->iwhich[m1];
                        dptr=g->whichclass[iwr]==k?glda->mean_colsum[k]:glda->mean_multicolsum[k];
                        for(n=0;n<g->nihave;n++) g->vec[n]=0.;
                        for(kk=g->whichtrialsnonzero[iwr],n=0;n<g->nonzero1[g->whichtrials[iwr]];n++,kk++) {
                            if((iwv=g->havei[g->ilistidx[g->whichvoxel[kk]]])!=-1)
                                g->vec[iwv]=(double)temp_float[kk]-*(dptr+g->ilistidx[g->whichvoxel[kk]]);
                            }
                        gsl_blas_dsyr(CblasUpper,1.0,&gvec.vector,&gs.matrix);
                        }
                    }
                }
            for(l=0;l<g->nihave;l++)for(m=l;m<g->nihave;m++) 
                g->s[m][l] = (g->s[l][m]/=(double)(g->colsumn[k][g->ihave[m]]+g->sumtdim[k][g->ihave[m]]-2));
            if(lambda>0.)for(l=0;l<g->nihave;l++)g->s[l][l]+=lambda;
            for(dptr=g->s[0],l=0;l<g->nihave*g->nihave;l++) temp_double[l]=*dptr++;
            if(cond_norm1(temp_double,g->nihave,&cond,g->sinv[0],perm))cond=0.;
            if(cond>condmax||cond==0.) {
                for(dptr=g->s[0],l=0;l<g->nihave*g->nihave;l++) temp_double[l]=*dptr++;
                gsl_svd_golubreinsch(temp_double,g->nihave,g->nihave,0.,g->V,g->S,g->sinv[0],&cond_norm2,&ndep,g->work);
                }
            for(l=0;l<g->nihave;l++) {
                glda->halfmean1plusmean2[l]=.5*(glda->mean_colsum[k][g->ihave[l]]+glda->mean_multicolsum[k][g->ihave[l]]);
                glda->mean1minusmean2[l]=glda->mean_colsum[k][g->ihave[l]]-glda->mean_multicolsum[k][g->ihave[l]];
                }
            gsl_blas_dsymv(CblasUpper,1.0,&gsinv.matrix,&gmean1minusmean2.vector,0.0,&gtdvm.vector);
            for(l1=j1,l=0;l<loo->list[loo->listi[j]];l++,l1++) {
                iwt = g->whichtrials[iwr=loo->iwhich[l1]];
                for(g->v[iwt][k]=0.,kk=g->whichtrialsnonzero[iwr],m=0;m<g->nonzero1[iwt];m++,kk++) {
                    if((iwv=g->havei[g->ilistidx[g->whichvoxel[kk]]])!=-1) 
                        g->v[iwt][k]+=((double)temp_float[kk]-glda->halfmean1plusmean2[iwv])*g->tdvm[iwv]; 
                    }
                if(nclass0==1) g->v[iwt][1] = -g->v[iwt][0];
                }
            if(nclass0>1&&k==g->whichclass[j]) {
                for(l1=j1,l=0;l<loo->list[loo->listi[j]];l++,l1++) {
                    iwt = g->whichtrials[iwr=loo->iwhich[l1]];
                    for(kk=g->whichtrialsnonzero[iwr],m=0;m<g->nonzero1[iwt];m++,kk++) {
                        iwv=g->havei[g->ilistidx[g->whichvoxel[kk]]];
                        g->wts[iwv] = g->tdvm[iwv]; 
                        }
                    }
                }
            }
        for(k1=j1,k=0;k<loo->list[loo->listi[j]];k++,k1++) {
            iwt = g->whichtrials[iwr=loo->iwhich[k1]];
            for(max=g->v[iwt][0],g->imaxv[iwt]=0,l=1;l<nclass;l++) if(g->v[iwt][l]>max){max=g->v[iwt][l];g->imaxv[iwt]=l;}
            if(g->imaxv[iwt]==g->whichclass[iwt]) {
                correct++;g->correctbyclass[g->whichclass[iwt]]++;if(correctby_)correctby_[loo->listi[j]]++;
                if(nclass0==1){wc0=0;wtsptr=g->tdvm;}else{wc0=g->whichclass[j];wtsptr=g->wts;}
                for(kk=g->whichtrialsnonzero[iwr],m=0;m<g->nonzero1[iwt];m++,kk++) {
                    iwv=g->havei[g->ilistidx[g->whichvoxel[kk]]];
                    w->wts[wc0][g->whichvoxel[kk]][i]+=wtsptr[iwv];
                    w->wtsn[wc0][g->whichvoxel[kk]][i]++;
                    }
                }
            }
        gsl_permutation_free(perm);
        }
    return correct;
    } 

//int lrguts(Gutsv *g,Lrv *glr,Loo *loo,Wts *w,size_t nclass,size_t nclass0,size_t n_,size_t nlist_,size_t *correctby_,
//    float *temp_float,double *temp_double,size_t i,int ncoor,double lambda){
//START170203
int lrguts(Gutsv *g,Lrv *glr,Loo *loo,Wts *w,int nclass,int nclass0,int n_,int nlist_,size_t *correctby_,float *temp_float,
    double *temp_double,int i,int ncoor,double lambda){

    int j,k,l,m,n,kk,j1,l2,iwv,correct,ndep;  
    size_t k1,l1,m1,l3,iwr,iwt,wc0,increment,iterations;
    double cond_norm2,max,*wtsptr,*dptr,cond,condmax=10000.,ll_old,ll,temp,wTX,a;
    for(j=0;j<nclass;j++)g->correctbyclass[j]=0;
    if(correctby_) for(j=0;j<n_;j++)correctby_[j]=0;
    if(n_) gsl_sort_int_index(loo->iwhich,loo->which,(size_t)1,(size_t)g->nwhichtrials);
    for(correct=0,j1=j=0;j<nlist_;j1+=loo->list[loo->listi[j++]]) {
        for(k=0;k<nclass;k++)for(l=0;l<g->nilistidx;l++){g->colsumn[k][l]=g->colsumalltrialsn[k][l];}
        for(k1=j1,k=0;k<loo->list[loo->listi[j]];k++,k1++) {
            iwt = g->whichtrials[iwr=loo->iwhich[k1]];
            for(kk=g->whichtrialsnonzero[iwr],l=0;l<g->nonzero1[iwt];l++,kk++) {
                iwv=g->ilistidx[g->whichvoxel[kk]];
                g->colsumn[g->whichclass[iwr]][iwv]--;
                }
            }
        for(k=0;k<g->nilistidx;k++){g->have[k]=0;g->havei[k]=-1;}
        for(k=0;k<nclass;k++)for(l=0;l<g->nilistidx;l++)if(g->colsumn[k][l])g->have[l]++;
        for(g->nihave=k=0;k<g->nilistidx;k++)if(g->have[k]==nclass){g->ihave[g->nihave]=k;g->havei[k]=g->nihave++;}
        /*classifiers eg 1 vs 234, 2 vs 134, ... */
        for(k=0;k<nclass;k++) {
            for(l=0;l<g->nihave;l++) g->sumtdim[k][g->ihave[l]]=0;
            for(l=0;l<nclass;l++)for(m=0;m<g->nihave;m++)g->sumtdim[k][g->ihave[m]]+=g->colsumn[l][g->ihave[m]];
            }
        for(glr->sumtdimmax=0,k=0;k<nclass;k++) 
            for(l=0;l<g->nihave;l++) if(g->sumtdim[k][g->ihave[l]]>glr->sumtdimmax)glr->sumtdimmax=g->sumtdim[k][g->ihave[l]];

        #if 0
        printf("g->sumtdim\n");
        for(k=0;k<nclass;k++) {
            printf("    k=%d ",k);for(l=0;l<g->nihave;l++)printf("%d ",g->sumtdim[k][g->ihave[l]]);printf("\n");
            }
        printf("glr->sumtdimmax=%d\n",glr->sumtdimmax);
        #endif


        gsl_matrix_view gs = gsl_matrix_view_array(g->s[0],g->nihave+1,g->nihave+1);
        gsl_matrix_view gsinv = gsl_matrix_view_array(g->sinv[0],g->nihave+1,g->nihave+1);
        for(k=1;k<g->nihave+1;k++) {g->s[k]=g->s[k-1]+g->nihave+1;}
        gsl_vector_view gXAz = gsl_vector_view_array(g->tdvm,g->nihave+1);
        gsl_vector_view gvec = gsl_vector_view_array(g->vec,g->nihave+1);
        gsl_vector_view gwnew = gsl_vector_view_array(glr->wnew,g->nihave+1);
        gsl_permutation *perm = gsl_permutation_alloc(g->nihave+1);
        for(l=1;l<glr->sumtdimmax;l++) glr->X[l]=glr->X[l-1]+g->nihave+1;
        for(l=0;l<glr->sumtdimmax;l++) {glr->N[l]=0; for(m=0;m<g->nihave;m++)glr->X[l][m]=0.; glr->X[l][g->nihave]=1.;}
        for(l=0;l<g->nihave+1;l++) g->tdvm[l]=0.;
        for(l3=l2=l1=l=0;l<nlist_;l1+=loo->list[loo->listi[l++]]) {
            if(l!=j) {
                for(m1=l1,m=0;m<loo->list[loo->listi[l]];m++,m1++,l2++) {
                    iwr=loo->iwhich[m1];
                    for(kk=g->whichtrialsnonzero[iwr],n=0;n<g->nonzero1[g->whichtrials[iwr]];n++,kk++)
                        if((iwv=g->havei[g->ilistidx[g->whichvoxel[kk]]])!=-1) {
                            glr->X[l2][iwv]=(double)temp_float[kk];
                            glr->Iwv[l3++]=iwv;
                            glr->N[l2]++;
                            }
                    }
                }
            }
        if(l2>glr->sumtdimmax) {
            printf("fidlError: i=%d l2=%d glr->sumtdimmax=%d l2 must be less than or equal to glr->sumtdimmax\n",i,l2,
                glr->sumtdimmax); fflush(stdout);exit(-1);
            }
        if(l2<glr->sumtdimmax){printf("CHECK i=%d glr->sumtdimmax=%d l2=%d\n",i,glr->sumtdimmax,l2);fflush(stdout);}
        for(l=0;l<l2;l++)if(glr->N[l]!=g->nihave){printf("CHECK glr->N[%d]=%d g->nihave=%d\n",l,glr->N[l],g->nihave);fflush(stdout);}

        /*printf("i=%d j=%d X\n",i,j);
        for(l=0;l<l2;l++){for(m=0;m<g->nihave+1;m++)printf("%.2f ",glr->X[l][m]); printf("\n");}fflush(stdout);*/


        for(k=0;k<nclass0;k++) {
            for(l2=l1=l=0;l<nlist_;l1+=loo->list[loo->listi[l++]]) {
                if(l!=j) {
                    for(m1=l1,m=0;m<loo->list[loo->listi[l]];m++,m1++,l2++) {
                        iwr=loo->iwhich[m1];
                        glr->Az[l2]=(glr->y[l2]=g->whichclass[iwr]==k?1.:-1.)/2.; 
                        }
                    }
                }
            gsl_vector_view gAz = gsl_vector_view_array(glr->Az,l2);

            //START170203
            //gsl_vector_view gy = gsl_vector_view_array(glr->y,l2);

            gsl_matrix_view gX = gsl_matrix_view_array(glr->X[0],l2,g->nihave+1);
            ll_old = (double)l2*(double)M_LN2;
            gsl_blas_dsyrk(CblasLower,CblasTrans,.25,&gX.matrix,0.0,&gs.matrix);
            increment=iterations=0;
            do {
                gsl_blas_dgemv(CblasTrans,1.0,&gX.matrix,&gAz.vector,0.0,&gXAz.vector);
                if(lambda>0.)for(l=0;l<g->nihave+1;l++)g->s[l][l]+=lambda;
                for(n=l=0;l<g->nihave+1;l++)for(m=l;m<g->nihave+1;m++,n++) temp_double[n] = g->s[l][m] = g->s[m][l];
                if(cond_norm1(g->s[0],g->nihave+1,&cond,g->sinv[0],perm))cond=0.;
                if(cond>condmax||cond==0.) {
                    for(n=l=0;l<g->nihave+1;l++)for(m=l;m<g->nihave+1;m++,n++) g->s[l][m] = g->s[m][l] = temp_double[n];
                    gsl_svd_golubreinsch(g->s[0],g->nihave+1,g->nihave+1,0.,g->V,g->S,g->sinv[0],&cond_norm2,&ndep,g->work);
                    }
                gsl_blas_dsymv(CblasUpper,1.0,&gsinv.matrix,&gXAz.vector,0.0,&gwnew.vector);
                for(dptr=g->s[0],l=0;l<(g->nihave+1)*(g->nihave+1);l++)*dptr++=0.;
                for(ll=0.,l3=l=0;l<l2;l++) {
                    for(m=0;m<g->nihave;m++)g->vec[m]=0.; g->vec[g->nihave]=1.;
                    for(wTX=0.,m=0;m<glr->N[l];m++,l3++)
                        wTX += glr->wnew[glr->Iwv[l3]]*(g->vec[glr->Iwv[l3]]=glr->X[l][glr->Iwv[l3]]); wTX += glr->wnew[g->nihave];
                    temp = 1./(1.+exp(wTX));
                    a = temp*(1-temp);
                    temp = 1. + exp(-glr->y[l]*wTX);
                    glr->Az[l] = a*wTX + (1.-1./temp)*glr->y[l];
                    ll += log(temp);
                    gsl_blas_dsyr(CblasLower,a,&gvec.vector,&gs.matrix);
                    }
                if(lambda>0.) {for(a=0.,l=0;l<g->nihave+1;l++)a+=glr->wnew[l]*glr->wnew[l]; ll+=lambda*a/2.;}

                //increment = fabs(ll-ll_old)<=(double)TOL ? ++increment : 0;
                //START170203
                if(fabs(ll-ll_old)<=(double)TOL){++increment;}else{increment=0;}

                /*printf("ll=%f ll_old=%f ll-ll_old=%f TOL=%f increment=%d iterations=%d\n",ll,ll_old,ll-ll_old,TOL,increment,
                    iterations);fflush(stdout);*/
                ll_old = ll;iterations++;
                } while(increment<(size_t)MAXINC && iterations<(size_t)MAXIT);
            for(l1=j1,l=0;l<loo->list[loo->listi[j]];l++,l1++) {
                iwt = g->whichtrials[iwr=loo->iwhich[l1]];
                for(g->v[iwt][k]=0.,kk=g->whichtrialsnonzero[iwr],m=0;m<g->nonzero1[iwt];m++,kk++) {
                    if((iwv=g->havei[g->ilistidx[g->whichvoxel[kk]]])!=-1) g->v[iwt][k]+=(double)temp_float[kk]*glr->wnew[iwv]; 
                    }
                g->v[iwt][k]+=glr->wnew[g->nihave];
                if(nclass0==1) g->v[iwt][1] = -g->v[iwt][0];
                }
            if(nclass0>1&&k==g->whichclass[j]) {
                for(l1=j1,l=0;l<loo->list[loo->listi[j]];l++,l1++) {
                    iwt = g->whichtrials[iwr=loo->iwhich[l1]];
                    for(kk=g->whichtrialsnonzero[iwr],m=0;m<g->nonzero1[iwt];m++,kk++) {
                        iwv=g->havei[g->ilistidx[g->whichvoxel[kk]]];
                        g->wts[iwv] = glr->wnew[iwv];
                        }
                    g->wts[g->nihave] = glr->wnew[g->nihave];
                    }
                }
            }
        for(k1=j1,k=0;k<loo->list[loo->listi[j]];k++,k1++) {
            iwt = g->whichtrials[iwr=loo->iwhich[k1]];
            for(max=g->v[iwt][0],g->imaxv[iwt]=0,l=1;l<nclass;l++) if(g->v[iwt][l]>max){max=g->v[iwt][l];g->imaxv[iwt]=l;}
            if(g->imaxv[iwt]==g->whichclass[iwt]) {
                correct++;g->correctbyclass[g->whichclass[iwt]]++;if(correctby_)correctby_[loo->listi[j]]++;
                if(nclass0==1){wc0=0;wtsptr=glr->wnew;}else{wc0=g->whichclass[j];wtsptr=g->wts;}
                for(kk=g->whichtrialsnonzero[iwr],m=0;m<g->nonzero1[iwt];m++,kk++) {
                    iwv=g->havei[g->ilistidx[g->whichvoxel[kk]]];
                    w->wts[wc0][g->whichvoxel[kk]][i]+=wtsptr[iwv];
                    w->wtsn[wc0][g->whichvoxel[kk]][i]++;
                    }
                w->wts[wc0][ncoor][i]+=wtsptr[g->nihave];
                w->wtsn[wc0][ncoor][i]++;
                }
            }
        gsl_permutation_free(perm);
        }
    return correct;
    } 
int svmguts(Gutsv *g,Svmv *gsvm,Svmtrainv *svmtrainv,Svmbinarysvcprobabilityv *svmbinarysvcprobabilityv,
    Svmsvrprobabilityv *svmsvrprobabilityv,Svmpredictv *svmpredictv,Loo *loo,int nclass,int n_,
    int nlist_,size_t *correctby_,float *temp_float,int i) {
    int j,k,l,m,n,j1,l2,iwv,correct,max_index;  
    size_t kk,k1,l1,m1,l3,iwt,iwr;
    double td;
    //FILE *fp;
    for(j=0;j<nclass;j++)g->correctbyclass[j]=0;
    if(correctby_) for(j=0;j<n_;j++)correctby_[j]=0;
    if(n_) gsl_sort_int_index(loo->iwhich,loo->which,(size_t)1,(size_t)g->nwhichtrials);
    for(correct=0,j1=j=0;j<nlist_;j1+=loo->list[loo->listi[j++]]) {
        for(k=0;k<nclass;k++)for(l=0;l<g->nilistidx;l++){g->colsumn[k][l]=g->colsumalltrialsn[k][l];}
        for(k1=j1,k=0;k<loo->list[loo->listi[j]];k++,k1++) {
            iwt = g->whichtrials[iwr=loo->iwhich[k1]];
            for(kk=g->whichtrialsnonzero[iwr],l=0;l<g->nonzero1[iwt];l++,kk++) {
                iwv=g->ilistidx[g->whichvoxel[kk]];
                g->colsumn[g->whichclass[iwr]][iwv]--;
                }
            }
        for(k=0;k<g->nilistidx;k++){g->have[k]=0;g->havei[k]=-1;}
        for(k=0;k<nclass;k++)for(l=0;l<g->nilistidx;l++)if(g->colsumn[k][l])g->have[l]++;
        for(g->nihave=k=0;k<g->nilistidx;k++)if(g->have[k]==nclass){g->ihave[g->nihave]=k;g->havei[k]=g->nihave++;}
        /*classifiers eg 1 vs 234, 2 vs 134, ... */
        for(k=0;k<nclass;k++) {
            for(l=0;l<g->nihave;l++) g->sumtdim[k][g->ihave[l]]=0;
            for(l=0;l<nclass;l++)for(m=0;m<g->nihave;m++)g->sumtdim[k][g->ihave[m]]+=g->colsumn[l][g->ihave[m]];
            }
        for(gsvm->sumtdimmax=0,k=0;k<nclass;k++) 
            for(l=0;l<g->nihave;l++) if(g->sumtdim[k][g->ihave[l]]>gsvm->sumtdimmax)gsvm->sumtdimmax=g->sumtdim[k][g->ihave[l]];

        //START170206
        //gsl_vector_view gvec = gsl_vector_view_array(g->vec,g->nihave+1);
        //gsl_vector_view gwnew = gsl_vector_view_array(gsvm->wnew,g->nihave+1);

        for(l=0;l<gsvm->sumtdimmax;l++)gsvm->N[l]=0;
        for(l=0;l<g->nihave+1;l++) g->tdvm[l]=0.;
        gsvm->prob->l = gsvm->sumtdimmax;
        for(l=0;l<g->nihave;l++){gsvm->feature_max[l]=-DBL_MAX;gsvm->feature_min[l]=DBL_MAX;}
        for(l3=l2=l1=l=0;l<nlist_;l1+=loo->list[loo->listi[l++]]) {
            if(l!=j) {
                for(m1=l1,m=0;m<loo->list[loo->listi[l]];m++,m1++,l2++) {
                    iwr=loo->iwhich[m1];
                    for(kk=g->whichtrialsnonzero[iwr],n=0;n<g->nonzero1[g->whichtrials[iwr]];n++,kk++)
                        if((iwv=g->havei[g->ilistidx[g->whichvoxel[kk]]])!=-1) {
                            if((td=(double)temp_float[kk])>gsvm->feature_max[iwv]) gsvm->feature_max[iwv]=td;
                            if(td<gsvm->feature_min[iwv]) gsvm->feature_min[iwv]=td;
                            gsvm->N[l2]++;
                            /*printf("***********HERE************** gsvm->N[%lu]=%lu\n",l2,gsvm->N[l2]);*/
                            }
                    }
                }
            }
        for(l=0;l<g->nihave;l++)gsvm->feature_dif[l]=gsvm->feature_max[l]-gsvm->feature_min[l];
        for(l=0;l<gsvm->sumtdimmax;l++) gsvm->prob->n[l]=gsvm->N[l];

        #if 0
        printf("gsvm->prob->n= ");for(l=0;l<gsvm->sumtdimmax;l++) printf("%lu ",gsvm->prob->n[l]);printf("\n");
        for(l=0;l<g->nihave;l++)printf("gsvm->feature_max[%d]=%f gsvm->feature_min[%d]=%f gsvm->feature_dif[%d]=%f\n",l,
            gsvm->feature_max[l],l,gsvm->feature_min[l],l,gsvm->feature_dif[l]);
        printf("gsvm->lower=%f gsvm->upper=%f gsvm->dif=%f\n",gsvm->lower,gsvm->upper,gsvm->dif);
        printf("g->nihave=%d gsvm->sumtdimmax=%d\n",g->nihave,gsvm->sumtdimmax);
        for(k=0;k<nclass;k++)for(l=0;l<g->nihave;l++) printf("g->sumtdim[%d][%d]=%d\n",k,g->ihave[l],g->sumtdim[k][g->ihave[l]]);
        for(l=0;l<gsvm->sumtdimmax;l++)printf("gsvm->N[%d]=%d\n",l,gsvm->N[l]);
        for(l=0;l<nclass;l++)for(m=0;m<g->nihave;m++)printf("g->colsumn[%d][%d]=%d\n",l,g->ihave[m],g->colsumn[l][g->ihave[m]]);
        for(k=0;k<nclass;k++)for(l=0;l<g->nilistidx;l++)printf("g->colsumalltrialsn[%d][%d]=%d\n",k,l,g->colsumalltrialsn[k][l]);
        for(k=0;k<g->nilistidx;k++)printf("g->have[%d]=%d\n",k,g->have[k]);
        #endif

        if(l2>gsvm->sumtdimmax) {
            printf("fidlError: i=%d l2=%d gsvm->sumtdimmax=%d l2 must be less than or equal to gsvm->sumtdimmax\n",i,l2,
                gsvm->sumtdimmax); fflush(stdout);exit(-1);
            }
        if(l2<gsvm->sumtdimmax){printf("CHECK i=%d gsvm->sumtdimmax=%d l2=%d\n",i,gsvm->sumtdimmax,l2);fflush(stdout);}
        for(l=0;l<l2;l++)if(gsvm->N[l]!=g->nihave)
            {printf("CHECK gsvm->N[%d]=%d g->nihave=%d\n",l,gsvm->N[l],g->nihave);fflush(stdout);}

        /*if(!(fp=fopen_sub("svm_example.scale","w"))) return 0;*/
        /*for(l3=l=0;l<gsvm->sumtdimmax;l++,l3+=g->nihave+1)gsvm->x_space[l3].index=-1;*/
        for(max_index=l3=l2=l1=l=0;l<nlist_;l1+=loo->list[loo->listi[l++]]) {
            if(l!=j) {
                for(m1=l1,m=0;m<loo->list[loo->listi[l]];m++,m1++,l2++) {
                    gsvm->prob->x[l2]=&gsvm->x_space[l3];
                    iwr=loo->iwhich[m1];
                    gsvm->prob->y[l2]=(double)g->whichclass[iwr]+1.;
                    /*fprintf(fp,"%d ",g->whichclass[iwr]+1);*/
                    for(kk=g->whichtrialsnonzero[iwr],n=0;n<g->nonzero1[g->whichtrials[iwr]];n++,kk++)
                        if((iwv=g->havei[g->ilistidx[g->whichvoxel[kk]]])!=-1) {
                            if((gsvm->x_space[l3].index=iwv+1)>max_index) max_index=gsvm->x_space[l3].index;
                            if((td=(double)temp_float[kk])==gsvm->feature_min[iwv])td=gsvm->lower;
                            else if(td==gsvm->feature_max[iwv])td=gsvm->upper;
                            else td=gsvm->lower+gsvm->dif*(td-gsvm->feature_min[iwv])/gsvm->feature_dif[iwv];
                            gsvm->x_space[l3++].value = td;
                            /*fprintf(fp,"%d:%f ",iwv+1,td);*/
                            }

                    /*START140122*/
                    /*l3++;*/ /*increment for -1 flag*/
                    gsvm->x_space[l3++].index=-1;

                    /*fprintf(fp,"\n");*/
                    }
                }
            }
        /*fclose(fp);*/

        /*printf("svm_train\n");*/
        /*svm_train(gsvm->prob,&gsvm->param);*/

        /*printf("gsvm->prob->n= ");for(l=0;l<gsvm->sumtdimmax;l++) printf("%lu ",gsvm->prob->n[l]);printf("\n");*/

        svm_train1(gsvm->prob,&gsvm->param,svmtrainv,svmbinarysvcprobabilityv,svmsvrprobabilityv);

        /*START131205*/
        /*for(m=0;m<g->nihave;m++)gsvm->x[m].index=-1;*/
        for(l1=j1,l=0;l<loo->list[loo->listi[j]];l++,l1++) {
            iwr=loo->iwhich[l1];
            /*gsvm->target_label=(double)g->whichclass[iwr]+1.;*/
            for(kk=g->whichtrialsnonzero[iwr],m1=m=0;m<g->nonzero1[g->whichtrials[iwr]];m++,kk++) {
                if((iwv=g->havei[g->ilistidx[g->whichvoxel[kk]]])!=-1) {
                    gsvm->x[m1].index=iwv+1;
                    if((td=(double)temp_float[kk])==gsvm->feature_min[iwv])td=gsvm->lower;
                    else if(td==gsvm->feature_max[iwv])td=gsvm->upper;
                    else td=gsvm->lower+gsvm->dif*(td-gsvm->feature_min[iwv])/gsvm->feature_dif[iwv];
                    gsvm->x[m1++].value = td;
                    }
                }
            gsvm->x[m1].index=-1;
            td=svm_predict1(svmtrainv->model,gsvm->x,svmpredictv);
            /*printf("target_label=%d predict_label=%g\n",g->whichclass[iwr]+1,td);*/
            if((g->whichclass[iwr]+1)==td) {
                correct++;g->correctbyclass[g->whichclass[iwr]]++;if(correctby_)correctby_[loo->listi[j]]++;
                }
            }
        }
    return correct;
    } 
int printreg(char *f,char *which,Regions_By_File *rbf,Pc *pc,int class_lnptri,int nclass,char **ad_lnptr,
    double *xx,double *yy,double *zz,Atlas_Param *ap,double *coor1,int nclass0,double ***_wts_,int n_) {
    char outf[MAXNAME],string[MAXNAME];
    int i,j,k,l;
    FILE *fp;
    strcpy(string,f);
    if(!get_tail_sans_ext(string)) return 0;
    sprintf(outf,"%s_%s_results.txt",string,which);
    if(!(fp=fopen_sub(outf,"w"))) return 0;
    fprintf(fp,"%*s\tpercent correct\n",rbf->max_length,"region");
    for(i=0;i<rbf->nreg;i++) fprintf(fp,"%*s\t%.2f\n",rbf->max_length,rbf->region_names_ptr[i],pc->trial[i]);
    fprintf(fp,"\npercent correct by class\n");
    fprintf(fp,"%*s\t",rbf->max_length,"region");
    for(j=class_lnptri,i=0;i<nclass;i++,j++) fprintf(fp,"%s\t",ad_lnptr[j]); fprintf(fp,"\n");
    for(i=0;i<rbf->nreg;i++) {
        fprintf(fp,"%*s\t",rbf->max_length,rbf->region_names_ptr[i]);
        for(j=0;j<nclass;j++) fprintf(fp,"%.2f\t",pc->class[i][j]);
        fprintf(fp,"\n");
        }
    if(pc->pc) {
        fprintf(fp,"%s\t",which); for(i=0;i<n_;i++) fprintf(fp,"%d\t",i+1); fprintf(fp,"\n");
        for(i=0;i<rbf->nreg;i++) {
            fprintf(fp,"%*s\t",rbf->max_length,rbf->region_names_ptr[i]);
            for(j=0;j<n_;j++) fprintf(fp,"%.2f\t",pc->pc[i][j]);
            fprintf(fp,"\n");
            }
        }
    fflush(fp);fclose(fp);
    printf("Output written to %s\n",outf);
    if(_wts_) {
        if(ap->xdim) {
            col_row_slice(rbf->nvoxels,rbf->indices,xx,yy,zz,ap);
            get_atlas_coor(rbf->nvoxels,xx,yy,zz,(double)ap->zdim,ap->center,ap->mmppix,coor1);
            }
        sprintf(outf,"%s_%s_wts.txt",string,which);
        if(!(fp=fopen_sub(outf,"w"))) return 0;
        if(ap->xdim) fprintf(fp,"weights\nx\ty\tz\twt\n");
        for(l=i=0;i<rbf->nreg;i++) {
            fprintf(fp,"%s\n",rbf->region_names_ptr[i]);
            for(k=class_lnptri,j=0;j<nclass0;j++,k++)fprintf(fp,"%s\t",ad_lnptr[k]); fprintf(fp,"\n");
            for(k=0;k<rbf->nvoxels_region[i];k++,l+=3) {
                if(ap->xdim) fprintf(fp,"%g\t%g\t%g\t",coor1[l],coor1[l+1],coor1[l+2]);
                for(j=0;j<nclass0;j++)fprintf(fp,"%g\t",_wts_[j][k][i]); fprintf(fp,"\n");
                }
            fprintf(fp,"\n");
            }
        fflush(fp);fclose(fp);
        printf("Output written to %s\n",outf);
        }
    return 1;
    }
int printimg(Mask_Struct *ms,float *temp_float,double *_pctrial_,char *f,Interfile_header *ifh,int class_lnptri,int ncoor,
    char **ad_lnptr,double ***_wts_,int nclass0){
    char outf[MAXNAME],string[MAXNAME];
    int i,j,k,i1;
    FILE *fp;
    for(i=0;i<ms->lenvol;i++) temp_float[i]=0.;
    for(i=0;i<ms->lenbrain;i++) temp_float[ms->brnidx[i]]=(float)_pctrial_[i];
    if(!writestack(f,temp_float,sizeof*temp_float,(size_t)ms->lenvol,0)) return 0;
    min_and_max_init(&ifh->global_min,&ifh->global_max);
    min_and_max_doublestack(_pctrial_,ms->lenbrain,&ifh->global_min,&ifh->global_max);
    ifh->dim4 = 1;
    if(!write_ifh(f,ifh,0)) return 0;
    printf("Output written to %s\n",f);
    if(_wts_) {
        strcpy(string,f);
        if(!get_tail_sans_ext(string)) return 0;
        strcat(string,"_wts");
        ifh->dim4 = ncoor;
        for(i1=class_lnptri,i=0;i<nclass0;i++,i1++) {
            sprintf(outf,"%s_%s.4dfp.img",string,ad_lnptr[i1]);
            min_and_max_init(&ifh->global_min,&ifh->global_max);
            min_and_max_doublestack(&_wts_[i][0][0],ncoor*ms->lenbrain,&ifh->global_min,&ifh->global_max);
            if(!(fp=fopen_sub(outf,"w"))) return 0;
            for(j=0;j<ncoor;j++) {
                for(k=0;k<ms->lenbrain;k++) temp_float[ms->brnidx[k]]=(float)_wts_[i][j][k];
                if(!(fwrite_sub(temp_float,sizeof(float),(size_t)ms->lenvol,fp,0))) return 0;
                }
            fflush(fp);fclose(fp);
            if(!write_ifh(outf,ifh,0)) return 0;
            printf("Output written to %s\n",outf);
            }
        }
    return 1;
    }
void printfp(FILE *fp,int nclass,char *region_names_ptr,Gutsv *g){
    int j,k;
    fprintf(fp,"class\t");for(j=0;j<nclass;j++)fprintf(fp,"%10d\t",j+1);
    fprintf(fp,"\n\n%s\n",region_names_ptr);
    for(j=0;j<g->nwhichtrials;j++) {
        for(k=0;k<nclass;k++) fprintf(fp,"%.2f\t",g->v[j][k]);
        fprintf(fp,"%s\n",g->imaxv[j]==g->whichclass[j]?"correct":"incorrect");
        }
    }
