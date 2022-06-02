/* Copyright 9/17/19 Washington University.  All Rights Reserved.
   fidl_residuals.cxx  $Revision: 1.1 $ */
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <iostream>
#include "fidl.h"
#include "gauss_smoth2.h"
#include "read_frames_file.h"
#include "get_atlas_param.h"
#include "dim_param2.h"
#include "mask.h"

#if 0
#include "region.h"
#include "find_regions_by_file_cover.h"
#endif
//START211020
#include "region3.h"
#include "find_regions_by_file_cover3.h"

#include "map_disk.h"
#include "shouldiswap.h"
#include "subs_util.h"
#include "t4_atlas.h"
#include "filetype.h"
#include "subs_cifti.h"
#include "subs_nifti.h"
#include "write1.h"
#include "get_grand_mean_struct.h"
#include "d2double.h"
#include "checkOS.h"
#include "get_atlas.h"
#include "read_xform.h"
#include "twoA_or_twoB.h"
#include "check_dimensions.h"
#include "utilities2.h"

struct Mask_Struct{
    int lenbrain,*brnidx,nuns,*unsi,vol;
    };
char const* Fileext[]={"",".4dfp.img",".dscalar.nii",".nii",".dtseries.nii"};

static char rcsid[] = "$Header: /home/hannah/mcavoy/idl/clib/RCS/fidl_residuals.c,v 1.3 2017/02/21 20:37:29 mcavoy Exp $";
int read_set_write_ifh(char *bold_file,char *xform_file,int dim4,char *filename,Interfile_header *glm_ifh,Atlas_Param *ap,
    int scratchdir,int how_many,int SunOS_Linux,int lcfloat);
int main(int argc,char **argv)
{
char *glm_file=NULL,*root=NULL,filename[MAXNAME],*strptr,dum[MAXNAME],*xform_file=NULL,*ac_name=NULL,*mask_file=NULL,
    *frames_file=NULL,*s1="BOLDS    ",*s2="         ",*s3="REGIONS  ",*gcstr=NULL,*scratchdir=NULL,rootname[MAXNAME],
    autocorrroot[MAXNAME],crosscorrroot[MAXNAME],crosscovroot[MAXNAME],covroot[MAXNAME],varroot[MAXNAME],concname[MAXNAME],
    atlas[7]="",glm_atlas[7]=""; //*gcmask=NULL
int i,j,k,l,m,n,o,p,t,kkk,lll,ii,kk,j1,k1,vol,num_bold_files=0,*retain_estimates_index=NULL,num_retain_estimates=0,*tdim_usable,
    lc_skip=0,lc_resid=1,lc_average=0,lc_one_file=0,lc_pctchg=0,start_b,num_regions=0,*roi=NULL,num_region_files=0,A_or_B_or_U=0,
    *num_usable_per_frame,num_frames=0,*frames_index=NULL,*valid_frames,lc_regional_avg_cond_across_runs=0,nstart=0,nlength=0,
    *start_index=NULL,*length=NULL,length_max=0,*den=NULL,*ncond_per_bold=NULL,pairs,lc_t4_before=0,nest,*est=NULL,*temp_int,do_more,
    SunOS_Linux,nindex=0,*index=NULL,nbold_index,*bold_index,nframes_code=0,*frames_code=NULL,ntc=0,argc_tc=0,incrementlll,
    nregional_residual_name=0,*validframeindex,lc_pca=0,nvfi,lc_voxels=0,lc_unformatted=0,*ivs=NULL,nvs=0,
    nidentify=0,lc_names_only=0,how_many,lc_autocorr=0,lc_crosscov=0,lc_crosscorr=0,dim1,dim4,lc_cov=0,
    lc_var=0,maxlik_unbias=1,ntemp_double,swapbytes,lc_names_ifh=0,lcfloat=0,lc_voxelsall=0,*iout=NULL,niout=0,lcvarall=0,
    nbold_index1,*bold_index1,t0; //nglm_index=0,*glm_index=NULL,avi_seg=0,q,lc_rgc=0
size_t i1;
float *temp_float,*t4=NULL,***region_tc_bold=NULL,fwhm=0,*temp_float2=NULL;
double *temp_double,*stat,**avgreg=NULL,**ac_numer=NULL,**ac_denom=NULL,*pairs_stack=NULL,*bold,*avg=NULL,*voxel_tc=NULL,*x=NULL,
    *voxelsum=NULL,td,*y=NULL,*SSyy=NULL,*SSE=NULL,*R2=NULL,*Ra2=NULL,***region_tc=NULL,*corr=NULL,
    *cov=NULL,*var=NULL,*sum=NULL,*sum2=NULL,*td1,*td2,*td3=NULL,*td4=NULL,*dptr,*dptr1=NULL; //*storage=NULL,*gc=NULL,xtx,xty,b
FILE *fp,*op=NULL,*cp;
LinearModel *glm;
Grand_Mean_Struct *gms=NULL;
Regions **reg; //*reg_gc
Regions_By_File *rbf=NULL; //*rbf_gc
Atlas_Param *ap;
Memory_Map *mm=NULL;
Dim_Param2 *dp;
Files_Struct *bold_files=NULL,*region_files=NULL,*regional_residual_name=NULL,*identify=NULL; /* *addregfiles=NULL */
Mask_Struct *ms,*ms_glm,*ms_bold; //*ms_gc=NULL
mask ms0,ms0_bold,ms0_glm;
TC *tcs;
FS *fs=NULL;
PC *pc;
gauss_smoth2_struct *gs=NULL;
void* niftiPtr=NULL;
W1 *w1=NULL;
int64_t dims[4];

if(argc < 3) {

    #if 0
    fprintf(stderr,"    -glm_file:                     *.glm file that contains the linear model.\n");
    fprintf(stderr,"    -bold_files:                   imgs or concs.\n");
    fprintf(stderr,"    -skip                          Skip frames that are not listed as valid frames in the glm.\n");
    fprintf(stderr,"    -retain_estimates:             Estimates to be retained in the residuals. First estimate is 1.\n");
    fprintf(stderr,"                                   Note, each column of the design matrix represents a single estimate.\n");
    fprintf(stderr,"    -store_model_fit               By default residuals are computed.\n");
    fprintf(stderr,"                                   This option computes the model fit instead.\n");
    fprintf(stderr,"                                   By default all estimates are retained.\n");
    fprintf(stderr,"    -average_across_runs           Residuals are averaged across runs.\n");
    fprintf(stderr,"    -onefile                       Write output to a single file. The default is to output as many files as bolds.\n");
    fprintf(stderr,"    -root:                         Root name to be given to output file.\n");
    fprintf(stderr,"                                   Include the full path to ensure a proper conc file.\n");
    fprintf(stderr,"    -percent_change                Express in units of percent change. The default is raw MR units.\n");
    fprintf(stderr,"    -xform_file:                   t4 file defining the transform to atlas space.\n");
    fprintf(stderr,"    -atlas:                        Either 111, 222 or 333. Used with -xform_files option.\n");
    fprintf(stderr,"    -region_file:                  *.4dfp.img file(s) that specifies regions of interest.\n");
    fprintf(stderr,"                                   Magnitudes and time courses are averaged over the region.\n");
    #endif
    //START201007
    std::cout<<"    -glm_file:                     *.glm file that contains the linear model."<<std::endl;
    std::cout<<"    -bold_files:                   imgs or concs."<<std::endl;
    std::cout<<"    -skip                          Skip frames that are not listed as valid frames in the glm."<<std::endl;
    std::cout<<"    -retain_estimates:             Estimates to be retained in the residuals. First estimate is 1."<<std::endl;
    std::cout<<"                                   Note, each column of the design matrix represents a single estimate."<<std::endl;
    std::cout<<"    -store_model_fit               By default residuals are computed."<<std::endl;
    std::cout<<"                                   This option computes the model fit instead."<<std::endl;
    std::cout<<"                                   By default all estimates are retained."<<std::endl;
    std::cout<<"    -average_across_runs           Residuals are averaged across runs."<<std::endl;
    std::cout<<"    -onefile                       Write output to a single file. The default is to output as many files as bolds."<<std::endl;
    std::cout<<"    -root:                         Root name to be given to output file."<<std::endl;
    std::cout<<"                                   Include the full path to ensure a proper conc file."<<std::endl;
    std::cout<<"    -percent_change                Express in units of percent change. The default is raw MR units."<<std::endl;
    std::cout<<"    -xform_file:                   t4 file defining the transform to atlas space."<<std::endl;
    std::cout<<"    -atlas:                        Either 111, 222 or 333. Used with -xform_files option."<<std::endl;
    std::cout<<"    -region_file:                  *.4dfp.img file(s) that specifies regions of interest."<<std::endl;
    std::cout<<"                                   Magnitudes and time courses are averaged over the region."<<std::endl;


    fprintf(stderr,"    -regions_of_interest:          Calculate statistics for selected regions in the region file(s).\n");
    fprintf(stderr,"                                   First region is one.\n");
    fprintf(stderr,"    -regional_residual_name:       Output filename(s) for regional residuals.\n");
    fprintf(stderr,"    -print_frames:                 For a regional analysis, these frames will be printed\n");
    fprintf(stderr,"    -regional_avg_cond_across_runs For a regional analysis, a condition is averaged across runs.\n");
    fprintf(stderr,"    -start                         Start of condition. First frame of first BOLD is 1.\n"); 
    fprintf(stderr,"    -length                        Length of each condition listed after -start.\n"); 
    fprintf(stderr,"    -autocorrelation_name:         Output filename for sample autocorrelation.\n"); 
    fprintf(stderr,"    -gauss_smoth:                  Amount of smoothing in units of voxels with a 3D gaussian filter.\n"); 
    fprintf(stderr,"    -mask:                         Only voxels within the mask are analyzed.\n"); 
    fprintf(stderr,"    -frames_code:                  Identifier for each trial. Use with -tc to specify the estimates \n");
    fprintf(stderr,"                                   associated with that trial.\n");
    fprintf(stderr,"    -tc:                           Estimates to retain on a per trial basis as denoted by -frames_code\n");
    fprintf(stderr,"    -frames:                       Frames to be retained. First frame is 1.\n");
    fprintf(stderr,"                                   -frames_code and -tc specify the specific estimate to be retained.\n");

    //START190918
    //fprintf(stderr,"    -avi_seg                       Region file is an Avi segmented image. (1000=CSF 2000=gray 3000=white)\n");

    fprintf(stderr,"    -unformatted                   Text output is numbers only.\n");
    fprintf(stderr,"    -pca                           Principal components analysis.\n");
    fprintf(stderr,"    -voxels                        Output voxel values of regions. Only unique voxels are output.\n");
    fprintf(stderr,"    -voxels ALL                    Output voxel values of regions. All voxels are output.\n");

    //START190917
    //fprintf(stderr,"    -remove_global_covariate       Estimate and subtract global covariate.\n");

    fprintf(stderr,"    -global_covariate_mask:        Avi grey, white, CSF mask.\n");
    fprintf(stderr,"                                   If not specified, then mask specified with -mask is used.\n");
    fprintf(stderr,"    -identify                      Condition label. One for each bold run.\n");
    fprintf(stderr,"                                   Not used for computation, just carried over to the conc.\n");
    fprintf(stderr,"    -scratchdir:                   Output to this directory. Files will be compressed and not byte swapped.\n");
    fprintf(stderr,"    -names_only                    Generate filenames only.\n");
    fprintf(stderr,"    -names_ifh                     Generate ifh filenames. Used with -names_only\n");
    fprintf(stderr,"    -autocorr                      Compute autocorrelation. Files are scratch format.\n");
    fprintf(stderr,"    -crosscorr                     Compute crosscorrelation. Files are scratch format.\n");
    fprintf(stderr,"    -crosscov                      Compute crosscovariance. Files are scratch format.\n");
    fprintf(stderr,"    -cov                           Compute covariance. Files are scratch format.\n");
    fprintf(stderr,"    -var                           Compute variance for each run. Files are scratch format.\n");
    fprintf(stderr,"    -varall                        Compute variance across all runs.\n");
    fprintf(stderr,"    -unbiased                      Compute unbiased estimates of the variance-covariance (N-1).\n");
    fprintf(stderr,"    -maximumlikelihood             Compute maximum likelihood estimates of the variance-covariance (N).\n");
    fprintf(stderr,"    -float                         Output floats.\n");
    exit(-1);
    }
for(i=1;i<argc;i++) { 
    if(!strcmp(argv[i],"-glm_file") && argc > i+1)
        glm_file = argv[++i];
    if(!strcmp(argv[i],"-skip"))
        lc_skip = 1;
    if(!strcmp(argv[i],"-bold_files") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_bold_files;
        strptr = strrchr(argv[i+1],'.');
        if(!strcmp(strptr,".conc")) {
            if(!(bold_files=read_conc(num_bold_files,&argv[i+1]))) exit(-1);
            }
        else if(!strcmp(strptr,".img")){
            if(!(bold_files=get_files(num_bold_files,&argv[i+1]))) exit(-1);
            }
        else {
            printf("Error: -bold_files not concs or imgs. Abort!\n");fflush(stdout);
            exit(-1);
            }
        i+=num_bold_files;
        }
    if(!strcmp(argv[i],"-retain_estimates") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_retain_estimates;

        #if 0
        if(!(retain_estimates_index=malloc(sizeof*retain_estimates_index*num_retain_estimates))) {
            printf("Error: Unable to malloc retain_estimates_index\n");
            exit(-1);
            }
        #endif
        //START190917
        retain_estimates_index=new int[num_retain_estimates];

        for(j=0;j<num_retain_estimates;j++) retain_estimates_index[j] = atoi(argv[++i]) - 1;
        }
    if(!strcmp(argv[i],"-store_model_fit"))
        lc_resid = 0;
    if(!strcmp(argv[i],"-average_across_runs"))
        lc_average = 1;
    if(!strcmp(argv[i],"-onefile"))
        lc_one_file = 1;
    if(!strcmp(argv[i],"-root") && argc > i+1) {
        root = argv[++i];
        if((strptr=strstr(root,".4dfp.img"))) *strptr = 0;
        }
    if(!strcmp(argv[i],"-percent_change"))
        lc_pctchg = 1;
    if(!strcmp(argv[i],"-xform_file") && argc > i+1)
        xform_file = argv[++i];
    if(!strcmp(argv[i],"-region_file") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_region_files;
        if(!(region_files=get_files(num_region_files,&argv[i+1]))) exit(-1);
        i += num_region_files;
        }
    if(!strcmp(argv[i],"-regions_of_interest") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_regions;

        #if 0
        if(!(roi=malloc(sizeof*roi*num_regions))) {
            printf("Error: Unable to malloc roi\n");
            exit(-1);
            }
        for(j=0;j<num_regions;j++) roi[j] = atoi(argv[++i]) - 1;
        #endif
        //START190917
        roi=new int[num_regions];
        for(j=0;j<num_regions;j++) roi[j] = (int)strtol(argv[++i],NULL,10) - 1;

        }
    if(!strcmp(argv[i],"-regional_residual_name") && argc > i+1) {
        for(j=1;i+j < argc && strchr(argv[i+j],'-') != argv[i+j];j++) ++nregional_residual_name;
        if(!(regional_residual_name=get_files(nregional_residual_name,&argv[i+1]))) exit(-1);
        i += nregional_residual_name;
        }
    if(!strcmp(argv[i],"-print_frames") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_frames;

        #if 0
        if(!(frames_index=malloc(sizeof*frames_index*num_frames))) {
            printf("Error: Unable to malloc frames_index\n");
            exit(-1);
            }
        for(j=0;j<num_frames;j++) frames_index[j] = atoi(argv[++i]) - 1;
        #endif
        //START190917
        frames_index=new int[num_frames];
        for(j=0;j<num_frames;j++) frames_index[j] = (int)strtol(argv[++i],NULL,10) - 1;

        }
    if(!strcmp(argv[i],"-regional_avg_cond_across_runs"))
        lc_regional_avg_cond_across_runs = 1;
    if(!strcmp(argv[i],"-atlas") && argc > i+1)
        strcpy(atlas,argv[++i]);
    if(!strcmp(argv[i],"-start") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nstart;

        #if 0
        if(!(start_index=malloc(sizeof*start_index*nstart))) {
            printf("Error: Unable to malloc start_index\n");
            exit(-1);
            }
        for(j=0;j<nstart;j++) start_index[j] = atoi(argv[++i]) - 1;
        #endif
        //START190917
        start_index=new int[nstart];
        for(j=0;j<nstart;j++) start_index[j] = (int)strtol(argv[++i],NULL,10) - 1;

        }
    if(!strcmp(argv[i],"-length") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nlength;

        #if 0
        if(!(length=malloc(sizeof*length*nlength))) {
            printf("Error: Unable to malloc length\n");
            exit(-1);
            }
        for(j=0;j<nlength;j++) {
            length[j] = atoi(argv[++i]);
            if(length[j] > length_max) length_max = length[j];
            }
        #endif
        //START190917
        length=new int[nlength];
        for(j=0;j<nlength;j++) {
            length[j] = (int)strtol(argv[++i],NULL,10);
            if(length[j] > length_max) length_max = length[j];
            }

        }
    if(!strcmp(argv[i],"-autocorrelation_name") && argc > i+1)
        ac_name = argv[++i];
    if(!strcmp(argv[i],"-gauss_smoth") && argc > i+1)
        fwhm = atof(argv[++i]);
    if(!strcmp(argv[i],"-mask") && argc > i+1)
        mask_file = argv[++i];
    if(!strcmp(argv[i],"-frames_code") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nframes_code;

        #if 0
        if(!(frames_code=malloc(sizeof*frames_code*nframes_code))) {
            printf("Error: Unable to malloc frames_code\n");
            exit(-1);
            }
        for(j=0;j<nframes_code;j++) frames_code[j] = atoi(argv[++i]);
        #endif
        //START190917
        frames_code=new int[nframes_code];
        for(j=0;j<nframes_code;j++) frames_code[j] = (int)strtol(argv[++i],NULL,10);

        }
    if(!strcmp(argv[i],"-tc") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++ntc;
        argc_tc = i+1;
        i += ntc;
        }
    if(!strcmp(argv[i],"-frames") && argc > i+1)
        frames_file = argv[++i];

    //START190918
    #if 0
    if(!strcmp(argv[i],"-avi_seg"))
        avi_seg = 1;
    #endif

    if(!strcmp(argv[i],"-unformatted"))
        lc_unformatted = 1;
    if(!strcmp(argv[i],"-pca"))
        lc_pca = 1;
    if(!strcmp(argv[i],"-voxels")) {
        lc_voxels = 1;
        if(argc > i+1 && !strcmp(argv[i+1],"ALL")) {lc_voxelsall=1;++i;}
        }

    //START190917
    #if 0
    if(!strcmp(argv[i],"-remove_global_covariate"))
        lc_rgc = 1;
    if(!strcmp(argv[i],"-global_covariate_mask") && argc > i+1)
        gcmask = argv[++i];
    #endif

    if(!strcmp(argv[i],"-identify") && argc > i+1) {
        for(j=1;i+j < argc && strchr(argv[i+j],'-') != argv[i+j];j++) ++nidentify;
        if(!(identify=get_files(nidentify,&argv[i+1]))) exit(-1);
        i += nidentify;
        }
    if(!strcmp(argv[i],"-scratchdir") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        scratchdir = argv[++i];
    if(!strcmp(argv[i],"-names_only"))
        lc_names_only = 1;
    if(!strcmp(argv[i],"-names_ifh"))
        lc_names_ifh = 1;
    if(!strcmp(argv[i],"-autocorr"))
        lc_autocorr = 1;
    if(!strcmp(argv[i],"-crosscorr"))
        lc_crosscorr = 1;
    if(!strcmp(argv[i],"-crosscov"))
        lc_crosscov = 1;
    if(!strcmp(argv[i],"-cov"))
        lc_cov = 1;
    if(!strcmp(argv[i],"-var"))
        lc_var = 1;
    if(!strcmp(argv[i],"-varall"))
        lcvarall = 1;
    if(!strcmp(argv[i],"-unbiased"))
        maxlik_unbias = 1;
    if(!strcmp(argv[i],"-maximumlikelihood"))
        maxlik_unbias = 0;
    if(!strcmp(argv[i],"-float"))
        lcfloat = 1;
    }

#if 0
print_version_number(rcsid,lc_names_only?stderr:stdout);
if(lc_names_only) if(!scratchdir) if(!(scratchdir=make_scratchdir(0,"SCRATCH"))) exit(-1);
#endif
//START160116
if(lc_names_only){if(!scratchdir)if(!(scratchdir=make_scratchdir(0,"SCRATCH")))exit(-1);}
else{print_version_number(rcsid,lc_names_only?stderr:stdout);}

if(root) sprintf(rootname,"%s%s",scratchdir?scratchdir:"",root);
strptr = rootname;
if(lc_autocorr) {
    sprintf(autocorrroot,"%s_autocorr",rootname);
    strptr = autocorrroot;
    }
if(lc_crosscorr) {
    sprintf(crosscorrroot,"%s_crosscorr",rootname);
    strptr = crosscorrroot;
    }
if(lc_crosscov) {
    sprintf(crosscovroot,"%s_crosscov",rootname);
    strptr = crosscovroot;
    }
if(lc_cov) {
    sprintf(covroot,"%s_cov",rootname);
    strptr = covroot;
    }
if(lc_var||lcvarall) {
    sprintf(varroot,"%s_var",rootname);
    strptr = varroot;
    }
sprintf(concname,"%s.conc",strptr);
if(lc_names_only&&!lc_names_ifh) {
    printf("Concatenated file written to %s\n",concname);
    exit(0);
    }
if((SunOS_Linux=checkOS())==-1) exit(-1);

//START190917
#if 0
#ifndef MONTE_CARLO
    if(!lc_names_only) printf("lc_pca=%d lc_rgc=%d SunOS_Linux=%d lc_voxelsall=%d\n",lc_pca,lc_rgc,SunOS_Linux,lc_voxelsall);
#endif
#endif

if(nstart != nlength) {
    printf("You have %d numbers listed after -start and %d numbers listed after -length. Must be equal. Abort!\n",nstart,
        nlength);
    exit(-1);
    }
if(!glm_file) {
    printf("Error: No glm file. Abort!\n");
    exit(-1);
    }
if(!num_bold_files) {
    printf("Error: No bold files. Abort!\n");
    exit(-1);
    }
if(nidentify) {
    if(nidentify!=(int)bold_files->nfiles) {
        printf("fidlError: fidl_residuals nidentify=%d bold_files->nfiles=%lu  Must be equal. Abort!\n",nidentify,
            (unsigned long)bold_files->nfiles);
        fflush(stdout);exit(-1);
        }
    }
fprintf(lc_names_only?stderr:stdout,"Reading %s\n",glm_file);
if(!(glm=read_glm(glm_file,(int)FALSE,SunOS_Linux))) {

    //printf("Error: Unable to read %s  Abort!\n",glm_file); 
    //START190918
    std::cout<<"fidlError: Unable to read "<<glm_file<<"  Abort!"<<std::endl;

    exit(-1);
    }

#if 0
if(mask_file){
    if(glm->ifh->mask)free(glm->ifh->mask);
    if(!(glm->ifh->mask=malloc(sizeof*glm->ifh->mask*(strlen(mask_file)+1)))){
        printf("fidlError: Unable to malloc glm->ifh->mask\n");
        exit(-1);
        }
    strcpy(glm->ifh->mask,mask_file);
    }
#endif
//START190918
if(mask_file){
    if(glm->ifh->mask)if(strcmp(glm->ifh->mask,mask_file))free(glm->ifh->mask);
    if(!glm->ifh->mask){
        if(!(glm->ifh->mask=(char*)malloc(sizeof*glm->ifh->mask*(strlen(mask_file)+1)))){
            std::cout<<"fidlError: Unable to malloc glm->ifh->mask"<<std::endl;
            exit(-1);
            }
        strcpy(glm->ifh->mask,mask_file);
        }
    }


swapbytes = shouldiswap(SunOS_Linux,glm->ifh->bigendian);
vol = glm->ifh->glm_xdim*glm->ifh->glm_ydim*glm->ifh->glm_zdim;
get_atlas(vol,glm_atlas);
if(atlas[0])strcpy(atlas,glm_atlas);
start_b = find_b(glm);
if((int)bold_files->nfiles > glm->ifh->glm_num_files) {
    printf("bold_files->nfiles=%lu  glm->ifh->glm_num_files=%d\n",(unsigned long)bold_files->nfiles,glm->ifh->glm_num_files);
    printf("fidlError: fidl_residuals num_bold_files must be less than or equal to glm->ifh->num_files ABORT!\n");
    fflush(stdout);exit(-1);
    }

//if(!(dp=dim_param2(bold_files->nfiles,bold_files->files,SunOS_Linux)))exit(-1);
//START190917
if(!(dp=dim_param2(bold_files->nfiles,bold_files->files)))exit(-1);

std::cout<<"dp->centerf[0][0]="<<dp->centerf[0][0]<<" dp->centerf[0][1]="<<dp->centerf[0][1]<<" dp->centerf[0][2]="<<dp->centerf[0][2]<<std::endl;
std::cout<<"dp->mmppixf[0][0]="<<dp->mmppixf[0][0]<<" dp->mmppixf[0][1]="<<dp->mmppixf[0][1]<<" dp->mmppixf[0][2]="<<dp->mmppixf[0][2]<<std::endl;

if(!dp->volall==-1){printf("fidlError: fidl_residuals All files must have the same volume.\n");fflush(stdout);exit(-1);}

//if(!(ap=get_atlas_param(atlas,atlas[0]?NULL:glm->ifh))) exit(-1);
//START190918
if(!(ap=get_atlas_param(atlas,atlas[0]?NULL:glm->ifh,(char*)NULL))) exit(-1);

//if(!(ms=get_mask_struct(mask_file,ap->vol,(int*)NULL,SunOS_Linux,(LinearModel*)NULL,ap->vol))) exit(-1);
//START190917
if(!(ms0.get_mask(mask_file,ap->vol,(int*)NULL,(LinearModel*)NULL,ap->vol)))exit(-1);
int ms_cf_flip=ms0.get_cf_flip();
if(!lc_names_only)printf("glm->ifh->cf_flip=%d dp->cf_flipall=%d ms_cf_flip=%d\n",glm->ifh->cf_flip,dp->cf_flipall,ms_cf_flip); 
if(glm->ifh->cf_flip!=dp->cf_flipall||dp->cf_flipall!=ms_cf_flip){
    printf("fidlError: glm->ifh->cf_flip=%d dp->cf_flipall=%d ms_cf_flip=%d Must be equal. Code must added to handle different orientations.\n",
        glm->ifh->cf_flip,dp->cf_flipall,ms_cf_flip);
    exit(-1);
    }
ms=new Mask_Struct[1];
//ms->brnidx=ms0.get_brnidx(ms->lenbrain);
//START190918
ms->brnidx=ms0.get_brnidx(ms->lenbrain,ms->vol);

//START190919
ms->nuns=0;

#if 1
if(!lc_names_only)printf("vol=%d dp->volall=%d ms->lenbrain=%d glm->ifh->nregions=%d ap->vol=%d atlas=%s\n",vol,dp->volall,
    ms->lenbrain,glm->ifh->nregions,ap->vol,atlas);
if(!glm->ifh->nregions){
    if(dp->volall!=vol&&dp->volall!=ms->lenbrain){
#endif
#if 0
//START190917
if(!lc_names_only)printf("vol=%d dp->volall=%d mslenbrain=%d glm->ifh->nregions=%d ap->vol=%d atlas=%s\n",vol,dp->volall,mslenbrain,glm->ifh->nregions,ap->vol,atlas);
if(!glm->ifh->nregions){
    if(dp->volall!=vol&&dp->volall!=mslenbrain){
#endif

        if(!xform_file){
            printf("fidlError: fidl_residuals %s has a vol of %d. The bolds have a vol of %d. Need a t4. Abort!\n",glm_file,vol,
               dp->volall);
            fflush(stdout);exit(-1);
            }
        lc_t4_before=1;
        }
    }
else if(glm->ifh->nregions!=num_regions) {
    printf("fidlError: fidl_residuals glm->ifh->nregions=%d num_regions=%d  Must be equal.\n",glm->ifh->nregions,num_regions);
    fflush(stdout);exit(-1);
    }
if(lc_average || lc_regional_avg_cond_across_runs) {
    if(!dp->tdimall==-1){printf("fidlError: fidl_residuals All files must have the same time dimension.\n");
    fflush(stdout);exit(-1);}
    }
if(xform_file) {

    #if 0
    if(!(t4=malloc(sizeof*t4*(size_t)T4SIZE))) {
        printf("fidlError: Unable to malloc t4\n");
        exit(-1);
        }
    #endif
    //START190918
    t4=new float[T4SIZE];

    if(!read_xform(xform_file,t4)) exit(-1);
    if((A_or_B_or_U=twoA_or_twoB(xform_file)) == 2) exit(-1);
    }
if(!(tcs=read_tc_string_new(ntc,(int*)NULL,argc_tc,argv))) exit(-1);
#if 0
    printf("ntc=%d\n",ntc);
    printf("tcs->num_tc=%d\n",tcs->num_tc);
    printf("tcs->num_tc_to_sum="); for(i=0;i<tcs->num_tc;i++) printf("%d ",tcs->num_tc_to_sum[i][0]); printf("\n");
    printf("tcs->each="); for(i=0;i<tcs->num_tc;i++) printf("%d ",tcs->each[i]); printf("\n");
    printf("tcs->tc=\n");
    for(i=0;i<tcs->num_tc;i++) {
        for(k=0;k<tcs->each[i];k++) {
            for(j=0;j<tcs->num_tc_to_sum[i][k];j++) printf("%d ",(int)tcs->tc[i][k][j]);
            printf("    ");
            }
        printf("\n");
        }
#endif
if(frames_file) {
    if(!(fs=read_frames_file(frames_file,0,0,0))) exit(-1);
    if(nframes_code!=fs->nlines) {
        printf("Error: nframes_code=%d fs->nframes=%d  Must be equal. Abort!\n",nframes_code,fs->nframes);
        exit(-1);
        }
    }

#if 0
if(!(temp_float=malloc(sizeof*temp_float*(!glm->ifh->nregions?ap->vol:dp->volall)))) {
    printf("fidlError: fidl_residuals Unable to malloc temp_float\n");
    exit(-1);
    }
if(dp->filetypeall==(int)CIFTI||dp->filetypeall==(int)CIFTI_DTSERIES){
    if(!(temp_float2=malloc(sizeof*temp_float2*dp->lenvol_max))) {
        printf("fidlError: fidl_residuals Unable to malloc temp_float2\n");
        exit(-1);
        }
    }
#endif
//START190918
temp_float=new float[!glm->ifh->nregions?ap->vol:dp->volall];
if(dp->filetypeall==(int)CIFTI||dp->filetypeall==(int)CIFTI_DTSERIES)temp_float2=new float[dp->lenvol_max];


ntemp_double = ap->vol>(num_regions)?ap->vol:(num_regions);
if(!lc_names_only)printf("atlas=%s ap->xdim=%d ap->ydim=%d ap->zdim=%d glm_atlas=%s ntemp_double=%d\n",atlas,ap->xdim,ap->ydim,
    ap->zdim,glm_atlas,ntemp_double);

#if 0
if(!(temp_double=malloc(sizeof*temp_double*ntemp_double))) {
    printf("fidlError: fidl_residuals Unable to malloc temp_double\n");
    exit(-1);
    }
if(!(stat=malloc(sizeof*stat*ntemp_double))) {
    printf("fidlError: fidl_residuals Unable to malloc stat\n");
    exit(-1);
    }
if(!(td1=malloc(sizeof*td1*ntemp_double))) {
    printf("fidlError: fidl_residuals Unable to malloc td1\n");
    exit(-1);
    }
if(!(td2=malloc(sizeof*td2*ntemp_double))) {
    printf("fidlError: fidl_residuals Unable to malloc td2\n");
    exit(-1);
    }
if(!(bold=malloc(sizeof*bold*(!glm->ifh->nregions?ap->vol:dp->volall)))) {
    printf("fidlError: fidl_residuals Unable to malloc bold\n");
    exit(-1);
    }
if(!(tdim_usable=malloc(sizeof*tdim_usable*bold_files->nfiles))) {
    printf("fidlError: fidl_residuals Unable to malloc tdim_usable\n");
    exit(-1);
    }
for(i=0;i<(int)bold_files->nfiles;i++) tdim_usable[i] = 0;
if(!(num_usable_per_frame=malloc(sizeof*num_usable_per_frame*dp->tdim_max))) {
    printf("fidlError: fidl_residuals Unable to malloc num_usable_per_frame\n");
    exit(-1);
    }
for(i=0;i<dp->tdim_max;i++) num_usable_per_frame[i] = 0;
#endif
//START190918
temp_double=new double[ntemp_double];
stat=new double[ntemp_double];
td1=new double[ntemp_double];
td2=new double[ntemp_double];
bold=new double[!glm->ifh->nregions?ap->vol:dp->volall];
tdim_usable=new int[bold_files->nfiles];
for(i1=0;i1<bold_files->nfiles;++i1) tdim_usable[i1] = 0;
num_usable_per_frame=new int[dp->tdim_max];
for(i=0;i<dp->tdim_max;++i) num_usable_per_frame[i] = 0;


if(lc_resid) {

    #if 0
    if(!(temp_int=malloc(sizeof*temp_int*glm->ifh->glm_Mcol))) {
        printf("fidlError: fidl_residuals Unable to malloc temp_int\n");
        exit(-1);
        }
    #endif
    temp_int=new int[glm->ifh->glm_Mcol];

    for(i=0;i<glm->ifh->glm_Mcol;i++) temp_int[i] = 0;
    for(i=0;i<num_retain_estimates;i++) temp_int[retain_estimates_index[i]] = 1;
    nest = glm->ifh->glm_Mcol - num_retain_estimates;
    if(nest) {

        #if 0
        if(!(est=malloc(sizeof*est*nest))) {
            printf("Error: Unable to malloc est\n");
            exit(-1);
            }
        #endif
        //START190918
        est=new int[nest]; 

        for(j=i=0;i<glm->ifh->glm_Mcol;i++) if(!temp_int[i]) est[j++] = i;
        if(j!=nest) {

            //printf("Error: j=%d nest=%d Must be the same. Abort!\n",j,nest);
            //START190918
            std::cout<<"fidlError: j="<<j<<" nest="<<nest<<" Must be the same. Abort!"<<std::endl;

            exit(-1);
            }
        }

    //free(temp_int);
    //START190918
    delete[] temp_int;

    /*printf("nest=%d\n",nest); printf("est="); for(i=0;i<nest;i++) printf("%d ",est[i]); printf("\n");*/
    }
else {
    if(num_retain_estimates) {
        nest = num_retain_estimates;
        est = retain_estimates_index;
        }
    else {
        nest = glm->ifh->glm_Mcol;

        #if 0
        if(!(est=malloc(sizeof*est*nest))) {
            printf("Error: Unable to malloc est\n");
            exit(-1);
            }
        for(i=0;i<nest;i++) est[i] = i;
        #endif
        //START190918
        est=new int[nest]; 
        for(i=0;i<nest;++i) est[i] = i;

        }
    }

if(num_region_files) {
    if(!check_dimensions(num_region_files,region_files->files,!glm->ifh->nregions?ap->vol:dp->volall))exit(-1);
    if(!(reg=(Regions**)malloc(sizeof*reg*(num_region_files)))) {
        printf("fidlError: fidl_residuals Unable to malloc reg\n");
        exit(-1);
        }
    for(m=0;m<num_region_files;m++){

        #if 0
        if(avi_seg)if(!readstack(region_files->files[m],(float*)temp_float,sizeof(float),
            (size_t)(!glm->ifh->nregions?ap->vol:dp->vol[i]),SunOS_Linux,-1))exit(-1);
        if(!(reg[m]=extract_regions(region_files->files[m],avi_seg,avi_seg?(!glm->ifh->nregions?ap->vol:dp->vol[i]):0,
            avi_seg?temp_float:(float*)NULL,0,SunOS_Linux,(char**)NULL))) exit(-1);
        #endif
        //START190918
        int lcflip=shouldiflip(region_files); 
        //if(!(reg[m]=get_reg(region_files->files[m],0,(float*)NULL,0,(char**)NULL,lcflip,0)))exit(-1); 
        //START200929
        if(!(reg[m]=get_reg(region_files->files[m],0,(float*)NULL,0,(char**)NULL,lcflip,0,0,(int*)NULL,(char*)NULL)))exit(-1); 

        }
    if(!num_regions) for(m=0;m<num_region_files;m++) num_regions += reg[m]->nregions;

    //if(!(rbf=find_regions_by_file_cover(num_region_files,num_regions,reg,roi))) exit(-1);
    //START211020
    if(!(rbf=find_regions_by_file_cover(num_region_files,num_regions,reg,roi,NULL)))exit(-1);

    if(!lc_names_only) {printf("rbf->nvoxels=%d rbf->nindices_uniqsort=%d\n",rbf->nvoxels,rbf->nindices_uniqsort);fflush(stdout);}
    if(!lc_voxels) {
        if(!glm->ifh->nregions){
            if(!(region_tc=d3double(num_regions,bold_files->nfiles,dp->tdim_max)))exit(-1);

            #if 0
            if(!(td3=malloc(sizeof*td3*num_regions))){printf("fidlError: fidl_residuals Unable to malloc td3\n");exit(-1);}
            if(!lc_resid){
                if(!(td4=malloc(sizeof*td4*num_regions))){printf("fidlError: fidl_residuals Unable to malloc td4\n");exit(-1);}
                }
            #endif
            //START190918
            td3=new double[num_regions]; 
            if(!lc_resid)td4=new double[num_regions];

            }
        if(!lc_resid) {
            if(!(region_tc_bold=d3float(num_regions,bold_files->nfiles,dp->tdim_max))) exit(-1);

            #if 0
            if(!(y=malloc(sizeof*y*num_regions))) {
                printf("Error: Unable to malloc y\n");
                exit(-1);
                }
            if(!(SSyy=malloc(sizeof*SSyy*num_regions))) {
                printf("Error: Unable to malloc SSyy\n");
                exit(-1);
                }
            if(!(SSE=malloc(sizeof*SSE*num_regions))) {
                printf("Error: Unable to malloc SSE\n");
                exit(-1);
                }
            if(!(R2=malloc(sizeof*R2*num_regions))) {
                printf("Error: Unable to malloc R2\n");
                exit(-1);
                }
            if(!(Ra2=malloc(sizeof*Ra2*num_regions))) {
                printf("Error: Unable to malloc Ra2\n");
                exit(-1);
                }
            #endif
            //START190918
            y=new double[num_regions];            
            SSyy=new double[num_regions];
            SSE=new double[num_regions];
            R2=new double[num_regions];
            Ra2=new double[num_regions];

            }
        }
    else {

        #if 0
        if(!(voxel_tc=malloc(sizeof*voxel_tc*glm->ifh->glm_tdim*rbf->nvoxels))) {
            printf("Error: Unable to malloc voxel_tc\n");
            exit(-1);
            }
        #endif
        //START190918
        voxel_tc=new double[glm->ifh->glm_tdim*rbf->nvoxels]; 
       
        }
    }


#if 0
if(xform_file) {
    if(!(ms_bold=get_mask_struct((char*)NULL,dp->volall,(int*)NULL,SunOS_Linux,(LinearModel*)NULL,ap->vol))) exit(-1);
    ms_glm = ms_bold;
    }
else {
    ms_bold = ms_glm = ms;
    }
if(glm_atlas[0]){
    if(!rbf) {
        if(!glm->ifh->glm_masked){
            if(!(ms_glm=get_mask_struct(glm_file,ap->vol,(int*)NULL,SunOS_Linux,glm,ap->vol)))exit(-1);
            }
        else{
            if(!(ms_glm=get_mask_struct((char*)NULL,glm->nmaski,glm->maski,SunOS_Linux,(LinearModel*)NULL,ap->vol)))exit(-1);
            }
        }
    else {
        if(!(ms_glm=get_mask_struct((char*)NULL,rbf->nindices_uniqsort,rbf->indices_uniqsort,SunOS_Linux,(LinearModel*)NULL,ap->vol))) 
            exit(-1);
        }
    }
#endif
//START190917
if(xform_file) {
    if(!(ms0_bold.get_mask((char*)NULL,dp->volall,(int*)NULL,(LinearModel*)NULL,ap->vol)))exit(-1);
    ms_bold=new Mask_Struct[1];
    ms_bold->brnidx=ms0_bold.get_brnidx(ms_bold->lenbrain,ms_bold->vol);
    ms_glm = ms_bold;
    }
else {
    ms_bold = ms_glm = ms;
    }
if(glm_atlas[0]){
    if(!rbf) {
        if(!glm->ifh->glm_masked){
            if(!(ms0_glm.get_mask(glm_file,ap->vol,(int*)NULL,glm,ap->vol)))exit(-1);

            }
        else{
            if(!(ms0_glm.get_mask((char*)NULL,glm->nmaski,glm->maski,(LinearModel*)NULL,ap->vol)))exit(-1);
            }
        }
    else {
        if(!(ms0_glm.get_mask((char*)NULL,rbf->nindices_uniqsort,rbf->indices_uniqsort,(LinearModel*)NULL,ap->vol)))exit(-1);
        }
    ms_glm=new Mask_Struct[1];
    ms_glm->brnidx=ms0_glm.get_brnidx(ms_glm->lenbrain,ms_glm->vol);
    ms_glm->unsi=ms0_glm.get_unsi(ms_glm->nuns);

    //std::cout<<"here0 ms_glm->nuns="<<ms_glm->nuns<<std::endl;
    }

//STARTHERE

if((xform_file&&lc_t4_before)||fwhm>0.){
    nbold_index=dp->volall;

    #if 0
    if(!(bold_index=malloc(sizeof*bold_index*nbold_index))) {
        printf("Error: Unable to malloc bold_index\n");
        exit(-1);
        }
    for(i=0;i<nbold_index;i++) bold_index[i]=i;
    #endif
    //START190918
    bold_index=new int[nbold_index]; 
    for(i=0;i<nbold_index;++i) bold_index[i]=i;

    nbold_index1=nbold_index;
    bold_index1=bold_index;

    //START161007
    #if 0
    if(fwhm>0.) {
        nglm_index = nbold_index;
        glm_index = bold_index;
        }
    else {
        nglm_index = ms_glm->lenbrain;
        glm_index = ms_glm->brnidx;
        }
    #endif

    }
else if(!glm->ifh->nregions) {
    if(dp->volall!=ms->lenbrain){
        nbold_index1=nbold_index=ms_bold->lenbrain;
        bold_index1=bold_index=ms_bold->brnidx;
        }
     else{
        nbold_index=dp->volall;

        #if 0
        if(!(bold_index=malloc(sizeof*bold_index*nbold_index))){
            printf("fidlError: fidl_residuals Unable to malloc bold_index\n");
            exit(-1);
            }
        for(i=0;i<nbold_index;i++)bold_index[i]=i;
        #endif
        //START190918
        bold_index=new int[nbold_index]; 
        for(i=0;i<nbold_index;++i)bold_index[i]=i;
        
        nbold_index1=ms_bold->lenbrain;
        bold_index1=ms_bold->brnidx;
        }
    }
else {
    nbold_index1=nbold_index = rbf->nindices_uniqsort;
    bold_index1=bold_index = rbf->indices_uniqsort;
    }
if(lc_voxels) {
    if(!lc_voxelsall){niout=ms_glm->lenbrain;iout=ms_glm->brnidx;}
    else {niout=rbf->nvoxels;iout=rbf->indices;}
    }

//START190917
#if 0
if(lc_rgc) {
    if(!(storage=malloc(sizeof*storage*ms->lenbrain*dp->tdim_max))) {
        printf("Error: Unable to malloc storage in fidl_residuals\n");
        exit(-1);
        }
    if(!(gc=malloc(sizeof*gc*dp->tdim_max))) {
        printf("Error: Unable to malloc gc in fidl_residuals\n");
        exit(-1);
        }
    if(!(gcstr=malloc(sizeof*gcstr*4))) {
        printf("Error: Unable to malloc gcstr in fidl_residuals\n");
        exit(-1);
        }
    strcpy(gcstr,"_gc");
    if(!gcmask) {
        ms_gc = ms;
        }
    else {
        printf("global covariate mask = %s\n",gcmask);
        if(!check_dimensions(1,&gcmask,dp->volall)) exit(-1);
        if(!(reg_gc=extract_regions(gcmask,1,0,(float*)NULL,0,SunOS_Linux,(char**)NULL))) exit(-1);
        i=1;
        if(!(rbf_gc=find_regions_by_file_cover(1,1,&reg_gc,&i))) exit(-1);
        printf("rbf_gc->nvoxels_region[0]=%d\n",rbf_gc->nvoxels_region[0]);
        if(!(ms_gc=get_mask_struct((char*)NULL,rbf_gc->nvoxels,rbf_gc->indices,SunOS_Linux,(LinearModel*)NULL,ap->vol))) exit(-1);
        }
    }
#endif

#if 0
if(!glm->ifh->glm_masked)if(!(index=malloc(sizeof*index*ms_glm->lenbrain))){
    printf("fidlError: Unable to malloc index\n");
    exit(-1);
    }
#endif
//START190918
if(!glm->ifh->glm_masked)index=new int[ms_glm->lenbrain];

if(glm->ifh->nregions) for(nindex=num_regions,k=0;k<num_regions;k++) index[k]=k;
if(lc_average&&!num_region_files) {

    #if 0
    if(!(avg=malloc(sizeof*avg*dp->tdim_max*ms->lenbrain))) {
        printf("dp->tdim_max=%d ms->lenbrain=%d\nError: Unable to malloc avg\n",dp->tdim_max,ms->lenbrain);
        exit(-1);
        }
    for(i=0;i<dp->tdim_max*ms->lenbrain;i++) avg[i] = 0.;
    #endif
    //START190918
    avg=new double[dp->tdim_max*ms->lenbrain];  
    for(i=0;i<dp->tdim_max*ms->lenbrain;++i)avg[i]=0.;
    
    }
if(!lc_names_only) {
    printf("ms->lenbrain=%d ms_glm->lenbrain=%d ms_bold->lenbrain=%d\n",ms->lenbrain,ms_glm->lenbrain,ms_bold->lenbrain);
    printf("ap->vol=%d dp->volall=%d nbold_index=%d nbold_index1=%d vol=%d \n",ap->vol,dp->volall,nbold_index,nbold_index1,vol);
    }
if(ac_name) {
    num_regions = ms->lenbrain;
    if(!(ac_numer = d2double(bold_files->nfiles,num_regions))) exit(-1);
    if(!(ac_denom = d2double(bold_files->nfiles,num_regions))) exit(-1);

    #if 0
    if(!(pairs_stack=malloc(sizeof*pairs_stack*num_regions))) {
        printf("Error: Unable to malloc pairs_stack\n");
        exit(-1);
        }
    #endif
    //START1900918
    pairs_stack=new double[num_regions]; 

    }

#if 0
if(!(valid_frames=malloc(sizeof*valid_frames*glm->ifh->glm_tdim))) {
    printf("Error: Unable to malloc valid_frames\n");
    exit(-1);
    }
for(i=0;i<glm->ifh->glm_tdim;i++) valid_frames[i] = 0;
#endif
//START190918
valid_frames=new int[glm->ifh->glm_tdim];
for(i=0;i<glm->ifh->glm_tdim;++i)valid_frames[i]=0;

if(nstart) {
    /*printf("glm->ifh->glm_tdim=%d length_max=%d\n",glm->ifh->glm_tdim,length_max);
    printf("start_index="); for(i=0;i<nstart;i++) printf("%d ",start_index[i]); printf("\n");
    printf("length="); for(i=0;i<nstart;i++) printf("%d ",length[i]); printf("\n");*/

    #if 0
    if(!(den=malloc(sizeof*den*length_max))) {
        printf("Error: Unable to malloc den\n");
        exit(-1);
        }
    for(i=0;i<length_max;i++) den[i] = 0;
    #endif
    //START190918
    den=new int[length_max];
    for(i=0;i<length_max;++i)den[i]=0.;

    for(i=0;i<nstart;i++) {
        for(j=0;j<length[i];j++) {
            valid_frames[start_index[i]+j] = 1;
            den[j]++;
            }
        }
    }
else if(num_frames) {
    for(i=0;i<num_frames;i++) valid_frames[frames_index[i]] = 1;
    }
else if(lc_skip) {
    for(i=0;i<glm->ifh->glm_tdim;i++) valid_frames[i] = (int)glm->valid_frms[i];
    }
else {
    for(i=0;i<glm->ifh->glm_tdim;i++) valid_frames[i] = 1;
    }

#if 0
if(!(validframeindex=malloc(sizeof*validframeindex*glm->ifh->glm_tdim))) {
    printf("Error: Unable to malloc validframeindex\n");
    exit(-1);
    }
#endif
//START190918
validframeindex=new int[glm->ifh->glm_tdim];


//printf("valid_frames=(first frame is 1)\n");for(i=0;i<glm->ifh->glm_tdim;i++) if(valid_frames[i])printf("%d ",i+1); printf("\n");

std::cout<<"lc_pctchg="<<lc_pctchg<<" lc_resid="<<lc_resid<<std::endl;

if(lc_pctchg)if(!(gms=get_grand_mean(glm,glm_file,(_IO_FILE*)(lc_names_only?stderr:stdout))))exit(-1);

//printf("gms->grand_mean\n");for(i=0;i<glm->nmaski;i++)printf("%f ",gms->grand_mean[i]);printf("\n");

if(lc_autocorr||lc_crosscorr||lc_crosscov) {
    corr=new double[dp->tdim_max]; 
    }
how_many = k1 = !num_regions ? ms->lenbrain : num_regions;
if(lc_cov) {
    cov=new double[how_many*how_many]; 
    }
if(lc_var) {
    var=new double[how_many]; 
    }
if(lcvarall) {
    sum=new double[how_many];
    sum2=new double[how_many];
    for(i=0;i<how_many;++i)sum[i]=sum2[i]=0.;
    }
if(!lc_names_only){if(fwhm>0.) if(!(gs=gauss_smoth2_init(ap->xdim,ap->ydim,ap->zdim,fwhm,fwhm))) exit(-1);}


#if 0
if(!(w1=write1_init()))exit(-1);
w1->cifti_xmldata=glm->cifti_xmldata;
w1->xmlsize=glm->ifh->glm_cifti_xmlsize;
//for(i=0;i<w1->xmlsize;i++)printf("%c",w1->cifti_xmldata[i]);printf("\nEND OF XML\n");
//STARTHERE NEED TO PULL XML FROM EACH BOLD INDIVIDUALLY. LOOK IN linmod5.c FOR HOW TO DO THIS.
w1->filetype=dp->filetypeall;
w1->swapbytes=0;
if(dp->filetypeall==(int)NIFTI||dp->filetypeall==(int)CIFTI||dp->filetypeall==(int)CIFTI_DTSERIES){
    dims[0]=glm->ifh->glm_xdim;dims[1]=glm->ifh->glm_ydim;dims[2]=glm->ifh->glm_zdim;
    w1->dims=dims;
    w1->center=glm->ifh->center;
    w1->mmppix=glm->ifh->mmppix;
    }
w1->temp_double=NULL;
w1->lenbrain=0;
#endif
//START190920
if(!(w1=write1_init()))exit(-1);
w1->cifti_xmldata=glm->cifti_xmldata;
w1->xmlsize=glm->ifh->glm_cifti_xmlsize;
//for(i=0;i<w1->xmlsize;i++)printf("%c",w1->cifti_xmldata[i]);printf("\nEND OF XML\n");
//STARTHERE NEED TO PULL XML FROM EACH BOLD INDIVIDUALLY. LOOK IN linmod5.c FOR HOW TO DO THIS.
w1->filetype=dp->filetypeall;
if(dp->filetypeall==(int)NIFTI||dp->filetypeall==(int)CIFTI||dp->filetypeall==(int)CIFTI_DTSERIES){
    dims[0]=glm->ifh->glm_xdim;dims[1]=glm->ifh->glm_ydim;dims[2]=glm->ifh->glm_zdim;
    w1->dims=dims;
    w1->center=glm->ifh->center;
    w1->mmppix=glm->ifh->mmppix;
    }


std::cout<<"glm->ifh->glm_xdim="<<glm->ifh->glm_xdim<<" glm->ifh->glm_ydim="<<glm->ifh->glm_ydim<<" glm->ifh->glm_zdim="<<glm->ifh->glm_zdim<<std::endl;
std::cout<<"glm->ifh->center[0]="<<glm->ifh->center[0]<<" glm->ifh->center[1]="<<glm->ifh->center[1]<<" glm->ifh->center[2]="<<glm->ifh->center[2]<<std::endl;
std::cout<<"glm->ifh->mmppix[0]="<<glm->ifh->mmppix[0]<<" glm->ifh->mmppix[1]="<<glm->ifh->mmppix[1]<<" glm->ifh->mmppix[2]="<<glm->ifh->mmppix[2]<<std::endl;


if(!(fp=fopen_sub(glm_file,"r"))) exit(-1);
if(!lc_average && lc_one_file && (!num_regions||scratchdir)) {
    sprintf(filename,"%s%s%s.4dfp.img",scratchdir?scratchdir:"",root,gcstr?gcstr:"");
    if(!lc_names_only) if(!(op=fopen_sub(filename,"w"))) exit(-1);
    }
if(!(cp=fopen_sub(concname,"w"))) exit(-1);
for(t=0,nvfi=o=ii=kk=lll=l=i=0;i<(int)bold_files->nfiles;i++) {


    //std::cout<<"here50"<<std::endl;

    if(!lc_average&&!lc_one_file&&((!num_regions&&!lcvarall)||scratchdir||(glm->ifh->nregions&&!scratchdir))) {
        sprintf(filename,"%s%s_b%d%s",scratchdir?scratchdir:"",root,i+1,Fileext[dp->filetypeall]);
        if(!lc_names_only){

            //START190919
            if(dp->filetypeall==(int)NIFTI)w1->dims[3]=dp->tdim[i]; 

            std::cout<<"dims[0]="<<dims[0]<<" dims[1]="<<dims[1]<<" dims[2]="<<dims[2]<<" dims[3]="<<dims[3]<<std::endl;

//w1->tdim=(int64_t)dp->tdim[i];

            //if(dp->filetypeall==(int)IMG||dp->filetypeall==(int)NIFTI){if(!(op=open2(filename,w1)))exit(-1);}
            //START190918
            if(dp->filetypeall==(int)IMG||dp->filetypeall==(int)NIFTI){if(!(op=(FILE*)open2(filename,w1)))exit(-1);}

            }
        }

    //std::cout<<"here51"<<std::endl;

    if(!lc_names_only) {
        printf("Processing %s\n",bold_files->files[i]);
        if(dp->filetypeall==(int)IMG){if(!(mm=map_disk(bold_files->files[i],dp->tdim[i]*dp->vol[i],0,sizeof(float))))exit(-1);}
        else if(dp->filetypeall==(int)CIFTI||dp->filetypeall==(int)CIFTI_DTSERIES){if(!cifti_getstack(bold_files->files[i],temp_float2))exit(-1);}
        else if(!(niftiPtr=nifti_openRead(bold_files->files[i])))exit(-1);
        }

    //std::cout<<"here52"<<std::endl;


    for(kkk=pairs=j=0;j<dp->tdim[i];j++,l++) {

        //std::cout<<"here53"<<std::endl;

        /*printf("valid_frames[%d]=%d\n",l,valid_frames[l]);*/
        t0=j*dp->vol[i];
        if(!valid_frames[l]) {
            if(ac_name && pairs) pairs = -1;
            if(dp->filetypeall==(int)CIFTI||dp->filetypeall==(int)CIFTI_DTSERIES){
                for(k=0;k<ap->vol;k++)temp_float2[t0+k]=0.;
                }
            }
        else {


            if(!lc_names_only) {
                if(ac_name && (pairs==-1)) {
                    std::cout<<"fidlError: Autocorrelation can only be calculated for runs with contiguous valid frames. Abort!"<<std::endl;
                    exit(-1);
                    }
                for(k=0;k<ntemp_double;k++)temp_double[k]=0.;
                do_more = 0;
                if(glm->valid_frms[l]>0){
                    for(incrementlll=k=0;k<nest;k++){

                        /* Doesn't cycle through all estimates, only ones you want to remove.
                           So retain list should not include estimates to be kept by trial. */
                        if(glm->AT[est[k]][t]) {
                            /*printf("k=%d glm->AT[%d][%d]=%f\n",k,est[k],t,glm->AT[est[k]][t]);*/
                            if(frames_file) {
                                /*printf("l=%d fs->frames[%d]=%d est[%d]=%d tcs->tc[%d][%d][0]-1=%d ii=%d\n",
                                    l,lll,fs->frames[lll],k,est[k],frames_code[ii],kk,(int)tcs->tc[frames_code[ii]][kk][0]-1,ii);*/
                                if(l==fs->frames[lll] && est[k]==((int)tcs->tc[frames_code[ii]][kk][0]-1)) {
                                    incrementlll=1;
                                    continue;
                                    }
                                }
                            if(fseek(fp,(long)(start_b+sizeof(float)*glm->nmaski*est[k]),SEEK_SET)){
                                std::cout<<"fidlError: fidl_residuals occured while seeking to "<<fp<<" in "<<glm_file<<std::endl;
                                exit(-1);
                                }
                            if(!fread_sub(temp_float,sizeof(float),(size_t)glm->nmaski,fp,swapbytes,glm_file)){
                                std::cout<<"fidlError: fidl_residuals reading parameter estimates from "<<glm_file<<std::endl;
                                exit(-1);
                                }
                            if(!glm->ifh->glm_masked)
                                for(m=0;m<ms_glm->lenbrain;m++) 
                                    temp_double[ms_glm->brnidx[m]]+=(double)glm->AT[est[k]][t]*(double)temp_float[ms_glm->brnidx[m]];
                            else
                                for(m=0;m<glm->nmaski;m++) 
                                    temp_double[glm->maski[m]]+=(double)glm->AT[est[k]][t]*(double)temp_float[m];
                            do_more = 1;
                            }
                        }
                    if(incrementlll){
                        lll++;
                        if(++kk==fs->frames_per_line[ii]){ii++;kk=0;}
                        }

                    //for(m=0;m<ms_glm->lenbrain;m++)printf("%f ",temp_double[ms_glm->brnidx[m]]);printf("\n");exit(-1);

                    }
                if(lc_resid)do_more=1;
                if(do_more){



//Rewrite this part using fidl_tc_ss.cxx as a template
                    if(dp->filetypeall==(int)IMG){
                        for(k=0;k<nbold_index;k++)temp_float[k]=mm->ptr[t0+bold_index[k]];
                        if(dp->swapbytes[i])swap_bytes((unsigned char *)temp_float,sizeof(float),(size_t)nbold_index); 
                        }
                    else if(dp->filetypeall==(int)CIFTI||dp->filetypeall==(int)CIFTI_DTSERIES){
                        for(k=0;k<nbold_index;k++)temp_float[k]=temp_float2[t0+bold_index[k]];
                        }
                    else{
                        if(!nifti_getvol(niftiPtr,(int64_t)j,temp_float))exit(-1);
                        for(k=0;k<nbold_index;k++)temp_float[k]=temp_float[bold_index[k]];
                        }

                    for(k=0;k<ap->vol;k++)bold[k]=0.;
                    for(k=0;k<nbold_index1;k++)bold[bold_index1[k]]=temp_float[k]==(float)UNSAMPLED_VOXEL?(double)UNSAMPLED_VOXEL:(double)temp_float[k];
//Rewrite end 

#if 0
fidl_tc_ss.cxx
                    if(dp->filetypeall==(int)IMG){
                        for(j=0;j<dp->vol[m];j++) temp_float[j] = mm->ptr[p+j];
                        if(dp->swapbytes[m])swap_bytes((unsigned char *)temp_float,sizeof(float),(size_t)dp->vol[m]);
                        }
                    else{
                        if(!nifti_getvol(niftiPtr,(int64_t)fbf->roi_by_file[m][i],temp_float))exit(-1);
                        }
#endif





                    dptr=bold;
                    if(xform_file&&lc_t4_before){
                        if(!t4_atlas(bold,stat,t4,dp->xdim[i],dp->ydim[i],dp->zdim[i],dp->dx[i],dp->dz[i],A_or_B_or_U,dp->orientation[i],ap,(double*)NULL))exit(-1);
                        dptr=stat;
                        }
                    if(!glm->ifh->nregions){
                        if(!glm->ifh->glm_masked)for(nindex=k=0;k<ms_glm->lenbrain;k++)
                            if(dptr[ms_glm->brnidx[k]]!=(double)UNSAMPLED_VOXEL)index[nindex++]=k;
                        if(fwhm>0.){
                            gauss_smoth2(dptr,td1,gs); //UNSAMPLED_VOXELS now tracked in gauss_smoth2
                            dptr=td1;
                            }
                        }
                    else {
                        crs(dptr,td1,rbf,(char*)NULL);
                        dptr=td1; 
                        }

//HERE


                    #if 0
                    if(lc_resid){
                        if(glm->valid_frms[l]>0){ 
                            if(!glm->ifh->glm_masked)
                                for(k=0;k<nindex;k++)dptr[ms_glm->brnidx[index[k]]]-=temp_double[ms_glm->brnidx[index[k]]];
                            else
                                for(k=0;k<glm->nmaski;k++)dptr[glm->maski[k]]-=temp_double[glm->maski[k]];
                            }
                        for(k=0;k<ms_glm->lenbrain;k++)temp_double[ms_glm->brnidx[k]]=dptr[ms_glm->brnidx[k]];
                        }
                    #endif
                    //START201007
                    if(lc_resid){
                        if(glm->valid_frms[l]>0){ 
                            if(!glm->ifh->glm_masked){
                                //std::cout<<"here66a"<<std::endl;
                                for(k=0;k<nindex;k++)temp_double[ms_glm->brnidx[index[k]]]=dptr[ms_glm->brnidx[index[k]]]-temp_double[ms_glm->brnidx[index[k]]];
                                }
                            else{
                                //std::cout<<"here66b"<<std::endl;
                                for(k=0;k<glm->nmaski;k++)temp_double[glm->maski[k]]=dptr[glm->maski[k]]-temp_double[glm->maski[k]];
                                }
                            }
                        }
                    

                    //std::cout<<"here67"<<std::endl;

                    if(lc_pctchg){
                        if(!glm->ifh->glm_masked){
                            //std::cout<<"here67a"<<std::endl;
                            for(k=0;k<nindex;k++) temp_double[ms_glm->brnidx[index[k]]] *=
                                (gms->grand_mean[ms_glm->brnidx[index[k]]] < gms->grand_mean_thresh ?
                                0 : gms->pct_chng_scl/gms->grand_mean[ms_glm->brnidx[index[k]]]);
                            }
                        else{
                            //std::cout<<"here67b"<<std::endl;
                            for(k=0;k<glm->nmaski;k++)temp_double[glm->maski[k]]*=gms->grand_mean[k]<gms->grand_mean_thresh?
                                0.:gms->pct_chng_scl/gms->grand_mean[k];

                            //for(k=0;k<glm->nmaski;k++)printf("temp_double[%d]=%f gms->grand_mean[%d]=%f\n",glm->maski[k],temp_double[glm->maski[k]],k,gms->grand_mean[k]);
                            }
                        }

                    //std::cout<<"here68"<<std::endl;

                    dptr1=temp_double;
                    if(xform_file&&!lc_t4_before){
                        if(!t4_atlas(temp_double,stat,t4,dp->xdim[i],dp->ydim[i],dp->zdim[i],dp->dx[i],dp->dz[i],A_or_B_or_U,dp->orientation[i],ap,(double*)NULL))exit(-1);
                        dptr1=stat;
                        if(!lc_resid) {
                            if(!t4_atlas(dptr,td2,t4,dp->xdim[i],dp->ydim[i],dp->zdim[i],dp->dx[i],dp->dz[i],A_or_B_or_U,dp->orientation[i],ap,(double*)NULL))exit(-1);
                            dptr=td2;
                            }
                        }

                    if(num_region_files&&!glm->ifh->nregions){
                        if(!lc_voxels){
                            crs(dptr1,td3,rbf,(char*)NULL);
                            if(!ac_name){for(k=0;k<num_regions;k++)region_tc[k][i][j]=td3[k];}else{dptr1=td3;}
                            if(!lc_resid){
                                crs(dptr,td4,rbf,(char*)NULL);
                                if(!ac_name)for(k=0;k<num_regions;k++)region_tc_bold[k][i][j]=(float)td4[k];
                                }
                            }
                        else{
                            for(k=0;k<niout;k++,o++)voxel_tc[o]=dptr1[iout[k]];
                            }
                        }

                    } /*do_more*/

                //std::cout<<"here69"<<std::endl;

                //START190917
                //if(!lc_rgc) {
                
                    if((!num_region_files&&!ac_name)||glm->ifh->nregions) {

                        //std::cout<<"here70 lc_average="<<lc_average<<std::endl;

                        if(!lc_average) {

                            //std::cout<<"here71 scratchdir="<<scratchdir<<std::endl;

                            if(!scratchdir) {
                                if(!lcvarall) {
                                    for(k=0;k<ap->vol;k++) temp_float[k] = 0.;

                                    //std::cout<<"here70"<<std::endl;

                                    #if 0
                                    if(do_more)for(k=0;k<ms->lenbrain;k++)temp_float[ms->brnidx[k]]=(float)temp_double[ms->brnidx[k]];
                                    if(do_more){
                                        for(k=0;k<ms_glm->nuns;k++)temp_float[ms_glm->unsi[k]]=(float)UNSAMPLED_VOXEL;
                                        }
                                    #endif
                                    //START160929
                                    if(do_more){

                                        //std::cout<<"here71"<<std::endl;

                                        for(k=0;k<ms->lenbrain;++k)temp_float[ms->brnidx[k]]=(float)dptr1[ms->brnidx[k]];

                                        //std::cout<<"here72 ms_glm->nuns="<<ms_glm->nuns<<std::endl;

                                        //for(k=0;k<ms_glm->nuns;++k)temp_float[ms_glm->unsi[k]]=(float)UNSAMPLED_VOXEL;
                                        for(k=0;k<ms_glm->nuns;++k){
                                            std::cout<<"ms_glm->unsi["<<k<<"]]="<<ms_glm->unsi[k]<<std::endl;
                                            //temp_float[ms_glm->unsi[k]]=(float)UNSAMPLED_VOXEL;
                                            }
                                        
                                        for(k=0;k<ms_glm->nuns;++k)temp_float[ms_glm->unsi[k]]=(float)UNSAMPLED_VOXEL;

                                        //std::cout<<"here73"<<std::endl;
                                        }

                                    //std::cout<<"here74"<<std::endl;

                                    if(dp->filetypeall==(int)IMG){
                                        if(!fwrite_sub(temp_float,sizeof(float),(size_t)ap->vol,op,0)){
                                            printf("fidlError: fidl_residuals Could not write to %s\n",filename);
                                            exit(-1);
                                            }
                                        }
                                    else if(dp->filetypeall==(int)CIFTI||dp->filetypeall==(int)CIFTI_DTSERIES){
                                        for(k=0;k<ap->vol;k++)temp_float2[t0+k]=temp_float[k];
                                        }
                                    else{ 
                                        if(!nifti_putvol(op,(int64_t)j,temp_float))exit(-1);
                                        }
                                    }
                                else {

                                    #if 0
                                    for(k=0;k<ms->lenbrain;k++) {
                                        sum[k]+=temp_double[ms->brnidx[k]];
                                        sum2[k]+=temp_double[ms->brnidx[k]]*temp_double[ms->brnidx[k]];
                                        }
                                    #endif
                                    //START160929
                                    for(k=0;k<ms->lenbrain;k++){
                                        sum[k]+=dptr1[ms->brnidx[k]];
                                        sum2[k]+=dptr1[ms->brnidx[k]]*dptr1[ms->brnidx[k]];
                                        }

                                    }
                                }
                            }
                        else {

                            //if(do_more) for(j1=j*ms->lenbrain,k=0;k<ms->lenbrain;k++,j1++) avg[j1] += temp_double[k];
                            //START160928
                            if(do_more)for(j1=j*ms->lenbrain,k=0;k<ms->lenbrain;k++,j1++)avg[j1]+=dptr1[k];

                            }
                        }

                //START190917
                #if 0
                    }
                else {

                    #if 0
                    for(k=0;k<ms->lenbrain;k++,kkk++)storage[kkk]=temp_double[ms->brnidx[k]];
                    for(gc[tdim_usable[i]]=0.,k=0;k<ms_gc->lenbrain;k++)gc[tdim_usable[i]]+=temp_double[ms_gc->brnidx[k]];
                    #endif
                    //START160929
                    for(k=0;k<ms->lenbrain;k++,kkk++)storage[kkk]=dptr1[ms->brnidx[k]];
                    for(gc[tdim_usable[i]]=0.,k=0;k<ms_gc->lenbrain;k++)gc[tdim_usable[i]]+=dptr1[ms_gc->brnidx[k]];

                    gc[tdim_usable[i]]/=(double)ms_gc->lenbrain;
                    }
                #endif

                if(scratchdir) {

                    //std::cout<<"here80"<<std::endl;

                    if(!lcfloat) {

                        #if 0
                        if((!num_region_files&&!ac_name)||glm->ifh->nregions)
                            for(k=0;k<(k1=ms->lenbrain);k++) stat[k]=temp_double[ms->brnidx[k]];
                        else if(lc_voxels)
                            for(k=0;k<(k1=ms_glm->lenbrain);k++) stat[k]=temp_double[ms_glm->brnidx[k]];
                        if(!fwrite_sub(stat,sizeof(double),(size_t)k1,op,0)) {
                             printf("Error: Could not write to %s\n",filename);
                             exit(-1);
                             }
                        #endif
                        //START160929
                        if((!num_region_files&&!ac_name)||glm->ifh->nregions)
                            for(k=0;k<(k1=ms->lenbrain);k++)td1[k]=dptr1[ms->brnidx[k]];
                        else if(lc_voxels)
                            for(k=0;k<(k1=ms_glm->lenbrain);k++)td1[k]=dptr1[ms_glm->brnidx[k]];
                        if(!fwrite_sub(td1,sizeof(double),(size_t)k1,op,0)){
                             printf("fidlError: fidl_residuals Could not write to %s\n",filename);
                             exit(-1);
                             }

                        }
                    else {

                        #if 0
                        if((!num_region_files&&!ac_name)||glm->ifh->nregions)
                            for(k=0;k<(k1=ms->lenbrain);k++)temp_float[k]=(float)dptr1[ms->brnidx[k]];
                        else if(lc_voxels)
                            for(k=0;k<(k1=niout);k++)temp_float[k]=(float)dptr1[iout[k]];
                        if(dp->filetypeall==(int)IMG){
                            if(!fwrite_sub(temp_float,sizeof(float),(size_t)k1,op,0)) {
                                printf("fidlError: fidl_residuals Could not write to %s\n",filename);
                                exit(-1);
                                }
                            }
                        else if(dp->filetypeall==(int)CIFTI||dp->filetypeall==(int)CIFTI_DTSERIES){
                            std::cout<<"Needs to implemented! Abort!"<<std::endl;
                            exit(-1);
                            }
                        else{
                            for(k=0;k<ap->vol;k++) temp_float[k] = 0.;
                            for(k=0;k<ms->lenbrain;++k)temp_float[ms->brnidx[k]]=(float)dptr1[ms->brnidx[k]];
                            if(!nifti_putvol(op,(int64_t)j,temp_float))exit(-1);
                            }
                        #endif
                        //START201021
                        if(dp->filetypeall==(int)IMG){
                            if((!num_region_files&&!ac_name)||glm->ifh->nregions)
                                for(k=0;k<(k1=ms->lenbrain);k++)temp_float[k]=(float)dptr1[ms->brnidx[k]];
                            else if(lc_voxels)
                                for(k=0;k<(k1=niout);k++)temp_float[k]=(float)dptr1[iout[k]];
                            if(!fwrite_sub(temp_float,sizeof(float),(size_t)k1,op,0)) {
                                printf("fidlError: fidl_residuals Could not write to %s\n",filename);
                                exit(-1);
                                }
                            }
                        else if(dp->filetypeall==(int)CIFTI||dp->filetypeall==(int)CIFTI_DTSERIES){
                            std::cout<<"Needs to implemented! Abort!"<<std::endl;
                            exit(-1);
                            }
                        else{
                            for(k=0;k<ap->vol;k++) temp_float[k] = 0.;
                            for(k=0;k<ms->lenbrain;++k)temp_float[ms->brnidx[k]]=(float)dptr1[ms->brnidx[k]];
                            if(!nifti_putvol(op,(int64_t)j,temp_float))exit(-1);
                            }
                        

                        }
                    }

                #if 0
                if(ac_name) {
                    if(pairs++) for(k=0;k<num_regions;k++) ac_numer[i][k] += temp_double[k]*pairs_stack[k];
                    for(k=0;k<num_regions;k++) {
                        ac_denom[i][k] += temp_double[k]*temp_double[k];
                        pairs_stack[k] = temp_double[k]; 
                        }
                    }
                #endif
                //START160929
                if(ac_name){
                    if(pairs++)for(k=0;k<num_regions;k++)ac_numer[i][k]+=dptr1[k]*pairs_stack[k];
                    for(k=0;k<num_regions;k++){
                        ac_denom[i][k]+=dptr1[k]*dptr1[k];
                        pairs_stack[k]=dptr1[k];
                        }
                    }

                }
            ++tdim_usable[i];
            ++num_usable_per_frame[j];
            validframeindex[nvfi++] = j;
            }
        if(glm->valid_frms[l]>0 && t<glm->ifh->glm_Nrow) ++t; /*glm->valid_frms is needed for proper indexing of glm->A*/
        }
    if(!lc_names_only&&dp->filetypeall==(int)IMG)if(!unmap_disk(mm))exit(-1);

    //std::cout<<"here77"<<std::endl;

    //START190917
    //if(!lc_rgc) {

        if((!lc_average&&!lc_one_file&&!num_regions)||scratchdir||(glm->ifh->nregions&&!scratchdir)) {

            //if(op)fclose(op);
            //START190919
            if(op&&dp->filetypeall!=(int)NIFTI)fclose(op);

            if(lc_autocorr||lc_crosscorr||lc_crosscov||lc_cov||lc_var) {
                if(lc_autocorr) {
                    sprintf(dum,"%s_b%d.4dfp.img",autocorrroot,i+1);
                    if(!autocorr_guts(filename,dum,corr,how_many,tdim_usable[i])) exit(-1);
                    dim4 = how_many;
                    dim1 = tdim_usable[i];
                    glm->ifh->df1 = (float)(tdim_usable[i]-maxlik_unbias);
                    if(!read_set_write_ifh(bold_files->files[i],xform_file,dim4,dum,glm->ifh,ap,1,dim1,SunOS_Linux,lcfloat)) exit(-1);
                    #ifndef MONTE_CARLO
                        printf("Output written to %s\n",dum);
                    #endif
                    }
                if(lc_crosscorr) {
                    sprintf(dum,"%s_b%d.4dfp.img",crosscorrroot,i+1);
                    if(!crosscorr_guts(filename,dum,corr,how_many,tdim_usable[i],-1,(double*)NULL)) exit(-1);
                    dim4 = how_many*how_many;
                    dim1 = tdim_usable[i];
                    glm->ifh->df1 = (float)(tdim_usable[i]-maxlik_unbias);
                    if(!read_set_write_ifh(bold_files->files[i],xform_file,dim4,dum,glm->ifh,ap,1,dim1,SunOS_Linux,lcfloat)) exit(-1);
                    #ifndef MONTE_CARLO
                        printf("Output written to %s\n",dum);
                    #endif
                    }
                if(lc_crosscov) {
                    sprintf(dum,"%s_b%d.4dfp.img",crosscovroot,i+1);
                    if(!crosscorr_guts(filename,dum,corr,how_many,tdim_usable[i],maxlik_unbias,(double*)NULL)) exit(-1);
                    dim4 = how_many*how_many;
                    dim1 = tdim_usable[i]-maxlik_unbias;
                    glm->ifh->df1 = (float)(tdim_usable[i]-maxlik_unbias);
                    if(!read_set_write_ifh(bold_files->files[i],xform_file,dim4,dum,glm->ifh,ap,1,dim1,SunOS_Linux,lcfloat)) exit(-1);
                    #ifndef MONTE_CARLO
                        printf("Output written to %s\n",dum);
                    #endif
                    }
                if(lc_cov) {
                    sprintf(dum,"%s_b%d.4dfp.img",covroot,i+1);
                    if(!cov_guts(filename,dum,cov,how_many,tdim_usable[i],maxlik_unbias)) exit(-1);
                    dim4 = how_many*how_many;
                    dim1 = 1;
                    glm->ifh->df1 = (float)(tdim_usable[i]-maxlik_unbias);
                    if(!read_set_write_ifh(bold_files->files[i],xform_file,dim4,dum,glm->ifh,ap,1,dim1,SunOS_Linux,lcfloat)) exit(-1);
                    #ifndef MONTE_CARLO
                        printf("Output written to %s\n",dum);
                    #endif
                    }
                if(lc_var) {
                    sprintf(dum,"%s_b%d.4dfp.img",varroot,i+1);
                    if(!var_guts(filename,dum,var,how_many,tdim_usable[i],maxlik_unbias)) exit(-1);
                    dim4 = how_many;
                    dim1 = 1;
                    glm->ifh->df1 = (float)(tdim_usable[i]-maxlik_unbias);
                    if(!read_set_write_ifh(bold_files->files[i],xform_file,dim4,dum,glm->ifh,ap,1,dim1,SunOS_Linux,lcfloat)) exit(-1);
                    #ifndef MONTE_CARLO
                        printf("Output written to %s\n",dum);
                    #endif
                    }
                sprintf(dum,"rm %s",filename);
                if(system(dum)==-1) {
                    printf("Error: system call in fidl_residuals. Abort!\n");
                    exit(-1);
                    }
                }
            else if(lcvarall){
                /*do nothing*/
                }
            else {
                if(dp->filetypeall==(int)IMG){
                    dim4 = tdim_usable[i];
                    dim1 = k1;
                    glm->ifh->number_format = (int)DOUBLE_IF;
                    glm->ifh->bytes_per_pix = 8;
                    glm->ifh->bigendian = !SunOS_Linux ? 1:0;
                    if(!read_set_write_ifh(bold_files->files[i],xform_file,dim4,filename,glm->ifh,ap,scratchdir||glm->ifh->nregions,
                        dim1,SunOS_Linux,lcfloat)) exit(-1);
                    }
                else if(dp->filetypeall==(int)CIFTI||dp->filetypeall==(int)CIFTI_DTSERIES){
                    w1->temp_float=temp_float2;
                    w1->tdim=(int64_t)dp->tdim[i];
                    if(!write1(filename,w1))exit(-1);
                    }
                
                //START190919
                //#ifndef MONTE_CARLO

                    //if(!lc_names_only) printf("Output written to %s\n",filename);
                    //START190919
                    if(!lc_names_only){
                        if(dp->filetypeall==(int)NIFTI){
                            //std::string str="gzip " + filename
                            //if(system(str.cstr())==-1)
                            //    std::cout<<"fidlError: "<<str<<std::endl;
                            sprintf(dum,"gzip -f %s",filename);
                            if(system(dum)==-1)
                                std::cout<<"fidlError: "<<dum<<std::endl;
                            else
                                strcat(filename,".gz");
                            }
                        printf("Output written to %s\n",filename);
                        }

                    fprintf(cp,"%s %s\n",filename,!identify?"":identify->files[i]);
                
                //START190919
                //#endif

                }
            }

    //START190917
    #if 0 
        }
    else {
        /*printf("gc="); for(q=0;q<tdim_usable[i];q++) printf("%f ",gc[q]); printf("\n");*/
        for(xtx=0.,q=0;q<tdim_usable[i];q++) xtx += gc[q]*gc[q]; 
        for(k=0;k<ms->lenbrain;k++) {
            for(xty=0.,q=0;q<tdim_usable[i];q++) xty += gc[q]*storage[q*ms->lenbrain+k];
            /*printf("k=%d xtx=%f xty=%f b=%f\n",k,xtx,xty,b);*/
            for(b=xty/xtx,q=0;q<tdim_usable[i];q++) storage[q*ms->lenbrain+k] -= gc[q]*b;
            }
        for(kkk=q=0;q<tdim_usable[i];q++) {
            for(k=0;k<ms->lenbrain;k++,kkk++) temp_float[ms->brnidx[k]] = (float)storage[kkk];
            if(!fwrite_sub(temp_float,sizeof(float),(size_t)ap->vol,op,SunOS_Linux)) {
                printf("Error: Could not write to %s\n",filename);
                exit(-1);
                }
            }
        if(op) fclose(op);
        if(!read_set_write_ifh(bold_files->files[i],xform_file,tdim_usable[i],filename,glm->ifh,ap,scratchdir||glm->ifh->nregions,
            how_many,SunOS_Linux,lcfloat)) exit(-1);
        printf("Output written to %s\n",filename);
        }
    #endif

    }
fclose(fp);
fclose(cp);


//START170131
if(lc_names_only&&lc_names_ifh) {
    printf("Concatenated file written to %s\n",concname);
    exit(0);
    }


if(lcvarall) {
    for(j=i=0;i<(int)bold_files->nfiles;i++) j += tdim_usable[i];

    //for(i=0;i<ms->lenvol;i++) temp_float[i]=0.;
    //START190918
    for(i=0;i<ms->vol;++i)temp_float[i]=0.;

    for(i=0;i<ms->lenbrain;i++) temp_float[ms->brnidx[i]] = (sum2[i] - sum[i]*sum[i]/(double)j)/(double)(j-maxlik_unbias);
    sprintf(dum,"%s.4dfp.img",varroot);

    //if(!writestack(dum,temp_float,sizeof(float),(size_t)ms->lenvol,0)) exit(-1);
    //START190918
    if(!writestack(dum,temp_float,sizeof(float),(size_t)ms->vol,0)) exit(-1);

    if(!read_set_write_ifh(bold_files->files[0],xform_file,1,dum,glm->ifh,ap,0,how_many,SunOS_Linux,1)) exit(-1);
    printf("Output written to %s\n",dum);
    } 
if(ac_name){
    for(i=0;i<(int)bold_files->nfiles;i++) for(k=0;k<num_regions;k++) ac_numer[i][k] /= ac_denom[i][k]; 
    for(i=0;i<(int)bold_files->nfiles;i++) {

        //for(k=0;k<uns->num_unsampled_voxels;k++) ac_numer[i][uns->unsampled_voxels_idx[k]] = (double)UNSAMPLED_VOXEL;
        //START190917
        for(k=0;k<ms_glm->nuns;k++) ac_numer[i][ms_glm->unsi[k]] = (double)UNSAMPLED_VOXEL;

        }

    #if 0
    free(temp_float);
    if(!(temp_float=malloc(sizeof*temp_float*(bold_files->nfiles*ap->vol)))) {
        printf("Error: Unable to malloc temp_float\n");
        exit(-1); 
        }
    #endif
    //START190918
    delete[] temp_float; 
    temp_float=new float[bold_files->nfiles*ap->vol];

    for(i=0;i<(int)bold_files->nfiles*ap->vol;i++) temp_float[i]=0.;
    for(j=i=0;i<(int)bold_files->nfiles;i++,j+=ap->vol)for(k=0;k<num_regions;k++)temp_float[j+ms->brnidx[k]]=(float)(ac_numer[i][k]); 
    if(!writestack(ac_name,temp_float,sizeof(float),bold_files->nfiles*(size_t)ap->vol,SunOS_Linux)) exit(-1);
    if(!read_set_write_ifh(bold_files->files[0],xform_file,bold_files->nfiles,ac_name,glm->ifh,ap,scratchdir||glm->ifh->nregions,
        how_many,SunOS_Linux,lcfloat)) exit(-1);
    printf("Sample AR(1) autocorrelation written to %s\n",ac_name);
    }
else if(!lcvarall) {
    if(lc_average) {
        for(i=k=0,j=tdim_usable[0];i<(int)bold_files->nfiles;i++) if(j != tdim_usable[i]) ++k;
        if(k) {
            fprintf(stderr,"Note: The number of usable frames differs by run.\n");
            for(i=0;i<(int)bold_files->nfiles;i++) fprintf(stderr,"%s %d\n",bold_files->files[i],tdim_usable[i]);
            }
        }
    if(!num_region_files) {
        if(!lc_average) {
            if(lc_one_file){
                fclose(op);
                for(i=j=0;i<(int)bold_files->nfiles;i++) j += tdim_usable[i];
                if(!read_set_write_ifh(bold_files->files[0],xform_file,j,filename,glm->ifh,ap,scratchdir||glm->ifh->nregions,
                    how_many,SunOS_Linux,lcfloat)) exit(-1);
                #ifndef MONTE_CARLO
                    printf("Output written to %s\n",filename);
                #endif
                }
            }
        else {
            for(i=0;i<ap->vol;i++) temp_float[i] = 0.;
            sprintf(filename,"%s.4dfp.img",root);
            if(!(op = fopen_sub(filename,"w"))) exit(-1);
            for(n=j=0;j<dp->tdim_max;j++) {
                if(num_usable_per_frame[j]) {
                    /*printf("num_usable_per_frame[%d]=%d\n",j,num_usable_per_frame[j]);fflush(stdout);*/
                    for(k=j*ms->lenbrain,i=0;i<ms->lenbrain;i++,k++) 
                        temp_float[ms->brnidx[i]] = (float)(avg[k]/(double)num_usable_per_frame[j]);
                    if(!fwrite_sub(temp_float,sizeof(float),(size_t)ap->vol,op,0)) {
                        printf("Error: Could not write to %s\n",filename);
                        exit(-1);
                        }
                    n++;
                    }
                }
            fclose(op);
            if(!read_set_write_ifh(bold_files->files[0],xform_file,n,filename,glm->ifh,ap,scratchdir||glm->ifh->nregions,how_many,
                SunOS_Linux,lcfloat)) exit(-1);
            #ifndef MONTE_CARLO
                printf("Output written to %s\n",filename);
            #endif
            }
        }
    else {
        if(lc_average) {
            for(k=0;k<num_regions;k++)
                for(i=1;i<(int)bold_files->nfiles;i++) {
                    for(j=0;j<dp->tdim_max;j++) region_tc[k][0][j] += region_tc[k][i][j];
                    }
            for(k=0;k<num_regions;k++) {
                for(j=0;j<dp->tdim_max;j++) if(num_usable_per_frame[j]) region_tc[k][0][j] /= num_usable_per_frame[j];
                }
            }
        else if(lc_regional_avg_cond_across_runs) {

            #if 0
            if(!(ncond_per_bold=malloc(sizeof*ncond_per_bold*bold_files->nfiles))) {
                printf("Error: Unable to malloc ncond_per_bold\n");
                exit(-1);
                }
            for(k=0;k<num_regions;k++) ncond_per_bold[k]=0;
            #endif
            //START190918
            ncond_per_bold=new int[bold_files->nfiles]; 
            for(k=0;k<num_regions;++k)ncond_per_bold[k]=0;

            if(!(avgreg = d2double(num_regions,length_max))) exit(-1);
            for(k=0;k<num_regions;k++) {
                for(m=l=i=0;i<(int)bold_files->nfiles;i++) {
                    for(n=j=0;j<dp->tdim[i];j++,l++) {
                        if(valid_frames[l]) {
                            avgreg[k][n++] += region_tc[k][i][j];
                            if(n == length[m]) {
                                n = 0;
                                m++;
                                if(!k) ncond_per_bold[i]++;
                                }
                            }
                        }
                    }
                }
            for(k=0;k<num_regions;k++) for(n=0;n<length_max;n++) avgreg[k][n] /= (double)den[n];
            }
        if(!lc_voxels) {
            if(!(op = fopen_sub(regional_residual_name->files[0],"w"))) exit(-1);
            if(!lc_unformatted) fprintf(op,"GLM      %s\n",glm_file);
            if(!lc_regional_avg_cond_across_runs) {
                if(!lc_unformatted) {
                    if(lc_resid) {
                        fprintf(op,"BOLDS    b1  -> %s\n",bold_files->files[0]);
                        for(k=1;k<(int)bold_files->nfiles;k++) fprintf(op,"         b%-2d -> %s\n",k+1,bold_files->files[k]);
                        fprintf(op,"REGIONS  r1  -> %s\n",rbf->region_names_ptr[0]);
                        for(k=1;k<num_regions;k++) fprintf(op,"         r%-2d -> %s\n",k+1,rbf->region_names_ptr[k]);
                        fprintf(op,"\nFirst frame is 1.\n\n  FRAME      ");
                        for(k=0;k<num_regions;k++) fprintf(op,"r%-8d",k+1);
                        fprintf(op,"\n");
                        if(lc_average) bold_files->nfiles = 1;
                        for(l=i=0;i<(int)bold_files->nfiles;l+=dp->tdim[i++]) {
                            if(tdim_usable[i]) {
                                if(lc_average) {
                                    m = 1;
                                    }
                                else {
                                    fprintf(op,"b%-2d",i+1);
                                    m = 0;
                                    }
                                for(j=0;j<dp->tdim[i];j++) {
                                    /*printf("valid_frames[%d]=%d\n",l+j,valid_frames[l+j]);*/
                                    if(valid_frames[l+j]>0) {
                                        if(!m) {
                                            fprintf(op,"%4d  ",j+1);
                                            m = 1;
                                            }
                                        else {
                                            fprintf(op,"   %4d  ",j+1);
                                            }
                                        for(k=0;k<num_regions;k++) fprintf(op,"%8.4f ",region_tc[k][i][j]);
                                        fprintf(op,"\n");
                                        }
                                    }
                                }
                            }
                        } 
                    else {
                        for(k=0;k<num_regions;k++) SSE[k] = SSyy[k] = y[k] = 0.; 
                        for(n=l=i=0;i<(int)bold_files->nfiles;l+=dp->tdim[i++]) {
                            if(tdim_usable[i]) {
                                for(j=0;j<dp->tdim[i];j++) {
                                    /*if(valid_frames[l+j]>0) {*/
                                    if(glm->valid_frms[l+j]>0) {
                                        for(k=0;k<num_regions;k++) {
                                            td = region_tc[k][i][j]-region_tc_bold[k][i][j];
                                            SSE[k] += td*td;
                                            SSyy[k] += region_tc_bold[k][i][j]*region_tc_bold[k][i][j];
                                            y[k] += region_tc_bold[k][i][j]; 
                                            }
                                        n++;
                                        }
                                    }
                                }
                            }
                        td = (double)(n-1) / (double)(n-glm->ifh->glm_all_eff); 
                        for(k=0;k<num_regions;k++) {
                            SSyy[k] = SSyy[k] - y[k]*y[k]/(double)n; 
                            R2[k] = 1. - SSE[k]/SSyy[k];
                            Ra2[k] = 1. - td*SSE[k]/SSyy[k]; 
                            }
                        fprintf(op,"BOLDS    b1  -> %s\n",bold_files->files[0]);
                        for(k=1;k<(int)bold_files->nfiles;k++) fprintf(op,"         b%-2d -> %s\n",k+1,bold_files->files[k]);
                        fprintf(op,"\n");
                        for(k=0;k<num_regions;k++) {
                            fprintf(op,"%s\t%d\tR2=%f\tRa2=%f\nframe\t",rbf->region_names_ptr[k],rbf->nvoxels_region[k],R2[k],Ra2[k]);
                            for(j=0;j<(int)bold_files->nfiles;j++) fprintf(op,"b%d data\t b%d fit\t",j+1,j+1);
                            fprintf(op,"\n");
                            for(j=0;j<dp->tdim_max;j++) {
                                fprintf(op,"%4d\t",j+1);
                                for(i=0;i<(int)bold_files->nfiles;i++)
                                    fprintf(op,"%9.4f\t%9.4f\t",region_tc_bold[k][i][j],region_tc[k][i][j]);
                                fprintf(op,"\n");
                                }
                            fprintf(op,"\n");
                            }

                        }
                    }
                else {
                    if(lc_average) bold_files->nfiles = 1;
                    for(l=i=0;i<(int)bold_files->nfiles;l+=dp->tdim[i++]) {
                        if(tdim_usable[i]) {
                            for(j=0;j<dp->tdim[i];j++) {
                                if(valid_frames[l+j]>0) {
                                    for(k=0;k<num_regions;k++) fprintf(op,"%8.4f ",region_tc[k][i][j]);
                                    fprintf(op,"\n");
                                    }
                                }
                            }
                        }
                    }
                }
            else {
                for(m=i=0;i<(int)bold_files->nfiles;i++) {
                    if(ncond_per_bold[i]) {
                        fprintf(op,"%sb%-2d -> %s %d\n",!m?s1:s2,i+1,bold_files->files[i],ncond_per_bold[i]);
                        if(!m) m = 1;
                        }
                    }
                for(k=0;k<num_regions;k++) fprintf(op,"%sr%-2d -> %s\n",!k?s3:s2,k+1,rbf->region_names_ptr[k]);
                fprintf(op,"\n  FRAME      ");
                for(k=0;k<num_regions;k++) fprintf(op,"r%-8d",k+1);
                fprintf(op,"\n");
                for(n=0;n<length_max;n++) {
                    fprintf(op,"   %4d  ",n+1);
                    for(k=0;k<num_regions;k++) fprintf(op,"%8.4f ",avgreg[k][n]);
                    fprintf(op,"\n");
                    }
                }
            fclose(op);
            #ifndef MONTE_CARLO
                printf("Output written to %s\n",regional_residual_name->files[0]);
            #endif
            }
        else {
            printf("lc_pca=%d\n",lc_pca);
            if(lc_pca) {

                #if 0
                if(!(x=malloc(sizeof*x*nvfi*rbf->nvoxels_region_max))) {
                    printf("Error: Unable to malloc x\n");
                    exit(-1);
                    }
                if(!(voxelsum=malloc(sizeof*voxelsum*rbf->nvoxels_region_max))) {
                    printf("Error: Unable to malloc x\n");
                    exit(-1);
                    }
                if(!(ivs=malloc(sizeof*ivs*rbf->nvoxels_region_max))) {
                    printf("Error: Unable to malloc x\n");
                    exit(-1);
                    }
                #endif
                //START190918
                x=new double[nvfi*rbf->nvoxels_region_max]; 
                voxelsum=new double[rbf->nvoxels_region_max];
                ivs=new int[rbf->nvoxels_region_max];
                
                }
            for(kk=k=0;k<num_regions;kk+=rbf->nvoxels_region[k++]) {
                if(lc_pca) {
                    for(i=0;i<rbf->nvoxels_region[k];i++) voxelsum[i] = 0.;
                    for(i=0;i<nvfi;i++) {
                        for(o=i*rbf->nvoxels,n=0;n<rbf->nvoxels_region[k];n++) {
                            voxelsum[n] += abs(voxel_tc[o+kk+n]);
                            }
                        }
                    /*printf("voxelsum="); for(i=0;i<rbf->nvoxels_region[k];i++) printf("%f ",voxelsum[i]); printf("\n");*/
                    for(nvs=i=0;i<rbf->nvoxels_region[k];i++) if(voxelsum[i] > 0.000001) ivs[nvs++] = i; 
                    for(j=i=0;i<nvfi;i++) {
                        for(o=i*rbf->nvoxels,n=0;n<nvs;n++,j++) {
                            x[j] = voxel_tc[o+kk+ivs[n]];
                            }
                        }
                    }
                #if 0
                    x = [frame 1 nvs voxels
                         frame 2 nvs voxels
                         . . .
                         frame nvfi nvs voxels]
                #endif 

                if(!(op=fopen_sub(regional_residual_name->files[k],"w"))) exit(-1);
                fprintf(op,"BOLDS    b1  -> %s\n",bold_files->files[0]);
                for(i=1;i<(int)bold_files->nfiles;i++) fprintf(op,"         b%-2d -> %s\n",i+1,bold_files->files[i]);
                fprintf(op,"\nFirst frame is 1.\n\n  FRAME  Each voxel is a column. %d voxels.\n",rbf->nvoxels_region[k]);
                for(m=p=ii=i=0;i<(int)bold_files->nfiles;i++) {
                    if(tdim_usable[i]) {
                        fprintf(op,"b%-2d",i+1);
                        for(n=0;n<tdim_usable[i];n++,ii++,p++) {
                            fprintf(op,"%s%4d  ",!n?"":"   ",validframeindex[p]+1);
                            for(o=ii*rbf->nvoxels,j=0;j<rbf->nvoxels_region[k];j++,m++) {
                                l = o+kk+j;
                                /*if(lc_pca) x[m] = voxel_tc[l];*/
                                fprintf(op,"%9.4f ",voxel_tc[l]);
                                }
                            fprintf(op,"\n");
                            }
                        }
                    }
                if(lc_pca) {
                    if(!(pc=pca(x,nvfi,nvs))) exit(-1);
                    fprintf(op,"\nPrincipal components analysis  By column, raw eigenvalue, scaled eigenvalue, eigenvector.\n");
                    fprintf(op,"    Analysis includes %d voxels, %d timepoints.\n",pc->n,nvfi);
                    for(temp_double[0]=0.,m=1;m<=pc->n;m++) {
                        temp_double[0] += pc->EIGVAL[m];
                        /*fprintf(op,"%9.4f ",pc->EIGVAL[m]);*/
                        /*fprintf(op,"%10.4f ",pc->EIGVAL[m]);*/
                        #if 0
                        if(pc->EIGVAL[m]<=99999.) {
                            fprintf(op,"%10.4f ",pc->EIGVAL[m]); 
                            }
                        else {
                            /*fprintf(op,"%10g ",pc->EIGVAL[m]);*/
                            fprintf(op,"%10e ",pc->EIGVAL[m]); 
                            }
                        #endif
                        fprintf(op,"%10.4e ",pc->EIGVAL[m]); 
                        }
                    fprintf(op,"\n");
                    for(m=1;m<=pc->n;m++) fprintf(op,"%10.4f ",pc->EIGVAL[m]/temp_double[0]); 
                    fprintf(op,"\n");
                    for(m=1;m<=pc->n;m++) fprintf(op,"---------- "); 
                    fprintf(op,"\n");
                    for(m=1;m<=pc->n;m++) { 
                        for(n=1;n<=pc->n;n++) fprintf(op,"%10.4f ",pc->EIGVEC[m][n]); 
                        fprintf(op,"\n");
                        }
                    free_pc(pc);
                    }
                fclose(op);
                #ifndef MONTE_CARLO
                    printf("Output written to %s\n",regional_residual_name->files[k]);
                #endif
                }

            }
        }
    }
}
int read_set_write_ifh(char *bold_file,char *xform_file,int dim4,char *filename,Interfile_header *glm_ifh,Atlas_Param *ap,
    int scratchdir,int how_many,int SunOS_Linux,int lcfloat){

    //START160825
    int lcmask=0;

    Interfile_header *ifh;
    if(!(ifh=read_ifh(bold_file,(Interfile_header*)NULL))) return 0; 
    if(scratchdir) {
        ifh->dim1 = how_many;
        ifh->dim2 = 1;
        ifh->dim3 = 1;
        ifh->mmppix[0] = 0;
        ifh->mmppix[1] = 0;
        ifh->mmppix[2] = 0;
        ifh->center[0] = 0;
        ifh->center[1] = 0;
        ifh->center[2] = 0;
        ifh->number_format = !lcfloat ? (int)DOUBLE_IF : (int)FLOAT_IF;
        ifh->bytes_per_pix = !lcfloat ? 8 : 4; 
        ifh->bigendian = !SunOS_Linux ? 1:0;
        }
    else if(xform_file) {
        ifh->dim1 = ap->xdim;
        ifh->dim2 = ap->ydim;
        ifh->dim3 = ap->zdim;
        ifh->voxel_size_1 = ap->voxel_size[0];
        ifh->voxel_size_2 = ap->voxel_size[1];
        ifh->voxel_size_3 = ap->voxel_size[2];
        ifh->bigendian = !SunOS_Linux ? 1:0;
        }
    else {
        ifh->voxel_size_1 = glm_ifh->glm_dxdy;
        ifh->voxel_size_2 = glm_ifh->glm_dxdy;
        ifh->voxel_size_3 = glm_ifh->glm_dz;
        ifh->bigendian = !SunOS_Linux ? 1:0;
        }
    ifh->dim4 = dim4;
    ifh->box_correction = (float)UNSAMPLED_VOXEL;
    ifh->smoothness = (float)UNSAMPLED_VOXEL;
    ifh->df1 = glm_ifh->df1;
    ifh->nregions = glm_ifh->nregions;
    ifh->region_names = glm_ifh->region_names;

    //START160825
    if(!ifh->mask&&glm_ifh->mask){lcmask=1;ifh->mask=glm_ifh->mask;}


    if(!write_ifh(filename,ifh,(int)FALSE)) return 0;
    ifh->nregions=0; /*set to zero to prevent from being freed, since it is not allocated*/

    //START160825
    if(lcmask)ifh->mask=NULL; //set to zero to prevent from being freed, since it is not allocated

    free_ifh(ifh,0);
    return 1;
    }
