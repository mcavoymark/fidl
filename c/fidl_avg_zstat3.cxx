/* Copyright 2/24/20 Washington University.  All Rights Reserved.;
   fidl_avg_zstat3.cxx  $Revision: 1.1 $*/
#include <iostream>
#include <string>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_cdf.h>
#include <gsl/gsl_multifit_nlin.h>
#include "fidl.h"
#include "anova_header_new.h"
#include "gauss_smoth2.h"
#include "read_data.h"
#include "mask.h"
#include "shouldiswap.h"
#include "subs_util.h"
#include "minmax.h"
#include "get_atlas_param.h"
#include "t4_atlas.h"
#include "read_tc_string_new.h"
#include "subs_nifti.h"
#include "subs_cifti.h"
#include "filetype.h"
#include "write1.h"
#include "get_grand_mean_struct.h"
#include "d2double.h"
#include "tags_tc_glmstack.h"
#include "map_disk.h"
#include "checkOS.h"

//#include "region.h"
#include "region3.h"
#include "find_regions_by_file_cover3.h"

#include "check_dimensions.h"
#include "get_atlas.h"
#include "read_xform.h"
#include "twoA_or_twoB.h"
#include "d2float.h"
#include "d2intvar.h"
#include "get_indbase.h"

//START200224
#include "lookuptable.h"

//START210123
#include "ptrw.h"

char const* Fileext[]={"",".4dfp.img",".dscalar.nii",".nii",".dtseries.nii"};

//static char rcsid[] = "$Header: /home/hannah/mcavoy/idl/clib/RCS/fidl_avg_zstat2.c,v 1.6 2017/04/11 19:53:41 mcavoy Exp $";
typedef struct {
    size_t n;
    double *t,*y,*Y,*sigma;
    } Nonlinlsqdata;
int read_confile(char *confile,int how_many,char **region_names_ptr,int num_tc_frames,char **tcstrptr,int *tc_frames,
    double *contrasts,int num_tc);
int boy_f(const gsl_vector *x,void *data,gsl_vector *f);
int boy_df(const gsl_vector *x,void *data,gsl_matrix *J);
int boy_fdf(const gsl_vector *x,void *data,gsl_vector *f,gsl_matrix *J);
int boyfit(double ***time_courses,double **time_courses_avg,int how_many,int nglmpersub,int num_tc_frames,int *tc_frames,
    double TR,int tc_frames_max,Tags_Tc_Struct2 *tags_tcs,Files_Struct *wfiles,Regions_By_File *rbf,
    char **glm_names,int num_wfiles,int lcprint_scaled_tc,int lcprint_unscaled_tc,TC *stimlens,
    double ***time_courses_fit,double **time_courses_avg_fit,int spaces,int lcsem,int lcsd,int lc_within_subject_sd,int lcphase,
    int lcphase_only,int sdsemspace,char *sdsemstrptr,char *outfile,int num_regions,size_t *len_glm_names,int *hipass,
    Files_Struct *glm_list_file,double ***time_courses_param,double **time_courses_avg_param,char **glmregnames);
int boyprint(double ***time_courses,double **time_courses_avg,float **time_courses_sd,double ***time_courses_unscaled,
    double **time_courses_avg_unscaled,float **time_courses_sd_unscaled,double ***within_subject_sd,double ***tcphase_unscaled,
    float **tcavgphase_unscaled,int how_many,int num_regions,int nglmpersub,int num_tc_frames,int *tc_frames,int *hipass,
    Tags_Tc_Struct2 *tags_tcs,Files_Struct *wfiles,Regions_By_File *rbf,int spaces,char **glm_names,
    size_t *len_glm_names,int num_wfiles,int lcprint_scaled_tc,int lcprint_unscaled_tc,int lcsem,int lcsd,int lc_within_subject_sd,
    int lcphase,int lcphase_only,int sdsemspace,char *sdsemstrptr,char *outfile,double TR,Files_Struct *glm_list_file,
    char **glmregnames);
int boyprintparam(double ***time_courses_param,double **time_courses_avg_param,int num_regions,int nglmpersub,int num_tc_frames,
    int *tc_frames,Tags_Tc_Struct2 *tags_tcs,Files_Struct *wfiles,Regions_By_File *rbf,int spaces,
    char **glm_names,size_t *len_glm_names,int num_wfiles,int lcsem,int lcsd,char *outfile,double TR,Files_Struct *glm_list_file,
    int how_many);
int boyprintvox(double ***voxels,double ***voxels_unscaled,int nglmpersub,int num_tc_frames,int *tc_frames,
    Regions_By_File *rbf,char **glm_names,int lcprint_scaled_tc,int lcprint_unscaled_tc,char *outfile,double TR,
    Files_Struct *glm_list_file,int num_wfiles,Files_Struct *wfiles,Atlas_Param *ap,char **tags_tcptr,char **tags_tc_effectsptr);
int main(int argc,char **argv)
{
char outfile[MAXNAME],**tags=NULL,*maskfile=NULL,fwhmstr[10],outsdfile[MAXNAME],outsemfile[MAXNAME],appendstr[MAXNAME],string[MAXNAME],
     string2[MAXNAME],**glm_names,*regional_avgstat_name=NULL,
     *directory=NULL,*group_name=NULL,delay_type_str[3],outfile_unscaled[MAXNAME],
     outsdfile_unscaled[MAXNAME],outsemfile_unscaled[MAXNAME],magnorm_str[2],delay_str[7],*glm_list_str=NULL,
     *str_ptr,unscaled_mag_str[2],firststr[MAXNAME],*one_file_root="mag",onefile[MAXNAME],*sdstr="sd",*semstr="sem",*sdsemstrptr,
     *concfile=NULL,ampstr[9],outphase_unscaled[MAXNAME],*correction=NULL,*confile=NULL,**glmregnames=NULL,string3[MAXNAME],
     string4[MAXNAME],*scratchdir=NULL,*cleanup=NULL,atlas[7]="",*lutf=NULL,*strptr=NULL;
     //*Nimage_name=NULL,*print_mask_file=NULL
int vol=0,i,j,k,l,m,n,o,p,q,r,jj,jjj,kk,ll,mm,nn,j1,j2,c_index,num_contrasts=0,lcmag=0,lcsd=0,lcsem=0,num_tc=0,
    how_many,nglmfiles=0,num_xform_files=0,lcall=0,argc_tc=0,argc_c=0,lccolumns=0,spaces,
    num_regions=0,*roi=NULL,num_delays=3,delay_flag=0,delay,cnorm_index=0,lc_within_subject_sd=0,*A_or_B_or_U=NULL,
    lcaccumulate=0,lccolumns_individual=0,num_region_files=0,*nsubjects=NULL,lcprint_unscaled_tc=0,
    num_tc_frames=0,*tc_frames=NULL,tc_frames_max=0,lcprint_unscaled_mag=0,dont_use_c,lc_largest_mag=0,lcprint_scaled_tc=1, //*tc_frames=0
    *which_delay=NULL,decimals=4,vol_region=0,lc_magxreg=1,num_wfiles=1,lc_one_file=0,lctony=0,*temp_int,
    lcmatlab=0,SunOS_Linux,ntc_names=0,nc_names=0,nreg=0,lc_names_only=0,lcflip=0,*hipass=NULL,sdsemspace=0,lcmsqa=0,
    lcindhipass=0,lcphase=0,lcphase_only=0,Mcolmax,lcdegrees=0,lcHz=0,lcscargle=0,ipairs,bigendian=1,lcinvvar=0,
    lcannalisa=0,gotoboy=0,nstimlen=0,argc_stimlen=0,nglmpersub=0,*glmpersub=NULL,*Mcol,glmi,*kk1=NULL,tci,nroots=0,*nc,ncmax,
    lcvoxels=0,nglm_list_file=0,lczstat=0,lcindbase=0,**indbase_col=NULL,*swapbytesin,num_tc_to_sum_actual,num_regions1,*t_to_zi=NULL,
    *con_frames=NULL,nreg1=0,swapbytes,flag,lcmaxdef=0,lcmaxdefonly=0,lcaccumulateseparate=0,lcaccumulateone=0,argc_tc_weights=0,
    num_tc_weights=0,nframesout=0,*framesout=NULL,*brnidx=NULL,how_many1=0,filetype=IMG,*off=NULL,*nsubjects0=NULL,lcroiIND=0,*superbird=NULL;
    //ami,asdi
long *startb;
size_t l1,*len_glm_names; //*off=NULL
float *temp_float=NULL,min,max,sdmin,sdmax,semmin,semmax,*t4=NULL,fwhm=0,**time_courses_sd=NULL,delay_inc=1,init_delay=0,dt,
      *c,**accumulate_mag=NULL,min_dly,max_dly,sdmin_unscaled,sdmax_unscaled,semmin_unscaled,                                  //min_unscaled,max_unscaled
      semmax_unscaled,*dlymaxmin=NULL,**regavg=NULL,**regsem=NULL,**regavg_see=NULL,**regsem_see=NULL,min_of,max_of,
      **time_courses_sd_unscaled=NULL,minphase_unscaled,maxphase_unscaled,*temp_float2=NULL,*tf0=NULL,
      **tcavgphase_unscaled=NULL,*maxdef=NULL,*maxdeffr=NULL,**accumulate_z=NULL,*regavginvvar=NULL; //*accumulate_sd=NULL
double *mag=NULL,*stat=NULL,*temp_double=NULL,scl=0,*avg=NULL,*avg2=NULL,*avg_zstat=NULL,*avg_cc=NULL,*magmaxmin=NULL,
    *temp_double_unscaled=NULL,*avg_unscaled=NULL,*avg2_unscaled=NULL,*scl_stack=NULL,*sqrt_scl_stack=NULL,**scl_stackvox=NULL,
    *scl_mag_stack=NULL,*weights=NULL,***regmag=NULL,***within_subject_sd=NULL, ***time_courses=NULL,***time_courses_unscaled=NULL,
    *phase_unscaled=NULL,*avgphase_unscaled=NULL,***tcphase_unscaled=NULL,td=0.,td2,twopi,scargle,*pairs=NULL,*pairsun=NULL,
    *mag_unscaled=NULL,***within_subject_sd_unscaled=NULL,scalar=1.,*avgmeantovar,*avginvvar=NULL,*temp_double2=NULL,sign,
    *contrasts=NULL,*conmag=NULL,*conmean=NULL,*consem=NULL,*df=NULL,**time_courses_avg=NULL,**time_courses_avg_unscaled=NULL,TR=0.,
    ***time_courses_fit=NULL,**time_courses_avg_fit=NULL,***time_courses_param=NULL,**time_courses_avg_param=NULL,***voxels=NULL,
    ***voxels_unscaled=NULL,*td1=NULL,*td1un=NULL,***zstat=NULL,***cc=NULL,*td3,***tstat=NULL,***tstatdf=NULL,***B=NULL,***seB=NULL,
    df1,***B1=NULL,*td4=NULL,*dptr,*dptr1=NULL,*td5=NULL,*td6=NULL,*dptr2=NULL,*td7=NULL,*td8=NULL,*td9=NULL,sumdf,*dptr6=NULL,*dptr7,mean0,mean02,
    **tczm=NULL,*tczm_avg=NULL,*tczm_sem=NULL;
FILE *fp,*fprn=NULL,*fprn0=NULL,*fp1;
LinearModel **glmstack;
Interfile_header *ifh,*ifh_out=NULL,*ifh_out1=NULL;
Regions **reg=NULL,*glmreg=NULL;
Regions_By_File *rbf=NULL,**fbf=NULL,**fbfcon=NULL;
TC *tcs=NULL,*stimlens=NULL,*con,*tc_weights=NULL;
Grand_Mean_Struct **gms_stack=NULL;
Tags_Tc_Struct2 *tags_tcs=NULL;
Atlas_Param *ap;
Memory_Map **mm_wfiles=NULL;
Files_Struct *glmfiles=NULL,*xform_files=NULL,*region_files=NULL,*wfiles=NULL,*tc_names=NULL,*c_names=NULL,*bolds,*roots=NULL,*glm_list_file=NULL;
Length_And_Max **lams;
Data *dcorrection=NULL;
Meancol **meancol_stack=NULL;
gauss_smoth2_struct *gs=NULL;
W1 *w1=NULL;

//int64_t dims[3];
//START210122
int64_t dims[4]={0,0,0,1};

void *fpw=NULL,*fpmaxdef=NULL,*fpof=NULL,*fpdly=NULL,*fpwsd=NULL,*fpwsd_unscaled=NULL,*fpwsem_unscaled=NULL,*fpwphase_unscaled=NULL,*fpwsem=NULL;  //*fpw_unscaled=NULL

std::string zmf;

//START210124
//ptrw p0;
//START210913
ptrw p0,p1;


twopi = 2.*(double)M_PI;
if(argc < 5) {
    fprintf(stderr,"    -glm_files:           List of *.glm files whose estimates are to be averaged.\n\n");
    fprintf(stderr,"    -contrasts:           List of contrasts to be analyzed. The first contrast is 1.\n");
    fprintf(stderr,"    -contrasts ALL        Analyze all contrasts.\n");
    fprintf(stderr,"    -c_names:             Optional list of of magnitude, tstat, and/or zstat names. One for each contrast.\n");
    fprintf(stderr,"    -mag                  Store average magnitude images.\n");
    fprintf(stderr,"    -mag ONLY             Compute and store only average magnitude images.\n");
    fprintf(stderr,"    -zstat                Store average zstat images.\n");
    fprintf(stderr,"    -accumulate           Accumulate subject images for each contrast. Each subject is a time point.\n");
    fprintf(stderr,"                          Magnitude and Z statistic images may be accumulated.\n");
    fprintf(stderr,"                          This option is for a voxel level analysis.\n\n");
    fprintf(stderr,"    -accumulate ONE       One image is output for each contrast. Each subject is a timepoint.\n");
    fprintf(stderr,"    -accumulate SEPARATE  Separate image for each subject and contrast.\n");
    fprintf(stderr,"    -tc:                  List of estimates. First estimate is 1. 0 indicates not present.\n");
    fprintf(stderr,"    -tc_names:            Optional list of timecourse names.\n");
    fprintf(stderr,"                          The first estimate is 1.\n");
    fprintf(stderr,"    -sem                  Sandard error of the mean.\n");
    fprintf(stderr,"    -sd                   Standard deviation.\n");
    fprintf(stderr,"    -xform_files:         List of t4 files defining the transform to atlas space.\n");
    fprintf(stderr,"                          If *.glm files are not in atlas space, then one is needed for each *.glm file.\n\n"); 
    fprintf(stderr,"    -atlas:               Either 111, 222 or 333. Used with -xform_files option.\n");
    fprintf(stderr,"    -gauss_smoth:         Amount of smoothing to be done in units of fwhm with a 3D gaussian filter.\n");
    fprintf(stderr,"                          Magnitudes and timecourses are smoothed after being put in atlas space.\n");
    fprintf(stderr,"    -region_file:         4dfp or wmparc.nii.gz\n");
    fprintf(stderr,"                          Magnitudes and timecourses are averaged over the region.\n");
    fprintf(stderr,"    -regions_of_interest: Calculate statistics for selected regions in the region file(s).\n");
    fprintf(stderr,"                          First region is one.\n");
    fprintf(stderr,"                          INDIVIDUAL is used with wmparcs so all the regions in the first wmparc are applied to first subject and so on.\n");
    fprintf(stderr,"    -within_subject_sd    For each region in the region_file, the standard error of the estimate averaged over the region for each subject.\n");
    fprintf(stderr,"    -columns              Put regional timecourses in columns. Only group results are given.\n");
    fprintf(stderr,"    -columns INDIVIDUAL   Put regional timecourses in columns. Group and individual results are given.\n\n");
    fprintf(stderr,"    The following three options are used to compute z-statistics for contrasts with delays.\n");
    fprintf(stderr,"    New contrasts using the Boynton model are computed for each specified delay.\n");
    fprintf(stderr,"    All three options must be included for this processing to take place.\n");
    fprintf(stderr,"    -number_of_delays:               default = 3\n");
    fprintf(stderr,"    -delay_increment_in_seconds:     default = 1\n");
    fprintf(stderr,"    -initial_delay:                  default = 0 (Time-shift from default (2.00 seconds))\n");
    fprintf(stderr,"    -largest_tstat                   Selected delay will yield the largest absolute tstat. This is the default.\n");
    fprintf(stderr,"    -largest_mag                     Selected delay will yield the largest absolute mag.\n\n");


    //START200825
    #if 0
    fprintf(stderr,"    -print_mask_file:     Name of *.4dfp.img file. A mask of sampled voxels is printed to this file.\n");
    fprintf(stderr,"                          A sampled voxel gets a 1. The program exits after printing the mask.\n");
    fprintf(stderr,"                          This option was added to make a blockhead mask for unsampled voxels.\n");
    #endif

    fprintf(stderr,"    -regional_avgstat_name: Output filename for regional statistics.\n");
    
    //START200825
    //fprintf(stderr,"    -Nimage_name:         Output filename for number of subjects at each voxel.\n");
    
    fprintf(stderr,"    -mask:                Voxels outside the mask are set to 0.\n");
    fprintf(stderr,"                          The mask is currently not used to minimize computation.\n\n");
    fprintf(stderr,"    -directory:           Specify output storage path.\n\n");
    fprintf(stderr,"    -group_name:          Group name for subjects. Added to output filename.\n");
    fprintf(stderr,"    -print_unscaled_tc    Print timecourses in MR units.\n");
    fprintf(stderr,"    -dont_print_scaled_tc Only print timecourse in MR_units.\n");
    fprintf(stderr,"    -print_unscaled_mag   Print magnitudes in MR units.\n");
    fprintf(stderr,"    -magnorm              Contrasts are normalized to have a magnitude of 1.\n");
    fprintf(stderr,"                          This is used for ANOVAs and t-tests, since the number of conditions\n");
    fprintf(stderr,"                          in a given contrast can vary from subject to subject.\n");
    fprintf(stderr,"    -regxmag              For regional magnitudes there are two output formats. The default is magnitude by region.\n");
    fprintf(stderr,"                          This option changes the output format to region by magnitude.\n");
    fprintf(stderr,"    -glm_list_file:       If present this goes into the ifh 'name of data file' field of the output files.\n");
    fprintf(stderr,"    -weight_files:        4dfp weight files. Provide weights for regional weighted means.\n");
    fprintf(stderr,"                          Without this option the regional arithmetic mean is computed.\n");
    fprintf(stderr,"    -one_file:            Voxel magnitudes are written to a single 4dfp with default name mag.4dfp.img\n");
    fprintf(stderr,"    -tony                 Regional timecourses are output to a text file in Tony format.\n");
    fprintf(stderr,"    -annalisa             Regional timecourses are output to a text file in annalisa format (ie each timecourse is a column).\n");
    fprintf(stderr,"    -annalisa:            Regional timecourses are output to a text file in annalisa format (ie each timecourse is a column).\n");
    fprintf(stderr,"                          Name of contrast file includes weights for magnitude computation.\n");
    fprintf(stderr,"                              Magnitude computed for each region.\n");
    fprintf(stderr,"                              First column: region, second column: condition, remaining columns: contrast\n");
    fprintf(stderr,"    -matlab               Regional timecourses are output to text files for matlab input.\n");
    fprintf(stderr,"    -flip                 Regional text output for flipper.  region_name mean sd\n");
    fprintf(stderr,"    -msqa                 Mean square amplitued power map. Default is root msqa.\n");
    fprintf(stderr,"    -scargle              Scargle correction. Divide by sum, squared regressor values.\n");
    fprintf(stderr,"    -conc                 Conc used to create GLM. Used to create data set for bayfidl.\n");
    fprintf(stderr,"    -indhipass            Output individual subject frequency power maps.\n");
    fprintf(stderr,"    -phase                Output amplitude and phase frequency power maps.\n");
    fprintf(stderr,"    -phase ONLY           Output only phase frequency power maps.\n");
    fprintf(stderr,"    -degrees              Output phase in degrees. Radians is the default\n");
    fprintf(stderr,"    -correction:          Text file. Each subject is a row. Order must match GLMs.\n");
    fprintf(stderr,"                          Need correction factor for each run. Order must match tc's. Used for phase maps.\n");
    fprintf(stderr,"    -scalar:              All output is multiplied by this value.\n");
    fprintf(stderr,"    -invvar               Scale magnitudes by the inverse of the variance.\n");
    fprintf(stderr,"    -stimlen              Duration ranges to initialize Boynton HRF fit. Ex. 4,8 8,12 12,16\n");
    fprintf(stderr,"    -glmpersub:           Number of glms for each subject. One number per subject.\n");
    fprintf(stderr,"    -roots:               Output roots. One identifier per subject.\n");
    fprintf(stderr,"    -frames:              Number of frames for each timecourse.\n");
    fprintf(stderr,"    -voxels               Output voxel values of regions.\n");
    fprintf(stderr,"    -indbase              Individual baseline applied to each effect for normalization to percent change.\n");
    fprintf(stderr,"    -maxdef               Output maximum deflection for each timecourse.\n");
    fprintf(stderr,"    -maxdefonly           Only output maximum deflection for each timecourse.\n");
    fprintf(stderr,"    -tc_weights           Weighting to apply to summed timepoints (eg contrast).\n");
    fprintf(stderr,"    -scratchdir:          Scratch directory. Include the slash at the end.\n");
    fprintf(stderr,"    -names_only           Generate filenames. No computation is performed. No files are created.\n");
    fprintf(stderr,"    -cleanup:             Remove this directory.\n");
    fprintf(stderr,"    -lut:                 Lookup table. Ex. $FREESURFER_HOME/FreeSurferColorLUT.txt\n");
    exit(-1);
    }
magnorm_str[0] = 0; /*This must be up here.*/
delay_str[0] = 0;
appendstr[0] = 0;
unscaled_mag_str[0] = 0;
ampstr[0] = 0;
for(i=1;i<argc;i++) { 
    if(!strcmp(argv[i],"-glm_files") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nglmfiles;
        if(!(glmfiles=get_files(nglmfiles,&argv[i+1]))) exit(-1);
        i += nglmfiles;
        }
    if(!strcmp(argv[i],"-contrasts") && argc > i+1) {
        if(argc > i+1 && !strcmp(argv[i+1],"ALL")) {
            lcall = 1;
            ++i;
            }
        else {
            for(j=1;i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_contrasts;
	    argc_c = i+1;
            i += num_contrasts;
            }
        }
    if(!strcmp(argv[i],"-c_names") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nc_names;
        if(!(c_names=get_files(nc_names,&argv[i+1]))) exit(-1);
        i += nc_names;
        }
    if(!strcmp(argv[i],"-mag"))
        lcmag = 1;
    if(!strcmp(argv[i],"-zstat"))
        lczstat = 1;
    if(!strcmp(argv[i],"-accumulate")) {
        lcaccumulate = 1;
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) {
            if(!strcmp(argv[i+j],"SEPARATE")) lcaccumulateseparate=1; else if(!strcmp(argv[i+j],"ONE")) lcaccumulateone=1;
            }
        i += j-1;
        } 
    if(!strcmp(argv[i],"-tc") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_tc;
	argc_tc = i+1;
        i += num_tc;
        }
    if(!strcmp(argv[i],"-tc_weights") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_tc_weights;
	argc_tc_weights = i+1;
        i += num_tc_weights;
        }
    if(!strcmp(argv[i],"-tc_names") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++ntc_names;
        if(!(tc_names=get_files(ntc_names,&argv[i+1]))) exit(-1);
        i += ntc_names;
        }
    if(!strcmp(argv[i],"-sem"))
        lcsem = 1;
    if(!strcmp(argv[i],"-sd"))
        lcsd = 1;
    if(!strcmp(argv[i],"-xform_files") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_xform_files;
        if(!(xform_files=get_files(num_xform_files,&argv[i+1]))) exit(-1);
        i += num_xform_files;
        }
    if(!strcmp(argv[i],"-gauss_smoth") && argc > i+1) {
        fwhm = atof(argv[++i]);
        sprintf(fwhmstr,"_fwhm%.1f",fwhm);
        }
    if(!strcmp(argv[i],"-region_file") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_region_files;
        if(!(region_files=read_files(num_region_files,&argv[i+1]))) exit(-1);
        i += num_region_files;
        }
    if(!strcmp(argv[i],"-regions_of_interest") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_regions;
        if(num_regions){
            if(!strcmp(argv[i+1],"INDIVIDUAL")){
                lcroiIND=1;++i;
                }
            else{
                if(!(roi=(int*)malloc(sizeof*roi*num_regions))) {
                    printf("fidlError: Unable to malloc roi\n");
                    exit(-1);
                    }
                for(j=0;j<num_regions;j++) roi[j] = atoi(argv[++i]) - 1;
                }
            }
        }
    if(!strcmp(argv[i],"-within_subject_sd"))
        lc_within_subject_sd = 1;
    if(!strcmp(argv[i],"-columns")) {
        lccolumns = 1;
        if(argc > i+1 && !strcmp(argv[i+1],"INDIVIDUAL")) {
            lccolumns_individual = 1;
            ++i;
            }
        }
    if(!strcmp(argv[i],"-number_of_delays")) {
        ++delay_flag;
        if(argc > i+1 && strchr(argv[i+1],'-') != argv[i+1]) num_delays = atoi(argv[++i]);
        }
    if(!strcmp(argv[i],"-delay_increment_in_seconds")) {
        ++delay_flag;
        if(argc > i+1 && strchr(argv[i+1],'-') != argv[i+1]) delay_inc = atoi(argv[++i]);
        }
    if(!strcmp(argv[i],"-initial_delay")) {
        ++delay_flag;
        /*if(argc > i+1 && strchr(argv[i+1],'-') != argv[i+1]) init_delay = atof(argv[++i]);*/
        if(argc > i+1) {
            k = strlen(argv[i+1]);
            for(m=j=0;j<k;j++) if(isdigit(argv[i+1][j])) m = 1; /*printf("%c\n",argv[i+1][j]);*/
            if(m) init_delay = atof(argv[++i]);
            } 
        }
    if(!strcmp(argv[i],"-largest_mag"))
        lc_largest_mag = 1;

    //START200825
    //if(!strcmp(argv[i],"-print_mask_file") && argc > i+1)
    //    print_mask_file = argv[++i];

    if(!strcmp(argv[i],"-regional_avgstat_name") && argc > i+1)
        regional_avgstat_name = argv[++i];

    //START200825
    //if(!strcmp(argv[i],"-Nimage_name") && argc > i+1)
    //    Nimage_name = argv[++i];

    if(!strcmp(argv[i],"-mask") && argc > i+1)
        maskfile = argv[++i];
    if(!strcmp(argv[i],"-directory") && argc > i+1)
        directory = argv[++i];
    if(!strcmp(argv[i],"-group_name") && argc > i+1)
        group_name = argv[++i];
    if(!strcmp(argv[i],"-print_unscaled_tc"))
        lcprint_unscaled_tc = 1;
    if(!strcmp(argv[i],"-dont_print_scaled_tc"))
        lcprint_scaled_tc = 0;
    if(!strcmp(argv[i],"-print_unscaled_mag")) {
        lcprint_unscaled_mag = 1;
        strcpy(unscaled_mag_str,"U");
        }
    if(!strcmp(argv[i],"-magnorm"))
        strcpy(magnorm_str,"1");
    if(!strcmp(argv[i],"-atlas") && argc > i+1)
        strcpy(atlas,argv[++i]);
    if(!strcmp(argv[i],"-regxmag"))
        lc_magxreg = 0;
    if(!strcmp(argv[i],"-glm_list_file") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nglm_list_file;
        if(!(glm_list_file=get_files(nglm_list_file,&argv[i+1]))) exit(-1);
        i += nglm_list_file;
        }
    if(!strcmp(argv[i],"-weight_files") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_wfiles;
        num_wfiles--; /*Initialized to 1 instead of 0 in declaration.*/
        if(!(wfiles=get_files(num_wfiles,&argv[i+1]))) exit(-1);
        i += num_wfiles;
        }
    if(!strcmp(argv[i],"-one_file")) {
        lc_one_file = 1;
        if(argc > i+1 && strchr(argv[i+1],'-') != argv[i+1]) one_file_root = argv[++i];
        }
    if(!strcmp(argv[i],"-tony"))
        lctony = 1;
    if(!strcmp(argv[i],"-annalisa")) {
        lcannalisa = 1;
        if(argc > i+1 && strchr(argv[i+1],'-') != argv[i+1]) confile = argv[++i];
        }
    if(!strcmp(argv[i],"-matlab"))
        lcmatlab = 1;
    if(!strcmp(argv[i],"-flip"))
        lcflip = 1;
    if(!strcmp(argv[i],"-msqa"))
        lcmsqa = 1;
    if(!strcmp(argv[i],"-scargle"))
        lcscargle = 1;
    if(!strcmp(argv[i],"-conc") && argc > i+1)
        concfile = argv[++i];
    if(!strcmp(argv[i],"-indhipass"))
        lcindhipass = 1;
    if(!strcmp(argv[i],"-phase")) {
        lcphase = 1;
        if(argc > i+1 && !strcmp(argv[i+1],"ONLY")) {
            lcphase_only = 1;
            ++i;
            }
        }
    if(!strcmp(argv[i],"-degrees"))
        lcdegrees = 1;
    if(!strcmp(argv[i],"-correction") && argc > i+1)
        correction = argv[++i];
    if(!strcmp(argv[i],"-scalar") && argc > i+1)
        scalar = atof(argv[++i]);
    if(!strcmp(argv[i],"-invvar"))
        lcinvvar = 1;
    if(!strcmp(argv[i],"-stimlen") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nstimlen;
        argc_stimlen = i+1;
        i += nstimlen;
        }
    if(!strcmp(argv[i],"-glmpersub") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nglmpersub;
        if(!(glmpersub=(int*)malloc(sizeof*glmpersub*nglmpersub))) {
            printf("fidlError: Unable to malloc glmpersub\n");
            exit(-1);
            }
        for(j=0;j<nglmpersub;j++) glmpersub[j]=atoi(argv[++i]);
        }
    if(!strcmp(argv[i],"-roots") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nroots;
        if(!(roots=get_files(nroots,&argv[i+1]))) exit(-1);
        i += nroots;
        }
    if(!strcmp(argv[i],"-frames") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_tc_frames;
        if(!(tc_frames=(int*)malloc(sizeof*tc_frames*num_tc_frames))) {
            printf("fidlError: Unable to malloc tc_frames\n");
            exit(-1);
            }
        for(j=0;j<num_tc_frames;j++) if((tc_frames[j]=atoi(argv[++i]))>tc_frames_max) tc_frames_max=tc_frames[j];
        }
    if(!strcmp(argv[i],"-voxels"))
        lcvoxels = 1;
    if(!strcmp(argv[i],"-indbase"))
        lcindbase = 1;
    if(!strcmp(argv[i],"-maxdef"))
        lcmaxdef = 1;
    if(!strcmp(argv[i],"-maxdefonly"))
        lcmaxdefonly = 1;
    if(!strcmp(argv[i],"-scratchdir") && argc > i+1)
        scratchdir = argv[++i];
    if(!strcmp(argv[i],"-names_only"))
        lc_names_only = 1;
    if(!strcmp(argv[i],"-framesout") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nframesout;
        if(!(framesout=(int*)malloc(sizeof*framesout*nframesout))) {
            printf("fidlError: Unable to malloc framesout\n");
            exit(-1);
            }
        for(j=0;j<nframesout;j++) framesout[j]=atoi(argv[++i]);
        }
    if(!strcmp(argv[i],"-cleanup") && argc > i+1)
        cleanup = argv[++i];
    if(!strcmp(argv[i],"-lut")) {
        if(argc > i+1 && strchr(argv[i+1],'-') != argv[i+1]) {
            lutf = argv[++i];
            }
        else {
            printf("fidlError: No lookup table specified after -lut option. Abort!\n");
            exit(-1);
            }
        }
    }
//if(!lc_names_only) print_version_number(rcsid,stdout);
if(ntc_names) {if(num_tc_frames>ntc_names) num_tc_frames=ntc_names;}
if((SunOS_Linux=checkOS())==-1) exit(-1);
swapbytes = shouldiswap(SunOS_Linux,bigendian);
if(scratchdir) swapbytes=0;
if(scratchdir&&directory) {
    printf("fidlError: You have specified both -scratchdir and -directory. Use only one.\n");fflush(stdout);
    exit(-1);
    }
if(!num_contrasts&&!num_tc) {
    fprintf(stdout,"No contrasts or timecourses were specified.\n");
    exit(-1);
    }
if(!nglmpersub) {
    nglmpersub = nglmfiles;
    if(!(glmpersub=(int*)malloc(sizeof*glmpersub*nglmpersub))) {
        printf("fidlError: Unable to malloc glmpersub\n");
        exit(-1);
        }
    for(i=0;i<nglmpersub;i++) glmpersub[i]=1;
    }

//START170522
#if 0
else if(nglmpersub!=nglmfiles){
    printf("fidlError: nglmpersub=%d nglmfiles=%d Must be equal. Abort!\n",nglmpersub,nglmfiles);fflush(stdout);exit(-1);
    }
#endif


if(lcsem && lcsd) {
    printf("fidlError: Not set up to compute both -sd and -sem\n");
    exit(-1);
    }
if(correction) if(!(dcorrection=read_data(correction,1,0,0,0))) exit(-1);
sdsemstrptr = lcsd ? sdstr : semstr;
if(lcsd) sdsemspace++;
firststr[0] = 0;
if(directory) strcat(firststr,directory);
if(glm_list_str) {
    strcat(firststr,glm_list_str);
    strcat(firststr,"_");
    }
if(group_name) {
    strcat(firststr,group_name);
    strcat(firststr,"_");
    }
if(!(glmstack=(LinearModel**)malloc(sizeof*glmstack*nglmfiles))) {
    printf("fidlError: Unable to malloc glmstack\n");
    exit(-1);
    }
if(!lcindbase) {
    if(!(gms_stack=(Grand_Mean_Struct**)malloc(sizeof*gms_stack*nglmfiles))) {
        printf("fidlError: Unable to malloc gms_stack\n");
        exit(-1);
        }
    }
else {
    if(!(meancol_stack=(Meancol**)malloc(sizeof*meancol_stack*nglmfiles))) {
        printf("fidlError: Unable to malloc meancol_stack\n");
        exit(-1);
        }
    }
if(!(swapbytesin=(int*)malloc(sizeof*swapbytesin*nglmfiles))) {
    printf("fidlError: Unable to malloc swapbytesin\n");
    exit(-1);
    }
if(!(startb=(long*)malloc(sizeof*startb*nglmfiles))) {
    printf("fidlError: Unable to malloc startb\n");
    exit(-1);
    }
if(!(Mcol=(int*)malloc(sizeof*Mcol*nglmfiles))) {
    printf("fidlError: Unable to malloc Mcol\n");
    exit(-1);
    }
if(!(nc=(int*)malloc(sizeof*nc*nglmfiles))) {
    printf("fidlError: Unable to malloc nc\n");
    exit(-1);
    }
if((i=nglmfiles)<num_contrasts) i=num_contrasts;
if(i<num_tc) i=num_tc;
if(!(temp_int=(int*)malloc(sizeof*temp_int*i))) {
    printf("fidlError: Unable to malloc temp_int\n");
    exit(-1);
    }
n = lc_names_only ? 1 : nglmfiles;
for(ncmax=Mcolmax=0,k=m=0;m<n;m++) {
    if(!lc_names_only) printf("Reading %s\n",glmfiles->files[m]);
    if(!(glmstack[m] = read_glm(glmfiles->files[m],(int)SMALL,SunOS_Linux)))exit(-1);
    if(!m) {
        vol = glmstack[m]->ifh->glm_xdim*glmstack[m]->ifh->glm_ydim*glmstack[m]->ifh->glm_zdim;
        }
    else if(vol != glmstack[m]->ifh->glm_xdim*glmstack[m]->ifh->glm_ydim*glmstack[m]->ifh->glm_zdim) {
        temp_int[k++] = m;
        }
    if(!lcindbase) {

        //if(!(gms_stack[m]=get_grand_mean_struct(glmstack[m]->grand_mean,vol,glmfiles->files[m],stdout))) exit(-1);
        //START160928
        if(!(gms_stack[m]=get_grand_mean(glmstack[m],glmfiles->files[m],stdout)))exit(-1);

        }
    else {
        if(!(meancol_stack[m]=get_meancol(glmstack[m]->ifh,0))) exit(-1);
        }
    free(glmstack[m]->grand_mean);
    startb[m] = (long)find_b(glmstack[m]);
    if((Mcol[m]=glmstack[m]->ifh->glm_Mcol)>Mcolmax) Mcolmax = glmstack[m]->ifh->glm_Mcol;

    //std::cout<<"here0 glmstack["<<m<<"]->ifh->glm_nc="<<glmstack[m]->ifh->glm_nc<<std::endl;

    if((nc[m]=glmstack[m]->ifh->glm_nc)>ncmax) ncmax = glmstack[m]->ifh->glm_nc;
    swapbytesin[m] = shouldiswap(SunOS_Linux,glmstack[m]->ifh->bigendian);
    }
if(!lc_names_only) {
    if((nreg=glmstack[0]->ifh->nregions)) {
        glmregnames = glmstack[0]->ifh->region_names;

        //if(!(glmreg=get_reg((char*)NULL,0,(float*)NULL,nreg,glmregnames,0,0)))exit(-1);
        //START200221
        if(!(glmreg=get_reg((char*)NULL,0,(float*)NULL,nreg,glmregnames,0,lc_names_only,0,(int*)NULL,lutf)))exit(-1);

        }
    if(k) {
        printf("fidlError: GLMs all need to be in the same space. Abort!\n");
        for(m=0;m<k;m++) {
            i = temp_int[m];
            printf("fidlError: %s xdim=%d ydim=%d zdim=%d\n",glmfiles->files[i],glmstack[i]->ifh->glm_xdim,
                glmstack[i]->ifh->glm_ydim,glmstack[i]->ifh->glm_zdim);
            }
        printf("fidlError: all the rest have dimensions xdim=%d ydim=%d zdim=%d\n",glmstack[0]->ifh->glm_xdim,
            glmstack[0]->ifh->glm_ydim,glmstack[0]->ifh->glm_zdim);
        exit(-1);
        }
    TR = (double)glmstack[0]->ifh->glm_TR;
    filetype=get_filetype2(glmstack[0]->ifh->glm_cifti_xmlsize,glmstack[0]->ifh->glm_boldtype);
    }
strcpy(delay_type_str,!lc_largest_mag?"LT":"LM");
if(delay_flag == 3) sprintf(delay_str,"_d%d%s",num_delays,delay_type_str);
if(num_region_files) {

    #if 0
    if(!(ifh = read_ifh(region_files->files[0],(Interfile_header*)NULL))) { 
        if(!(region_files2=read_conc(1,&region_files->files[0]))) exit(-1);
        free_files_struct(region_files);
        region_files=region_files2;
        if(!(ifh = read_ifh(region_files->files[0],(Interfile_header*)NULL))) exit(-1);
        for(l1=0;l1<region_files->nfiles;l1++) printf("%zd %s\n",l1+1,region_files->files[l1]);
        }
    vol_region = ifh->dim1*ifh->dim2*ifh->dim3;
    free_ifh(ifh,0);
    #endif
    //START200221
    header h0;
    if(!(vol_region=h0.header0(region_files->files[0])))exit(-1); 

    if(!check_dimensions(region_files->nfiles,region_files->files,vol_region)) exit(-1);
    if(!(reg=(Regions**)malloc(sizeof*reg*region_files->nfiles))) {
        printf("fidlError: Unable to malloc reg\n");
        exit(-1);
        }

    for(l1=0;l1<region_files->nfiles;l1++){

        //if(!(reg[l1]=get_reg(region_files->files[l1],0,(float*)NULL,0,(char**)NULL,0,0))) exit(-1);
        //START200224
        if(!(reg[l1]=get_reg(region_files->files[l1],0,(float*)NULL,0,(char**)NULL,0,lc_names_only,0,(int*)NULL,lutf))) exit(-1);

        //std::cout<<"reg["<<l1<<"]->nregions="<<reg[l1]->nregions<<std::endl;
        }

    #if 0
    for(num_regions1=l1=0;l1<region_files->nfiles;l1++) num_regions1 += (int)reg[l1]->nregions;
    if(num_regions>num_regions1) {
        num_regions = num_regions1;
        }
    else if(!num_regions) {
        num_regions = num_regions1;
        if(!(roi=(int*)malloc(sizeof*roi*num_regions))) {
            printf("fidlError: Unable to malloc roi\n");
            exit(-1);
            }
        for(j=0;j<num_regions;j++) roi[j] = j;
        }
    if(!(rbf=find_regions_by_file_cover(region_files->nfiles,num_regions,reg,roi))) exit(-1);
    #endif
    //START200224
    if(!lcroiIND){ 
        for(num_regions1=l1=0;l1<region_files->nfiles;l1++) num_regions1 += (int)reg[l1]->nregions;
        if(num_regions>num_regions1) {
            num_regions = num_regions1;
            }
        else if(!num_regions) {
            num_regions = num_regions1;
            if(!(roi=(int*)malloc(sizeof*roi*num_regions))) {
                printf("fidlError: Unable to malloc roi\n");
                exit(-1);
                }
            for(j=0;j<num_regions;j++) roi[j] = j;
            }
        }
    else{
        for(num_regions=reg[0]->nregions,l1=1;l1<region_files->nfiles;l1++){
            if(reg[l1]->nregions!=num_regions){
                std::cout<<"fidlError: reg["<<l1<<"]->nregions="<<reg[l1]->nregions<<" num_regions="<<num_regions<<" Must be the same. Abort!"<<std::endl;
                }
            }
        superbird = new int[num_regions](); //empty parentheses zero intializes
        for(l1=1;l1<region_files->nfiles;l1++){
            for(i=0;i<reg[l1]->nregions;++i)if(reg[l1]->nvoxels_region[i])superbird[i]++;
            }
        }

    //if(!(rbf=find_regions_by_file_cover(!lcroiIND?region_files->nfiles:0,!lcroiIND?num_regions:0,reg,roi))) exit(-1);
    //START210910
    if(!(rbf=find_regions_by_file_cover(!lcroiIND?region_files->nfiles:0,!lcroiIND?num_regions:0,reg,roi,NULL))) exit(-1);

    std::cout<<"num_regions="<<num_regions<<std::endl;
 

    if(!(weights=(double*)malloc(sizeof*weights*vol_region))) {
        printf("fidlError: Unable to malloc weights\n");
        exit(-1);
        }
    if(wfiles) {
        if(!check_dimensions(num_wfiles,wfiles->files,vol_region)) exit(-1);
        if(!(mm_wfiles=(Memory_Map**)malloc(sizeof*mm_wfiles*num_wfiles))) {
            printf("fidlError: Unable to malloc mm_wfiles\n");
            exit(-1); 
            }
        for(i=0;i<num_wfiles;i++) if(!(mm_wfiles[i] = map_disk(wfiles->files[i],vol_region,0,sizeof(float)))) exit(-1);
        }
    else {
        for(i=0;i<vol_region;i++) weights[i] = 1;
        }
    }
if(!num_xform_files) {
    get_atlas(vol,atlas);
    }     
else {
    if(num_region_files) get_atlas(vol_region,atlas);
    if(!(t4=(float*)malloc(sizeof*t4*(size_t)T4SIZE*nglmfiles))) {
        printf("fidlError: Unable to malloc t4\n");
        exit(-1);
        }
    if(!(A_or_B_or_U=(int*)malloc(sizeof*A_or_B_or_U*nglmfiles))) {
        printf("fidlError: Unable to malloc A_or_B_or_U\n");
        exit(-1);
        }
    for(i=0;i<nglmfiles;i++) {
        if(!read_xform(xform_files->files[i],&t4[i*(int)T4SIZE])) exit(-1); 
        if((A_or_B_or_U[i]=twoA_or_twoB(xform_files->files[i])) == 2) exit(-1); 
        }
    }

//if(!(ap=get_atlas_param(atlas,(Interfile_header*)NULL))) exit(-1);
//START161214
//if(!(ap=get_atlas_param(atlas,!strstr(atlas,"MNI")?(Interfile_header*)NULL:glmstack[0]->ifh)))exit(-1);
//START181109
if(!(ap=get_atlas_param(atlas,!strstr(atlas,"MNI")?(Interfile_header*)NULL:glmstack[0]->ifh,(char*)NULL)))exit(-1);

if(!atlas[0]) {
    ap->vol = vol;
    ap->xdim = glmstack[0]->ifh->glm_xdim;
    ap->ydim = glmstack[0]->ifh->glm_ydim;
    ap->zdim = glmstack[0]->ifh->glm_zdim;
    ap->voxel_size[0] = glmstack[0]->ifh->glm_dxdy;
    ap->voxel_size[1] = glmstack[0]->ifh->glm_dxdy;
    ap->voxel_size[2] = glmstack[0]->ifh->glm_dz;
    }
else {
    if(!num_region_files) strcpy(appendstr,ap->str);
    }
how_many = ap->vol;
if(fwhm>0.) strcat(appendstr,fwhmstr);
if(lcall) num_contrasts = glmstack[0]->ifh->glm_nc;
for(i=0;i<(num_contrasts>num_tc?num_contrasts:num_tc);i++) temp_int[i] = nglmpersub;
if(num_contrasts) {
    if(!(con=read_tc_string_new(num_contrasts,temp_int,argc_c,argv))) exit(-1);
    if(!(fbfcon=(Regions_By_File**)malloc(sizeof*fbf*nglmpersub))) {
        printf("fidlError: Unable to malloc fbfcon\n");
        exit(-1);
        }
    for(k=j=i=0;i<nglmpersub;j+=glmpersub[i++]) {

        //std::cout<<"here3a glmpersub["<<i<<"]="<<glmpersub[i]<<std::endl;
        //std::cout<<"here3a con->total_each["<<i<<"]="<<con->total_each[i]<<std::endl;
        //std::cout<<"here3a nc["<<j<<"]="<<nc[j]<<std::endl;
        //std::cout<<"here3a con->total_eachi["<<k<<"]="<<con->total_eachi[k]<<std::endl;

        //if(!(fbfcon[i]=find_regions_by_file(glmpersub[i],con->total_each[i],&nc[j],&con->total_eachi[k]))) exit(-1);
        //START210910
        if(!(fbfcon[i]=find_regions_by_file(glmpersub[i],con->total_each[i],&nc[j],&con->total_eachi[k],NULL))) exit(-1);

        k+=con->total_each[i];
        }
    if(!(tags=(char**)malloc(sizeof*tags*num_contrasts))) {
        printf("fidlError: Unable to malloc tags\n");
        exit(-1);
        }
    for(k=0;k<num_contrasts;k++) {
        glmi = fbfcon[0]->file_index[k];
        tags[k] = glmstack[glmi]->contrast_labels[fbfcon[0]->roi_index[k]];
        }
    if(lcvoxels) {
        if(!(con_frames=(int*)malloc(sizeof*con_frames*num_contrasts))) {
            printf("fidlError: Unable to malloc con_frames\n");
            exit(-1);
            }
        for(i=0;i<num_contrasts;i++) con_frames[i]=1;
        }
    }
if(num_tc) {
    if(!(tcs=read_tc_string_new2(num_tc,argc_tc,argv,1))) exit(-1);
    #if 0
    printf("tcs->num_tc=%d num_tc=%d nglmpersub=%d\n",tcs->num_tc,num_tc,nglmpersub);
    printf("tcs->num_tc_to_sum=\n"); 
    for(j=0;j<nglmpersub;j++) {
        printf("sub%d\t",j+1); for(i=0;i<tcs->num_tc;i++) printf("%d ",tcs->num_tc_to_sum[i][j]); printf("\n");
        }
    printf("tcs->tc=\n");
    for(i=0;i<tcs->num_tc;i++) {
        for(j=0;j<tcs->num_tc_to_sum[i][0];j++) printf("%d ",(int)tcs->tc[i][0][j]);
        printf("\n");
        }
    printf("tcs->total=%d\n",tcs->total);
    fflush(stdout);
    #endif
    if(!(fbf=(Regions_By_File**)malloc(sizeof*fbf*nglmpersub))) {
        printf("fidlError: Unable to malloc fbf\n");
        exit(-1);
        }
    for(k=j=i=0;i<nglmpersub;j+=glmpersub[i++]) {

        //if(!(fbf[i]=find_regions_by_file(glmpersub[i],tcs->total_each[i],&Mcol[j],&tcs->total_eachi[k]))) exit(-1);
        //START210910
        if(!(fbf[i]=find_regions_by_file(glmpersub[i],tcs->total_each[i],&Mcol[j],&tcs->total_eachi[k],NULL))) exit(-1);

        k+=tcs->total_each[i];
        }
    if(num_tc_weights) {
        if(!(tc_weights=read_tc_string_new2(num_tc_weights,argc_tc_weights,argv,0))) exit(-1);
        #if 0
        printf("tc_weights->num_tc=%d num_tc_weights=%d\n",tc_weights->num_tc,num_tc_weights);
        printf("tc_weights->num_tc_to_sum=\n");
        for(j=0;j<nglmpersub;j++) {
            printf("sub%d\t",j+1); for(i=0;i<tc_weights->num_tc;i++) printf("%d ",tc_weights->num_tc_to_sum[i][j]); printf("\n");
            }
        printf("tc_weights->tc=\n");
        for(i=0;i<tc_weights->num_tc;i++) {
            for(j=0;j<tc_weights->num_tc_to_sum[i][0];j++) printf("%f ",tc_weights->tc[i][0][j]);
            printf("\n");
            }
        printf("tc_weights->total=%d\n",tc_weights->total);fflush(stdout);
        #endif
        if(tc_weights->num_tc!=tcs->num_tc) {
            printf("fidlError: tc_weights->num_tc=%d tcs->num_tc=%d  They must be equal. Abort!\n",tc_weights->num_tc,tcs->num_tc);
            exit(-1);
            }
        for(j=0;j<nglmpersub;j++) {
            for(i=0;i<tc_weights->num_tc;i++) {
                if(tc_weights->num_tc_to_sum[i][j]!=tcs->num_tc_to_sum[i][j]) {
                    printf("fidlError: tc_weights->num_tc_to_sum[%d][%d]=%d tcs->num_tc_to_sum[%d][%d]=%d Must be equal. Abort!\n",
                        i,j,tc_weights->num_tc_to_sum[i][j],i,j,tcs->num_tc_to_sum[i][j]);
                    }
                }
            }
        }
    if(!(tags_tcs=get_tags_tc_glmstack(tcs,glmpersub[0],glmstack,!num_tc_weights?(TC*)NULL:tc_weights,fbf[0]))) exit(-1);
    #if 0
    printf("tags_tcs->tags_tcptr=\n"); for(i=0;i<tcs->num_tc;i++) printf("%s\n",tags_tcs->tags_tcptr[i]);
    if(tags_tcs->tags_tc_effects==NULL) printf("YES\n"); 
    else printf("tags_tcs->tags_tc_effectsptr=\n"); for(i=0;i<tcs->num_tc;i++) printf("%s\n",tags_tcs->tags_tc_effectsptr[i]);
    fflush(stdout);
    #endif
    if(!(hipass=(int*)malloc(sizeof*hipass*tcs->num_tc))) {
        printf("fidlError: Unable to malloc hipass\n");
        exit(-1);
        }
    for(lcHz=i=0;i<tcs->num_tc;i++) lcHz += hipass[i] = strstr(tags_tcs->tags_tcptr[i],"Hz") ? 1 : 0;
    /*printf("hipass="); for(i=0;i<tcs->num_tc;i++) printf("%d ",hipass[i]); printf("\n");*/
    if(lcHz) strcat(ampstr,lcmsqa?"_msqamp":"_rmsqamp");
    if(nstimlen) {
        if(nstimlen!=num_tc_frames) {
            printf("fidlError:nstimlen=%d num_tc_frames=%d  Must be equal.\n",nstimlen,num_tc_frames);
            exit(-1);
            }
        stimlens = read_tc_string_new(nstimlen,(int*)NULL,argc_stimlen,argv);
        }
    if(!(kk1=(int*)malloc(sizeof*kk1*nglmpersub))) {
        printf("fidlError: Unable to malloc kk1\n");
        exit(-1);
        }
    for(i=0;i<nglmpersub;i++) kk1[i]=0;
    }
free(temp_int);
if(!lc_names_only) {
    if(num_contrasts) {
        if(!(magmaxmin=(double*)malloc(sizeof*magmaxmin*vol))) {
            printf("fidlError: Unable to malloc magmaxmin\n");
            exit(-1);
            }
        if(!(t_to_zi=(int*)malloc(sizeof*t_to_zi*vol))) {
            printf("fidlError: Unable to malloc t_to_zi\n");
            exit(-1);
            }
        }

    #if 0
    if(num_contrasts || lc_within_subject_sd) {
        if(!(mag=malloc(sizeof*mag*ap->vol))) {
            printf("fidlError: Unable to malloc mag\n");
            exit(-1);
            }
        if(!(mag_unscaled=malloc(sizeof*mag_unscaled*ap->vol))) {
            printf("fidlError: Unable to malloc mag_unscaled\n");
            exit(-1);
            }
        }
    #endif
    //START161115
    if(!(mag=(double*)malloc(sizeof*mag*ap->vol))) {
        printf("fidlError: Unable to malloc mag\n");
        exit(-1);
        }
    if(!(mag_unscaled=(double*)malloc(sizeof*mag_unscaled*ap->vol))) {
        printf("fidlError: Unable to malloc mag_unscaled\n");
        exit(-1);
        }


    if(!(stat=(double*)malloc(sizeof*stat*ap->vol))) {
        printf("fidlError: Unable to malloc stat\n");
        exit(-1);
        }
    if(!(temp_float=(float*)malloc(sizeof*temp_float*ap->vol))) {
        printf("fidlError: Unable to malloc temp_float\n");
        exit(-1);
        }
    if(!(temp_double=(double*)malloc(sizeof*temp_double*ap->vol))) {
        printf("fidlError: Unable to malloc temp_double\n");
        exit(-1);
        }
    if(!(temp_double_unscaled=(double*)malloc(sizeof*temp_double_unscaled*ap->vol))) {
        printf("fidlError: Unable to malloc temp_double_unscaled\n");
        exit(-1);
        }
    if(!(td1=(double*)malloc(sizeof*td1*ap->vol))) {
        printf("fidlError: Unable to malloc td1\n");
        exit(-1);
        }
    if(!(td1un=(double*)malloc(sizeof*td1un*ap->vol))) {
        printf("fidlError: Unable to malloc td1un\n");
        exit(-1);
        }

    //START160926
    if(!(td4=(double*)malloc(sizeof*td4*ap->vol))){
        printf("fidlError: Unable to malloc td4\n");
        exit(-1);
        }
    if(!(td5=(double*)malloc(sizeof*td5*ap->vol))){
        printf("fidlError: Unable to malloc td5\n");
        exit(-1);
        }
    //START160927
    if(!(td6=(double*)malloc(sizeof*td6*ap->vol))){
        printf("fidlError: Unable to malloc td6\n");
        exit(-1);
        }

    //START161006
    if(!(td7=(double*)malloc(sizeof*td7*ap->vol))){
        printf("fidlError: Unable to malloc td7\n");
        exit(-1);
        }
    if(!(td8=(double*)malloc(sizeof*td8*ap->vol))){
        printf("fidlError: Unable to malloc td8\n");
        exit(-1);
        }
    if(!(td9=(double*)malloc(sizeof*td9*ap->vol))){
        printf("fidlError: Unable to malloc td9\n");
        exit(-1);
        }
    if(!(df=(double*)malloc(sizeof*df*ap->vol))) {
        printf("fidlError: Unable to malloc df\n");
        exit(-1);
        }
    if(lcphase) {
        if(!(phase_unscaled=(double*)malloc(sizeof*phase_unscaled*ap->vol))) {
            printf("fidlError: Unable to malloc phase_unscaled\n");
            exit(-1);
            }
        if(!(avgphase_unscaled=(double*)malloc(sizeof*avgphase_unscaled*ap->vol))) {
            printf("fidlError: Unable to malloc avgphase_unscaled\n");
            exit(-1);
            }
        }
    if(correction) {
        if(lcprint_scaled_tc) {
            if(!(pairs=(double*)malloc(sizeof*pairs*ap->vol))) {
                printf("fidlError: Unable to malloc pairs\n");
                exit(-1);
                }
            }
        if(lcprint_unscaled_tc) {
            if(!(pairsun=(double*)malloc(sizeof*pairsun*ap->vol))) {
                printf("fidlError: Unable to malloc pairsun\n");
                exit(-1);
                }
            }
        }
    if(!(temp_double2=(double*)malloc(sizeof*temp_double2*ap->vol))) {
        printf("fidlError: Unable to malloc temp_double2\n");
        exit(-1);
        }
    if(lcinvvar) {
        if(!(avgmeantovar=(double*)malloc(sizeof*avgmeantovar*ap->vol))) {
            printf("fidlError: Unable to malloc avgmeantovar\n");
            exit(-1);
            }
        if(!(avginvvar=(double*)malloc(sizeof*avginvvar*ap->vol))) {
            printf("fidlError: Unable to malloc avginvvar\n");
            exit(-1);
            }
        }
    } /*if(!lc_names_only)*/
if(num_region_files) how_many = num_regions;

std::cout<<"how_many="<<how_many<<std::endl;

if(delay_flag < 3) num_delays = 1;
if(!glmstack[0]->ATAm1) {
    if(!(off=(int*)malloc(sizeof*off*Mcolmax))) {
        printf("fidlError: Unable to malloc off\n");
        exit(-1);
        }
    for(off[0]=0,i=1;i<Mcolmax;i++) off[i]=i+off[i-1];

    //printf("off=");for(i=0;i<Mcolmax;i++)printf("%d ",off[i]);printf("\n");fflush(stdout);
    //printf("num_delays=%d vol=%d\n",num_delays,vol);fflush(stdout);

    if(!(scl_stackvox=d2double(num_delays,vol))) {
        printf("fidlError: Unable to malloc scl_stackvox\n");
        exit(-1);
        }
    }
if(num_contrasts) {
    if(!(which_delay=(int*)malloc(sizeof*which_delay*vol))) {
        printf("fidlError: Unable to malloc which_delay\n");
        exit(-1);
        }

    //START160923
    //for(i=0;i<vol;i++) which_delay[i] = 0;

    if(delay_flag>=3) {
        if(!(dlymaxmin=(float*)malloc(sizeof*dlymaxmin*vol))) {
            printf("fidlError: Unable to malloc dlymaxmin\n");
            exit(-1);
            }
        }
    if(glmstack[0]->ATAm1) {
        if(!(scl_stack=(double*)malloc(sizeof*scl_stack*num_delays))) {
            printf("fidlError: Unable to malloc scl_stack\n");
            exit(-1);
            }
        if(!(sqrt_scl_stack=(double*)malloc(sizeof*sqrt_scl_stack*num_delays))) {
            printf("fidlError: Unable to malloc sqrt_scl_stack\n");
            exit(-1);
            }
        }
    if(!(scl_mag_stack=(double*)malloc(sizeof*scl_mag_stack*num_delays))) {
        printf("fidlError: Unable to malloc scl_mag_stack\n");
        exit(-1);
        }
    for(i=0;i<num_delays;i++) scl_mag_stack[i] = 1;
    if(!lc_names_only) {
        if(num_region_files) {
            if(!(regavg = d2float(num_contrasts,num_regions*num_wfiles))) exit(-1);
            if(!(regsem = d2float(num_contrasts,num_regions*num_wfiles))) exit(-1);
            if(lc_within_subject_sd) {
                if(!(within_subject_sd = d3double(num_contrasts,nglmpersub,num_regions*num_wfiles))) exit(-1);
                if(!(regavg_see = d2float(num_contrasts,num_regions*num_wfiles))) exit(-1);
                if(!(regsem_see = d2float(num_contrasts,num_regions*num_wfiles))) exit(-1);
                }
            if(lcinvvar)if(!(regavginvvar=(float*)malloc(sizeof*regavginvvar*num_contrasts*num_regions*num_wfiles))) {
                printf("fidlError: Unable to malloc regavginvvar\n");
                exit(-1);
                }
            }
        if(num_region_files||nreg) {
            if(lcmag) if(!(regmag = d3double(num_contrasts,nglmpersub,how_many*num_wfiles))) exit(-1);
            if(lczstat) {
                if(!nreg&&!lcvoxels) {
                    printf("nreg=%d lcvoxels=%d You need either a regional glm or -voxels\n",nreg,lcvoxels);
                    exit(-1);
                    }
                if(!(zstat=d3double(nreg?how_many*num_wfiles:rbf->nvoxels,nglmpersub,num_contrasts))) exit(-1);
                if(nreg) {
                    if(!(tstat=d3double(nreg?how_many*num_wfiles:rbf->nvoxels,nglmpersub,num_contrasts))) exit(-1);
                    if(!(tstatdf=d3double(nreg?how_many*num_wfiles:rbf->nvoxels,nglmpersub,num_contrasts))) exit(-1);
                    if(!(B=d3double(nreg?how_many*num_wfiles:rbf->nvoxels,nglmpersub,num_contrasts))) exit(-1);
                    if(!(seB=d3double(nreg?how_many*num_wfiles:rbf->nvoxels,nglmpersub,num_contrasts))) exit(-1);
                    if(!(B1=d3double(nreg?how_many*num_wfiles:rbf->nvoxels,nglmpersub,num_contrasts))) exit(-1);
                    if(!(td3=(double*)malloc(sizeof*td3*how_many))) {
                        printf("fidlError: Unable to malloc td3\n");
                        exit(-1);
                        }
                    }
                if(!(cc=d3double(nreg?how_many*num_wfiles:rbf->nvoxels,nglmpersub,num_contrasts))) exit(-1);
                if(lcvoxels) {
                    for(i=0;i<rbf->nvoxels;i++)for(j=0;j<nglmpersub;j++)for(k=0;k<num_contrasts;k++)
                       zstat[i][j][k]=cc[i][j][k]=(double)UNSAMPLED_VOXEL;
                    }
                } 
            }
        else if(lczstat) {
            if(!(avg_zstat=(double*)malloc(sizeof*avg_zstat*how_many))) {
                printf("fidlError: Unable to malloc avg_zstat\n");
                exit(-1);
                }
            if(!(avg_cc=(double*)malloc(sizeof*avg_cc*how_many))) {
                printf("fidlError: Unable to malloc avg_cc\n");
                exit(-1);
                }
            if(lcaccumulate&&!nreg) if(!(accumulate_z=d2float(nglmpersub,how_many))) exit(-1);
            }
        }
    }
if(nroots) {
    len_glm_names = roots->strlen_files;
    glm_names = roots->files;
    spaces = roots->max_length+1;
    }
else {
    if(!(len_glm_names=(size_t*)malloc(sizeof*len_glm_names*nglmfiles))) {
        printf("fidlError: Unable to malloc len_glm_names\n");
        exit(-1);
        }
    for(spaces=m=0;m<nglmfiles;m++) {
        strcpy(string2,glmfiles->files[m]);
        if(!(str_ptr = get_tail_sans_ext(string2))) exit(-1);
        len_glm_names[m] = strlen(str_ptr)+1;
        if((int)len_glm_names[m] > spaces) spaces = (int)len_glm_names[m];
        }
    ++spaces;
    if(!(glm_names=d2charvar(nglmfiles,len_glm_names))) exit(-1);
    for(m=0;m<nglmfiles;m++) {
        strcpy(string2,glmfiles->files[m]);
        if(!(str_ptr = get_tail_sans_ext(string2))) exit(-1);
        strcpy(glm_names[m],str_ptr);
        }
    } 

//START200225
mask ms0,ms_reg0,*ms=NULL,*ms_reg=NULL;
if(!lcroiIND)if(!(ms0.get_mask(maskfile,!rbf?how_many:rbf->nindices_uniqsort,!rbf?(int*)NULL:rbf->indices_uniqsort,(LinearModel*)NULL,!rbf?how_many:ap->vol)))exit(-1);
ms=&ms0;
if(rbf||lcroiIND){if(!(ms_reg0.get_mask(maskfile,how_many,(int*)NULL,(LinearModel*)NULL,how_many)))exit(-1);}
ms_reg=rbf||lcroiIND?&ms_reg0:&ms0;


if(!scratchdir) {

    #if 0
    brnidx = ms->brnidx;
    how_many1 = how_many;
    #endif
    //START200224
    if(ms){brnidx = ms->brnidx;how_many1 = how_many;}



    }
else {
    how_many1 = num_region_files ? num_regions : ms->lenbrain;
    }
if(!lc_names_only) {
    if(!(nsubjects=(int*)malloc(sizeof*nsubjects*how_many*num_wfiles))) {
        printf("fidlError: Unable to malloc nsubjects\n");
        exit(-1);
        }

    //START181109
    if(!(nsubjects0=(int*)malloc(sizeof*nsubjects0*how_many*num_wfiles))) {
        printf("fidlError: Unable to malloc nsubjects0\n");
        exit(-1);
        }


    if(!(avg=(double*)malloc(sizeof*avg*how_many*num_wfiles))) {
        printf("fidlError: Unable to malloc avg\n");
        exit(-1);
        }
    if(!(avg2=(double*)malloc(sizeof*avg2*how_many*num_wfiles))) {
        printf("fidlError: Unable to malloc avg\n");
        exit(-1);
        }
    if(lcprint_unscaled_tc || (num_contrasts && lc_within_subject_sd)) {
        if(!(avg_unscaled=(double*)malloc(sizeof*avg_unscaled*how_many*num_wfiles))) {
            printf("fidlError: Unable to malloc avg_unscaled\n");
            exit(-1);
            }
        if(!(avg2_unscaled=(double*)malloc(sizeof*avg2_unscaled*how_many*num_wfiles))) {
            printf("fidlError: Unable to malloc avg2_unscaled\n");
            exit(-1);
            }
        }
    if(scratchdir) {
        if(!(brnidx=(int*)malloc(sizeof*brnidx*ms->lenbrain))) {
            printf("fidlError: Unable to malloc brnidx\n");
            exit(-1);
            }
        for(i=0;i<ms->lenbrain;i++) brnidx[i]=i;
        }
    }
if(regional_avgstat_name) {
    outfile[0] = 0; /*Doesn't need group_name because user named.*/
    if(directory) strcat(outfile,directory);
    strcat(outfile,regional_avgstat_name);

    //START200303
    zmf.assign(outfile);
    std::size_t found=zmf.find_last_of(".");
    zmf.erase(zmf.begin()+found,zmf.end());
    zmf.append("_zeromean.txt");
    //std::cout<<"here0 zmf = "<<zmf<<std::endl;

    nreg1=nreg;
    if(num_tc) {
        if((num_region_files||regional_avgstat_name) && !lc_names_only) {
            if(concfile) {
                /*do nothing*/
                }
            else if(lcmatlab) {
                /*do nothing*/
                }
            else if(lcflip) {
                /*do nothing*/
                }
            else if(!lctony) {

                std::cout<<"here-1 lccolumns="<<lccolumns<<std::endl;

                if(!lccolumns) gotoboy=1;
                }
            nreg1=0;
            }
        }
    else if(!lcmag) {
        gotoboy=1;
        }

    //if(!lcmatlab && !lc_names_only && !concfile && !gotoboy && !nreg1) if(!(fprn=fopen_sub(outfile,"w"))) exit(-1);
    //START200303
    if(!lcmatlab && !lc_names_only && !concfile && !gotoboy && !nreg1){
        if(!(fprn=fopen_sub(outfile,"w"))) exit(-1);
        if(!(fprn0=fopen_sub((char*)zmf.c_str(),"w"))) exit(-1);
        }

    }
else {
    if(!(ifh_out=init_ifh(4,ap->xdim,ap->ydim,ap->zdim,1,ap->voxel_size[0],ap->voxel_size[1],ap->voxel_size[2],bigendian,(float*)NULL,
        (float*)NULL)))exit(-1);
    ifh_out->mmppix[0] = ap->mmppix[0];
    ifh_out->mmppix[1] = ap->mmppix[1];
    ifh_out->mmppix[2] = ap->mmppix[2];
    ifh_out->center[0] = ap->center[0];
    ifh_out->center[1] = ap->center[1];
    ifh_out->center[2] = ap->center[2];
    if(!scratchdir) {
        ifh_out1 = ifh_out;
        if(!(w1=write1_init()))exit(-1);
        w1->cifti_xmldata=glmstack[0]->cifti_xmldata;
        w1->xmlsize=glmstack[0]->ifh->glm_cifti_xmlsize;
        w1->filetype=filetype;
        w1->how_many=how_many;
        w1->swapbytes=swapbytes;
        if(filetype==(int)NIFTI||filetype==(int)CIFTI||filetype==(int)CIFTI_DTSERIES){
            dims[0]=glmstack[0]->ifh->glm_xdim;dims[1]=glmstack[0]->ifh->glm_ydim;dims[2]=glmstack[0]->ifh->glm_zdim;
            w1->dims=dims;
            w1->center=glmstack[0]->ifh->center;
            w1->mmppix=glmstack[0]->ifh->mmppix;
            }
        w1->ifh=ifh_out;
        w1->lenbrain=0;
        w1->temp_double=NULL;
        }
    else {
        if(!(ifh_out1=init_ifh(4,how_many1,1,1,1,ap->voxel_size[0],ap->voxel_size[1],ap->voxel_size[2],SunOS_Linux?0:1,(float*)NULL,(float*)NULL)))exit(-1);
        ifh_out1->mmppix[0] = ap->mmppix[0];
        ifh_out1->mmppix[1] = ap->mmppix[1];
        ifh_out1->mmppix[2] = ap->mmppix[2];
        ifh_out1->center[0] = ap->center[0];
        ifh_out1->center[1] = ap->center[1];
        ifh_out1->center[2] = ap->center[2];
        }
    }
if(lcaccumulate) {
    if(lcmag) {
        if(!nreg) {
            if(!(accumulate_mag=d2float(nglmpersub,how_many))) exit(-1);
            }

        //START160927
        #if 0
        else {
            if(lc_within_subject_sd)if(!(accumulate_sd=malloc(sizeof*accumulate_sd*num_contrasts*nglmpersub*vol))){
                printf("fidlError: fidl_avg_zstat Unable to malloc accumulate_sd\n");fflush(stdout);exit(-1);}
            }
        #endif

        }
    }
if(lccolumns_individual && !lcmatlab && !lc_names_only && !concfile) { 
    for(m=0;m<nglmpersub;m++)fprintf(fprn,"%d\t%s\n",m+1,glm_names[m]);fprintf(fprn,"\n");

    //START200303
    for(m=0;m<nglmpersub;m++)fprintf(fprn0,"%d\t%s\n",m+1,glm_names[m]);fprintf(fprn0,"\n");
    }
if(!lc_names_only) {
    if(fwhm>0.){if(!(gs=gauss_smoth2_init(ap->xdim,ap->ydim,ap->zdim,fwhm,fwhm))) exit(-1);}
    printf("Finished preprocessing. Starting computation. Please be patient.\n");
    }
if(lc_one_file) {
    if((str_ptr=strstr(one_file_root,".4dfp.img"))) *str_ptr = 0;
    if((str_ptr=strrchr(one_file_root,'/'))) {
        str_ptr++;
        }
    else {
        str_ptr = one_file_root;
        }
    sprintf(onefile,"%s%s%s%s",firststr,str_ptr,appendstr,Fileext[filetype]);
    if(!(fpof=open2(onefile,w1)))exit(-1);
    min_and_max_init(&min_of,&max_of);
    }
if(num_contrasts) {
    if(!lc_names_only) {
        string[0]=0; 
        printf("Computing ");
        if(lcmag) printf("magnitudes%s\n",lczstat?" and Z statistics.":".");
        else printf("Z statistics.\n"); 
        fflush(stdout);
        }
    if(!(c=(float*)malloc(sizeof*c*Mcolmax))) {
        printf("fidlError: Unable to malloc c\n");
        exit(-1);
        }

    //START200825
    //ami=asdi=0;

    for(k=0;k<num_contrasts;k++) {
        for(m=0;m<how_many*num_wfiles;m++) {avg[m]=avg2[m]=0.;nsubjects[m]=0;} 
        if(lcinvvar) for(m=0;m<how_many*num_wfiles;m++) avginvvar[m]=0.; 
        if(avg_zstat) for(m=0;m<how_many;m++) avg_zstat[m]=0.;
        if(avg_cc) for(m=0;m<how_many;m++) avg_cc[m]=0.;
        if(lc_within_subject_sd) for(m=0;m<how_many*num_wfiles;m++) avg_unscaled[m]=avg2_unscaled[m]=0;
        if(delay_flag==3&&!regional_avgstat_name) {
            string2[0] = 0; /*Don't use outfile, screws up for regional analysis.*/
            if(directory) strcat(string2,directory);
            if(!group_name) {
                strcat(string2,"avgstat_");
                }
            else {
                strcat(string2,group_name);
                strcat(string2,"_");
                }
            sprintf(string,"%s_delaysec%s.4dfp.img",tags[k],Fileext[filetype]);
            strcat(string2,string);
            if(nglmpersub > 1) {
                min_dly=1000;max_dly=0;
                if(!(fpdly=open2(string2,w1)))exit(-1);
                }
            }
        for(mm=m=0;m<nglmpersub;mm+=glmpersub[m++]) {
            glmi = mm+fbfcon[m]->file_index[k];
            if(!(fp=fopen_sub(glmfiles->files[glmi],"r"))) exit(-1);
            c_index = fbfcon[m]->roi_index[k]*glmstack[glmi]->ifh->glm_Mcol;
            if(delay_flag == 3) cnorm_index = fbfcon[m]->roi_index[k]*glmstack[glmi]->ifh->glm_tot_eff;
            for(i=0;i<vol;i++){stat[i]=magmaxmin[i]=0.;which_delay[i]=0;}
            if(delay_flag>=3)for(i=0;i<vol;i++)dlymaxmin[i]=0.;
            for(dt=0,delay=0;delay<num_delays;delay++,dt+=delay_inc) {
                dont_use_c = 0;
                if(delay_flag == 3) {
                    dont_use_c = (int)boynton_contrast(glmstack[glmi]->ifh->glm_tot_eff,glmstack[glmi]->ifh->glm_effect_length,
                        glmstack[glmi]->ifh->glm_effect_TR,glmstack[glmi]->ifh->glm_effect_shift_TR,dt,init_delay,
                        glmstack[glmi]->stimlen,&glmstack[glmi]->cnorm[cnorm_index],glmstack[glmi]->ifh->glm_Mcol,c);
                    }
                if(!dont_use_c) {
                    for(i=0;i<glmstack[glmi]->ifh->glm_Mcol;i++) c[i] = glmstack[glmi]->c[c_index+i];
                    }
                if(fseek(fp,startb[glmi],(int)SEEK_SET)) {
                    printf("fidlError: fidl_avg_zstat Occured while seeking to %ld in %s.\n",startb[glmi],glmfiles->files[glmi]);
                    fflush(stdout);exit(-1);
                    }
                for(j=0;j<vol;j++)mag[j]=temp_double[j]=0.;

                #if 0
                for(j=0;j<glmstack[glmi]->ifh->glm_Mcol;j++) {
                    if(c[j]) {
                        if(!fread_sub(temp_float,sizeof(float),(size_t)vol,fp,swapbytesin[glmi],glmfiles->files[glmi])) {
                            printf("fidlError reading parameter estimates from %s.\n",glmfiles->files[glmi]);
                            exit(-1);
                            }
                        for(i=0;i<vol;i++) {
                            if(temp_float[i]==(float)UNSAMPLED_VOXEL) mag[i] = (double)UNSAMPLED_VOXEL;
                            else if(mag[i]!=(double)UNSAMPLED_VOXEL) mag[i] += (double)c[j] * (double)temp_float[i];
                            }
                        }
                    else {
                        offset = (long)(sizeof(float)*vol);
                        if(fseek(fp,offset,(int)SEEK_CUR)) {
                            printf("fidlError: fidl_avg_zstat Occured while seeking to %ld from the current position in %s.\n",offset,
                                glmfiles->files[glmi]);
                            fflush(stdout);exit(-1);
                            }
                        }
                    }
                #endif
                //START160923
                for(j=0;j<glmstack[glmi]->ifh->glm_Mcol;j++){
                    if(c[j]){
                        if(!fread_sub(temp_float,sizeof(float),(size_t)glmstack[glmi]->nmaski,fp,swapbytesin[glmi],
                            glmfiles->files[glmi]))exit(-1);
                        for(i=0;i<glmstack[glmi]->nmaski;i++){
                            if(temp_float[i]==(float)UNSAMPLED_VOXEL)mag[glmstack[glmi]->maski[i]]=(double)UNSAMPLED_VOXEL;
                            else if(mag[glmstack[glmi]->maski[i]]!=(double)UNSAMPLED_VOXEL)
                                mag[glmstack[glmi]->maski[i]]+=(double)c[j]*(double)temp_float[i];
                            }
                        }
                    else{
                        if(fseek(fp,glmstack[glmi]->nmaski*sizeof(float),SEEK_CUR)){
                            printf("fidlError: fidl_avg_zstat occured while seeking to %p in %s.\n",fp,glmfiles->files[glmi]);
                            exit(-1);
                            }
                        }
                    }


                if(glmstack[0]->ATAm1) {

                    #if 0
                    for(scl_stack[delay]=0.,i=0;i<glmstack[glmi]->ifh->glm_Mcol;i++) {
                        if(c[i]) {
                            for(j=0;j<glmstack[glmi]->ifh->glm_Mcol;j++) {
                                scl_stack[delay] += c[i]*glmstack[glmi]->ATAm1[i][j]*c[j];
                                }
                            }
                        }
                    if((sqrt_scl_stack[delay]=sqrt(scl_stack[delay])) <= 0.) {
                        fprintf(stderr,"fidlError: scl_stack[%d] = %f < 0  trouble: program aborted\n",delay,scl_stack[delay]);
                        exit(-1);
                        }
                    for(i=0;i<vol;i++) {
                        temp_double[i]=mag[i]==(double)UNSAMPLED_VOXEL||glmstack[glmi]->sd[i]<=0.?(double)UNSAMPLED_VOXEL:
                            mag[i]/((double)glmstack[glmi]->sd[i]*sqrt_scl_stack[delay]);
                        }
                    if(seB) {
                        for(i=0;i<vol;i++) {
                            seB[i][m][k]=mag[i]==(double)UNSAMPLED_VOXEL||glmstack[glmi]->sd[i]<=0.?(double)UNSAMPLED_VOXEL:
                                (double)glmstack[glmi]->sd[i]*sqrt_scl_stack[delay];
                            }
                        }
                    #endif
                    //START160923
                    for(scl_stack[delay]=0.,i=0;i<glmstack[glmi]->ifh->glm_Mcol;i++){
                        if(c[i])for(j=0;j<glmstack[glmi]->ifh->glm_Mcol;j++)
                            scl_stack[delay]+=(double)c[i]*glmstack[glmi]->ATAm1[i][j]*(double)c[j];
                        }
                    if(scl_stack[delay]<=0.){
                        printf("fidlError: scl_stack[%d] = %f <= 0  trouble: program aborted\n",delay,scl_stack[delay]);
                        exit(-1);
                        }
                    sqrt_scl_stack[delay]=sqrt(scl_stack[delay]);
                    for(i=0;i<glmstack[glmi]->nmaski;i++)temp_double[glmstack[glmi]->maski[i]]=
                        mag[glmstack[glmi]->maski[i]]==(double)UNSAMPLED_VOXEL||glmstack[glmi]->var[i]<=0.?
                        (double)UNSAMPLED_VOXEL:mag[glmstack[glmi]->maski[i]]/sqrt(glmstack[glmi]->var[i]*scl_stack[delay]);
                    if(seB){
                        for(i=0;i<glmstack[glmi]->nmaski;i++)seB[glmstack[glmi]->maski[i]][m][k]=
                            mag[glmstack[glmi]->maski[i]]==(double)UNSAMPLED_VOXEL||glmstack[glmi]->var[i]<=0.?
                                (double)UNSAMPLED_VOXEL:sqrt(glmstack[glmi]->var[i]*scl_stack[delay]);
                        }

                    }
                else {

                    #if 0
                    for(i=0;i<vol;i++) scl_stackvox[delay][i]=0.; 
                    for(i=0;i<glmstack[glmi]->ifh->glm_Mcol;i++) {
                        if(c[i]) {
                            for(j=0;j<vol;j++) scl_stackvox[delay][j] += 
                                c[i]*c[i]*glmstack[glmi]->ATAm1vox[i*glmstack[glmi]->ifh->glm_Mcol+i-off[i]][j];
                            for(j=i+1;j<glmstack[glmi]->ifh->glm_Mcol;j++) {
                                if(c[j]) {
                                    for(l1=0;l1<(size_t)vol;l1++) scl_stackvox[delay][l1] += 
                                        2.*c[i]*c[j]*glmstack[glmi]->ATAm1vox[i*glmstack[glmi]->ifh->glm_Mcol+j-off[i]][l1];
                                    }
                                }
                            }
                        }
                    for(i=0;i<vol;i++) {
                        temp_double[i]=mag[i]!=(double)UNSAMPLED_VOXEL&&glmstack[glmi]->var[i]>0.?
                            mag[i]/sqrt(glmstack[glmi]->var[i]*scl_stackvox[delay][i]):(double)UNSAMPLED_VOXEL;
                        }
                    if(seB){
                        for(i=0;i<vol;i++){
                            seB[i][m][k]=mag[i]==(double)UNSAMPLED_VOXEL||glmstack[glmi]->var[i]>0.?
                                sqrt(glmstack[glmi]->var[i]*scl_stackvox[delay][i]):(double)UNSAMPLED_VOXEL;
                            }
                        }
                    #endif
                    //START160923
                    for(i=0;i<glmstack[glmi]->nmaski;i++)scl_stackvox[delay][glmstack[glmi]->maski[i]]=0.;

                    for(i=0;i<glmstack[glmi]->ifh->glm_Mcol;i++){
                        if(c[i]){
                            for(j=0;j<glmstack[glmi]->nmaski;j++)scl_stackvox[delay][glmstack[glmi]->maski[j]]+=
                                c[i]*c[i]*glmstack[glmi]->ATAm1vox[i*glmstack[glmi]->ifh->glm_Mcol+i-off[i]][glmstack[glmi]->maski[j]];
                            for(j=i+1;j<glmstack[glmi]->ifh->glm_Mcol;j++){
                                if(c[j]){
                                    for(l=0;l<glmstack[glmi]->nmaski;l++)scl_stackvox[delay][glmstack[glmi]->maski[l]]+=2.*c[i]*c[j]*
                                        glmstack[glmi]->ATAm1vox[i*glmstack[glmi]->ifh->glm_Mcol+j-off[i]][glmstack[glmi]->maski[l]];
                                    }
                                }
                            }
                        }
                    for(i=0;i<glmstack[glmi]->nmaski;i++)temp_double[glmstack[glmi]->maski[i]]=
                        mag[glmstack[glmi]->maski[i]]!=(double)UNSAMPLED_VOXEL&&glmstack[glmi]->var[i]>0.?
                        mag[glmstack[glmi]->maski[i]]/sqrt(glmstack[glmi]->var[i]*scl_stackvox[delay][glmstack[glmi]->maski[i]]):
                        (double)UNSAMPLED_VOXEL;
                    if(seB){
                        for(i=0;i<glmstack[glmi]->nmaski;i++)seB[glmstack[glmi]->maski[i]][m][k]=
                            mag[glmstack[glmi]->maski[i]]!=(double)UNSAMPLED_VOXEL||glmstack[glmi]->var[i]>0.?
                            sqrt(glmstack[glmi]->var[i]*scl_stackvox[delay][i]):(double)UNSAMPLED_VOXEL;
                        }
                    }

                #if 0
                if(magnorm_str[0]) {
                    for(scl_mag_stack[delay]=i=0;i<glmstack[glmi]->ifh->glm_Mcol;i++) scl_mag_stack[delay] += c[i]*c[i];
                    if((scl_mag_stack[delay]=sqrt(scl_mag_stack[delay])) <= 0.) {
                        fprintf(stderr,"fidlError: scl_mag_stack[%d] = %f < 0  trouble: program aborted\n",delay,scl_mag_stack[delay]);
                        exit(-1);
                        }
                    }
                #endif
                //START160923
                if(magnorm_str[0]){
                    for(scl_mag_stack[delay]=0.,i=0;i<glmstack[glmi]->ifh->glm_Mcol;i++)scl_mag_stack[delay]+=c[i]*c[i];
                    if(scl_mag_stack[delay]<=0.){
                        printf("fidlError: scl_mag_stack[%d] = %f <= 0  trouble: program aborted\n",delay,scl_mag_stack[delay]);
                        exit(-1);
                        }
                    scl_mag_stack[delay]=sqrt(scl_mag_stack[delay]); 
                    }

                #if 0
                if(delay) {
                    if(!lc_largest_mag) {
                        for(i=0;i<vol;i++) {
                            if(fabs(temp_double[i]) > fabs(stat[i])) {
                                stat[i] = temp_double[i];
                                magmaxmin[i] = mag[i];
                                dlymaxmin[i] = (float)HRF_DELTA + init_delay + dt;
                                which_delay[i] = delay;
                                }
                            }
                        }
                    else {
                        for(i=0;i<vol;i++) {
                            if(fabs(mag[i]) > fabs(magmaxmin[i])) {
                                stat[i] = temp_double[i];
                                magmaxmin[i] = mag[i];
                                dlymaxmin[i] = (float)HRF_DELTA + init_delay + dt;
                                which_delay[i] = delay;
                                }
                            }
                        }
                    }
                else {
                    for(i=0;i<vol;i++) {
                        stat[i] = temp_double[i];
                        magmaxmin[i] = mag[i];
                        }
                    if(delay_flag == 3) for(i=0;i<vol;i++) dlymaxmin[i] = (float)HRF_DELTA + init_delay + dt;
                    }
                #endif
                //START160926
                if(delay){
                    if(!lc_largest_mag){
                        for(i=0;i<glmstack[glmi]->nmaski;i++){
                            if(fabs(temp_double[glmstack[glmi]->maski[i]])>fabs(stat[glmstack[glmi]->maski[i]])){
                                stat[glmstack[glmi]->maski[i]]=temp_double[glmstack[glmi]->maski[i]];
                                magmaxmin[glmstack[glmi]->maski[i]]=mag[glmstack[glmi]->maski[i]];
                                dlymaxmin[glmstack[glmi]->maski[i]]=(float)HRF_DELTA+init_delay+dt;
                                which_delay[glmstack[glmi]->maski[i]]=delay;
                                }
                            }
                        }
                    else{
                        for(i=0;i<glmstack[glmi]->nmaski;i++){
                            if(fabs(mag[glmstack[glmi]->maski[i]])>fabs(magmaxmin[glmstack[glmi]->maski[i]])){
                                stat[glmstack[glmi]->maski[i]]=temp_double[glmstack[glmi]->maski[i]];
                                magmaxmin[glmstack[glmi]->maski[i]]=mag[glmstack[glmi]->maski[i]];
                                dlymaxmin[glmstack[glmi]->maski[i]]=(float)HRF_DELTA+init_delay+dt;
                                which_delay[glmstack[glmi]->maski[i]]=delay;
                                }
                            }
                        }
                    }
                else{
                    for(i=0;i<glmstack[glmi]->nmaski;i++){
                        stat[glmstack[glmi]->maski[i]]=temp_double[glmstack[glmi]->maski[i]];
                        magmaxmin[glmstack[glmi]->maski[i]]=mag[glmstack[glmi]->maski[i]];
                        }
                    if(delay_flag==3)
                        for(i=0;i<glmstack[glmi]->nmaski;i++)dlymaxmin[glmstack[glmi]->maski[i]]=(float)HRF_DELTA+init_delay + dt;
                    }

                } /*for(dt=0,delay=0;delay<num_delays;delay++)*/

            if(lczstat) {

                #if 0
                if(glmstack[0]->ATAm1)
                    for(i=0;i<vol;i++) df[i]=(double)glmstack[glmi]->ifh->glm_df;
                else
                    for(i=0;i<vol;i++) df[i]=(double)glmstack[glmi]->df[i];
                if(tstat)for(i=0;i<vol;i++){tstat[i][m][k]=stat[i];tstatdf[i][m][k]=df[i];B[i][m][k]=magmaxmin[i];}
                #endif
                //START160926
                for(i=0;i<vol;i++)df[i]=0.;
                if(glmstack[0]->ATAm1)
                    for(i=0;i<glmstack[glmi]->nmaski;i++)df[glmstack[glmi]->maski[i]]=(double)glmstack[glmi]->ifh->glm_df;
                else
                    for(i=0;i<glmstack[glmi]->nmaski;i++)df[glmstack[glmi]->maski[i]]=(double)glmstack[glmi]->df[i];
                if(tstat)for(i=0;i<glmstack[glmi]->nmaski;i++){
                    tstat[glmstack[glmi]->maski[i]][m][k]=stat[glmstack[glmi]->maski[i]];
                    tstatdf[glmstack[glmi]->maski[i]][m][k]=df[glmstack[glmi]->maski[i]];
                    B[glmstack[glmi]->maski[i]][m][k]=magmaxmin[glmstack[glmi]->maski[i]];
                    }
                t_to_z(stat,temp_double,vol,df,t_to_zi);

                #if 0
                if(cc||avg_cc) {
                    for(i=0;i<vol;i++)
                        td1[i]=stat[i]==(double)UNSAMPLED_VOXEL?(double)UNSAMPLED_VOXEL:stat[i]/sqrt(stat[i]*stat[i]+df[i]);
                    }
                #endif
                //START160926
                if(cc||avg_cc){
                    for(i=0;i<vol;i++)td1[i]=0.;
                    for(i=0;i<glmstack[glmi]->nmaski;i++)td1[glmstack[glmi]->maski[i]]=
                        stat[glmstack[glmi]->maski[i]]==(double)UNSAMPLED_VOXEL?(double)UNSAMPLED_VOXEL: 
                        stat[glmstack[glmi]->maski[i]]/
                        sqrt(stat[glmstack[glmi]->maski[i]]*stat[glmstack[glmi]->maski[i]]+df[glmstack[glmi]->maski[i]]);
                    dptr1=td1;
                    }

                #if 0
                if(num_xform_files) {
                    if(!t4_atlas(temp_double,stat,&t4[glmi*(int)T4SIZE],glmstack[glmi]->ifh->glm_xdim,glmstack[glmi]->ifh->glm_ydim,
                        glmstack[glmi]->ifh->glm_zdim,glmstack[glmi]->ifh->glm_dxdy,glmstack[glmi]->ifh->glm_dz,A_or_B_or_U[glmi],
                        glmstack[glmi]->ifh->orientation,ap,(double*)NULL)) exit(-1);
                    for(i=0;i<ap->vol;i++) temp_double[i] = stat[i];
                    if(cc||avg_cc) {
                        if(!t4_atlas(td1,stat,&t4[glmi*(int)T4SIZE],glmstack[glmi]->ifh->glm_xdim,glmstack[glmi]->ifh->glm_ydim,
                            glmstack[glmi]->ifh->glm_zdim,glmstack[glmi]->ifh->glm_dxdy,glmstack[glmi]->ifh->glm_dz,A_or_B_or_U[glmi],
                            glmstack[glmi]->ifh->orientation,ap,(double*)NULL)) exit(-1);
                        for(i=0;i<ap->vol;i++) td1[i] = stat[i];
                        }
                    }
                if(fwhm > 0.) {
                    gauss_smoth2(temp_double,stat,gs);
                    for(i=0;i<ap->vol;i++) temp_double[i] = stat[i];
                    }
                if(accumulate_z) for(i=0;i<how_many;i++) accumulate_z[m][i] = (float)temp_double[i]; 
                #endif
                //START160926
                dptr=temp_double;
                if(num_xform_files){
                    if(!t4_atlas(temp_double,stat,&t4[glmi*(int)T4SIZE],glmstack[glmi]->ifh->glm_xdim,glmstack[glmi]->ifh->glm_ydim,
                        glmstack[glmi]->ifh->glm_zdim,glmstack[glmi]->ifh->glm_dxdy,glmstack[glmi]->ifh->glm_dz,A_or_B_or_U[glmi],
                        glmstack[glmi]->ifh->orientation,ap,(double*)NULL))exit(-1);
                    dptr=stat;
                    if(cc||avg_cc){
                        if(!t4_atlas(td1,td4,&t4[glmi*(int)T4SIZE],glmstack[glmi]->ifh->glm_xdim,glmstack[glmi]->ifh->glm_ydim,
                            glmstack[glmi]->ifh->glm_zdim,glmstack[glmi]->ifh->glm_dxdy,glmstack[glmi]->ifh->glm_dz,A_or_B_or_U[glmi],
                            glmstack[glmi]->ifh->orientation,ap,(double*)NULL)) exit(-1);
                        dptr1=td4;
                        }
                    }
                if(fwhm>0.){
                    gauss_smoth2(dptr,td5,gs);
                    dptr=td5;
                    }
                if(accumulate_z)for(i=0;i<ms->lenbrain;i++)accumulate_z[m][ms->brnidx[i]]=(float)dptr[ms->brnidx[i]]; 

                #if 0
                if(avg_zstat) {
                    for(i=0;i<how_many;i++) {
                        if(temp_double[i]!=(double)UNSAMPLED_VOXEL) {
                            avg_zstat[i] += temp_double[i];
                            if(!lcmag) nsubjects[i]++;
                            }
                        }
                    }
                #endif
                //START160926
                if(avg_zstat)for(i=0;i<ms->lenbrain;i++)if(dptr[ms->brnidx[i]]!=(double)UNSAMPLED_VOXEL){
                    avg_zstat[ms->brnidx[i]]+=dptr[ms->brnidx[i]];
                    if(!lcmag)nsubjects[ms->brnidx[i]]++;
                    }

                #if 0
                if(avg_cc) {
                    for(i=0;i<how_many;i++) {
                        if(td1[i]!=(double)UNSAMPLED_VOXEL) {
                            avg_cc[i] += td1[i];
                            if(!lcmag&&!avg_zstat) nsubjects[i]++;
                            }
                        }
                    }
                #endif
                //START160926
                if(avg_cc)for(i=0;i<ms->lenbrain;i++)if(dptr1[ms->brnidx[i]]!=(double)UNSAMPLED_VOXEL){
                    avg_cc[ms->brnidx[i]]+=dptr1[ms->brnidx[i]];
                    if(!lcmag&&!avg_zstat)nsubjects[ms->brnidx[i]]++;
                    }

                #if 0
                if(zstat) {
                    if(nreg) {
                        for(i=0;i<how_many;i++) zstat[i][m][k] = temp_double[i]; 
                        }
                    else {
                        for(i=0;i<rbf->nvoxels;i++) zstat[i][m][k] = temp_double[rbf->indices[i]]; 
                        }
                    }
                if(cc) {
                    if(nreg) {
                        for(i=0;i<how_many;i++) cc[i][m][k] = td1[i]; 
                        }
                    else {
                        for(i=0;i<rbf->nvoxels;i++) cc[i][m][k] = td1[rbf->indices[i]]; 
                        }
                    }
                #endif
                //START160926
                if(zstat){
                    if(nreg)for(i=0;i<ms->lenbrain;i++)zstat[ms->brnidx[i]][m][k]=dptr[ms->brnidx[i]];
                    else for(i=0;i<rbf->nvoxels;i++)zstat[i][m][k]=dptr[rbf->indices[i]];
                    }
                if(cc){
                    if(nreg)for(i=0;i<ms->lenbrain;i++)cc[ms->brnidx[i]][m][k]=dptr1[ms->brnidx[i]];
                    else for(i=0;i<rbf->nvoxels;i++)cc[i][m][k]=dptr1[rbf->indices[i]];
                    }

                } 
            if(delay_flag==3&&!regional_avgstat_name){
                ifh_out->dim4=nglmpersub;
                if(nglmpersub==1){
                    w1->how_many=vol;
                    w1->temp_float=dlymaxmin;
                    if(!write1(string2,w1))exit(-1);
                    fprintf(stderr,"Delays written to %s\n",string2); /*Changed to stderr so as not to screw up the anova program.*/
                    w1->how_many=how_many;
                    }
                else {
                    min_and_max_new(dlymaxmin,vol,&min_dly,&max_dly,(int)FALSE);
                    if(!fwrite_sub(dlymaxmin,sizeof(float),vol,(FILE*)fpdly,swapbytes))exit(-1);
                    if(m==nglmpersub-1){
                        fclose((FILE*)fpdly);
                        if(filetype==(int)IMG){
                            ifh_out->global_min = min_dly;
                            ifh_out->global_max = max_dly;
                            if(!write_ifh(string2,ifh_out,(int)FALSE)) exit(-1);
                            }
                        fprintf(stderr,"Delays written to %s\n",string2);/*Changed to stderr so as not to screw up the anova.*/
                        }
                    }
                }
	    if(lcmag||B1){

                #if 0
                if(!lcinvvar) {
                    for(i=0;i<vol;i++) temp_double[i] = magmaxmin[i]==(double)UNSAMPLED_VOXEL ? (double)UNSAMPLED_VOXEL :
                        magmaxmin[i]/scl_mag_stack[which_delay[i]];
                    }
                else {
                    for(i=0;i<vol;i++) {
                        if(magmaxmin[i]==(double)UNSAMPLED_VOXEL) {
                            temp_double[i] = temp_double2[i] = (double)UNSAMPLED_VOXEL;
                            }
                        else {
                            temp_double2[i] = glmstack[glmi]->var[i]*scl_stack[which_delay[i]];
                            temp_double[i] = magmaxmin[i]/temp_double2[i];
                            }
                        }
                    }
                #endif
                //START160926
                if(!lcinvvar){
                    for(i=0;i<glmstack[glmi]->nmaski;i++)temp_double[glmstack[glmi]->maski[i]]=
                        magmaxmin[glmstack[glmi]->maski[i]]==(double)UNSAMPLED_VOXEL?(double)UNSAMPLED_VOXEL:
                        magmaxmin[glmstack[glmi]->maski[i]]/scl_mag_stack[which_delay[glmstack[glmi]->maski[i]]];
                    }
                else{
                    for(i=0;i<glmstack[glmi]->nmaski;i++){
                        if(magmaxmin[glmstack[glmi]->maski[i]]==(double)UNSAMPLED_VOXEL){
                            temp_double[glmstack[glmi]->maski[i]]=temp_double2[glmstack[glmi]->maski[i]]=(double)UNSAMPLED_VOXEL;
                            }
                        else{
                            temp_double2[glmstack[glmi]->maski[i]]=
                                glmstack[glmi]->var[i]*scl_stack[which_delay[glmstack[glmi]->maski[i]]];
                            temp_double[glmstack[glmi]->maski[i]]=
                                magmaxmin[glmstack[glmi]->maski[i]]/temp_double2[glmstack[glmi]->maski[i]];
                            }
                        }
                    }


                #if 0
                if(!lcprint_unscaled_mag) {
                    for(i=0;i<vol;i++) {
                        if(temp_double[i]!=(double)UNSAMPLED_VOXEL) {
                            temp_double[i] *= (gms_stack[glmi]->grand_mean[i] < gms_stack[glmi]->grand_mean_thresh ?
                                0. : gms_stack[glmi]->pct_chng_scl/gms_stack[glmi]->grand_mean[i]);
                            if(B1)B1[i][m][k]=temp_double[i];
                            }
                        }
                    if(lcinvvar) {
                        for(i=0;i<vol;i++) {
                            if(temp_double2[i]!=(double)UNSAMPLED_VOXEL) {
                                temp_double2[i] *= (gms_stack[glmi]->grand_mean[i] < gms_stack[glmi]->grand_mean_thresh ?
                                    0. : gms_stack[glmi]->pct_chng_scl/gms_stack[glmi]->grand_mean[i]);
                                }
                            }
                        }
                    }
                #endif
                //START160927
                if(!lcprint_unscaled_mag){
                    for(i=0;i<glmstack[glmi]->nmaski;i++){
                        if(temp_double[glmstack[glmi]->maski[i]]!=(double)UNSAMPLED_VOXEL){
                            temp_double[glmstack[glmi]->maski[i]]*=gms_stack[glmi]->grand_mean[i]<gms_stack[glmi]->grand_mean_thresh?
                                0.:gms_stack[glmi]->pct_chng_scl/gms_stack[glmi]->grand_mean[i];
                            if(B1)B1[glmstack[glmi]->maski[i]][m][k]=temp_double[glmstack[glmi]->maski[i]];
                            }
                        }
                    if(lcinvvar){
                        for(i=0;i<glmstack[glmi]->nmaski;i++){
                            if(temp_double2[glmstack[glmi]->nmaski]!=(double)UNSAMPLED_VOXEL){
                                temp_double2[glmstack[glmi]->nmaski]*=
                                    (gms_stack[glmi]->grand_mean[i]<gms_stack[glmi]->grand_mean_thresh?
                                    0.:gms_stack[glmi]->pct_chng_scl/gms_stack[glmi]->grand_mean[i]);
                                }
                            }
                        }
                    }


                #if 0 
                if(lc_within_subject_sd) {
                    for(i=0;i<vol;i++) {
                        if(temp_double[i]==(double)UNSAMPLED_VOXEL) { /*temp_double is correct*/
                            mag[i] = (double)UNSAMPLED_VOXEL;
                            }
                        else {
                            mag[i] = glmstack[glmi]->sd[i]*sqrt_scl_stack[which_delay[i]] * (gms_stack[glmi]->grand_mean[i] < 
                                gms_stack[glmi]->grand_mean_thresh ? 0. : 
                                gms_stack[glmi]->pct_chng_scl/gms_stack[glmi]->grand_mean[i]);
                            }
                        if(nreg) accumulate_sd[asdi++] = mag[i];
                        }
                    }
                #endif
                //START160927
                if(lc_within_subject_sd){
                    for(i=0;i<glmstack[glmi]->nmaski;i++) /*temp_double is correct*/
                        mag[glmstack[glmi]->maski[i]]=temp_double[glmstack[glmi]->maski[i]]==(double)UNSAMPLED_VOXEL?
                            (double)UNSAMPLED_VOXEL:(gms_stack[glmi]->grand_mean[i]<gms_stack[glmi]->grand_mean_thresh?0.:
                            sqrt(glmstack[glmi]->var[i]*scl_stack[which_delay[glmstack[glmi]->maski[i]]])*
                            gms_stack[glmi]->pct_chng_scl/gms_stack[glmi]->grand_mean[i]);
                    }


                #if 0
                if(num_xform_files) {
                    if(!t4_atlas(temp_double,stat,&t4[glmi*(int)T4SIZE],glmstack[glmi]->ifh->glm_xdim,glmstack[glmi]->ifh->glm_ydim,
                        glmstack[glmi]->ifh->glm_zdim,glmstack[glmi]->ifh->glm_dxdy,glmstack[glmi]->ifh->glm_dz,A_or_B_or_U[glmi],
                        glmstack[glmi]->ifh->orientation,ap,(double*)NULL)) exit(-1);
                    for(i=0;i<ap->vol;i++) temp_double[i] = stat[i];
                    if(lc_within_subject_sd) {
                        if(!t4_atlas(mag,stat,&t4[glmi*(int)T4SIZE],glmstack[glmi]->ifh->glm_xdim,glmstack[glmi]->ifh->glm_ydim,
                            glmstack[glmi]->ifh->glm_zdim,glmstack[glmi]->ifh->glm_dxdy,glmstack[glmi]->ifh->glm_dz,
                            A_or_B_or_U[glmi],glmstack[glmi]->ifh->orientation,ap,(double*)NULL)) exit(-1);
                        for(i=0;i<ap->vol;i++) mag[i] = stat[i];
                        }
                    if(lcinvvar) {
                        if(!t4_atlas(temp_double2,stat,&t4[glmi*(int)T4SIZE],glmstack[glmi]->ifh->glm_xdim,
                            glmstack[glmi]->ifh->glm_ydim,glmstack[glmi]->ifh->glm_zdim,glmstack[glmi]->ifh->glm_dxdy,
                            glmstack[glmi]->ifh->glm_dz,A_or_B_or_U[glmi],glmstack[glmi]->ifh->orientation,ap,(double*)NULL)) 
                            exit(-1);
                        for(i=0;i<ap->vol;i++) temp_double2[i] = stat[i];
                        }
                    }
                if(fwhm > 0.) {
                    gauss_smoth2(temp_double,stat,gs);
	            for(i=0;i<ap->vol;i++) temp_double[i] = stat[i];
                    if(lcinvvar) {
                        gauss_smoth2(temp_double2,stat,gs);
	                for(i=0;i<ap->vol;i++) temp_double2[i] = stat[i];
                        }
	 	    }
                if(num_region_files) {
                    for(q=j=p=0;p<num_wfiles;p++) {
                        if(wfiles) for(i=0;i<ap->vol;i++) weights[i] = (double)mm_wfiles[p]->ptr[i];
                        crsw(temp_double,stat,rbf,weights,rbf->indices);
                        for(i=0;i<how_many;i++,j++) {
                            regmag[k][m][j] = stat[i];
                            }
                        if(lc_within_subject_sd) {
                            crsw(mag,stat,rbf,weights,rbf->indices);
                            for(i=0;i<how_many;i++,q++) within_subject_sd[k][m][q] = stat[i];
                            }
                        }
                    for(i=0;i<how_many*num_wfiles;i++) temp_double[i] = regmag[k][m][i];
                    if(lc_within_subject_sd) for(i=0;i<how_many*num_wfiles;i++) mag[i] = within_subject_sd[k][m][i];
                    }
                else if(lcaccumulate) {
                    if(!nreg) for(i=0;i<how_many;i++) accumulate_mag[m][i] = (float)temp_double[i];
                    }
                else if(regmag) {
                    for(i=0;i<how_many*num_wfiles;i++) regmag[k][m][i] = temp_double[i];
                    }
                for(i=0;i<how_many*num_wfiles;i++) {
                    if(temp_double[i]!=(double)UNSAMPLED_VOXEL) {
                        avg[i] += temp_double[i];
                        nsubjects[i]++;
                        }
                    }
                if(lcsd||lcsem||regsem) {
                    for(i=0;i<how_many*num_wfiles;i++)
                        if(temp_double[i]!=(double)UNSAMPLED_VOXEL) avg2[i]+=temp_double[i]*temp_double[i];
                    }
                if(lc_within_subject_sd) {
                    for(i=0;i<how_many*num_wfiles;i++) avg_unscaled[i] += mag[i];
                    if(lcsd||lcsem) for(i=0;i<how_many*num_wfiles;i++) avg2_unscaled[i] += mag[i]*mag[i];
                    }
                if(lcinvvar) for(i=0;i<how_many*num_wfiles;i++) avginvvar[i] += 1/temp_double2[i];
                #endif
                //START160927

                //printf("here5\n");fflush(stdout);

                dptr=temp_double;
                if(lc_within_subject_sd)dptr1=mag;
                if(lcinvvar)dptr2=temp_double2;
                if(num_xform_files){
                    if(!t4_atlas(dptr,stat,&t4[glmi*(int)T4SIZE],glmstack[glmi]->ifh->glm_xdim,glmstack[glmi]->ifh->glm_ydim,
                        glmstack[glmi]->ifh->glm_zdim,glmstack[glmi]->ifh->glm_dxdy,glmstack[glmi]->ifh->glm_dz,A_or_B_or_U[glmi],
                        glmstack[glmi]->ifh->orientation,ap,(double*)NULL))exit(-1);
                    dptr=stat;
                    if(lc_within_subject_sd){
                        if(!t4_atlas(dptr1,td4,&t4[glmi*(int)T4SIZE],glmstack[glmi]->ifh->glm_xdim,glmstack[glmi]->ifh->glm_ydim,
                            glmstack[glmi]->ifh->glm_zdim,glmstack[glmi]->ifh->glm_dxdy,glmstack[glmi]->ifh->glm_dz,
                            A_or_B_or_U[glmi],glmstack[glmi]->ifh->orientation,ap,(double*)NULL))exit(-1);
                        dptr1=td4;
                        }
                    if(lcinvvar){
                        if(!t4_atlas(dptr2,td5,&t4[glmi*(int)T4SIZE],glmstack[glmi]->ifh->glm_xdim,
                            glmstack[glmi]->ifh->glm_ydim,glmstack[glmi]->ifh->glm_zdim,glmstack[glmi]->ifh->glm_dxdy,
                            glmstack[glmi]->ifh->glm_dz,A_or_B_or_U[glmi],glmstack[glmi]->ifh->orientation,ap,(double*)NULL))exit(-1);
                        dptr2=td5;
                        }
                    }
                if(fwhm>0.){
                    gauss_smoth2(dptr,td1,gs);
                    dptr=td1;
                    if(lcinvvar){
                        gauss_smoth2(dptr2,td6,gs);
                        dptr2=td6;
                        }
	 	    }
                if(num_region_files){
                    for(j=p=0;p<num_wfiles;p++){
                        if(wfiles)for(i=0;i<ap->vol;i++)weights[i]=(double)mm_wfiles[p]->ptr[i];
                        crsw(dptr,&regmag[k][m][j],rbf,weights,rbf->indices);
                        if(lc_within_subject_sd)crsw(dptr1,&within_subject_sd[k][m][j],rbf,weights,rbf->indices);
                        j+=how_many;
                        }
                    dptr=&regmag[k][m][0];
                    if(lc_within_subject_sd)dptr2=&within_subject_sd[k][m][0];
                    }
                else if(lcaccumulate){
                    if(!nreg)for(i=0;i<how_many;i++)accumulate_mag[m][i]=(float)dptr[i];
                    }
                else if(regmag){
                    for(i=0;i<how_many*num_wfiles;i++)regmag[k][m][i]=dptr[i];
                    }
                for(i=0;i<how_many*num_wfiles;i++){
                    if(dptr[i]!=(double)UNSAMPLED_VOXEL){
                        avg[i]+=dptr[i];
                        nsubjects[i]++;
                        }
                    }
                if(lcsd||lcsem||regsem){
                    for(i=0;i<how_many*num_wfiles;i++)if(dptr[i]!=(double)UNSAMPLED_VOXEL)avg2[i]+=dptr[i]*dptr[i];
                    }
                if(lc_within_subject_sd){
                    for(i=0;i<how_many*num_wfiles;i++)if(dptr1[i]!=(double)UNSAMPLED_VOXEL)avg_unscaled[i]+=dptr1[i];
                    if(lcsd||lcsem)for(i=0;i<how_many*num_wfiles;i++)
                        if(dptr1[i]!=(double)UNSAMPLED_VOXEL)avg2_unscaled[i]+=dptr1[i]*dptr1[i];
                    }
                if(lcinvvar)for(i=0;i<how_many*num_wfiles;i++)if(dptr2[i]!=(double)UNSAMPLED_VOXEL)avginvvar[i]+=1/dptr2[i]; 



	        } /*if(lcmag)*/
            fclose(fp);
	    } /*for(m=0;m<nglmfiles;m++)*/
        if(!regional_avgstat_name) {
            ifh_out->dim4 = 1;
            if(glm_list_file&&!k) assign_glm_list_file_to_ifh_FS(ifh_out,glm_list_file);
            string[0] = 0;
            if(directory) strcat(string,directory);
            if(!nc_names) {
                if(glm_list_str) {
                    strcat(string,glm_list_str);
                    strcat(string,"_");
                    }
                if(group_name) {
                    strcat(string,group_name);
                    strcat(string,"_");
                    }
                strcat(string,tags[k]);
                strcat(string,"_");
                }
            sprintf(string2,"%s%s%s",delay_str,appendstr,Fileext[filetype]);
            if(avg_zstat) {

                #if 0
                for(i=0;i<how_many;i++) temp_float[i]=0.;
                for(i=0;i<ms->lenbrain;i++) temp_float[ms->brnidx[i]] = 
                    nsubjects[ms->brnidx[i]]?(float)(avg_zstat[ms->brnidx[i]]/sqrt(nsubjects[ms->brnidx[i]])):(float)UNSAMPLED_VOXEL;
                ifh_out->fwhm = sqrt(glmstack[0]->ifh->fwhm*glmstack[0]->ifh->fwhm+fwhm*fwhm);
                sprintf(outfile,"%s%s%s%s%s%szstat%s%s%s",string,nglmpersub>1?"":glm_names[0],!nc_names||nglmpersub<=1?"_":"",
                    !nc_names?"":c_names->files[k],!nc_names?"":"_",nglmpersub>1?"avg":"",magnorm_str,unscaled_mag_str,string2);
                w1->temp_float=temp_float;
                if(!write1(outfile,w1))exit(-1);
                printf("Z statistics written to %s\n",outfile);
                for(i=0;i<ms->lenbrain;i++)temp_float[ms->brnidx[i]]=
                    nsubjects[ms->brnidx[i]]?(float)(avg_cc[ms->brnidx[i]]/nsubjects[ms->brnidx[i]]):(float)UNSAMPLED_VOXEL;
                sprintf(outfile,"%s%s%s%s%s%scorcoeff%s%s%s",string,nglmpersub>1?"":glm_names[0],!nc_names||nglmpersub<=1?"_":"",
                    !nc_names?"":c_names->files[k],!nc_names?"":"_",nglmpersub>1?"arithmean":"",magnorm_str,unscaled_mag_str,string2);
                w1->temp_float=temp_float;
                if(!write1(outfile,w1))exit(-1);
                printf("Correlation coefficient written to %s\n",outfile);
                if(nglmpersub>1) printf("This is not a statistical image because it is an arithmetic mean.\n");
                #endif
                //START211104
                for(i=0;i<how_many;i++) temp_float[i]=0.;
                for(i=0;i<ms->lenbrain;i++) temp_float[ms->brnidx[i]] = 
                    nsubjects[ms->brnidx[i]]?(float)(avg_zstat[ms->brnidx[i]]/sqrt(nsubjects[ms->brnidx[i]])):(float)UNSAMPLED_VOXEL;
                if(!lutf){
                    ifh_out->fwhm = sqrt(glmstack[0]->ifh->fwhm*glmstack[0]->ifh->fwhm+fwhm*fwhm);
                    sprintf(outfile,"%s%s%s%s%s%szstat%s%s%s",string,nglmpersub>1?"":glm_names[0],!nc_names||nglmpersub<=1?"_":"",
                        !nc_names?"":c_names->files[k],!nc_names?"":"_",nglmpersub>1?"avg":"",magnorm_str,unscaled_mag_str,string2);
                    w1->temp_float=temp_float;
                    if(!write1(outfile,w1))exit(-1);
                    printf("Z statistics written to %s\n",outfile);
                    for(i=0;i<ms->lenbrain;i++)temp_float[ms->brnidx[i]]=
                        nsubjects[ms->brnidx[i]]?(float)(avg_cc[ms->brnidx[i]]/nsubjects[ms->brnidx[i]]):(float)UNSAMPLED_VOXEL;
                    sprintf(outfile,"%s%s%s%s%s%scorcoeff%s%s%s",string,nglmpersub>1?"":glm_names[0],!nc_names||nglmpersub<=1?"_":"",
                        !nc_names?"":c_names->files[k],!nc_names?"":"_",nglmpersub>1?"arithmean":"",magnorm_str,unscaled_mag_str,string2);
                    w1->temp_float=temp_float;
                    if(!write1(outfile,w1))exit(-1);
                    printf("Correlation coefficient written to %s\n",outfile);
                    if(nglmpersub>1) printf("This is not a statistical image because it is an arithmetic mean.\n");
                    }
                else{    
                    sprintf(outfile,"%s%s%s%s%s%szstat%s%s.txt",string,nglmpersub>1?"":glm_names[0],!nc_names||nglmpersub<=1?"_":"",
                        !nc_names?"":c_names->files[k],!nc_names?"":"_",nglmpersub>1?"avg":"",magnorm_str,unscaled_mag_str);
                    //std::cout<<"outfile="<<outfile<<std::endl;
                    FILE *fp;
                    if(!(fp=fopen_sub(outfile,"w")))exit(-1);
                    for(i=0;i<ms->lenbrain;i++)if(temp_float[ms->brnidx[i]]!=(float)UNSAMPLED_VOXEL){ 
                        fprintf(fp,"%s\t%f\n",lookuptable_name(lutf,i),temp_float[ms->brnidx[i]]);
                        }
                    fclose(fp);
                    printf("Z statistics written to %s\n",outfile);
                    }

                }
            if(accumulate_z) {
                if(lcaccumulateone) {
                    ifh_out->dim4 = nglmpersub;
                    sprintf(outfile,"%s%s_indzstat%s",string,!nc_names?"":c_names->files[k],string2);
                    w1->temp_float=accumulate_z[0];
                    w1->how_many=nglmpersub*how_many;
                    if(!write1(outfile,w1))exit(-1);
                    printf("Individual Z statistics written to %s\n",outfile);
                    w1->how_many=how_many;
                    }
                if(lcaccumulateseparate) {
                    for(i=0;i<how_many;i++) temp_float[i]=0.;
                    for(i=0;i<nglmpersub;i++) {
                        sprintf(outfile,"%s%s_%s_zstat%s",string,glm_names[i],!nc_names?"":c_names->files[k],string2);
                        w1->temp_float=accumulate_z[i];
                        if(!write1(outfile,w1))exit(-1);
                        printf("Z statistics written to %s\n",outfile);
                        }
                    }
                }
            }
        if(lcmag&&!nreg) {
            if(!lcinvvar) { 
                for(i=0;i<how_many*num_wfiles;i++) temp_float[i] = nsubjects[i] ? (float)(avg[i]/nsubjects[i]):(float)UNSAMPLED_VOXEL;
                }
            else {
                for(i=0;i<how_many*num_wfiles;i++) temp_float[i] = nsubjects[i] ? (float)(avg[i]/avginvvar[i]):(float)UNSAMPLED_VOXEL;
                }
            if(num_region_files) {
                for(i=0;i<how_many*num_wfiles;i++) regavg[k][i] = temp_float[i];
                }
 	    else {	
                if(!nreg) {
                    if(!lc_one_file) {
                        sprintf(outfile,"%s%s%s%s%s%s%s%s%s%s",string,nglmpersub>1?"":glm_names[0],nglmpersub>1?"":"_",
                            !nc_names?"":c_names->files[k],!nc_names?"":"_",nglmpersub>1?"avgmag":"mag",!lcinvvar?"":"tovar",
                            magnorm_str,unscaled_mag_str,string2);
                        w1->temp_float=temp_float;
                        if(!write1(outfile,w1))exit(-1);
                        printf("Magnitudes written to %s\n",outfile);
                        }
                    else {
                        min_and_max_floatstack(temp_float,how_many,&min_of,&max_of);
                        if(!fwrite_sub(temp_float,sizeof(float),(size_t)how_many,(FILE*)fpof,swapbytes)) exit(-1);
                        }
                    }
	        }
            if(lc_within_subject_sd) {
                for(i=0;i<how_many*num_wfiles;i++) 
                    temp_float[i] = nsubjects[i] ? (float)(avg_unscaled[i]/nsubjects[i]) : (float)UNSAMPLED_VOXEL;
                if(num_region_files) {
                    for(i=0;i<how_many*num_wfiles;i++) regavg_see[k][i] = temp_float[i];
                    }
                else { 
                    if(!nreg) {
                        sprintf(outfile,"%s_avgsee%s%s%s",string,magnorm_str,unscaled_mag_str,string2);
                        w1->temp_float=temp_float;
                        if(!write1(outfile,w1))exit(-1);
                        printf("Average SEE written to %s\n",outfile);
                        }
                    }
                }
            if(lcsd||lcsem||regsem) {
                for(i=0;i<how_many*num_wfiles;i++) temp_float[i] = nsubjects[i]>1 ?
                    (float)sqrt((avg2[i]-avg[i]*avg[i]/nsubjects[i])/(nsubjects[i]*(nsubjects[i]-1))) : (float)UNSAMPLED_VOXEL;
                if(num_region_files) {
                    for(i=0;i<how_many*num_wfiles;i++) regsem[k][i] = temp_float[i];
                    }
                else {
                    sprintf(outfile,"%s_avgmag%s%ssem%s",string,magnorm_str,unscaled_mag_str,string2);
                    w1->temp_float=temp_float;
                    if(!write1(outfile,w1))exit(-1);
                    printf("Standard error of the mean written to %s\n",outfile);
		    }
    
                if(lc_within_subject_sd) {
                    for(i=0;i<how_many*num_wfiles;i++) temp_float[i] = nsubjects[i]>1 ?
                        (float)sqrt((avg2_unscaled[i]-avg_unscaled[i]*avg_unscaled[i]/nsubjects[i])/(nsubjects[i]*(nsubjects[i]-1))) :
                        (float)UNSAMPLED_VOXEL;
                    if(num_region_files) {
                        for(i=0;i<how_many*num_wfiles;i++) regsem_see[k][i] = temp_float[i];
                        }
                    else {
                        sprintf(outfile,"%s_avgsee%s%ssem%s",string,magnorm_str,unscaled_mag_str,string2);
                        w1->temp_float=temp_float;
                        if(!write1(outfile,w1))exit(-1);
                        printf("SEE standard error of the estimate written to %s\n",outfile);
                        }
                    }
                }
            if(lcaccumulate && !nreg) {
                if(lcaccumulateone) {
                    ifh_out->dim4=nglmpersub;
                    sprintf(outfile,"%s%s_indmag%s%s%s",string,!nc_names?"":c_names->files[k],magnorm_str,unscaled_mag_str,string2);
                    w1->temp_float=accumulate_mag[0];
                    w1->how_many=nglmpersub*how_many;
                    if(!write1(outfile,w1))exit(-1);
                    printf("Individual magnitudes written to %s\n",outfile);
                    w1->how_many=how_many;
                    }
                if(lcaccumulateseparate) {
                    for(i=0;i<how_many;i++) temp_float[i]=0.;
                    for(i=0;i<nglmpersub;i++) {
                        ifh_out->dim4 = 1;
                        sprintf(outfile,"%s%s_%s%s%s%s",string,glm_names[i],!nc_names?"":c_names->files[k],magnorm_str,
                            unscaled_mag_str,string2);
                        w1->temp_float=accumulate_mag[i];
                        if(!write1(outfile,w1))exit(-1);
                        printf("Magnitude written to %s\n",outfile);
                        }
                    }
                }
            if(lcinvvar&&!nreg) {
                for(i=0;i<how_many*num_wfiles;i++) {
                    if(!nsubjects[i]) {
                        temp_float[i] = (float)UNSAMPLED_VOXEL;
                        }
                    else {
                        sign = GSL_SIGN(td=avg[i]/avginvvar[i]);
                        td = 100.*sign*(1.-gsl_cdf_gaussian_Q(sign*td,1./avginvvar[i]));
                        temp_float[i] = (float)(100.*sign*(1.-gsl_cdf_gaussian_Q(sign*td,1./avginvvar[i]))); 
                        }
                    }
                if(num_region_files) {
                    for(j=k*how_many*num_wfiles,i=0;i<how_many*num_wfiles;i++,j++) regavginvvar[j]=temp_float[i]; 
                    }
                else {
                    sprintf(outfile,"%s_posprob%s%s%s",string,magnorm_str,unscaled_mag_str,string2);
                    w1->temp_float=temp_float;
                    if(!write1(outfile,w1))exit(-1);
                    printf("Posterior probability written to %s\n",outfile);
                    }
                }
            } /*if(lcmag)*/
        } /*for(k=0;k<num_contrasts;k++)*/
    if((lcaccumulate||zstat||lcmag) && nreg) {
        if(!(lams=(Length_And_Max**)malloc(sizeof*lams*nglmpersub))) {
            printf("fidlError: Unable to malloc lams\n");
            exit(-1);
            }
        for(mm=m=0;m<nglmpersub;mm+=glmpersub[m++]) {
            glmi = mm+fbfcon[m]->file_index[0];
            if(!(lams[m]=get_length_and_max(glmstack[glmi]->ifh->nregions,glmstack[glmi]->ifh->region_names))) exit(-1);
            }
        string[0] = 0;
        if(directory) strcat(string,directory);
        strcat(string,regional_avgstat_name);
        if(!(str_ptr=get_tail_sans_ext(string))) exit(-1);
        strcat(string,"_");
        if(zstat) strcat(string,"zstat");
        if(lcmag) strcat(string,"regmag");
        strcat(string,".txt");
        if(!(fp=fopen_sub(string,"w"))) exit(-1);
        if(zstat) {
            for(flag=j=0;j<vol;j++) {
                for(mm=m=0;m<nglmpersub;mm+=glmpersub[m++]) {
                    glmi = mm+fbfcon[m]->file_index[0];
                    if(strcmp(glmregnames[j],glmstack[glmi]->ifh->region_names[j])) {flag=1;break;}
                    }
                if(flag) break;
                }
            if(flag) {
                fprintf(fp,"Z Statistics\n------------\n");
                for(mm=m=0;m<nglmpersub;mm+=glmpersub[m++]) {
                    fprintf(fp,"%s\t",glm_names[m]);
                    for(i=0;i<num_contrasts;i++) fprintf(fp,"%s\t",!nc_names?tags[i]:c_names->files[i]);
                    fprintf(fp,"\n");
                    glmi = mm+fbfcon[m]->file_index[0];
                    for(j=0;j<vol;j++) {
                        fprintf(fp,"%s\t",glmstack[glmi]->ifh->region_names[j]);
                        for(i=0;i<num_contrasts;i++) fprintf(fp,"%8.4f\t",zstat[j][m][i]);
                        fprintf(fp,"\n");
                        }
                    fprintf(fp,"\n");
                    }
                fprintf(fp,"mean\t");
                for(i=0;i<num_contrasts;i++) fprintf(fp,"%s\t",!nc_names?tags[i]:c_names->files[i]);
                fprintf(fp,"\n");
                for(j=0;j<vol;j++) {
                    fprintf(fp,"%s\t",glmregnames[j]);
                    if(nsubjects[j]<2) {
                        for(i=0;i<num_contrasts;i++) fprintf(fp,"--------\t");
                        }
                    else {
                        for(i=0;i<num_contrasts;i++) {
                            for(td=0.,mm=m=0;m<nglmpersub;mm+=glmpersub[m++]) td += zstat[j][m][i];
                            fprintf(fp,"%8.4f\t",td/sqrt((double)nsubjects[j]));
                            }
                        }
                    fprintf(fp,"\n");
                    }
                }
            else {
                if(!nreg) {
                    fprintf(fp,"Z Statistics\n------------\n");
                    for(i=0;i<num_contrasts;i++) {
                        fprintf(fp,"%s\t",!nc_names?tags[i]:c_names->files[i]);
                        for(j=0;j<vol;j++)fprintf(fp,"%s %d\t",glmreg->region_names[j],glmreg->nvoxels_region[j]);
                        fprintf(fp,"\n");
                        for(j=0;j<vol;j++) td1[j]=td1un[j]=0.;
                        for(mm=m=0;m<nglmpersub;mm+=glmpersub[m++]) {
                            fprintf(fp,"%s\t",glm_names[m]);
                            glmi = mm+fbfcon[m]->file_index[0];
                            for(j=0;j<vol;j++) {
                                fprintf(fp,"%8.4f\t",zstat[j][m][i]);
                                if(zstat[j][m][i]!=(double)UNSAMPLED_VOXEL) {td1[j]+=zstat[j][m][i];td1un[j]++;}
                                }
                            fprintf(fp,"\n");
                            }
                        fprintf(fp,"mean (fixed effects)\t");
                        for(j=0;j<vol;j++) fprintf(fp,"%8.4f\t",td1[j]/sqrt(td1un[j]));
                        fprintf(fp,"\n\n");
                        }
                    fprintf(fp,"\nCorrelation coefficients\n------------------------\n");
                    for(i=0;i<num_contrasts;i++) {
                        fprintf(fp,"%s\t",!nc_names?tags[i]:c_names->files[i]);
                        for(j=0;j<vol;j++)fprintf(fp,"%s %d\t",glmreg->region_names[j],glmreg->nvoxels_region[j]);
                        fprintf(fp,"\n");
                        for(j=0;j<vol;j++) td1[j]=td1un[j]=td3[j]=0.;
                        for(mm=m=0;m<nglmpersub;mm+=glmpersub[m++]) {
                            fprintf(fp,"%s\t",glm_names[m]);
                            glmi = mm+fbfcon[m]->file_index[0];
                            for(j=0;j<vol;j++) {
                                fprintf(fp,"%8.4f\t",cc[j][m][i]);
                                if(cc[j][m][i]!=(double)UNSAMPLED_VOXEL) 
                                    {td1[j]+=cc[j][m][i];td1un[j]++;td3[j]+=cc[j][m][i]*cc[j][m][i];}
                                }
                            fprintf(fp,"\n");
                            }
                        fprintf(fp,"mean\t");
                        for(j=0;j<vol;j++) fprintf(fp,"%8.4f\t",td1[j]/td1un[j]);
                        fprintf(fp,"\nsem\t");
                        for(j=0;j<vol;j++) fprintf(fp,"%8.4f\t",sqrt((td3[j]-td1[j]*td1[j]/td1un[j])/td1un[j]/(td1un[j]-1.)));
                        fprintf(fp,"\n\n");
                        }
                    fprintf(fp,"\nR2\n--\n");
                    for(i=0;i<num_contrasts;i++) {
                        fprintf(fp,"%s\t",!nc_names?tags[i]:c_names->files[i]);
                        for(j=0;j<vol;j++)fprintf(fp,"%s %d\t",glmreg->region_names[j],glmreg->nvoxels_region[j]);
                        fprintf(fp,"\n");
                        for(j=0;j<vol;j++) td1[j]=td1un[j]=0.;
                        for(mm=m=0;m<nglmpersub;mm+=glmpersub[m++]) {
                            fprintf(fp,"%s\t",glm_names[m]);
                            glmi = mm+fbfcon[m]->file_index[0];
                            for(j=0;j<vol;j++) {
                                if(cc[j][m][i]==(double)UNSAMPLED_VOXEL) td = (double)UNSAMPLED_VOXEL;
                                else {td1[j] += td = cc[j][m][i]*cc[j][m][i];td1un[j]++;}
                                fprintf(fp,"%8.4f\t",td);
                                }
                            fprintf(fp,"\n");
                            }
                        fprintf(fp,"mean\t");
                        for(j=0;j<vol;j++) fprintf(fp,"%8.4f\t",td1[j]/td1un[j]);
                        fprintf(fp,"\n\n");
                        }
                    fprintf(fp,"\n");
                    }
                else {
                    if(n>1){
                        string2[0]=0;
                        if(directory)strcat(string2,directory);
                        strcat(string2,regional_avgstat_name);
                        if(!get_tail_sans_ext(string2)) exit(-1);
                        strcat(string2,"_r.txt");
                        if(!(fp1=fopen_sub(string2,"w")))exit(-1);

                        //for(j=0;j<vol;j++)fprintf(fp1,"\t%s",glmreg->region_names[j]);
                        //START161206
                        for(j=0;j<vol;j++)fprintf(fp1,"\t%s %d",glmreg->region_names[j],glmreg->nvoxels_region[j]);

                        fprintf(fp1,"\n");
                        for(i=0;i<num_contrasts;i++){
                            fprintf(fp1,"%s",!nc_names?tags[i]:c_names->files[i]);
                            for(j=0;j<vol;j++){
                                for(td=td2=0.,n=m=0;m<nglmpersub;m++){
                                    if(cc[j][m][i]!=(double)UNSAMPLED_VOXEL&&isfinite(cc[j][m][i])){
                                        td+=cc[j][m][i];
                                        td2+=cc[j][m][i]*cc[j][m][i];
                                        n++;
                                        }
                                    }
                                fprintf(fp1,"\t%f %f",td/(double)n,sqrt((td2-td*td/(double)n)/(double)(n*(n-1))));
                                }
                            fprintf(fp1,"\n");
                            }
                        fclose(fp1);
                        printf("Average correlation coefficients written to %s\n",string2);
                        fprintf(fp,"group\tmean\tsem\tt\tdf\tZ\tp\n");
                        for(j=0;j<vol;j++){

                            //fprintf(fp,"REGION: %s\n",glmreg->region_names[j]);
                            //START161206
                            fprintf(fp,"REGION: %s %d\n",glmreg->region_names[j],glmreg->nvoxels_region[j]);

                            for(i=0;i<num_contrasts;i++) {
                                for(td=td2=0.,n=m=0;m<nglmpersub;m++){
                                    if(B1[j][m][i]!=(double)UNSAMPLED_VOXEL&&isfinite(B1[j][m][i])){
                                        td+=B1[j][m][i];
                                        td2+=B1[j][m][i]*B1[j][m][i];
                                        n++;
                                        }
    
                                    }
                                stat[0]=td/(double)n/sqrt((td2-td*td/(double)n)/(double)(n*(n-1)));
                                df1=(double)n-1.;
                                t_to_z(stat,temp_double,1,&df1,t_to_zi);
                                fprintf(fp,"%s\t%g\t%g\t%g\t%d\t%g\t%g\n",!nc_names?tags[i]:c_names->files[i],td/(double)n,
                                    sqrt((td2-td*td/(double)n)/(double)(n*(n-1))),stat[0],n-1,temp_double[0],
                                    temp_double[0]<0.?gsl_cdf_ugaussian_P(temp_double[0]):gsl_cdf_ugaussian_Q(temp_double[0]));
                                }
                            }
                        }
                    for(m=0;m<nglmpersub;m++) {
                        fprintf(fp,"%s\tZ\tt\tdf\tp\tB\tse(B)\tr\n",glm_names[m]);
                        for(j=0;j<vol;j++) {
                            fprintf(fp,"REGION: %s\n",glmreg->region_names[j]); 
                            for(i=0;i<num_contrasts;i++) {
                                fprintf(fp,"%s\t%g\t%g\t%g\t%g\t%g\t%g\t%g\n",!nc_names?tags[i]:c_names->files[i],zstat[j][m][i],
                                    tstat[j][m][i],tstatdf[j][m][i],zstat[j][m][i]<0.?gsl_cdf_ugaussian_P(zstat[j][m][i]):
                                    gsl_cdf_ugaussian_Q(zstat[j][m][i]),B[j][m][i],seB[j][m][i],cc[j][m][i]);
                                }
                            }
                        fprintf(fp,"\n");
                        }
                    }
                }
            }
        if(lcmag) {
            if(zstat) fprintf(fp,"\n");
            fprintf(fp,"Magnitudes\n----------\n");
            for(mm=m=0;m<nglmpersub;mm+=glmpersub[m++]) {
                fprintf(fp,"%s\t",glm_names[m]);
                for(i=0;i<num_contrasts;i++) fprintf(fp,"%s\t",!nc_names?tags[i]:c_names->files[i]);
                fprintf(fp,"\n");
                glmi = mm+fbfcon[m]->file_index[0];
                for(j=0;j<vol;j++) {
                    fprintf(fp,"%s\t",glmstack[glmi]->ifh->region_names[j]);
                    for(i=0;i<num_contrasts;i++) fprintf(fp,"%8.4f\t",regmag[i][m][j]);
                    fprintf(fp,"\n");
                    }
                fprintf(fp,"\n");
                }
            for(i=0;i<num_contrasts;i++) fprintf(fp,"\t%s\t",!nc_names?tags[i]:c_names->files[i]);
            fprintf(fp,"\n");
            for(i=0;i<num_contrasts;i++) fprintf(fp,"\tmean\tsem");
            fprintf(fp,"\n");
            for(j=0;j<vol;j++) {
                fprintf(fp,"%s\t",glmregnames[j]);
                if(nsubjects[j]<2) {
                    for(i=0;i<num_contrasts;i++) fprintf(fp,"--------\t");
                    }
                else {
                    for(i=0;i<num_contrasts;i++) {
                        for(td=td2=0.,mm=m=0;m<nglmpersub;mm+=glmpersub[m++]) 
                            {td+=regmag[i][m][j];td2+=regmag[i][m][j]*regmag[i][m][j];}
                        fprintf(fp,"%8.4f\t%8.4f\t",td/(double)nsubjects[j],
                            sqrt((td2-td*td/(double)nsubjects[j])/(double)(nsubjects[j]-1)/(double)nsubjects[j]));
                        }
                    }
                fprintf(fp,"\n");
                }
            }
        fflush(fp);
        fclose(fp);
        printf("Output written to %s\n",string);
        fflush(stdout);
        }
    if(num_region_files) {
        if(lcvoxels) {
            boyprintvox(zstat,(double***)NULL,nglmpersub,num_contrasts,con_frames,rbf,glm_names,
                1,0,outfile,TR,glm_list_file,num_wfiles,(Files_Struct*)NULL,ap,tags,(char**)NULL);
            }
        if(!lc_names_only) {
            if(regmag) {
                min_and_max_init(&min,&max);
                min_and_max_doublestack(&regmag[0][0][0],num_contrasts*nglmpersub*num_regions*num_wfiles,&min,&max);
                if(min <= -1000 || max >= 1000) decimals = 2;
                }
            if(!lc_within_subject_sd) {
                if(lcmag) {
                    if(lc_magxreg) {
                        for(i=0;i<num_regions;i++) {
                            fprintf(fprn,"%d\t%s%*d\n",i,rbf->region_names_ptr[i],rbf->max_length-rbf->length[i]+5,
                                rbf->nvoxels_region[i]);
                            }
                        for(j=p=0;p<num_wfiles;p++,j+=num_regions) {
                            if(wfiles) fprintf(fprn,"WEIGHT FILE : %s\n\n",wfiles->files[p]);
                            for(k=0;k<num_contrasts;k++) {
                                fprintf(fprn,"\n");
                                fprintf(fprn,"MAGNITUDE : %s\n",tags[k]);
                                for(i=0;i<how_many;i++) fprintf(fprn,"\t%d",i); fprintf(fprn,"\n");
                                for(m=0;m<nglmpersub;m++) {
                                    fprintf(fprn,"%s\t",glm_names[m]);
                                    for(i=0;i<how_many;i++) fprintf(fprn,"%9.*f\t",decimals,regmag[k][m][j+i]);
                                    fprintf(fprn,"\n");
                                    }
                                if(nglmpersub > 1) {
                                    fprintf(fprn,"avg\t");
                                    for(i=0;i<how_many;i++) fprintf(fprn,"%9.*f\t",decimals,regavg[k][j+i]);
                                    fprintf(fprn,"\n");
                                    fprintf(fprn,"sem\t");
                                    for(i=0;i<how_many;i++) fprintf(fprn,"%9.*f\t",decimals,regsem[k][j+i]);
                                    fprintf(fprn,"\n");
                                    }
                                fprintf(fprn,"\n");
                                }
                            }
                        }
                    else {
                        for(k=0;k<num_contrasts;k++) fprintf(fprn,"%d\t%s\n",k,tags[k]);
                        for(j=p=0;p<num_wfiles;p++,j+=num_regions) {
                            if(wfiles) fprintf(fprn,"WEIGHT FILE : %s\n\n",wfiles->files[p]);
                            for(i=0;i<num_regions;i++) {
                                fprintf(fprn,"\n");
                                fprintf(fprn,"REGION : %s %d\n",rbf->region_names_ptr[i],rbf->nvoxels_region[i]);
                                fprintf(fprn,"%*d",spaces+6,0);
                                for(k=1;k<num_contrasts;k++) fprintf(fprn,"%10d",k);
                                fprintf(fprn,"\n");
                                for(m=0;m<nglmpersub;m++) {
                                    fprintf(fprn,"%s%*.*f ",glm_names[m],spaces-(int)len_glm_names[m]+10,decimals,regmag[0][m][j+i]);
                                    for(k=1;k<num_contrasts;k++) fprintf(fprn,"%9.*f ",decimals,regmag[k][m][j+i]);
                                    fprintf(fprn,"\n");
                                    }
                                if(nglmpersub > 1) {
                                    fprintf(fprn,"%s%*.*f ",lcinvvar?"m(1/var)":"mean",lcinvvar?spaces+1:spaces+5,decimals,
                                        regavg[0][j+i]);
                                    for(k=1;k<num_contrasts;k++) fprintf(fprn,"%9.*f ",decimals,regavg[k][j+i]);
                                    fprintf(fprn,"\n");
                                    fprintf(fprn,"sem%*.*f ",spaces+6,decimals,regsem[0][j+i]);
                                    for(k=1;k<num_contrasts;k++) fprintf(fprn,"%9.*f ",decimals,regsem[k][j+i]);
                                    fprintf(fprn,"\n");
                                    }
                                fprintf(fprn,"\n");
                                }
                            }
                        }
                    }
                }
            else {
                for(j=p=0;p<num_wfiles;p++,j+=num_regions) {
                    if(wfiles) fprintf(fprn,"WEIGHT FILE : %s\n\n",wfiles->files[p]);
                    for(i=0;i<num_regions;i++) {
                        fprintf(fprn,"\n");
                        fprintf(fprn,"REGION : %s %d\n",rbf->region_names_ptr[i],rbf->nvoxels_region[i]);
                        fprintf(fprn,"%*s",spaces+10,tags[0]);
                        for(k=1;k<num_contrasts;k++) fprintf(fprn,"%20s",tags[k]);
                        fprintf(fprn,"\n");
                        strcpy(string,"mag       see");
                        fprintf(fprn,"%*s",spaces+16,string);
                        for(k=1;k<num_contrasts;k++) fprintf(fprn,"%20s",string);
                        fprintf(fprn,"\n");
                        for(m=0;m<nglmpersub;m++) {
                            fprintf(fprn,"%s%*.*f %9.*f ",glm_names[m],spaces-(int)len_glm_names[m]+10,decimals,regmag[0][m][j+i],
                                decimals,within_subject_sd[0][m][j+i]);
                            for(k=1;k<num_contrasts;k++) 
                                fprintf(fprn,"%9.*f %9.*f",decimals,regmag[k][m][j+i],decimals,within_subject_sd[k][m][j+i]);
                            fprintf(fprn,"\n");
                            }
                        if(nglmfiles > 1) {
                            fprintf(fprn,"average%*.*f %9.*f ",spaces+3,decimals,regavg[0][j+i],decimals,regavg_see[0][j+i]);
                            for(k=1;k<num_contrasts;k++) 
                                fprintf(fprn,"%9.*f %9.*f ",decimals,regavg[k][j+i],decimals,regavg_see[k][j+i]);
                            fprintf(fprn,"\n");
                            fprintf(fprn,"sem%*.*f %9.*f ",spaces+7,decimals,regsem[0][i],decimals,regsem_see[0][j+i]);
                            for(k=1;k<num_contrasts;k++) 
                                fprintf(fprn,"%9.*f %9.*f ",decimals,regsem[k][i],decimals,regsem_see[k][j+i]);
                            fprintf(fprn,"\n");
                            }
                        fprintf(fprn,"\n");
                        }
                    }
                free_d3double(within_subject_sd);
                }
            }
        }
    else if(lc_one_file) {
        fclose((FILE*)fpof);
        if(glm_list_file) assign_glm_list_file_to_ifh_FS(ifh_out,glm_list_file);
        if(filetype==(int)IMG){
            ifh_out->dim4 = num_contrasts;
            ifh_out->global_min = min_of;
            ifh_out->global_max = max_of;
            if(!write_ifh(onefile,ifh_out,(int)FALSE)) exit(-1);
            }
        fprintf(stdout,"Magnitude written to %s\n",onefile);
        }
    }



//STARTHERE
if(num_tc) {
    if(!lcprint_scaled_tc && !lcprint_unscaled_tc) {
        printf("fidlError: lcprint_scaled_tc and lcprint_unscaled_tc are both FALSE. Abort!\n");
        exit(-1);
        }
    if(!lc_names_only){
        printf("Computing average timecourses.\n");fflush(stdout);
        if(num_region_files||regional_avgstat_name){
            if(fprn) {
                if(concfile) {
                    /*do nothing*/
                    }
                else if(lcmatlab) {
                    *strrchr(outfile,'.') = 0;
                    sprintf(string,"%s_regnames.dat",outfile);
                    if(!(fprn=fopen_sub(string,"w"))) exit(-1);
                    for(i=0;i<num_regions;i++) fprintf(fprn,"%s %d\n",rbf->region_names_ptr[i],rbf->nvoxels_region[i]);
                    fclose(fprn);
                    printf("Region names written to %s\n",string);
                    }
                else if(lcflip) {
                    fprintf(fprn,"region mean sd\n");
                    }
                else if(!lctony) {
                    fprintf(fprn,"TR = %4.2f\n",TR);
                    if(glm_list_file) {
                        fprintf(fprn,"GLM LIST = %s\n",glm_list_file->files[0]);
                        for(l1=1;l1<glm_list_file->nfiles;l1++) fprintf(fprn,"           %s\n",glm_list_file->files[l1]);
                        }
                    fprintf(fprn,"\n");

                    //START200303
                    fprintf(fprn0,"TR = %4.2f\n",TR);
                    if(glm_list_file) {
                        fprintf(fprn0,"GLM LIST = %s\n",glm_list_file->files[0]);
                        for(l1=1;l1<glm_list_file->nfiles;l1++) fprintf(fprn0,"           %s\n",glm_list_file->files[l1]);
                        }
                    fprintf(fprn0,"\n");

                    }
                else {
                    fprintf(fprn,"glmfiles,%d",nglmfiles);
                    for(m=0;m<nglmfiles;m++) {
                        fprintf(fprn,",%s",glmfiles->files[m]);
                        if(lc_within_subject_sd) fprintf(fprn,",see");
                        }
                    if(nglmfiles>1) fprintf(fprn,",mean,sem");
                    fprintf(fprn,"\n");
                    fprintf(fprn,"regionfiles,%zd",region_files->nfiles);
                    for(l1=0;l1<region_files->nfiles;l1++) fprintf(fprn,",%s",region_files->files[l1]);
                    fprintf(fprn,"\n");
                    for(m=n=k=0;m<(int)region_files->nfiles;m++) {
                        fprintf(fprn,"regions,%d,%d",m+1,rbf->num_regions_by_file[m]);
                        for(j=0;j<rbf->num_regions_by_file[m];j++,n++) fprintf(fprn,",%s",rbf->region_names_ptr[n]);
                        for(j=0;j<rbf->num_regions_by_file[m];j++,k++) fprintf(fprn,",%d",rbf->nvoxels_region[k]);
                        fprintf(fprn,"\n");
                        }
                    fprintf(fprn,"timecourses,%d",num_tc_frames);
                    for(n=m=0;m<num_tc_frames;n+=tc_frames[m++]) fprintf(fprn,",%s",tags_tcs->tags_tcptr[n]);
                    fprintf(fprn,"\nTR,glmfile,regionfile,region,timecourse,numframes\n");
                    }
                }


            if(!lcHz||!lcphase_only) {
                if(!(time_courses=d3double(how_many*num_wfiles,nglmpersub,tcs->num_tc))) exit(-1);
                if(!(time_courses_unscaled=d3double(how_many*num_wfiles,nglmpersub,tcs->num_tc))) exit(-1);
                if(!(time_courses_avg = d2double(how_many*num_wfiles,tcs->num_tc))) exit(-1);
                if(!(time_courses_avg_unscaled = d2double(how_many*num_wfiles,tcs->num_tc))) exit(-1);
                if(!(time_courses_sd = d2float(how_many*num_wfiles,tcs->num_tc))) exit(-1);
                if(!(time_courses_sd_unscaled = d2float(how_many*num_wfiles,tcs->num_tc))) exit(-1);

                //START200228
                if(!(tczm=d2double(nglmpersub,tcs->num_tc))) exit(-1);
                if(!(tczm_avg=(double*)malloc(sizeof*tczm_avg*tcs->num_tc))) {
                    printf("fidlError: Unable to malloc tczm_avg\n");
                    exit(-1);
                    }
                if(!(tczm_sem=(double*)malloc(sizeof*tczm_sem*tcs->num_tc))) {
                    printf("fidlError: Unable to malloc tczm_sem\n");
                    exit(-1);
                    }


                if(nstimlen) {
                    if(!(time_courses_fit=d3double(how_many*num_wfiles,nglmpersub,tcs->num_tc))) exit(-1);
                    if(!(time_courses_avg_fit = d2double(how_many*num_wfiles,tcs->num_tc))) exit(-1);
                    if(!(time_courses_param=d3double(how_many*num_wfiles,nglmpersub,num_tc_frames*(HRF_P+2)))) exit(-1);
                    if(!(time_courses_avg_param = d2double(how_many*num_wfiles,num_tc_frames*(HRF_P+2)))) exit(-1);
                    }
                if(confile) {
                    if(!(contrasts=(double*)malloc(sizeof*contrasts*how_many*tcs->num_tc))) {
                        printf("fidlError: Unable to malloc contrasts\n");
                        exit(-1);
                        }
                    if(!(conmag=(double*)malloc(sizeof*conmag*nglmpersub*num_tc_frames))) {
                        printf("fidlError: Unable to malloc conmag\n");
                        exit(-1);
                        }
                    if(!(conmean=(double*)malloc(sizeof*conmean*num_tc_frames))) {
                        printf("fidlError: Unable to malloc conmean\n");
                        exit(-1);
                        }
                    if(!(consem=(double*)malloc(sizeof*consem*num_tc_frames))) {
                        printf("fidlError: Unable to malloc consem\n");
                        exit(-1);
                        }
                    if(!read_confile(confile,how_many,rbf->region_names_ptr,num_tc_frames,tags_tcs->tags_tcptr,tc_frames,contrasts,
                        tcs->num_tc)) exit(-1);
                    }
                if(lcvoxels) {
                    if(!(voxels=d3double(rbf->nvoxels*num_wfiles,nglmpersub,tcs->num_tc))) exit(-1);
                    if(!(voxels_unscaled=d3double(rbf->nvoxels*num_wfiles,nglmpersub,tcs->num_tc))) exit(-1);
                    for(i=0;i<rbf->nvoxels*num_wfiles;i++)for(j=0;j<nglmpersub;j++)for(k=0;k<tcs->num_tc;k++)
                        voxels[i][j][k]=voxels_unscaled[i][j][k]=(double)UNSAMPLED_VOXEL;
                    }
                }
            if(lcphase) {
                if(!(tcphase_unscaled=d3double(how_many*num_wfiles,nglmpersub,tcs->num_tc))) exit(-1);
                if(!(tcavgphase_unscaled=d2float(how_many*num_wfiles,tcs->num_tc))) exit(-1);
                }
            if(lc_within_subject_sd) {
            
                std::cout<<"here0"<<std::endl;

                if(!(within_subject_sd=d3double(how_many*num_wfiles,nglmpersub,tcs->num_tc))) exit(-1);
                if(!(within_subject_sd_unscaled=d3double(how_many*num_wfiles,nglmpersub,tcs->num_tc))) exit(-1);
                }
            }
        else if(filetype==(int)CIFTI||filetype==(int)CIFTI_DTSERIES){ 
            if(!(temp_float2=(float*)malloc(sizeof*temp_float2*how_many*tc_frames_max))){
                printf("fidlError: Unable to malloc temp_float2\n");
                exit(-1);
                }
            for(i=0;i<how_many*tc_frames_max;i++)temp_float2[i]=0.;
            }
        }


    //std::cout<<"here2"<<std::endl;

    if(lcindbase) {
        if(!(indbase_col=d2intvar(nglmpersub,tcs->total_each))) exit(-1);
        for(mm=m=0;m<nglmpersub;mm+=glmpersub[m++]) { 
            if(!(get_indbase(tcs,fbf[m],&glmstack[mm],&meancol_stack[mm],indbase_col[m],m))) exit(-1);
            printf("indbase_col subject %d\n",m+1);
            for(i=0;i<tcs->total;i++) printf("%d ",indbase_col[m][i]); printf("\n");
            }
        }
    if((lcmaxdef||lcmaxdefonly) && !lc_names_only) {
        if(!(maxdef=(float*)malloc(sizeof*maxdef*how_many))) {
            printf("fidlError: Unable to malloc maxdef\n");
            exit(-1);
            }
        if(!(maxdeffr=(float*)malloc(sizeof*maxdeffr*how_many))) {
            printf("fidlError: Unable to malloc maxdeffr\n");
            exit(-1);
            }
        for(m=0;m<how_many;m++) maxdef[m]=maxdeffr[m]=0.;
        }
    if(!lc_names_only) for(m=0;m<how_many*num_wfiles;m++) nsubjects[m]=0;

    //std::cout<<"here3"<<std::endl;

    for(j2=j1=l=jj=p=j=k=0;k<tcs->num_tc;k++){
        if(!lc_names_only){
            for(q=n=0;n<num_wfiles;n++,q+=how_many)for(i=0;i<ms_reg->lenbrain;i++)nsubjects[q+ms_reg->brnidx[i]]=0;
            if(!hipass[k]||!lcphase_only) {
                if(lcprint_scaled_tc) for(m=0;m<how_many*num_wfiles;m++) avg[m]=avg2[m]=0.;
                if(lcprint_unscaled_tc) for(m=0;m<how_many*num_wfiles;m++) avg_unscaled[m]=avg2_unscaled[m]=0.;
                }
            if(lcphase) {
                for(m=0;m<how_many*num_wfiles;m++) avgphase_unscaled[m]=0.;
                }
            for(mm=m=0;m<nglmpersub;mm+=glmpersub[m++]) {
                for(i=0;i<vol;i++)temp_double[i]=temp_double_unscaled[i]=td1[i]=td1un[i]=td6[i]=td7[i]=td8[i]=mag[i]=mag_unscaled[i]=0.;
                if(lcphase) for(i=0;i<vol;i++) phase_unscaled[i] = 0.;
                if(!num_tc_weights) {
                    scl = hipass[k] ? tcs->num_tc_to_sum[k][m]/2. : 0.;
                    if(!glmstack[0]->ATAm1) for(i=0;i<vol;i++) scl_stackvox[0][i] = scl;
                    }

                if(lcroiIND){
                    rbf->nreg=reg[m]->nregions;
                    rbf->nvoxels_region=reg[m]->nvoxels_region;
                    rbf->indices=&reg[m]->voxel_indices[0][0];
                    ms->lenbrain=reg[m]->nvoxels;
                    ms->brnidx=&reg[m]->voxel_indices[0][0];
                    }


                for(sumdf=0.,num_tc_to_sum_actual=ipairs=i=0;i<tcs->num_tc_to_sum[k][m];i++,kk1[m]++) {
                    glmi = mm+fbf[m]->file_index[kk1[m]];
                    tci = fbf[m]->roi_index[kk1[m]];
                    if(tci==-1) continue;
                    num_tc_to_sum_actual++;
                    if(!(fp=fopen_sub(glmfiles->files[glmi],"r"))) exit(-1);


                    #if 0
                    offset = startb[glmi]+(long)(sizeof(float)*vol*tci);
                    if(fseek(fp,offset,(int)SEEK_SET)) {
                        printf("fidlError: occurred while seeking to %ld in %s.\n",offset,glmfiles->files[glmi]);
                        exit(-1);
                        }
                    if(!fread_sub(temp_float,sizeof(float),(size_t)vol,fp,swapbytesin[glmi],glmfiles->files[glmi])) {
                        printf("fidlError: reading parameter estimates from %s.\n",glmfiles->files[m]);
                        exit(-1);
                        }
                    for(n=0;n<vol;n++) td1un[n] = temp_float[n]==(float)UNSAMPLED_VOXEL?(double)UNSAMPLED_VOXEL:(double)temp_float[n];
                    #endif
                    //START160928
                    if(fseek(fp,startb[glmi]+(long)(sizeof(float)*glmstack[glmi]->nmaski*tci),(int)SEEK_SET)){
                        printf("fidlError: occurred while seeking to %p in %s.\n",fp,glmfiles->files[glmi]);
                        exit(-1);
                        }
                    if(!fread_sub(temp_float,sizeof(float),(size_t)glmstack[glmi]->nmaski,fp,swapbytesin[glmi],glmfiles->files[glmi])){
                        printf("fidlError: reading parameter estimates from %s.\n",glmfiles->files[m]);
                        exit(-1);
                        }

                    //printf("here4 ap->vol=%d glmstack[%d]->nmaski=%d\n",ap->vol,glmi,glmstack[glmi]->nmaski);fflush(stdout);

                    for(n=0;n<glmstack[glmi]->nmaski;n++)td1un[glmstack[glmi]->maski[n]]=
                        temp_float[n]==(float)UNSAMPLED_VOXEL?(double)UNSAMPLED_VOXEL:(double)temp_float[n];


//#if 0
                    if(glmstack[0]->ATAm1&&lc_within_subject_sd&&!num_tc_weights&&(!hipass[k]||!lcphase_only))
                        for(n=0;n<glmstack[glmi]->nmaski;n++)td6[glmstack[glmi]->maski[n]]=
                            glmstack[glmi]->var[n]<=0.?(double)UNSAMPLED_VOXEL:glmstack[glmi]->var[n];

#if 1
                    if((!hipass[k]||!lcphase_only)&&lcprint_scaled_tc){
                        if(!lcindbase){

                            //printf("************** HERE0 *****************\n");fflush(stdout);

                            for(n=0;n<glmstack[glmi]->nmaski;n++)td1[glmstack[glmi]->maski[n]]=
                                td1un[glmstack[glmi]->maski[n]]==(double)UNSAMPLED_VOXEL?(double)UNSAMPLED_VOXEL:
                                (td1un[glmstack[glmi]->maski[n]]*(gms_stack[glmi]->grand_mean[n]<gms_stack[glmi]->grand_mean_thresh?
                                0.:gms_stack[glmi]->pct_chng_scl/gms_stack[glmi]->grand_mean[n]));

                            if(glmstack[0]->ATAm1&&lc_within_subject_sd&&!num_tc_weights){
                                for(n=0;n<glmstack[glmi]->nmaski;n++)td7[glmstack[glmi]->maski[n]]=
                                    td6[glmstack[glmi]->maski[n]]==(double)UNSAMPLED_VOXEL?(double)UNSAMPLED_VOXEL:
                                    (td6[glmstack[glmi]->maski[n]]*(gms_stack[glmi]->grand_mean[n]<gms_stack[glmi]->grand_mean_thresh?
                                    0.:gms_stack[glmi]->pct_chng_scl/gms_stack[glmi]->grand_mean[n]));
                                }
                            }
                        else {
                            if(fseek(fp,startb[glmi]+(long)(sizeof(float)*glmstack[glmi]->nmaski*indbase_col[m][kk1[m]]),SEEK_SET)){
                                printf("fidlError: fidl_avg_zstat Occurred while seeking to %p in %s.\n",fp,glmfiles->files[glmi]);
                                exit(-1);
                                }
                            if(!fread_sub(temp_float,sizeof(float),(size_t)glmstack[glmi]->nmaski,fp,swapbytesin[glmi],
                                glmfiles->files[glmi])){
                                printf("fidlError: reading parameter estimates from %s.\n",glmfiles->files[glmi]);
                                exit(-1);
                                }
                            for(n=0;n<glmstack[glmi]->nmaski;n++)td1[glmstack[glmi]->maski[n]]= 
                                td1un[glmstack[glmi]->maski[n]]==(double)UNSAMPLED_VOXEL?(double)UNSAMPLED_VOXEL: 
                                td1un[glmstack[glmi]->maski[n]]*100./(double)temp_float[n];
                            if(glmstack[0]->ATAm1&&lc_within_subject_sd&&!num_tc_weights){
                                for(n=0;n<glmstack[glmi]->nmaski;n++)td7[glmstack[glmi]->maski[n]]= 
                                    td6[glmstack[glmi]->maski[n]]==(double)UNSAMPLED_VOXEL?(double)UNSAMPLED_VOXEL: 
                                    td6[glmstack[glmi]->maski[n]]*100./(double)temp_float[n];
                                }

                            }
                        }
#endif

                    dptr1=td1un;dptr=td1;

                    //std::cout<<"here2 dptr=";for(n=0;n<glmstack[glmi]->nmaski;n++)std::cout<<" "<<dptr[glmstack[glmi]->maski[n]];std::cout<<std::endl;
                    //OK


                    if(glmstack[0]->ATAm1&&lc_within_subject_sd&&!num_tc_weights)dptr6=td6;dptr7=td7;
                    if(num_xform_files){
                        if(lcprint_unscaled_tc){
                            if(!t4_atlas(dptr1,td4,&t4[glmi*(int)T4SIZE],glmstack[glmi]->ifh->glm_xdim,
                                glmstack[glmi]->ifh->glm_ydim,glmstack[glmi]->ifh->glm_zdim,glmstack[glmi]->ifh->glm_dxdy,
                                glmstack[glmi]->ifh->glm_dz,A_or_B_or_U[glmi],glmstack[glmi]->ifh->orientation,ap,(double*)NULL))
                                exit(-1);
                            dptr1=td4;
                            if(glmstack[0]->ATAm1&&lc_within_subject_sd&&!num_tc_weights){
                                if(!t4_atlas(dptr6,td9,&t4[glmi*(int)T4SIZE],glmstack[glmi]->ifh->glm_xdim,
                                    glmstack[glmi]->ifh->glm_ydim,glmstack[glmi]->ifh->glm_zdim,glmstack[glmi]->ifh->glm_dxdy,
                                    glmstack[glmi]->ifh->glm_dz,A_or_B_or_U[glmi],glmstack[glmi]->ifh->orientation,ap,(double*)NULL))
                                    exit(-1);
                                dptr6=td9;
                                }
                            }
                        if(lcprint_scaled_tc){
                            if(!t4_atlas(dptr,stat,&t4[glmi*(int)T4SIZE],glmstack[glmi]->ifh->glm_xdim,
                                glmstack[glmi]->ifh->glm_ydim,glmstack[glmi]->ifh->glm_zdim,glmstack[glmi]->ifh->glm_dxdy,
                                glmstack[glmi]->ifh->glm_dz,A_or_B_or_U[m],glmstack[glmi]->ifh->orientation,ap,(double*)NULL))
                                exit(-1);
                            dptr=stat;
                            if(glmstack[0]->ATAm1&&lc_within_subject_sd&&!num_tc_weights){
                                if(!t4_atlas(dptr7,td8,&t4[glmi*(int)T4SIZE],glmstack[glmi]->ifh->glm_xdim,
                                    glmstack[glmi]->ifh->glm_ydim,glmstack[glmi]->ifh->glm_zdim,glmstack[glmi]->ifh->glm_dxdy,
                                    glmstack[glmi]->ifh->glm_dz,A_or_B_or_U[glmi],glmstack[glmi]->ifh->orientation,ap,(double*)NULL))
                                    exit(-1);
                                dptr7=td8;
                                }
                            }
                        }
                    if(num_tc_weights){
                        if(lcprint_unscaled_tc)for(n=0;n<ms->lenbrain;n++){
                            if(dptr1[ms->brnidx[n]]==(double)UNSAMPLED_VOXEL) 
                                temp_double_unscaled[ms->brnidx[n]]=(double)UNSAMPLED_VOXEL;
                            else if(temp_double_unscaled[n]!=(double)UNSAMPLED_VOXEL)
                                temp_double_unscaled[ms->brnidx[n]]+=dptr1[ms->brnidx[n]]*tc_weights->tc[k][m][i];
                            }
                        if(lcprint_scaled_tc)for(n=0;n<<ms->lenbrain;n++){
                            if(dptr[ms->brnidx[n]]==(double)UNSAMPLED_VOXEL) 
                                temp_double[ms->brnidx[n]]=(double)UNSAMPLED_VOXEL;
                            else if(temp_double[ms->brnidx[n]]!=(double)UNSAMPLED_VOXEL) 
                                temp_double[ms->brnidx[n]]+=dptr[ms->brnidx[n]]*tc_weights->tc[k][m][i];
                            }
                        }
                    else {
                        if(!hipass[k]){
                            if(glmstack[0]->ATAm1){
                                if(lcprint_unscaled_tc){
                                    for(n=0;n<ms->lenbrain;n++){
                                        if(dptr1[ms->brnidx[n]]==(double)UNSAMPLED_VOXEL) 
                                            temp_double_unscaled[ms->brnidx[n]]=(double)UNSAMPLED_VOXEL;
                                        else if(temp_double_unscaled[ms->brnidx[n]]!=(double)UNSAMPLED_VOXEL)
                                            temp_double_unscaled[ms->brnidx[n]]+=
                                            dptr1[ms->brnidx[n]]/(double)glmstack[glmi]->ATAm1[tci][tci];
                                        }
                                    }
                                if(lcprint_scaled_tc){
                                    for(n=0;n<ms->lenbrain;n++){
                                        if(dptr[ms->brnidx[n]]==(double)UNSAMPLED_VOXEL) 
                                            temp_double[ms->brnidx[n]]=(double)UNSAMPLED_VOXEL;
                                        else if(temp_double[ms->brnidx[n]]!=(double)UNSAMPLED_VOXEL)
                                            temp_double[ms->brnidx[n]]+=dptr[ms->brnidx[n]]/(double)glmstack[glmi]->ATAm1[tci][tci];
                                        }
                                    }
                                scl+=1./(double)glmstack[glmi]->ATAm1[tci][tci];
                                }
                            else{
                                if(lcprint_unscaled_tc)for(n=0;n<ms->lenbrain;n++){
                                    if(dptr1[ms->brnidx[n]]==(double)UNSAMPLED_VOXEL) 
                                        temp_double_unscaled[ms->brnidx[n]]=(double)UNSAMPLED_VOXEL;
                                    else if(temp_double_unscaled[ms->brnidx[n]]!=(double)UNSAMPLED_VOXEL)
                                        temp_double_unscaled[ms->brnidx[n]]+=dptr1[ms->brnidx[n]]/(double)glmstack[glmi]->
                                        ATAm1vox[tci*glmstack[glmi]->ifh->glm_Mcol+tci-off[tci]][ms->brnidx[n]];
                                    }
                                if(lcprint_scaled_tc)for(n=0;n<ms->lenbrain;n++){
                                    if(dptr[ms->brnidx[n]]==(double)UNSAMPLED_VOXEL) 
                                        temp_double[ms->brnidx[n]]=(double)UNSAMPLED_VOXEL;
                                    else if(temp_double[ms->brnidx[n]]!=(double)UNSAMPLED_VOXEL) 
                                        temp_double[ms->brnidx[n]]+=dptr[ms->brnidx[n]]/(double)glmstack[glmi]->
                                        ATAm1[tci*glmstack[glmi]->ifh->glm_Mcol+tci-off[tci]][ms->brnidx[n]];
                                    }
                                for(n=0;n<ms->lenbrain;n++)scl_stackvox[0][ms->brnidx[n]]+=
                                    1./(double)glmstack[glmi]->ATAm1vox[tci*glmstack[glmi]->ifh->glm_Mcol+tci-off[tci]][ms->brnidx[n]];
                                }
                            }
                        else {
                            if(!lcphase_only) {
                                if(!dcorrection) {
                                    if(!lcscargle) {
                                        scargle = 1.;
                                        }
                                    else {
                                        for(scargle=0.,n=0;n<glmstack[glmi]->ifh->glm_Nrow;n++) {
                                            td = (double)glmstack[glmi]->AT[tci][n];
                                            scargle += td*td;
                                            }
                                        /*printf("column=%d scargle=%f\n",tci+1,scargle);*/
                                        }
                                    if(lcprint_unscaled_tc)for(n=0;n<ms->lenbrain;n++){
                                        if(dptr1[ms->brnidx[n]]==(double)UNSAMPLED_VOXEL) 
                                            temp_double_unscaled[ms->brnidx[n]]=(double)UNSAMPLED_VOXEL;
                                        else if(temp_double_unscaled[ms->brnidx[n]]!=(double)UNSAMPLED_VOXEL)
                                            temp_double_unscaled[ms->brnidx[n]]+=dptr1[ms->brnidx[n]]*dptr1[ms->brnidx[n]]/scargle;
                                        }
                                    if(lcprint_scaled_tc)for(n=0;n<ms->lenbrain;n++){
                                        if(dptr[ms->brnidx[n]]==(double)UNSAMPLED_VOXEL) 
                                            temp_double[ms->brnidx[n]]=(double)UNSAMPLED_VOXEL;
                                        else if(temp_double[ms->brnidx[n]]!=(double)UNSAMPLED_VOXEL) 
                                            temp_double[ms->brnidx[n]]+=dptr[ms->brnidx[n]]*dptr[ms->brnidx[n]]/scargle;
                                        }
                                    }
                                else {
                                    if(!(i%2)){
                                        if(lcprint_unscaled_tc)for(n=0;n<ms->lenbrain;n++){
                                            if(dptr1[ms->brnidx[n]]==(double)UNSAMPLED_VOXEL)
                                                temp_double_unscaled[ms->brnidx[n]]=(double)UNSAMPLED_VOXEL;
                                            else pairsun[ms->brnidx[n]]=dptr1[ms->brnidx[n]]*dptr1[ms->brnidx[n]];
                                            }
                                        if(lcprint_scaled_tc)for(n=0;n<ms->lenbrain;n++){
                                            if(dptr[ms->brnidx[n]]==(double)UNSAMPLED_VOXEL) 
                                                temp_double[ms->brnidx[n]]=(double)UNSAMPLED_VOXEL;
                                            else pairs[ms->brnidx[n]]=dptr[ms->brnidx[n]]*dptr[ms->brnidx[n]];
                                            }
                                        }
                                    else{
                                        if(lcprint_unscaled_tc)for(n=0;n<ms->lenbrain;n++){
                                            if(dptr1[ms->brnidx[n]]==(double)UNSAMPLED_VOXEL) 
                                                temp_double_unscaled[ms->brnidx[n]]=(double)UNSAMPLED_VOXEL;
                                            else if(temp_double_unscaled[ms->brnidx[n]]!=(double)UNSAMPLED_VOXEL)
                                                temp_double_unscaled[ms->brnidx[n]]+=(dptr1[ms->brnidx[n]]*dptr1[ms->brnidx[n]]+
                                                pairsun[ms->brnidx[n]])*dcorrection->x[m][ipairs];
                                            }
                                        if(lcprint_scaled_tc)for(n=0;n<ms->lenbrain;n++){
                                            if(dptr[ms->brnidx[n]]==(double)UNSAMPLED_VOXEL) 
                                                temp_double[ms->brnidx[n]]=(double)UNSAMPLED_VOXEL;
                                            else if(temp_double[ms->brnidx[n]]!=(double)UNSAMPLED_VOXEL)
                                                temp_double[ms->brnidx[n]]+=(dptr[ms->brnidx[n]]*dptr[ms->brnidx[n]]+
                                                pairs[ms->brnidx[n]])*dcorrection->x[m][ipairs];
                                            }
                                        if(++ipairs==dcorrection->npoints_per_line[m])ipairs=0;
                                        }
                                    }
                                }
                            if(lcphase){
                                if(!(i%2))for(n=0;n<ms->lenbrain;n++){
                                    if(dptr1[ms->brnidx[n]]==(double)UNSAMPLED_VOXEL) 
                                        phase_unscaled[ms->brnidx[n]]=(double)UNSAMPLED_VOXEL;
                                    else td5[ms->brnidx[n]]=dptr1[ms->brnidx[n]];
                                    }
                                else for(n=0;n<ms->lenbrain;n++){
                                    if(dptr1[ms->brnidx[n]]==(double)UNSAMPLED_VOXEL) 
                                        phase_unscaled[ms->brnidx[n]]=(double)UNSAMPLED_VOXEL;
                                    else if(phase_unscaled[ms->brnidx[n]]!=(double)UNSAMPLED_VOXEL)
                                        phase_unscaled[ms->brnidx[n]]+=atan2(td5[ms->brnidx[n]],dptr1[ms->brnidx[n]]);
                                    }

                                }
                            }
                        if(glmstack[0]->ATAm1&&lc_within_subject_sd&&(!hipass[k]||!lcphase_only)){
                            if(lcprint_unscaled_tc){

                                //std::cout<<"here0a"<<std::endl;

                                //for(n=0;n<<ms->lenbrain;n++)
                                //START210519
                                for(n=0;n<ms->lenbrain;n++)

                                    if(dptr6[ms->brnidx[n]]==(double)UNSAMPLED_VOXEL) 
                                        mag_unscaled[ms->brnidx[n]]=(double)UNSAMPLED_VOXEL;
                                    else if(mag_unscaled[ms->brnidx[n]]!=(double)UNSAMPLED_VOXEL)
                                        mag_unscaled[ms->brnidx[n]]+=
                                            dptr6[ms->brnidx[n]]*glmstack[glmi]->ATAm1[tci][tci]*(double)glmstack[glmi]->ifh->glm_df;

                                //std::cout<<"here0a mag_unscaled=";for(n=0;i<ms->lenbrain;i++)std::cout<<" "<<mag_unscaled[ms->brnidx[i]];std::cout<<std::endl;
                                std::cout<<"here0a glmstack["<<glmi<<"]->ifh->glm_df="<<glmstack[glmi]->ifh->glm_df<<std::endl; 
                                }
                            if(lcprint_scaled_tc){
                                for(n=0;n<<ms->lenbrain;n++)
                                    if(dptr7[ms->brnidx[n]]==(double)UNSAMPLED_VOXEL) 
                                        mag[ms->brnidx[n]]=(double)UNSAMPLED_VOXEL;
                                    else if(mag[ms->brnidx[n]]!=(double)UNSAMPLED_VOXEL)
                                        mag[ms->brnidx[n]]+=
                                            dptr6[ms->brnidx[n]]*glmstack[glmi]->ATAm1[tci][tci]*(double)glmstack[glmi]->ifh->glm_df;
                                }
                            sumdf+=(double)glmstack[glmi]->ifh->glm_df;
                            } 
                        }
                    fclose(fp);
                    } /*for(ipairs=i=0;i<tcs->num_tc_to_sum[k][m];i++,kk1[m]++)*/
                if(!num_tc_to_sum_actual)continue; 
                if(!num_tc_weights&&(!hipass[k]||!lcphase_only)){
                    if(glmstack[0]->ATAm1){
                        if(lcprint_unscaled_tc){
                            for(i=0;i<ms->lenbrain;i++)
                                if(temp_double_unscaled[ms->brnidx[i]]!=(double)UNSAMPLED_VOXEL)
                                    temp_double_unscaled[ms->brnidx[i]]/=scl;
                            if(lc_within_subject_sd){
                                for(i=0;i<ms->lenbrain;i++)if(mag_unscaled[ms->brnidx[i]]!=(double)UNSAMPLED_VOXEL)mag_unscaled[ms->brnidx[i]]/=sumdf;

                                std::cout<<"here0b mag_unscaled=";for(i=0;i<ms->lenbrain;i++)std::cout<<" "<<mag_unscaled[ms->brnidx[i]];std::cout<<std::endl;
                                }
                            
                            }


                        if(lcprint_scaled_tc){
                            for(i=0;i<ms->lenbrain;i++) 
                                if(temp_double[ms->brnidx[i]]!=(double)UNSAMPLED_VOXEL)temp_double[ms->brnidx[i]]/=scl;
                            if(lc_within_subject_sd){
                                for(i=0;i<ms->lenbrain;i++)if(mag[ms->brnidx[i]]!=(double)UNSAMPLED_VOXEL)mag[ms->brnidx[i]]/=sumdf;
                                }

                            }
                        }
                    else {
                        if(lcprint_unscaled_tc){for(i=0;i<ms->lenbrain;i++) 
                           if(temp_double_unscaled[ms->brnidx[i]]!=(double)UNSAMPLED_VOXEL) 
                                temp_double_unscaled[ms->brnidx[i]]/=scl_stackvox[0][ms->brnidx[i]];
                            }
                        if(lcprint_scaled_tc){for(i=0;i<ms->lenbrain;i++)
                            if(temp_double[ms->brnidx[i]]!=(double)UNSAMPLED_VOXEL) 
                                temp_double[ms->brnidx[i]]/=scl_stackvox[0][ms->brnidx[i]];
                            }
                        }
                    }
                if(lcphase){
                    for(i=0;i<ms->lenbrain;i++){if(phase_unscaled[ms->brnidx[i]]!=(double)UNSAMPLED_VOXEL) 
                        if((phase_unscaled[ms->brnidx[i]]/=scl)<0.)phase_unscaled[ms->brnidx[i]]+=twopi;
                        }
                    if(lcdegrees){for(i=0;i<ms->lenbrain;i++)
                        if(phase_unscaled[ms->brnidx[i]]!=(double)UNSAMPLED_VOXEL)phase_unscaled[ms->brnidx[i]]*=180./(double)M_PI;
                        }
                    }

                if(hipass[k]&&!lcmsqa&&!lcphase_only) {
                    /*printf("root mean square amplitude power\n");*/
                    if(lcprint_unscaled_tc){for(i=0;i<ms->lenbrain;i++)if(temp_double_unscaled[ms->brnidx[i]]!=(double)UNSAMPLED_VOXEL)
                        temp_double_unscaled[ms->brnidx[i]]=sqrt(temp_double_unscaled[ms->brnidx[i]]);
                        }
                    if(lcprint_scaled_tc){for(i=0;i<ms->lenbrain;i++)if(temp_double[ms->brnidx[i]]!=(double)UNSAMPLED_VOXEL) 
                        temp_double[ms->brnidx[i]]=sqrt(temp_double[ms->brnidx[i]]);
                        }
                    }
                dptr1=temp_double_unscaled;dptr=temp_double;dptr2=phase_unscaled;
                if(fwhm>0.){
                    if(!hipass[k]||!lcphase_only){
                        if(lcprint_unscaled_tc){
                            gauss_smoth2(dptr1,td4,gs);
                            dptr1=td4;
                            }
                        if(lcprint_scaled_tc) {
                            gauss_smoth2(dptr,stat,gs);
                            dptr=stat;
                            }
                        }
                    if(lcphase){
                        if(lcprint_unscaled_tc){
                            gauss_smoth2(dptr2,td5,gs);
                            dptr2=td5;
                            }
                        }
                    }
                if(num_region_files) {
                    if(!hipass[k]||!lcphase_only) {

                        //START200224
                        if(lcroiIND){
                            rbf->nreg=reg[m]->nregions;
                            rbf->nvoxels_region=reg[m]->nvoxels_region;
                            rbf->indices=&reg[m]->voxel_indices[0][0];
                            }

                        if(lcprint_scaled_tc) {
                            if(lcvoxels)for(n=q=0;q<num_wfiles;q++)for(i=0;i<rbf->nvoxels;i++,n++)voxels[n][m][k]=dptr[rbf->indices[i]];
                            for(n=q=0;q<num_wfiles;q++){
                                if(wfiles)for(i=0;i<ap->vol;i++)weights[i]=(double)mm_wfiles[q]->ptr[i];
                                crsw(dptr,&td1[n],rbf,weights,rbf->indices);
                                for(i=0;i<how_many;i++,n++)time_courses[n][m][j]=td1[n];
                                }
                            dptr=td1; 
                            }
                        if(lcprint_unscaled_tc) {
                            if(lcvoxels)for(n=q=0;q<num_wfiles;q++)for(i=0;i<rbf->nvoxels;i++,n++)voxels_unscaled[n][m][k]=dptr1[rbf->indices[i]];
                            for(n=q=0;q<num_wfiles;q++){
                                if(wfiles)for(i=0;i<ap->vol;i++)weights[i]=(double)mm_wfiles[q]->ptr[i];
                                crsw(dptr1,&td1un[n],rbf,weights,rbf->indices);
                                for(i=0;i<how_many;i++,n++)time_courses_unscaled[n][m][j]=td1un[n];
                                }
                            dptr1=td1un;
                            }
                        if(lc_within_subject_sd) {
                            for(o=n=q=0;q<num_wfiles;q++) {
                                if(wfiles) for(i=0;i<ap->vol;i++) weights[i] = (double)mm_wfiles[q]->ptr[i];
                                crsw(mag,stat,rbf,weights,rbf->indices);
                                for(i=0;i<how_many;i++,n++) within_subject_sd[n][m][j] = stat[i];
                                crsw(mag_unscaled,stat,rbf,weights,rbf->indices);
                                for(i=0;i<how_many;i++,o++) within_subject_sd_unscaled[o][m][j] = stat[i];
                                }
                            }
                        }
                    if(lcphase) {
                        for(n=q=0;q<num_wfiles;q++){
                            if(wfiles)for(i=0;i<ap->vol;i++)weights[i]=(double)mm_wfiles[q]->ptr[i];
                            crsw(dptr2,&td6[n],rbf,weights,rbf->indices);
                            for(i=0;i<how_many;i++,n++)tcphase_unscaled[n][m][j]=td6[n];
                            }
                        dptr2=td6;
                        }
                    }
                else if(regional_avgstat_name) {

                    std::cout<<"here1"<<std::endl;

                    if(lcprint_scaled_tc)for(i=0;i<how_many;i++)time_courses[i][m][j]=dptr[i];
                    if(lcprint_unscaled_tc)for(i=0;i<how_many;i++)time_courses_unscaled[i][m][j]=dptr1[i];

                    //START210519
                    if(lc_within_subject_sd)for(i=0;i<how_many;i++)within_subject_sd_unscaled[i][m][j]=mag_unscaled[i]; 
                    if(lc_within_subject_sd){
                        std::cout<<"here1 mag_unscaled=";for(i=0;i<how_many;i++)std::cout<<" "<<mag_unscaled[i];std::cout<<std::endl;
                        for(i=0;i<how_many;i++)std::cout<<"here1 within_subject_sd_unscaled["<<i<<"]["<<m<<"]["<<j<<"]="<<within_subject_sd_unscaled[i][m][j]<<std::endl;
                        }


                    }
                else if(hipass[k]&&lcindhipass) {
                    if(!lcphase_only) {
                        sprintf(string,"%s_%s%s%s",glm_names[m],tags_tcs->tags_tcptr[k],appendstr,ampstr);
                        if(lcprint_scaled_tc) {
                            for(i=0;i<how_many;i++)temp_float[i]=(float)dptr[i];
                            sprintf(string2,"%s%s",string,Fileext[filetype]);
                            w1->temp_float=temp_float;
                            if(!write1(string2,w1))exit(-1);
                            printf("Output written to %s\n",string2);
                            }
                        if(lcprint_unscaled_tc) {
                            for(i=0;i<how_many;i++)temp_float[i]=(float)dptr1[i];
                            sprintf(string2,"%s_unscaled%s",string,Fileext[filetype]);
                            w1->temp_float=temp_float;
                            if(!write1(string2,w1))exit(-1);
                            printf("Output written to %s\n",string2);
                            }
                        }
                    if(lcphase) {
                        sprintf(string,"%s_%s%s_phaseuns%s",glm_names[m],tags_tcs->tags_tcptr[k],appendstr,lcphase?"deg":"rad");
                        if(tc_frames[p]>1) {
                            sprintf(string2,"_run%d",j+1);
                            strcat(string,string2);
                            }
                        for(i=0;i<how_many;i++)temp_float[i]=(float)dptr2[i];
                        sprintf(string2,"%s%s",string,Fileext[filetype]);
                        w1->temp_float=temp_float;
                        if(!write1(string2,w1))exit(-1);
                        printf("Output written to %s\n",string2);
                        for(td=0.,i=0;i<ms->lenbrain;i++) td += phase_unscaled[ms->brnidx[i]];
                        printf("average phase = %f\n",td/(double)ms->lenbrain);
                        }
                    }
                if(!hipass[k]||!lcphase_only) {
                    if(lcprint_scaled_tc) {

                        //printf("here99 k=%d\n",k);fflush(stdout);

                        for(q=n=0;n<num_wfiles;n++,q+=how_many)for(i=0;i<ms_reg->lenbrain;i++)
                            avg[q+ms_reg->brnidx[i]]+=dptr[q+ms_reg->brnidx[i]];
                        if(lcsd||lcsem)for(q=n=0;n<num_wfiles;n++,q+=how_many)for(i=0;i<ms_reg->lenbrain;i++)
                            avg2[q+ms_reg->brnidx[i]]+=dptr[q+ms_reg->brnidx[i]]*dptr[q+ms_reg->brnidx[i]];

                        //if(!k)for(q=n=0;n<num_wfiles;n++,q+=how_many)for(i=0;i<ms_reg->lenbrain;i++) 
                        //    if(dptr[q+ms_reg->brnidx[i]]!=(double)UNSAMPLED_VOXEL)nsubjects[q+ms_reg->brnidx[i]]++;
                        //START181113
                        for(q=n=0;n<num_wfiles;n++,q+=how_many)for(i=0;i<ms_reg->lenbrain;i++) 
                            if(dptr[q+ms_reg->brnidx[i]]!=(double)UNSAMPLED_VOXEL)nsubjects[q+ms_reg->brnidx[i]]++;


                        }
                    if(lcprint_unscaled_tc) {
                        
                        
                        for(q=n=0;n<num_wfiles;n++,q+=how_many)for(i=0;i<ms_reg->lenbrain;i++)
                            avg_unscaled[q+ms_reg->brnidx[i]]+=dptr1[q+ms_reg->brnidx[i]];
                        if(lcsd||lcsem)for(q=n=0;n<num_wfiles;n++,q+=how_many)for(i=0;i<ms_reg->lenbrain;i++)
                            avg2_unscaled[q+ms_reg->brnidx[i]]+=dptr1[q+ms_reg->brnidx[i]]*dptr1[q+ms_reg->brnidx[i]];

                        //if(!k&&!lcprint_scaled_tc)for(q=n=0;n<num_wfiles;n++,q+=how_many)for(i=0;i<ms_reg->lenbrain;i++) 
                        //    if(dptr1[i]!=(double)UNSAMPLED_VOXEL)nsubjects[i]++;
                        //START181113
                        if(!lcprint_scaled_tc)for(q=n=0;n<num_wfiles;n++,q+=how_many)for(i=0;i<ms_reg->lenbrain;i++) 
                            if(dptr1[q+ms_reg->brnidx[i]]!=(double)UNSAMPLED_VOXEL)nsubjects[q+ms_reg->brnidx[i]]++;

                        //std::cout<<"here20"<<std::endl;

                        }
                    }
                if(lcphase) {
                    for(q=n=0;n<num_wfiles;n++,q+=how_many)for(i=0;i<ms_reg->lenbrain;i++)
                        avgphase_unscaled[q+ms_reg->brnidx[i]]+=dptr2[q+ms_reg->brnidx[i]];
                    if(!k&&lcphase_only)for(q=n=0;n<num_wfiles;n++,q+=how_many)for(i=0;i<ms_reg->lenbrain;i++)
                        if(dptr2[i]!=(double)UNSAMPLED_VOXEL)nsubjects[i]++;
                    }
                } /*for(mm=m=0;m<nglmpersub;mm+=glmpersub[m++])*/
//ENDHERE0



            } /*if(!lc_names_only)*/

        //std::cout<<"here50"<<std::endl;

        if(!j&&!regional_avgstat_name){
            if(!j1){

                //std::cout<<"here51"<<std::endl;

                string[0] = 0;
                if(directory) strcat(string,directory);
                if(scratchdir) strcat(string,scratchdir);
                if(group_name){
                    strcat(string,group_name);
                    strcat(string,"_");
                    }
//STARTHERE
                //if(!ntc_names) strcat(string,"avgtc_");
                //START210124
                if(!ntc_names){
                    strcat(string,glmfiles->nfiles>1?"avgtc":glm_names[0]);
                    strcat(string,"_");
                    }

                string2[0] = 0;
                if(ntc_names){
                    strcat(string2,tc_names->files[jj++]);
                    }
                else {
                    strcat(string2,tags_tcs->tags_tcptr[k]);

                    #if 0
                    for(m=k;m<k+tc_frames[p];m++) {
                        strcat(string2,"_");
                        strcat(string2,tags_tcs->tags_tc_effectsptr[m]);
                        }
                    #endif
                    //START210122
                    if(tc_frames){ 
                        for(m=k;m<k+tc_frames[p];m++){
                            strcat(string2,"_");
                            strcat(string2,tags_tcs->tags_tc_effectsptr[m]);
                            }
                        }
                    }
                }

            //std::cout<<"here52"<<std::endl;

            if(!hipass[k]||!lcphase_only){
                if(lcprint_scaled_tc){
                    if(!j1){

                        //std::cout<<"here53"<<std::endl;

                        sprintf(string3,"%s%s%s%s",string,string2,appendstr,ampstr);
                        sprintf(outfile,"%s%s",string3,Fileext[filetype]);

                        //std::cout<<"here54"<<std::endl;

                        if(!lc_names_only){


                            #if 0
                            if(!lcmaxdefonly&&(filetype==(int)IMG||filetype==(int)NIFTI)){if(!(fpw=open2(outfile,w1)))exit(-1);}
                            min_and_max_init(&min,&max);
                            #endif
                            //START210913
                            if(!p1.ptrw0(outfile,w1))exit(-1);


                            }
                        if(nframesout&&(lcmaxdef||lcmaxdefonly)){
                            sprintf(string4,"%s_maxdef%s",string3,Fileext[filetype]);
                            if(!lc_names_only){
                                if(!(fpmaxdef=open2(string4,w1)))exit(-1);
                                min_and_max_init(&ifh_out1->global_min,&ifh_out1->global_max);
                                }
                            }

                        //std::cout<<"here60"<<std::endl;

                        }
                    }
                if(lcprint_unscaled_tc) {

                    //std::cout<<"here21"<<std::endl;

                    #if 0
                    sprintf(outfile_unscaled,"%s%s%s%s_unscaled%s",string,string2,appendstr,ampstr,Fileext[filetype]);
                    if(!lc_names_only) {
                        if(!(fpw_unscaled=open2(outfile_unscaled,w1)))exit(-1);
                        min_and_max_init(&min_unscaled,&max_unscaled);
                        }
                    #endif
                    //START210124
                    sprintf(outfile_unscaled,"%s%s%s%s_unscaled%s",string,string2,appendstr,ampstr,Fileext[filetype]);
                    if(!lc_names_only) {
                        if(!p0.ptrw0(outfile_unscaled,w1))exit(-1);
                        }

                    //std::cout<<"here22"<<std::endl;

                    }

                //std::cout<<"here61"<<std::endl;

                if(lcsd) {
                    if(lcprint_scaled_tc) {
                        sprintf(outsdfile,"%ssd_%s%s",string,string2,Fileext[filetype]);
                        if(!lc_names_only) {
                            if(!(fpwsd=open2(outsdfile,w1)))exit(-1);
                            min_and_max_init(&sdmin,&sdmax);
                            }
                        }
                    if(lcprint_unscaled_tc) {
                        sprintf(outsdfile_unscaled,"%ssd_%s_unscaled%s",string,string2,Fileext[filetype]);
                        if(!lc_names_only) {
                            if(!(fpwsd_unscaled=open2(outsdfile_unscaled,w1)))exit(-1);
                            min_and_max_init(&sdmin_unscaled,&sdmax_unscaled);
                            }
                        }
                    }
                if(lcsem) {
                    if(lcprint_scaled_tc) {
                        sprintf(outsemfile,"%ssem_%s%s",string,string2,Fileext[filetype]);
                        if(!lc_names_only) {
                            if(!(fpwsem=open2(outsemfile,w1)))exit(-1);
                            min_and_max_init(&semmin,&semmax);
                            }
                        }
                    if(lcprint_unscaled_tc) {
                        sprintf(outsemfile_unscaled,"%ssem_%s_unscaled%s",string,string2,Fileext[filetype]);
                        if(!lc_names_only) {
                            if(!(fpwsem_unscaled=open2(outsemfile_unscaled,w1)))exit(-1);
                            min_and_max_init(&semmin_unscaled,&semmax_unscaled);
                            }
                        }
                    }
                }
            if(lcphase) {
                sprintf(outphase_unscaled,"%s%s%s_phaseuns%s%s",string,string2,appendstr,lcdegrees?"deg":"rad",Fileext[filetype]);
                if(!lc_names_only) {
                    if(!(fpwphase_unscaled=open2(outphase_unscaled,w1)))exit(-1);
                    min_and_max_init(&minphase_unscaled,&maxphase_unscaled);
                    }
                }
            }

        //std::cout<<"here70"<<std::endl;

        if(!lc_names_only&&!concfile){

            //std::cout<<"here71"<<std::endl;

            if(!hipass[k]||!lcphase_only) {
                if(lcprint_scaled_tc) {

                    //std::cout<<"here72"<<std::endl;

                    if(regional_avgstat_name) {

                        //std::cout<<"here73"<<std::endl;

                        for(i=0;i<how_many*num_wfiles;i++)time_courses_avg[i][j] = nsubjects[i] ? avg[i]/(double)nsubjects[i] : (double)UNSAMPLED_VOXEL;

                        //std::cout<<"here74"<<std::endl;
                        }
                    else {
                        if(filetype==(int)CIFTI||filetype==(int)CIFTI_DTSERIES){
                            tf0=&temp_float2[j*how_many];
                            }
                        else{
                            for(i=0;i<how_many;i++)temp_float[i]=0.;
                            tf0=temp_float;
                            }
                        for(i=0;i<ms->lenbrain;i++) tf0[ms->brnidx[i]] = nsubjects[ms->brnidx[i]] ?
                            (float)(avg[ms->brnidx[i]]/(double)nsubjects[ms->brnidx[i]]) : (float)UNSAMPLED_VOXEL;


                        #if 0
                        if(!(filetype==(int)CIFTI||filetype==(int)CIFTI_DTSERIES)){
                            if(!lcmaxdefonly){
                                min_and_max_new(temp_float,how_many,&min,&max,0);
                                if(!fwrite_sub(temp_float,sizeof(float),(size_t)how_many,(FILE*)fpw,swapbytes))exit(-1);
                                }
                            }
                        #endif
                        //START210913
                        if(!p1.ptrwstack(tf0,how_many,j))exit(-1);


                        if(lcmaxdef||lcmaxdefonly) {
                            for(i=0;i<ms->lenbrain;i++) {
                                if(fabs(temp_float[ms->brnidx[i]])>fabs(maxdef[brnidx[i]])) {
                                    maxdef[brnidx[i]] = temp_float[ms->brnidx[i]];
                                    if(temp_float[ms->brnidx[i]]!=(float)UNSAMPLED_VOXEL) {
                                        maxdeffr[ms->brnidx[i]] = (float)j+1.;
                                        } 
                                    } 
                                } 
                            }
                        }
                    }

                //std::cout<<"here75"<<std::endl;

                if(lcprint_unscaled_tc) {

                    //START210124
                    for(i=0;i<how_many*num_wfiles;i++) {
                        temp_float[i] = nsubjects[i] ? (float)(avg_unscaled[i]/(double)nsubjects[i]) : (float)UNSAMPLED_VOXEL;
                        }
                    if(regional_avgstat_name) {
                        for(i=0;i<how_many*num_wfiles;i++) time_courses_avg_unscaled[i][j] = temp_float[i];
                        }
                    else {

                        #if 0
                        if(filetype==(int)IMG){
                            min_and_max_new(temp_float,how_many,&min_unscaled,&max_unscaled,(int)FALSE);
                            if(!fwrite_sub(temp_float,sizeof(float),(size_t)how_many,(FILE*)fpw_unscaled,swapbytes)) exit(-1);
                            }
                        else if(filetype==(int)CIFTI||filetype==(int)CIFTI_DTSERIES){
                            printf("fidlError: Need to add code to output CIFTI. Abort!\n");exit(-1);
                            }
                        else{
                            if(!nifti_putvol((FILE*)fpw_unscaled,(int64_t)j,temp_float))exit(-1);
                            }
                        #endif
                        //START210124
                        if(!p0.ptrwstack(temp_float,how_many,j))exit(-1);

                        }

                    }
                if(lcsd||lcsem) {

                    //std::cout<<"here76"<<std::endl;

                    if(lcprint_scaled_tc) {
                        for(i=0;i<how_many*num_wfiles;i++) temp_double[i] = nsubjects[i]>1 ?
                            sqrt((avg2[i]-avg[i]*avg[i]/(double)nsubjects[i])/(double)(nsubjects[i]-1)) : (double)UNSAMPLED_VOXEL;
                        }
                    if(lcprint_unscaled_tc) {
                        for(i=0;i<how_many*num_wfiles;i++) temp_double_unscaled[i] = nsubjects[i]>1 ?
                            sqrt((avg2_unscaled[i]-avg_unscaled[i]*avg_unscaled[i]/(double)nsubjects[i])/(double)(nsubjects[i]-1)) : 
                            (double)UNSAMPLED_VOXEL;
                        }

                    //std::cout<<"here77"<<std::endl;

                    if(lcsd) {
                        if(lcprint_scaled_tc) {
                            for(i=0;i<how_many*num_wfiles;i++) temp_float[i] = (float)temp_double[i];
                            if(regional_avgstat_name) {
                                for(i=0;i<how_many*num_wfiles;i++) time_courses_sd[i][j] = temp_float[i];
                                }
                            else {
                                min_and_max_new(temp_float,how_many,&sdmin,&sdmax,(int)FALSE);
                                if(!fwrite_sub(temp_float,sizeof(float),(size_t)how_many,(FILE*)fpwsd,swapbytes)) exit(-1);
                                }
                            }
                        if(lcprint_unscaled_tc) {
                            for(i=0;i<how_many*num_wfiles;i++) temp_float[i] = (float)temp_double_unscaled[i]; 
                            if(regional_avgstat_name) {
                                for(i=0;i<how_many*num_wfiles;i++) time_courses_sd_unscaled[i][j] = temp_float[i];
                                }
                            else {
                                min_and_max_new(temp_float,how_many,&sdmin_unscaled,&sdmax_unscaled,(int)FALSE);
                                if(!fwrite_sub(temp_float,sizeof(float),(size_t)how_many,(FILE*)fpwsd_unscaled,swapbytes)) exit(-1);
                                }
                            }
                        }
                    else if(lcsem) {

                        //std::cout<<"here78"<<std::endl;

                        if(lcprint_scaled_tc) {
                            for(i=0;i<how_many*num_wfiles;i++) temp_float[i] = nsubjects[i]>1 ? 
                                (float)(temp_double[i]/sqrt((double)nsubjects[i])) : (float)UNSAMPLED_VOXEL;
                            if(regional_avgstat_name) {
                                for(i=0;i<how_many*num_wfiles;i++) time_courses_sd[i][j] = temp_float[i];
                                }
                            else {
                                min_and_max_new(temp_float,how_many,&semmin,&semmax,(int)FALSE);
                                if(!fwrite_sub(temp_float,sizeof(float),(size_t)how_many,(FILE*)fpwsem,swapbytes)) exit(-1);
                                }
                            }
                        if(lcprint_unscaled_tc) {
                            for(i=0;i<how_many*num_wfiles;i++) temp_float[i] = nsubjects[i]>1 ? 
                                (float)(temp_double_unscaled[i]/sqrt((double)nsubjects[i])) : (float)UNSAMPLED_VOXEL;
                            if(regional_avgstat_name) {
                                for(i=0;i<how_many*num_wfiles;i++) time_courses_sd_unscaled[i][j] = temp_float[i];
                                }
                            else {
                                min_and_max_new(temp_float,how_many,&semmin_unscaled,&semmax_unscaled,(int)FALSE);
                                if(!fwrite_sub(temp_float,sizeof(float),(size_t)how_many,(FILE*)fpwsem_unscaled,swapbytes)) exit(-1);
                                }
                            }

                        //std::cout<<"here79"<<std::endl;

                        }
                    }
                }
            if(lcphase) {
                for(i=0;i<how_many*num_wfiles;i++) {
                    temp_float[i] = nsubjects[i] ? (float)(avgphase_unscaled[i]/(double)nsubjects[i]) : (float)UNSAMPLED_VOXEL;
                    }
                if(regional_avgstat_name) {
                    for(i=0;i<how_many*num_wfiles;i++) tcavgphase_unscaled[i][j] = temp_float[i];
                    }
                else {
                    min_and_max_new(temp_float,how_many,&minphase_unscaled,&maxphase_unscaled,(int)FALSE);
                    if(!fwrite_sub(temp_float,sizeof(float),(size_t)how_many,(FILE*)fpwphase_unscaled,swapbytes)) exit(-1);
                    }
                }
            } /*if(!lc_names_only && !concfile)*/

        //std::cout<<"here25"<<std::endl;
        //std::cout<<"here25 j="<<j<<" tc_frames["<<p<<"]="<<tc_frames[p]<<std::endl;

        //if(++j == tc_frames[p]) {
        //START210124
        if(++j == (tc_frames?tc_frames[p]:1)){

            //std::cout<<"here26"<<std::endl;

            if(concfile||lcannalisa||nstimlen||gotoboy) continue;
            j = 0;
            if(regional_avgstat_name) {

                //std::cout<<"here27"<<std::endl;

                if(!lc_names_only) {
                    if(lcmatlab) {
                        sprintf(string,"%s_%s.dat",outfile,tags_tcs->tags_tcptr[k]);
                        if(!(fprn=fopen_sub(string,"w"))) exit(-1);
                        for(r=q=0;q<num_wfiles;q++,r+=num_regions) {
                            for(n=0;n<tc_frames[p];n++) {
                                for(i=0;i<how_many;i++) fprintf(fprn,"%8.4f",time_courses_avg[r+i][n]);
                                fprintf(fprn,"\n");
                                }
                            }
                        fclose(fprn);
                        printf("Timecourses written to %s\n",string);
                        }
                    else if(lcflip) {
                        for(r=q=0;q<num_wfiles;q++,r+=num_regions) {
                            for(i=0;i<num_regions;i++) {
                                if(!rbf->nvoxels_region[i]) {
                                    if(!p) printf("Region %s has zero voxels.\n",rbf->region_names_ptr[i]);
                                    }
                                else {
                                    fprintf(fprn,"%s ",rbf->region_names_ptr[i]);
                                    for(n=0;n<tc_frames[p];n++) fprintf(fprn,"%f ",time_courses_avg[r+i][n]);
                                    for(n=0;n<tc_frames[p];n++) fprintf(fprn,"%f ",time_courses_sd[r+i][n]);
                                    fprintf(fprn,"\n");
                                    }
                                }
                            }
                        }
                    else if(!lctony) {

                        //std::cout<<"here21"<<std::endl; 

                        if(!lccolumns) {
                            /*do nothing*/
                            }
                        else {
                            if(!lccolumns_individual) {

                                //std::cout<<"here22"<<std::endl; 

                                if(!k&&!lcprint_scaled_tc&&lcprint_unscaled_tc) fprintf(fprn,"unscaled (MR units)\n"); 
                                if(tc_frames_max>1) {
                                    fprintf(fprn,"TIMECOURSE : %s",tags_tcs->tags_tcptr[k]);
                                    for(m=k-tc_frames[p]+1;m<=k;m++) fprintf(fprn," %s",tags_tcs->tags_tc_effectsptr[m]);
                                    fprintf(fprn,"\n\n");

                                    //START200303
                                    //fprintf(fprn0,"TIMECOURSE : %s",tags_tcs->tags_tcptr[k]);
                                    //for(m=k-tc_frames[p]+1;m<=k;m++) fprintf(fprn0," %s",tags_tcs->tags_tc_effectsptr[m]);
                                    //fprintf(fprn0,"\n\n");

                                    }

                                //std::cout<<"here23"<<std::endl; 

                                for(r=q=0;q<num_wfiles;q++,r+=how_many) {

                                    //std::cout<<"here24"<<std::endl; 

                                    if(wfiles) fprintf(fprn,"WEIGHT FILE : %s\n\n",wfiles->files[q]);
                                    if(!(temp_int=(int*)malloc(sizeof*temp_int*how_many))) {
                                        printf("Unable to malloc temp_int\n");
                                        exit(-1);
                                        }
                                    if(!k||tc_frames_max>1) fprintf(fprn,"\t");

                                    //std::cout<<"here25a"<<std::endl; 

                                    if(!rbf) {
                                        if(!nreg) {
                                            for(i=0;i<how_many;i++) temp_int[i]=0;
                                            }
                                        else {
                                            for(i=0;i<how_many;i++) {
                                                sprintf(string,"%s %d",glmreg->region_names[i],glmreg->nvoxels_region[i]);
                                                temp_int[i] = (int)strlen(string)+1;
                                                if(!k||tc_frames_max>1) fprintf(fprn,"%s\t",string);
                                                }
                                            }
                                        }
                                    else {

                                        //std::cout<<"here26a"<<std::endl; 


                                        #if 0
                                        for(i=0;i<how_many;i++) {
                                            sprintf(string,"%s %d",rbf->region_names_ptr[i],rbf->nvoxels_region[i]);
                                            temp_int[i] = (int)strlen(string)+1;
                                            if(!k||tc_frames_max>1) fprintf(fprn,"%s\t",string);
                                            }
                                        #endif
                                        //START211007
                                        if(!lcroiIND){
                                            for(i=0;i<how_many;i++) {
                                                sprintf(string,"%s %d",rbf->region_names_ptr[i],rbf->nvoxels_region[i]);
                                                temp_int[i] = (int)strlen(string)+1;
                                                if(!k||tc_frames_max>1) fprintf(fprn,"%s\t",string);
                                                }
                                            }
                                        else{
                                            for(i=0;i<how_many;i++) {
                                                sprintf(string,"%s",reg[0]->region_names[i]);
                                                temp_int[i] = (int)strlen(string)+1;
                                                if(!k||tc_frames_max>1) fprintf(fprn,"%s\t",string);
                                                }
                                            }


                                        //std::cout<<"here26b"<<std::endl; 

                                        }

                                    //std::cout<<"here26c"<<std::endl; 

                                    if(!k||tc_frames_max>1) fprintf(fprn,"\n");
                                    if(lc_within_subject_sd) {
                                        if(!k||tc_frames_max>1) {
                                            //for(i=0;i<how_many;i++) fprintf(fprn,"\tavg\tsd");
                                            for(i=0;i<how_many;i++) fprintf(fprn,"\tavg\t95CI");
                                            fprintf(fprn,"\n");
                                            }
                                        }
                                    if(lcsd) {
                                        for(i=0;i<how_many;i++) {
                                            sprintf(string,"%-*s sem",(int)strlen(string),"avg");
                                            fprintf(fprn,"%-*s",temp_int[i],string);
                                            }
                                        fprintf(fprn,"\n");
                                        }

                                    //std::cout<<"here27"<<std::endl; 

                                    for(n=0;n<tc_frames[p];n++) {
                                        if(tc_frames_max>1) {
                                            fprintf(fprn,"%8.4f\t",glmstack[0]->ifh->glm_effect_TR[l]*
                                                (float)n+glmstack[0]->ifh->glm_effect_shift_TR[l]);
                                            }
                                        else {
                                            fprintf(fprn,"%s\t",tags_tcs->tags_tcptr[k]);
                                            }
                                        for(i=0;i<how_many;i++) {

                                            #if 0
                                            if(!lcprint_scaled_tc&&lcprint_unscaled_tc) { 

                                                std::cout<<"here27 scalar="<<scalar<<std::endl; 
                                          
                                                sprintf(string,"%.4f",scalar*time_courses_avg_unscaled[r+i][n]);
                                                }
                                            else {
                                                sprintf(string,"%f",time_courses_avg[r+i][n]);
                                                }
                                            fflush(stdout);
                                            if(lc_within_subject_sd) {

                                                //std::cout<<"here28 "<<within_subject_sd[r+i][0][n]<<std::endl; 
                                                std::cout<<"here28 within_subject_sd["<<r+i<<"[0]["<<n<<"]="<<within_subject_sd[r+i][0][n]<<std::endl; 

                                                sprintf(string2," %.4f",within_subject_sd[r+i][0][n]);
                                                strcat(string,string2);
                                                }
                                            #endif
                                            //START210519
                                            if(!lcprint_scaled_tc&&lcprint_unscaled_tc) { 
                                                sprintf(string,"%.8f",scalar*time_courses_avg_unscaled[r+i][n]);
                                                if(lc_within_subject_sd){
                                                    //std::cout<<"here28 within_subject_sd_unscaled="<<within_subject_sd_unscaled[r+i][0][n]<<std::endl;
                                                    //std::cout<<"here28 glmstack["<<glmi<<"]->ifh->glm_df="<<glmstack[glmi]->ifh->glm_df<<std::endl;
                                                    //std::cout<<"here28 gsl_cdf_tdist_Qinv="<<gsl_cdf_tdist_Qinv(.025,(double)glmstack[glmi]->ifh->glm_df)<<std::endl;
                                                    //std::cout<<"here28 "<<within_subject_sd_unscaled[r+i][0][n]*gsl_cdf_tdist_Qinv(.025,(double)glmstack[glmi]->ifh->glm_df)<<std::endl;
                                                    sprintf(string2," %.8f",within_subject_sd_unscaled[r+i][0][n]*gsl_cdf_tdist_Qinv(.025,(double)glmstack[glmi]->ifh->glm_df));
                                                    strcat(string,string2);
                                                    }
                                                }
                                            else {
                                                sprintf(string,"%f",time_courses_avg[r+i][n]);
                                                if(lc_within_subject_sd) {
                                                    sprintf(string2," %.4f",within_subject_sd[r+i][0][n]);
                                                    strcat(string,string2);
                                                    }
                                                }
                                            fflush(stdout);


                                            if(lcsem||lcsd) {
                                                if(!lcprint_scaled_tc&&lcprint_unscaled_tc) { 
                                                    sprintf(string2," %.4f",scalar*time_courses_sd_unscaled[r+i][n]);
                                                    strcat(string,string2);
                                                    }
                                                else {
                                                    sprintf(string2," %f",time_courses_sd[r+i][n]);
                                                    strcat(string,string2);
                                                    }
                                                }
                                            fprintf(fprn,"%-*s\t",temp_int[i],string);
                                            }
                                        fprintf(fprn,"\n");
                                        }
                                    if(!hipass[k]) l += tcs->num_tc_to_sum[k][0];
                                    if(tc_frames_max>1) fprintf(fprn,"\n\n");
                                    free(temp_int);
                                    }
                                //std::cout<<"here33"<<std::endl; 
                                }
                            else {
                                fprintf(fprn,"TIMECOURSE : %s",tags_tcs->tags_tcptr[k]);fprintf(fprn0,"TIMECOURSE : %s",tags_tcs->tags_tcptr[k]);
                                for(m=k-tc_frames[p]+1;m<=k;m++){fprintf(fprn," %s",tags_tcs->tags_tc_effectsptr[m]);fprintf(fprn0," %s",tags_tcs->tags_tc_effectsptr[m]);}
                                fprintf(fprn,"\n\n");fprintf(fprn0,"\n\n");
                                if(!hipass[k]||!lcphase_only) {
                                    if(lcprint_scaled_tc) {
                                        for(r=q=0;q<num_wfiles;q++,r+=how_many) {
                                            if(wfiles) fprintf(fprn,"WEIGHT FILE : %s\n\n",wfiles->files[q]);
                                            for(i=0;i<how_many;i++) {
                                                if(superbird){if(!superbird[i])continue;}
                                                if(num_regions){
                                                    if(!lcroiIND){ 
                                                        fprintf(fprn,"REGION : %d %s %d\n",rbf->harolds_num[i],rbf->region_names_ptr[i],rbf->nvoxels_region[i]);
                                                        fprintf(fprn0,"REGION : %d %s %d\n",rbf->harolds_num[i],rbf->region_names_ptr[i],rbf->nvoxels_region[i]);
                                                        }
                                                    else{
                                                        fprintf(fprn,"REGION : %s\n",lookuptable_name(lutf,i));
                                                        fprintf(fprn0,"REGION : %s\n",lookuptable_name(lutf,i));
                                                        }
                                                    }
                                                else if(glmregnames){
                                                    fprintf(fprn,"\nREGION : %s\n",glmregnames[i]);
                                                    fprintf(fprn0,"\nREGION : %s\n",glmregnames[i]);
                                                    }

                                                #if 0
                                                for(m=0;m<nglmfiles;m++) fprintf(fprn,"%d\t",m+1);
                                                if(lcsem||lcsd) fprintf(fprn,"mean\tsem\t");
                                                if(lc_within_subject_sd)for(m=0;m<nglmfiles;m++) fprintf(fprn,"    see%d  ",m+1);
                                                fprintf(fprn,"\n");
                                                #endif
                                                //START200303
                                                
                                                //for(m=0;m<nglmfiles;m++){fprintf(fprn,"\t%d",m+1);fprintf(fprn0,"\t%d",m+1);}
                                                //START200918
                                                for(m=0;m<nglmpersub;m++){fprintf(fprn,"\t%d",m+1);fprintf(fprn0,"\t%d",m+1);}

                                                if(lcsem||lcsd){fprintf(fprn,"\tmean\tsem");fprintf(fprn0,"\tmean\tsem");}

                                                //if(lc_within_subject_sd)for(m=0;m<nglmfiles;m++)fprintf(fprn,"    see%d  ",m+1);
                                                //START200918
                                                if(lc_within_subject_sd)for(m=0;m<nglmpersub;m++)fprintf(fprn,"    see%d  ",m+1);

                                                fprintf(fprn,"\n");fprintf(fprn0,"\n");

                                                //std::cout<<"here38"<<std::endl;

                                                for(n=0;n<tc_frames[p];n++) {
                                                    fprintf(fprn,"%.2f\t",TR*n);

                                                    //for(m=0;m<nglmfiles;m++) fprintf(fprn,"%f\t",time_courses[r+i][m][n]);
                                                    //START200918
                                                    for(m=0;m<nglmpersub;m++) fprintf(fprn,"%f\t",time_courses[r+i][m][n]);

                                                    if(lcsem||lcsd) fprintf(fprn,"%f\t%f\t",time_courses_avg[r+i][n],time_courses_sd[r+i][n]);

                                                    //if(lc_within_subject_sd) for(m=0;m<nglmfiles;m++)fprintf(fprn,"%8.4f ",within_subject_sd[r+i][m][n]);
                                                    //START200918
                                                    if(lc_within_subject_sd) for(m=0;m<nglmpersub;m++)fprintf(fprn,"%8.4f ",within_subject_sd[r+i][m][n]);

                                                    fprintf(fprn,"\n");
                                                    }
                                                fprintf(fprn,"\n\n");

                                                //std::cout<<"here39"<<std::endl;

                                                //START200228
                                                //for(m=0;m<nglmfiles;m++){ 
                                                //START200918
                                                for(m=0;m<nglmpersub;m++){ 

                                                    //std::cout<<"here39a"<<std::endl;

                                                    for(mean0=0.,n=0;n<tc_frames[p];n++)mean0+=time_courses[r+i][m][n];

                                                    //std::cout<<"here39b"<<std::endl;

                                                    mean0/=(double)tc_frames[p];

                                                    //std::cout<<"here39c tc_frames["<<p<<"]="<<tc_frames[p]<<" tcs->num_tc="<<tcs->num_tc<<std::endl;

                                                    for(n=0;n<tc_frames[p];n++)tczm[m][n]=time_courses[r+i][m][n]-mean0;

                                                    //std::cout<<"here39d"<<std::endl;
                                                    }

                                                //std::cout<<"here40"<<std::endl;

                                                #if 0
                                                for(n=0;n<tc_frames[p];n++){
                                                    for(mean0=mean02=0.,m=0;m<nglmfiles;m++){mean0+=tczm[m][n];mean02+=tczm[m][n]*tczm[m][n];}
                                                    tczm_avg[n]=mean0/(double)nglmfiles;
                                                    tczm_sem[n]=sqrt((mean02-mean0*mean0/(double)nglmfiles)/(double)nglmfiles/(double)(nglmfiles-1));
                                                    }
                                                for(n=0;n<tc_frames[p];n++){
                                                    fprintf(fprn0,"%.2f\t",TR*n);
                                                    for(m=0;m<nglmfiles;m++)fprintf(fprn0,"%f\t",tczm[m][n]);
                                                    if(lcsem||lcsd)fprintf(fprn0,"%f\t%f\t",tczm_avg[n],tczm_sem[n]);
                                                    fprintf(fprn0,"\n");
                                                    }
                                                fprintf(fprn0,"\n\n");
                                                #endif
                                                //START200918
                                                for(n=0;n<tc_frames[p];n++){
                                                    for(mean0=mean02=0.,m=0;m<nglmpersub;m++){mean0+=tczm[m][n];mean02+=tczm[m][n]*tczm[m][n];}
                                                    tczm_avg[n]=mean0/(double)nglmpersub;
                                                    tczm_sem[n]=sqrt((mean02-mean0*mean0/(double)nglmpersub)/(double)nglmpersub/(double)(nglmpersub-1));
                                                    }
                                                for(n=0;n<tc_frames[p];n++){
                                                    fprintf(fprn0,"%.2f\t",TR*n);
                                                    for(m=0;m<nglmpersub;m++)fprintf(fprn0,"%f\t",tczm[m][n]);
                                                    if(lcsem||lcsd)fprintf(fprn0,"%f\t%f\t",tczm_avg[n],tczm_sem[n]);
                                                    fprintf(fprn0,"\n");
                                                    }
                                                fprintf(fprn0,"\n\n");

                                                //std::cout<<"here42"<<std::endl;


                                                }
                                            }
                                        }

                                    //std::cout<<"here40"<<std::endl;

                                    if(lcprint_unscaled_tc) {
                                        fprintf(fprn,"\n\nunscaled (MR units)\n-------------------\n");
                                        for(r=q=0;q<num_wfiles;q++,r+=how_many) {
                                            if(wfiles) fprintf(fprn,"WEIGHT FILE : %s\n\n",wfiles->files[q]);
                                            for(i=0;i<how_many;i++) {
                                                if(superbird)if(!superbird[i])continue;
                                                if(num_regions) {
                                                    if(!lcroiIND) 
                                                        fprintf(fprn,"REGION : %d %s %d\n",rbf->harolds_num[i],rbf->region_names_ptr[i],rbf->nvoxels_region[i]);
                                                    else
                                                        fprintf(fprn,"REGION : %s\n",lookuptable_name(lutf,i));
                                                    }
                                                else if(glmregnames){
                                                    fprintf(fprn,"\nREGION : %s\n",glmregnames[i]);
                                                    }
                                                for(m=0;m<nglmfiles;m++) fprintf(fprn,"%d\t",m+1);
                                                if(lcsem||lcsd) fprintf(fprn,"mean\tsem\t");
                                                if(lc_within_subject_sd) for(m=0;m<nglmfiles;m++) fprintf(fprn,"    see%d  ",m+1);
                                                fprintf(fprn,"\n");
                                                for(n=0;n<tc_frames[p];n++) {
                                                    for(m=0;m<nglmfiles;m++) fprintf(fprn,"%f\t",time_courses_unscaled[r+i][m][n]);
                                                    if(lcsem||lcsd) fprintf(fprn,"%f\t%f\t",time_courses_avg_unscaled[r+i][n],
                                                        time_courses_sd_unscaled[r+i][n]);
                                                    if(lc_within_subject_sd) for(m=0;m<nglmfiles;m++)
                                                        fprintf(fprn,"%8.4f ",within_subject_sd[r+i][m][n]);
                                                    fprintf(fprn,"\n");
                                                    }
                                                fprintf(fprn,"\n\n");
                                                }
                                            }
                                        }
                                    }
                                if(lcphase) {
                                    fprintf(fprn,"\n\nphase unscaled (radians)\n-------------------\n");
                                    for(r=q=0;q<num_wfiles;q++,r+=how_many) {
                                        if(wfiles) fprintf(fprn,"WEIGHT FILE : %s\n\n",wfiles->files[q]);
                                        for(i=0;i<how_many;i++) {
                                            fprintf(fprn,"REGION : %d %s %d\n",rbf->harolds_num[i],rbf->region_names_ptr[i],
                                                rbf->nvoxels_region[i]);
                                            for(m=0;m<nglmfiles;m++) fprintf(fprn,"%8d ",m+1);
                                            fprintf(fprn,"\n");
                                            for(n=0;n<tc_frames[p];n++) {
                                                for(m=0;m<nglmfiles;m++) 
                                                    fprintf(fprn,"%8.4f ",tcphase_unscaled[r+i][m][n]);
                                                fprintf(fprn,"\n");
                                                }
                                            fprintf(fprn,"\n\n");
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    else { 
                        for(m=0;m<nglmfiles;m++) {
                            for(r=q=0;q<(int)region_files->nfiles;q++) {
                                if(lcprint_scaled_tc) {
                                    for(i=0;i<rbf->num_regions_by_file[q];i++,r++) {
                                        fprintf(fprn,"%.2f,%d,%d,%d,%d,%d",glmstack[m]->ifh->glm_TR,m+1,q+1,i+1,p+1,tc_frames[p]);
                                        for(n=0;n<tc_frames[p];n++) fprintf(fprn,",%.4f",time_courses[r][m][n]);
                                        fprintf(fprn,"\n");
                                        }
                                     }
                                if(lcprint_unscaled_tc) {
                                    for(i=0;i<rbf->num_regions_by_file[q];i++,r++) {
                                        fprintf(fprn,"%.2f,%d,%d,%d,%d,%d",glmstack[m]->ifh->glm_TR,m+1,q+1,i+1,p+1,tc_frames[p]);
                                        for(n=0;n<tc_frames[p];n++) fprintf(fprn,",%.4f",time_courses_unscaled[r][m][n]);
                                        fprintf(fprn,"\n");
                                        }
                                     }
                                }
                            if(lc_within_subject_sd) {
                                if(lcprint_scaled_tc) {
                                    for(r=q=0;q<(int)region_files->nfiles;q++) {
                                        for(i=0;i<rbf->num_regions_by_file[q];i++,r++) {
                                            fprintf(fprn,"%.2f,%d,%d,%d,%d,%d",glmstack[m]->ifh->glm_TR,m+1,q+1,i+1,p+1,tc_frames[p]);
                                            for(n=0;n<tc_frames[p];n++) fprintf(fprn,",%.4f",within_subject_sd[r][m][n]);
                                            fprintf(fprn,"\n");
                                            }
                                        }
                                    }
                                if(lcprint_unscaled_tc) {
                                    for(r=q=0;q<(int)region_files->nfiles;q++) {
                                        for(i=0;i<rbf->num_regions_by_file[q];i++,r++) {
                                            fprintf(fprn,"%.2f,%d,%d,%d,%d,%d",glmstack[m]->ifh->glm_TR,m+1,q+1,i+1,p+1,tc_frames[p]);
                                            for(n=0;n<tc_frames[p];n++) fprintf(fprn,",%.4f",within_subject_sd_unscaled[r][m][n]);
                                            fprintf(fprn,"\n");
                                            }
                                        }
                                    }
                                }
                            }
                        if(nglmfiles>1) {
                            for(r=q=0;q<(int)region_files->nfiles;q++) {
                                for(i=0;i<rbf->num_regions_by_file[q];i++,r++) {
                                    fprintf(fprn,"%.2f,%d,%d,%d,%d,%d",TR,nglmfiles+1,q+1,i+1,p+1,tc_frames[p]);
                                    for(n=0;n<tc_frames[p];n++) fprintf(fprn,",%.4f",time_courses_avg[r][n]);
                                    fprintf(fprn,"\n");
                                    }
                                }
                            for(r=q=0;q<(int)region_files->nfiles;q++) {
                                for(i=0;i<rbf->num_regions_by_file[q];i++,r++) {
                                    fprintf(fprn,"%.2f,%d,%d,%d,%d,%d",TR,nglmfiles+2,q+1,i+1,p+1,tc_frames[p]);
                                    for(n=0;n<tc_frames[p];n++) fprintf(fprn,",%.4f",time_courses_sd[r][n]);
                                    fprintf(fprn,"\n");
                                    }
                                }
                            }
                        }
                    }
                }
            else {

                //if(!lc_names_only) ifh_out->dim4 = tc_frames[p];
                //START210124 
                if(!lc_names_only)ifh_out->dim4=tc_frames?tc_frames[p]:1;

                if(!hipass[k]||!lcphase_only) {
                    if(lcprint_scaled_tc) {
                        if(!lcmaxdefonly) {
                            if(!lc_names_only) {

                                #if 0
                                fclose(fpw);
                                if(filetype==(int)IMG){
                                    ifh_out->global_min = min;
                                    ifh_out->global_max = max;
                                    if(!write_ifh(outfile,ifh_out,0)) exit(-1);
                                    }
                                #endif
                                #if 0
                                //START170411
                                if(fpw)fclose((FILE*)fpw);
                                if(filetype==(int)IMG){
                                    ifh_out->global_min = min;
                                    ifh_out->global_max = max;
                                    if(!write_ifh(outfile,ifh_out,0)) exit(-1);
                                    }
                                else if(filetype==(int)CIFTI||filetype==(int)CIFTI_DTSERIES){
                                    w1->temp_float=temp_float2;
                                    w1->tdim=(int64_t)tc_frames[p];
                                    if(!write1(outfile,w1))exit(-1);
                                    }
                                #endif 
                                //START210913
                                if(!(strptr=p1.ptrwfree(ifh_out)))exit(-1);
                                }

                            //printf("Timecourses written to %s\n",outfile);
                            //START210913
                            //std::cout<<"Timecourses written to "<<!lc_names_only?strptr:outfile<<std::endl;
                            printf("Timecourses written to %s\n",!lc_names_only?strptr:outfile);fflush(stdout);

                            }
                        if(lcmaxdef||lcmaxdefonly) {
                            if(!nframesout) {
                                i = ifh_out->dim4;
                                ifh_out->dim4 = 1;
                                sprintf(string4,"%s_maxdef%s",string3,Fileext[filetype]);
                                w1->temp_float=maxdef;
                                if(!write1(string4,w1))exit(-1);
                                printf("Maximum deflection written to %s\n",string4);
                                sprintf(string4,"%s_maxdeffr%s",string3,Fileext[filetype]);
                                w1->temp_float=maxdeffr;
                                if(!write1(string4,w1))exit(-1);
                                printf("Frame of maximum deflection written to %s\n",string4);
                                ifh_out->dim4 = i;
                                }
                            else {
                                if(!lc_names_only) {
                                    if(!fwrite_sub(maxdef,sizeof(float),(size_t)how_many1,(FILE*)fpmaxdef,swapbytes)) exit(-1);
                                    min_and_max_new(maxdef,how_many1,&ifh_out1->global_min,&ifh_out1->global_max,0);
                                    }
                                if((++j1)==framesout[j2]) {
                                    if(!lc_names_only){
                                        fclose((FILE*)fpmaxdef);
                                        if(filetype==(int)IMG){
                                            ifh_out1->dim4=framesout[j2++];
                                            if(!write_ifh(string4,ifh_out1,0))exit(-1);
                                            }
                                        }
                                    printf("Maximum deflection written to %s\n",string4);
                                    j1=0;
                                    }
                                }
                            }
                        }
                    if(lcprint_unscaled_tc) {

                        #if 0
                        if(!lc_names_only) {
                            fclose((FILE*)fpw_unscaled);
                            if(filetype==(int)IMG){
                                ifh_out->global_min=min_unscaled;
                                ifh_out->global_max=max_unscaled;
                                if(!write_ifh(outfile_unscaled,ifh_out,0))exit(-1);
                                }
                            }
                        printf("Timecourses written to %s\n",outfile_unscaled);
                        #endif
                        //START210124
                        #if 0 
                        if(!lc_names_only) {
                            if(filetype==(int)IMG){
                                fclose((FILE*)fpw_unscaled);
                                ifh_out->global_min=min_unscaled;
                                ifh_out->global_max=max_unscaled;
                                if(!write_ifh(outfile_unscaled,ifh_out,0))exit(-1);
                                }
                            else if(filetype==(int)CIFTI||filetype==(int)CIFTI_DTSERIES){
                                printf("fidlError: Need to add code to output CIFTI. Abort!\n");exit(-1);
                                }
                            else{
                                std::string str="gzip -f "+(std::string)outfile_unscaled;
                                if(system(str.c_str())==-1)
                                    std::cout<<"fidlError: "<<str<<std::endl;
                                else
                                    strcat(outfile_unscaled,".gz");
                                }
                            }
                        printf("Timecourses written to %s\n",outfile_unscaled);
                        #endif
                        //if(!lc_names_only)if(!p0.ptrwfree(ifh_out))exit(-1);
                        if(!lc_names_only)if(!(strptr=p0.ptrwfree(ifh_out)))exit(-1);
                        //std::cout<<"Timecourses written to "<<outfile_unscaled<<std::endl;
                        //std::cout<<"Timecourses written to "<<strptr<<std::endl;
                        //START210913
                        //std::cout<<"Timecourses written to "<<!lc_names_only?strptr:outfile_unscaled<<std::endl;
                        printf("Timecourses written to %s\n",!lc_names_only?strptr:outfile_unscaled);fflush(stdout);


                        //std::cout<<"here26"<<std::endl;


                        }
                    if(lcsd) {
                        if(lcprint_scaled_tc) {
                            if(!lc_names_only) {
                                fclose((FILE*)fpwsd);
                                if(filetype==(int)IMG){
                                    ifh_out->global_min = sdmin;
                                    ifh_out->global_max = sdmax;
                                    if(!write_ifh(outsdfile,ifh_out,0))exit(-1);
                                    }
                                }
                            printf("Timecourses written to %s\n",outsdfile);
                            }
                        if(lcprint_unscaled_tc) {
                            if(!lc_names_only) {
                                fclose((FILE*)fpwsd_unscaled);
                                if(filetype==(int)IMG){
                                    ifh_out->global_min = sdmin_unscaled;
                                    ifh_out->global_max = sdmax_unscaled;
                                    if(!write_ifh(outsdfile_unscaled,ifh_out,0))exit(-1);
                                    }
                                }
                            printf("Timecourses written to %s\n",outsdfile_unscaled);
                            }
                        }
                    if(lcsem) {
                        if(lcprint_scaled_tc) {
                            if(!lc_names_only) {
                                fclose((FILE*)fpwsem);
                                if(filetype==(int)IMG){
                                    ifh_out->global_min = semmin;
                                    ifh_out->global_max = semmax;
                                    if(!write_ifh(outsemfile,ifh_out,0))exit(-1);
                                    }
                                }
                            printf("Timecourses written to %s\n",outsemfile);
                            }
                        if(lcprint_unscaled_tc) {
                            if(!lc_names_only) {
                                fclose((FILE*)fpwsem_unscaled);
                                if(filetype==(int)IMG){
                                    ifh_out->global_min = semmin_unscaled;
                                    ifh_out->global_max = semmax_unscaled;
                                    if(!write_ifh(outsemfile_unscaled,ifh_out,0))exit(-1);
                                    }
                                }
                            printf("Timecourses written to %s\n",outsemfile_unscaled);
                            }
                        }
                    }
                if(lcphase) {
                    if(!lc_names_only) {
                        fclose((FILE*)fpwphase_unscaled);
                        if(filetype==(int)IMG){
                            ifh_out->global_min = minphase_unscaled;
                            ifh_out->global_max = maxphase_unscaled;
                            if(!write_ifh(outphase_unscaled,ifh_out,0))exit(-1);
                            }
                        }
                    printf("Timecourses written to %s\n",outphase_unscaled);
                    }
                } /*else*/
            if((lcmaxdef||lcmaxdefonly) && !lc_names_only) for(i=0;i<how_many;i++) maxdef[i]=maxdeffr[i]=0;
            p++;
            } /*if(!j && !num_region_files)*/
        } /*for(l=jj=p=j=k=0;k<tcs->num_tc;k++)*/
    if(concfile) {
        if(!(bolds=read_conc(1,&concfile))) exit(-1);
        if(!(fpw=fopen_sub(outfile,"w"))) exit(-1);
        fprintf((FILE*)fpw,"    number_of_files:%zd\n",bolds->nfiles);
        ifh_out->dim1 = how_many;
        ifh_out->dim2 = 1;
        ifh_out->dim3 = 1;
        for(p=0,l=i=0;i<(int)bolds->nfiles;i++) {
            if(!(ifh=read_ifh(bolds->files[i],(Interfile_header*)NULL)))
            strcpy(string,bolds->files[i]);
            if(!(str_ptr=get_tail_sans_ext(string))) exit(-1);
            sprintf(string2,"%s%s_predicted.4dfp.img",directory,str_ptr);
            if(!(fp=fopen_sub(string2,"w"))) exit(-1);
            min_and_max_init(&ifh_out->global_min,&ifh_out->global_max);
            for(j=0;j<ifh->dim4;j++,l++) {
                for(k=0;k<how_many;k++) temp_double[k] = 1000.;
                if(glmstack[0]->valid_frms[l]>0.) {
                    for(k=0;k<tcs->num_tc;k++)  {
                        kk = (int)tcs->tc[k][0][0]-1;
                        if(glmstack[0]->AT[kk][p]) {
                            for(m=0;m<how_many;m++) temp_double[m] += (double)glmstack[0]->AT[kk][p]*time_courses_unscaled[m][0][k];
                            }
                        }
                     if(p<glmstack[0]->ifh->glm_Nrow) ++p;
                     }
                for(k=0;k<how_many;k++) temp_float[k] = (float)temp_double[k];
                if(!fwrite_sub(temp_float,sizeof(float),(size_t)how_many,fp,swapbytes)) {
                    printf("fidlError: Could not write to %s\n",string2);
                    exit(-1);
                    }
                min_and_max_doublestack(temp_double,how_many,&ifh_out->global_min,&ifh_out->global_max);
                }
            fclose(fp);
            ifh_out->dim4 = ifh->dim4;
            if(!write_ifh(string2,ifh_out,(int)FALSE)) exit(-1);
            printf("Bold file written to %s\n",string2);
            fprintf((FILE*)fpw,"               file:%s\n",string2);
            free_ifh(ifh,0);
            }
        fclose((FILE*)fpw);
        printf("Concatenated file written to %s\n",outfile);
        }
    else if(lcannalisa) {
        if(!(fprn=fopen_sub(outfile,"w"))) exit(-1);
        for(o=r=q=0;q<num_wfiles;q++,r+=how_many) {
            if(wfiles) fprintf(fprn,"WEIGHT FILE : %s\n",wfiles->files[q]);
            for(jjj=jj=i=0;i<how_many;i++,o++) {
                fprintf(fprn,"REGION : %d %s %d\n",rbf->harolds_num[i],rbf->region_names_ptr[i],rbf->nvoxels_region[i]);
                if(rbf->nvoxels_region[i]) {
                    if(confile) {
                        for(m=0;m<nglmfiles*num_tc_frames;m++) conmag[m] = (double)UNSAMPLED_VOXEL;
                        for(m=0;m<num_tc_frames;m++) conmean[m] = consem[m] = (double)UNSAMPLED_VOXEL;
                        for(n=m=j=0;j<num_tc_frames;jj+=tc_frames[j++]) {
                            if(contrasts[jj]!=(double)UNSAMPLED_VOXEL) {
                                for(conmean[j]=consem[j]=0.,k=0;k<nglmfiles;k++,m++) {
                                    if(time_courses[r+i][k][n]!=(double)UNSAMPLED_VOXEL) {
                                        for(conmag[m]=0.,nn=n,ll=jj,l=0;l<tc_frames[j];l++,ll++,nn++) {
                                            conmag[m]+=time_courses[r+i][k][nn]*contrasts[ll];
                                            }
                                        conmean[j]+=conmag[m]; 
                                        consem[j]+=conmag[m]*conmag[m]; 
                                        }
                                    }
                                consem[j]=sqrt((consem[j]-conmean[j]*conmean[j]/(double)nsubjects[o])/
                                    (double)(nsubjects[o]-1)/(double)nsubjects[o]);
                                conmean[j]/=(double)nsubjects[o];
                                }
                            n+=tc_frames[j];
                            }
                        }
                    for(kk=j=0;j<num_tc_frames;j++) {
                        for(k=0;k<tc_frames[j];k++,kk++) fprintf(fprn,"\t%s",tags_tcs->tags_tcptr[kk]); 
                        if(confile) fprintf(fprn,"\tmag\t");
                        }
                    for(m=0;m<nglmfiles;m++) {
                        fprintf(fprn,"\n%s",glm_names[m]);
                        for(kk=j=0;j<num_tc_frames;j++) {
                            for(k=0;k<tc_frames[j];k++,kk++) fprintf(fprn,"\t%12.8f",time_courses[r+i][m][kk]); 
                            if(confile) {
                                conmag[m+j*nglmfiles]!=(double)UNSAMPLED_VOXEL?
                                    fprintf(fprn,"\t%12.8f\t",conmag[m+j*nglmfiles]):fprintf(fprn,"\t\t");
                                }
                            }
                        }
                    if(lcsem||lcsd) {
                        fprintf(fprn,"\nmean");
                        for(kk=j=0;j<num_tc_frames;j++) {
                            for(k=0;k<tc_frames[j];k++,kk++) fprintf(fprn,"\t%12.8f",time_courses_avg[r+i][kk]);
                            if(confile) {
                                conmean[j]!=(double)UNSAMPLED_VOXEL?fprintf(fprn,"\t%12.8f\t",conmean[j]):fprintf(fprn,"\t\t");
                                }
                            }
                        fprintf(fprn,"\n%s",sdsemstrptr);
                        for(kk=j=0;j<num_tc_frames;j++) {
                            for(k=0;k<tc_frames[j];k++,kk++) fprintf(fprn,"\t%12.8f",time_courses_sd[r+i][kk]);
                            if(confile) {
                                consem[j]!=(double)UNSAMPLED_VOXEL?fprintf(fprn,"\t%12.8f\t",consem[j]):fprintf(fprn,"\t\t");
                                }
                            }
                        }
                    if(confile) {
                        fprintf(fprn,"\ncon");
                        for(j=0;j<num_tc_frames;j++) {
                            if(contrasts[jjj]!=(double)UNSAMPLED_VOXEL) {
                                for(k=0;k<tc_frames[j];k++,jjj++) fprintf(fprn,"\t%12.8f",contrasts[jjj]);
                                }
                            else {
                                for(k=0;k<tc_frames[j];k++,jjj++) fprintf(fprn,"\t\t");
                                }
                            fprintf(fprn,"\t\t");
                            }
                        }
                    fprintf(fprn,"\n");
                    }
                fprintf(fprn,"\n");
                }
            }
        }
    else if(nstimlen||gotoboy) {

        //std::cout<<"here30 nstimlen"<<nstimlen<<" gotoboy="<<gotoboy<<std::endl;

        boyprint(time_courses,time_courses_avg,time_courses_sd,time_courses_unscaled,time_courses_avg_unscaled,
            time_courses_sd_unscaled,within_subject_sd,tcphase_unscaled,tcavgphase_unscaled,how_many,num_regions,nglmpersub,
            num_tc_frames,tc_frames,hipass,tags_tcs,wfiles,rbf,spaces,glm_names,len_glm_names,num_wfiles,
            lcprint_scaled_tc,lcprint_unscaled_tc,lcsem,lcsd,lc_within_subject_sd,lcphase,lcphase_only,sdsemspace,sdsemstrptr,outfile,
            TR,glm_list_file,glmregnames);
        if(nstimlen) {
            boyfit(time_courses,time_courses_avg,how_many,nglmpersub,num_tc_frames,tc_frames,TR,tc_frames_max,
                tags_tcs,wfiles,rbf,glm_names,num_wfiles,lcprint_scaled_tc,lcprint_unscaled_tc,stimlens,
                time_courses_fit,time_courses_avg_fit,spaces,lcsem,lcsd,lc_within_subject_sd,lcphase,lcphase_only,sdsemspace,
                sdsemstrptr,outfile,num_regions,len_glm_names,hipass,glm_list_file,time_courses_param,time_courses_avg_param,
                glmregnames);
            }
        }
    if(lcvoxels) {
        boyprintvox(voxels,voxels_unscaled,nglmpersub,num_tc_frames,tc_frames,rbf,glm_names,lcprint_scaled_tc,
            lcprint_unscaled_tc,outfile,TR,glm_list_file,num_wfiles,wfiles,ap,tags_tcs->tags_tcptr,tags_tcs->tags_tc_effectsptr);

        }
    }    
if(fprn) { 
    fclose(fprn);
    fprintf(stdout,"Region results written to %s\n",outfile);
    }

//START200303
if(fprn){ 
    fclose(fprn0);
    std::cout<<"Region results written to "<<zmf<<std::endl;
    }

if(cleanup) {
    sprintf(string,"rm -rf %s",cleanup);
    if(system(string) == -1) printf("fidlError: unable to %s\n",string);
    }
}/*END*/

int read_confile(char *confile,int how_many,char **region_names_ptr,int num_tc_frames,char **tcstrptr,int *tc_frames,
    double *contrasts,int num_tc)
{
    char line[MAXNAME],write_back[MAXNAME],*write_back_ptr,string[MAXNAME];
    int i,j,jj,num_factors,regcol,len,nstrings;
    FILE *fp;

    if(!(fp=fopen_sub(confile,"r"))) return 0;
    if(!fgets(line,sizeof(line),fp)) {
        printf("%s is empty.\n",confile);
        return 0;
        }
    if((num_factors = count_strings(line,write_back,' ') - 1) < 2) { /*subtract 1 for "contrast"*/
        printf("fidlError: Need to list 'region condition contrast' on the first line of %s.\n",confile);
        return 0;
        }
    for(write_back_ptr=write_back,regcol=-1,j=0;j<num_factors;j++) {
        write_back_ptr = grab_string_new(write_back_ptr,line,&len);
        if(!strcmp("region",line)) regcol=j;
        }
    if(regcol==-1) {
        printf("fidlError: Need to list 'region' on the frist line of %s.\n",confile);
        return 0;
        }
    for(i=0;i<how_many*num_tc;i++) contrasts[i]=(double)UNSAMPLED_VOXEL; 
    for(jj=i=0;i<how_many;i++) {
        for(j=0;j<num_tc_frames;jj+=tc_frames[j++]) {
            rewind(fp);
            fgets(line,sizeof(line),fp);
            for(;fgets(line,sizeof(line),fp);) {
                nstrings = count_strings(line,write_back,' ')-2;
                if(nstrings>0) {
                    write_back_ptr = write_back;
                    write_back_ptr = grab_string_new(write_back_ptr,line,&len);
                    write_back_ptr = grab_string_new(write_back_ptr,string,&len);
                    if(!regcol) {
                        if(strcmp(region_names_ptr[i],line)) continue;
                        if(strcmp(tcstrptr[jj],string)) continue;
                        }
                    else {
                        if(strcmp(region_names_ptr[i],string)) continue;
                        if(strcmp(tcstrptr[jj],line)) continue;
                        }
                    /*printf("line=%s string=%s write_back_ptr=%s\n",line,string,write_back_ptr);*/
                    strings_to_double(write_back_ptr,&contrasts[jj],nstrings);
                    break;
                    }
                }
            }
        }
    fclose(fp);
    return 1;
}

int boy_f(const gsl_vector *x,void *data,gsl_vector *f)
{
    size_t i;
    Hrfparam param;
    param.delta = gsl_vector_get(x,0);
    param.tau = gsl_vector_get(x,1);
    param.A = gsl_vector_get(x,2);
    param.C = gsl_vector_get(x,3);
    param.duration = gsl_vector_get(x,4);
    size_t n = ((Nonlinlsqdata*)data)->n;
    double *t = ((Nonlinlsqdata*)data)->t;
    double *Y = ((Nonlinlsqdata*)data)->Y;
    double *y = ((Nonlinlsqdata*)data)->y;
    double *sigma = ((Nonlinlsqdata*)data)->sigma;
    boynton_model(n,t,&param,0,Y);
    for(i=0;i<n;i++) gsl_vector_set(f,i,(Y[i]-y[i])/sigma[i]);
    /*printf("Y=");for(i=0;i<n;i++)printf("%f ",Y[i]);printf("\n");*/
    return GSL_SUCCESS;
}
int boy_df(const gsl_vector *x,void *data,gsl_matrix *J)
{
    size_t i;
    double v1,v1sq,v1e,v1quade,v1combo,v2,v2sq,v2e,v2quade,v2quadetau,dA,ddelta,db,dtau,dC;
    double delta = gsl_vector_get(x,0);
    double tau = gsl_vector_get(x,1);
    double A = gsl_vector_get(x,2);
    double b = gsl_vector_get(x,4);
    size_t n = ((Nonlinlsqdata*)data)->n;
    double *t = ((Nonlinlsqdata*)data)->t;
    double *sigma = ((Nonlinlsqdata*)data)->sigma;
    for(i=0;i<n;i++) {
        v1 = (t[i]-delta-b)/tau;
        v1sq = v1*v1;
        v1e = exp(-v1);
        v1quade = (v1sq+2.*v1+2.)*v1e;
        v1combo = -(v1+1./tau)*2.*v1e + v1quade/tau;

        v2 = (t[i]-delta)/tau;
        v2sq = v2*v2;
        v2e = exp(-v2);
        v2quade = (v2sq+2.*v2+2.)*v2e;
        v2quadetau = v2quade/tau;

        dA = v1quade - v2quade;
        ddelta = A * ( v1combo + (v2+1./tau)*2.*v2e - v2quadetau );
        db = A*v1combo;
        dtau = A * ( (-v1sq+v1)*2./tau*v1e + v1quade*v1/tau -(-v2sq+v2)*2./tau*v2e - v2quadetau*v2 );
        dC = 1.;

        gsl_matrix_set(J,i,0,dA/sigma[i]);
        gsl_matrix_set(J,i,1,ddelta/sigma[i]);
        gsl_matrix_set(J,i,2,db/sigma[i]);
        gsl_matrix_set(J,i,3,dtau/sigma[i]);
        gsl_matrix_set(J,i,4,dC/sigma[i]);
        }
    return GSL_SUCCESS;
}
int boy_fdf(const gsl_vector *x,void *data,gsl_vector *f,gsl_matrix *J)
{
    boy_f(x,data,f);
    boy_df(x,data,J);
    return GSL_SUCCESS;
}
int boyfit(double ***time_courses,double **time_courses_avg,int how_many,int nglmpersub,int num_tc_frames,int *tc_frames,
    double TR,int tc_frames_max,Tags_Tc_Struct2 *tags_tcs,Files_Struct *wfiles,Regions_By_File *rbf,
    char **glm_names,int num_wfiles,int lcprint_scaled_tc,int lcprint_unscaled_tc,TC *stimlens,
    double ***time_courses_fit,double **time_courses_avg_fit,int spaces,int lcsem,int lcsd,int lc_within_subject_sd,int lcphase,
    int lcphase_only,int sdsemspace,char *sdsemstrptr,char *outfile,int num_regions,size_t *len_glm_names,int *hipass,
    Files_Struct *glm_list_file,double ***time_courses_param,double **time_courses_avg_param,char **glmregnames){
    Nonlinlsqdata *d;
    gsl_multifit_function_fdf f;
    double *x_init,y1,y2,SSyy=0,*SSE,SSEmin,*hrfest,x_init4;  
    unsigned int iter;
    int i,j,k,l,m,n,q,r,jj,nn,pp,p1,status,*stimlenn,stimlennmax,SSEmini=0;
    char filename[MAXNAME]; 

    if(!(d=(Nonlinlsqdata*)malloc(sizeof*d))) {
        printf("fidlError: Unable to malloc d\n");
        return 0;
        }
    if(!(d->t=(double*)malloc(sizeof*d->t*tc_frames_max))) {
        printf("fidlError: Unable to malloc d->t\n");
        return 0;
        }
    for(j=0;j<tc_frames_max;j++) d->t[j] = (double)j*TR; 
    if(!(d->y=(double*)malloc(sizeof*d->y*tc_frames_max))) {
        printf("fidlError: Unable to malloc d->y\n");
        return 0;
        }
    if(!(d->Y=(double*)malloc(sizeof*d->Y*tc_frames_max))) {
        printf("fidlError: Unable to malloc d->Y\n");
        return 0;
        }
    if(!(d->sigma=(double*)malloc(sizeof*d->sigma*tc_frames_max))) {
        printf("fidlError: Unable to malloc d->sigma\n");
        return 0;
        }
    for(j=0;j<tc_frames_max;j++) d->sigma[j] = 1.; /*unweighted least squares,since errors are unknown*/

    if(!(x_init=(double*)malloc(sizeof*x_init*(size_t)HRF_P))) {
        printf("fidlError: Unable to malloc x_init\n");
        return 0;
        }
    gsl_vector_view x = gsl_vector_view_array(x_init,(size_t)HRF_P);

    f.f = &boy_f;
    f.df = &boy_df;
    f.fdf = &boy_fdf;
    f.p = (size_t)HRF_P;
    f.params = d;

    if(!(stimlenn=(int*)malloc(sizeof*stimlenn*num_tc_frames))) {
        printf("fidlError: Unable to malloc stimlenn\n");
        return 0;
        }
    for(stimlennmax=j=0;j<num_tc_frames;j++) {
        if((stimlenn[j]=((int)(fabs(stimlens->tc[j][0][1]-stimlens->tc[j][0][0])/.1)+1))>stimlennmax) stimlennmax=stimlenn[j];
        } 
    if(!(SSE=(double*)malloc(sizeof*SSE*stimlennmax))) {
        printf("fidlError: Unable to malloc SSE\n");
        return 0;
        }
    for(j=0;j<stimlennmax;j++) SSE[j]=1e20;
    if(!(hrfest=(double*)malloc(sizeof*hrfest*HRF_P*stimlennmax))) {
        printf("fidlError: Unable to malloc hrfest\n");
        return 0;
        }
    for(p1=(int)HRF_P+2,pp=jj=j=0;j<num_tc_frames;jj+=tc_frames[j++],pp+=p1) {

        x_init4 = stimlens->tc[j][0][0]<stimlens->tc[j][0][1] ? stimlens->tc[j][0][0] : stimlens->tc[j][0][1];
        d->n = tc_frames[j];
        f.n = tc_frames[j];

        const gsl_multifit_fdfsolver_type *T = gsl_multifit_fdfsolver_lmder;
        gsl_multifit_fdfsolver *s = gsl_multifit_fdfsolver_alloc(T,tc_frames[j],(const size_t)HRF_P);

        for(r=q=0;q<num_wfiles;q++,r+=how_many) {
            for(i=0;i<how_many;i++) {
                if(rbf->nreg) {
                    if(!rbf->nvoxels_region[i]) continue;
                    }
                if(lcprint_scaled_tc) {
                    for(m=0;m<nglmpersub;m++) {


                        /*for(nn=jj,n=0;n<tc_frames[j];n++,nn++) d->y[n] = time_courses[r+i][m][nn];*/
                        for(y1=y2=0.,nn=jj,n=0;n<tc_frames[j];n++,nn++) {
                            y1 += d->y[n] = time_courses[r+i][m][nn];
                            y2 += time_courses[r+i][m][nn]*time_courses[r+i][m][nn];
                            }
                        SSyy = y2 - y1*y1/(double)tc_frames[j];


                        x_init[0]=(double)HRF_DELTA;
                        x_init[1]=(double)HRF_TAU;
                        x_init[2]=1.; /*A*/
                        x_init[3]=(double)HRF_C;
                        x_init[4] = x_init4; 
                        for(l=k=0;k<stimlenn[j];k++,x_init[4]+=.1) {
                            gsl_multifit_fdfsolver_set(s,&f,&x.vector);
                            iter=0;
                            do {
                                status = gsl_multifit_fdfsolver_iterate(s);
                                status = gsl_multifit_test_delta(s->dx,s->x,1e-20,1e-20);
                                } while(status==GSL_CONTINUE && (iter++)<500);
                            /*printf("%s iter=%d status=%s\n",glm_names[m],iter,gsl_strerror(status));
                            printf("x_init=");for(n=0;n<5;n++) printf("%f ",x_init[n]); printf("\n");*/
                            if(status==GSL_SUCCESS) {
                                /*printf("delta=%f tau=%f A=%f C=%f duration=%f\n",gsl_vector_get(s->x,0),gsl_vector_get(s->x,1),
                                    gsl_vector_get(s->x,2),gsl_vector_get(s->x,3),gsl_vector_get(s->x,4));*/
                                for(SSE[k]=0.,nn=jj,n=0;n<tc_frames[j];n++,nn++) SSE[k] += pow(time_courses[r+i][m][nn]-d->Y[n],2.);
                                /*R2[k] = 1.-SSE[k]/SSyy;*/
                                /*printf("SSE[%d]=%f\n",k,SSE[k]);
                                printf("time_courses d->Y=\n");
                                for(nn=jj,n=0;n<tc_frames[j];n++,nn++) printf("%12.8f %12.8f\n",time_courses[r+i][m][nn],d->Y[n]);*/
                                hrfest[l++]=gsl_vector_get(s->x,0);
                                hrfest[l++]=gsl_vector_get(s->x,1);
                                hrfest[l++]=gsl_vector_get(s->x,2);
                                hrfest[l++]=gsl_vector_get(s->x,3);
                                hrfest[l++]=gsl_vector_get(s->x,4);
                                }
                            else {
                                l+=(int)HRF_P;
                                }
                            }
                        for(SSEmin=1e20,k=0;k<stimlenn[j];k++) if(SSE[k]<SSEmin) {SSEmin=SSE[k];SSEmini=k;}
                        l=SSEmini*(int)HRF_P;
                        /*printf("%s SOLUTION\n",glm_names[m]);
                        printf("delta=%f tau=%f A=%f C=%f duration=%f SSE=%f\n",hrfest[l],hrfest[l+1],hrfest[l+2],hrfest[l+3],
                            hrfest[l+4],SSE[SSEmini]);*/

                        for(nn=pp,k=0;k<(int)HRF_P;k++,l++,nn++) time_courses_param[r+i][m][nn]= x_init[k] = hrfest[l];
                        time_courses_param[r+i][m][nn] = SSE[SSEmini];
                        /*time_courses_param[r+i][m][++nn] = R2[SSEmini];*/
                        time_courses_param[r+i][m][++nn] = 1.-SSE[SSEmini]/SSyy;

                        gsl_multifit_fdfsolver_set(s,&f,&x.vector);
                        /*printf("time_courses\td->Y=\n");
                        for(nn=jj,n=0;n<tc_frames[j];n++,nn++) printf("%12.8f\t%12.8f\n",time_courses[r+i][m][nn],d->Y[n]);*/
                        for(nn=jj,n=0;n<tc_frames[j];n++,nn++) time_courses_fit[r+i][m][nn]=d->Y[n];
                        }
                    for(nn=jj,n=0;n<tc_frames[j];n++,nn++) d->y[n] = time_courses_avg[r+i][nn];
                    x_init[0]=(double)HRF_DELTA;
                    x_init[1]=(double)HRF_TAU;
                    x_init[2]=1.; /*A*/
                    x_init[3]=(double)HRF_C;
                    x_init[4] = x_init4; 
                    for(l=k=0;k<stimlenn[j];k++,x_init[4]+=.1) {
                        gsl_multifit_fdfsolver_set(s,&f,&x.vector);
                        iter=0;
                        do {
                            status = gsl_multifit_fdfsolver_iterate(s);
                            status = gsl_multifit_test_delta(s->dx,s->x,1e-20,1e-20);
                            } while(status==GSL_CONTINUE && (iter++)<500);
                        if(status==GSL_SUCCESS) {
                            for(SSE[k]=0.,nn=jj,n=0;n<tc_frames[j];n++,nn++) SSE[k] += pow(time_courses_avg[r+i][nn]-d->Y[n],2.);
                            hrfest[l++]=gsl_vector_get(s->x,0);
                            hrfest[l++]=gsl_vector_get(s->x,1);
                            hrfest[l++]=gsl_vector_get(s->x,2);
                            hrfest[l++]=gsl_vector_get(s->x,3);
                            hrfest[l++]=gsl_vector_get(s->x,4);
                            }
                        else {
                            l+=(int)HRF_P;
                            }
                        }
                    for(SSEmin=1e20,k=0;k<stimlenn[j];k++) if(SSE[k]<SSEmin) {SSEmin=SSE[k];SSEmini=k;}
                    l=SSEmini*(int)HRF_P;
                    /*printf("average SOLUTION\n");
                    printf("delta=%f tau=%f A=%f C=%f duration=%f SSE=%f\n",hrfest[l],hrfest[l+1],hrfest[l+2],hrfest[l+3],hrfest[l+4],
                        SSE[SSEmini]);*/

                    for(nn=pp,k=0;k<(int)HRF_P;k++,l++,nn++) time_courses_avg_param[r+i][nn] = x_init[k] = hrfest[l];
                    time_courses_avg_param[r+i][nn] = SSE[SSEmini];
                    /*time_courses_avg_param[r+i][++nn] = R2[SSEmini];*/
                    time_courses_avg_param[r+i][++nn] = 1.-SSE[SSEmini]/SSyy; 

                    gsl_multifit_fdfsolver_set(s,&f,&x.vector);
                    /*printf("time_courses\td->Y=\n");
                    for(nn=jj,n=0;n<tc_frames[j];n++,nn++) printf("%12.8f\t%12.8f\n",time_courses_avg[r+i][nn],d->Y[n]);*/
                    for(nn=jj,n=0;n<tc_frames[j];n++,nn++) time_courses_avg_fit[r+i][nn]=d->Y[n];
                    }
                }
            }
        gsl_multifit_fdfsolver_free(s);
        }
    strcpy(filename,outfile);
    *strrchr(filename,'.') = 0;
    strcat(filename,"_boyfit.txt");
    boyprint(time_courses_fit,time_courses_avg_fit,(float**)NULL,(double ***)NULL,(double **)NULL,(float **)NULL,(double ***)NULL,
        (double ***)NULL,(float **)NULL,how_many,num_regions,nglmpersub,num_tc_frames,tc_frames,hipass,tags_tcs,wfiles,rbf,
        spaces,glm_names,len_glm_names,num_wfiles,lcprint_scaled_tc,lcprint_unscaled_tc,lcsem,
        lcsd,lc_within_subject_sd,lcphase,lcphase_only,sdsemspace,sdsemstrptr,filename,TR,glm_list_file,glmregnames);

    strcpy(filename,outfile);
    *strrchr(filename,'.') = 0;
    strcat(filename,"_boyfitparam.txt");
    boyprintparam(time_courses_param,time_courses_avg_param,num_regions,nglmpersub,num_tc_frames,tc_frames,tags_tcs,wfiles,rbf,
        spaces,glm_names,len_glm_names,num_wfiles,lcsem,lcsd,filename,TR,glm_list_file,how_many);

    free(hrfest);
    free(SSE);
    free(stimlenn);
    free(x_init);
    free(d->sigma);
    free(d->Y);
    free(d->y);
    free(d->t);
    free(d);
    return 1;
    }
int boyprint(double ***time_courses,double **time_courses_avg,float **time_courses_sd,double ***time_courses_unscaled,
    double **time_courses_avg_unscaled,float **time_courses_sd_unscaled,double ***within_subject_sd,double ***tcphase_unscaled,
    float **tcavgphase_unscaled,int how_many,int num_regions,int nglmpersub,int num_tc_frames,int *tc_frames,int *hipass,
    Tags_Tc_Struct2 *tags_tcs,Files_Struct *wfiles,Regions_By_File *rbf,int spaces,char **glm_names,
    size_t *len_glm_names,int num_wfiles,int lcprint_scaled_tc,int lcprint_unscaled_tc,int lcsem,int lcsd,int lc_within_subject_sd,
    int lcphase,int lcphase_only,int sdsemspace,char *sdsemstrptr,char *outfile,double TR,Files_Struct *glm_list_file,
    char **glmregnames){
    int i,j,m,n,q,r,jj,nn;
    size_t i1;
    FILE *fprn;

    std::cout<<"boyprint"<<std::endl;

    if(!(fprn=fopen_sub(outfile,"w")))return 0;
    fprintf(fprn,"TR = %4.2f\n",TR);
    if(glm_list_file) {
        fprintf(fprn,"GLM LIST = %s\n",glm_list_file->files[0]);
        for(i1=1;i1<glm_list_file->nfiles;i1++) fprintf(fprn,"           %s\n",glm_list_file->files[i1]);
        }
    for(jj=j=0;j<num_tc_frames;jj+=tc_frames[j++]) {
        fprintf(fprn,"\nTIMECOURSE : %s",tags_tcs->tags_tcptr[jj]);
        for(m=jj;m<jj+tc_frames[j];m++) fprintf(fprn," %s",tags_tcs->tags_tc_effectsptr[m]);
        fprintf(fprn,"\n");
        for(r=q=0;q<num_wfiles;q++,r+=how_many) {
            if(wfiles) fprintf(fprn,"WEIGHT FILE : %s\n\n",wfiles->files[q]);
            for(i=0;i<how_many;i++) {
                if(num_regions) {
                    fprintf(fprn,"\nREGION : %d %s %d",rbf->harolds_num[i],rbf->region_names_ptr[i],rbf->nvoxels_region[i]);
                    if(!rbf->nvoxels_region[i]) {
                        fprintf(fprn,"\n");
                        if(!j) printf("Region %s has zero voxels.\n",rbf->region_names_ptr[i]);
                        fprintf(fprn,"\n");
                        continue;
                        }
                    }
                else if(glmregnames) fprintf(fprn,"\nREGION : %s",glmregnames[i]);
                if(!hipass[jj]||!lcphase_only) {
                    if(lcprint_scaled_tc) {
                        fprintf(fprn,"\n");
                        for(m=0;m<nglmpersub;m++) {
                            nn=jj;
                            fprintf(fprn,"%s%*.8f ",glm_names[m],spaces+12-(int)len_glm_names[m],time_courses[r+i][m][nn++]);
                            for(n=1;n<tc_frames[j];n++,nn++) fprintf(fprn,"%12.8f ",time_courses[r+i][m][nn]);
                            fprintf(fprn,"\n");
                            }
                        if(lcsem||lcsd) {
                            nn=jj;

                            //fprintf(fprn,"average%*.8f ",spaces+4,time_courses_avg[r+i][nn++]);
                            //START150911
                            fprintf(fprn,"average%*.8f ",spaces+5,time_courses_avg[r+i][nn++]);

                            for(n=1;n<tc_frames[j];n++,nn++) fprintf(fprn,"%12.8f ",time_courses_avg[r+i][nn]);
                            fprintf(fprn,"\n");
                            if(time_courses_sd) {
                                nn=jj;

                                //fprintf(fprn,"%s%*.8f ",sdsemstrptr,sdsemspace+spaces+8,time_courses_sd[r+i][nn++]);
                                //START150911
                                fprintf(fprn,"%s%*.8f ",sdsemstrptr,sdsemspace+spaces+9,time_courses_sd[r+i][nn++]);

                                for(n=1;n<tc_frames[j];n++,nn++) fprintf(fprn,"%12.8f ",time_courses_sd[r+i][nn]);
                                fprintf(fprn,"\n");
                                }
                            }
                        }
                    if(lc_within_subject_sd) {
                        fprintf(fprn,"\n\nstandard deviations\n");
                        for(m=0;m<nglmpersub;m++) {
                            nn=jj;
                            fprintf(fprn,"%s%*.4f ",glm_names[m],spaces+9-(int)len_glm_names[m],within_subject_sd[r+i][m][nn++]);
                            for(n=1;n<tc_frames[j];n++,nn++) fprintf(fprn,"%8.4f ",within_subject_sd[r+i][m][nn]);
                            fprintf(fprn,"\n");
                            }
                        }
                    if(lcprint_unscaled_tc) {
                        fprintf(fprn,"    unscaled (MR units)\n");
                        for(m=0;m<nglmpersub;m++) {
                            nn=jj;
                            fprintf(fprn,"%s%*.4f ",glm_names[m],spaces+9-(int)len_glm_names[m],time_courses_unscaled[r+i][m][nn++]);
                            for(n=1;n<tc_frames[j];n++,nn++) fprintf(fprn,"%8.4f ",time_courses_unscaled[r+i][m][nn]);
                            fprintf(fprn,"\n");
                            }
                        if(lcsem||lcsd) {
                            nn=jj;

                            //fprintf(fprn,"average%*.4f ",spaces+1,time_courses_avg_unscaled[r+i][nn++]);
                            //START150911
                            fprintf(fprn,"average%*.4f ",spaces+2,time_courses_avg_unscaled[r+i][nn++]);

                            for(n=1;n<tc_frames[j];n++,nn++) fprintf(fprn,"%8.4f ",time_courses_avg_unscaled[r+i][nn]);
                            fprintf(fprn,"\n");
                            nn=jj;

                            //fprintf(fprn,"%s%*.4f ",sdsemstrptr,sdsemspace+spaces+5,time_courses_sd_unscaled[r+i][nn++]);
                            //START150911
                            fprintf(fprn,"%s%*.4f ",sdsemstrptr,sdsemspace+spaces+6,time_courses_sd_unscaled[r+i][nn++]);

                            for(n=1;n<tc_frames[j];n++,nn++) fprintf(fprn,"%8.4f ",time_courses_sd_unscaled[r+i][nn]);
                            fprintf(fprn,"\n");
                            }
                        }
                    }
                if(lcphase) {
                    nn=jj;
                    fprintf(fprn,"    phase unscaled (radians)\n");
                    for(m=0;m<nglmpersub;m++) {
                        fprintf(fprn,"%s%*.4f ",glm_names[m],spaces+9-(int)len_glm_names[m],tcphase_unscaled[r+i][m][nn++]);
                        for(n=1;n<tc_frames[j];n++,nn++) fprintf(fprn,"%8.4f ",tcphase_unscaled[r+i][m][nn]);
                        fprintf(fprn,"\n");
                        }
                    nn=jj;

                    //fprintf(fprn,"average%*.4f ",spaces+1,tcavgphase_unscaled[r+i][nn++]);
                    //START150911
                    fprintf(fprn,"average%*.4f ",spaces+2,tcavgphase_unscaled[r+i][nn++]);

                    for(n=1;n<tc_frames[j];n++,nn++) fprintf(fprn,"%8.4f ",tcavgphase_unscaled[r+i][nn]);
                    fprintf(fprn,"\n");
                    }
                }
            }
        }
    fflush(fprn);
    fclose(fprn);
    printf("Region results written to %s\n",outfile);
    fflush(stdout);
    return 1;
    }
int boyprintparam(double ***time_courses_param,double **time_courses_avg_param,int num_regions,int nglmpersub,int num_tc_frames,
    int *tc_frames,Tags_Tc_Struct2 *tags_tcs,Files_Struct *wfiles,Regions_By_File *rbf,int spaces,
    char **glm_names,size_t *len_glm_names,int num_wfiles,int lcsem,int lcsd,char *outfile,double TR,Files_Struct *glm_list_file,
    int how_many){
    int i,j,m,n,q,r,jj,nn,pp,p1;
    size_t i1;
    FILE *fprn;
    if(!(fprn=fopen_sub(outfile,"w")))return 0;
    fprintf(fprn,"TR = %4.2f\n",TR);
    if(glm_list_file) {
        fprintf(fprn,"GLM LIST = %s\n",glm_list_file->files[0]);
        for(i1=1;i1<glm_list_file->nfiles;i1++) fprintf(fprn,"           %s\n",glm_list_file->files[i1]);
        }
    fprintf(fprn,"\n");
    fprintf(fprn,"delta tau A C duration SSE R2\n");
    for(p1=(int)HRF_P+2,pp=jj=j=0;j<num_tc_frames;jj+=tc_frames[j++],pp+=p1) {
        fprintf(fprn,"\nTIMECOURSE : %s",tags_tcs->tags_tcptr[jj]);
        for(m=jj;m<jj+tc_frames[j];m++) fprintf(fprn," %s",tags_tcs->tags_tc_effectsptr[m]);
        fprintf(fprn,"\n");
        for(r=q=0;q<num_wfiles;q++,r+=how_many) {
            if(wfiles) fprintf(fprn,"WEIGHT FILE : %s\n\n",wfiles->files[q]);
            for(i=0;i<how_many;i++) {
                if(num_regions) {
                    fprintf(fprn,"\nREGION : %d %s %d",rbf->harolds_num[i],rbf->region_names_ptr[i],rbf->nvoxels_region[i]);
                    if(!rbf->nvoxels_region[i]) {
                        fprintf(fprn,"\n");
                        if(!j) printf("Region %s has zero voxels.\n",rbf->region_names_ptr[i]);
                        fprintf(fprn,"\n");
                        continue;
                        }
                    }
                fprintf(fprn,"\n");
                for(m=0;m<nglmpersub;m++) {
                    nn=pp;
                    fprintf(fprn,"%s%*.8f ",glm_names[m],spaces+12-(int)len_glm_names[m],time_courses_param[r+i][m][nn++]);
                    for(n=1;n<p1;n++,nn++) fprintf(fprn,"%12.8f ",time_courses_param[r+i][m][nn]);
                    fprintf(fprn,"\n");
                    }
                if(lcsem||lcsd) {
                    nn=pp;

                    //fprintf(fprn,"average%*.8f ",spaces+4,time_courses_avg_param[r+i][nn++]);
                    //START150911
                    fprintf(fprn,"average%*.8f ",spaces+5,time_courses_avg_param[r+i][nn++]);

                    for(n=1;n<p1;n++,nn++) fprintf(fprn,"%12.8f ",time_courses_avg_param[r+i][nn]);
                    fprintf(fprn,"\n");
                    }
                }
            }
        }
    fflush(fprn);
    fclose(fprn);
    printf("Region results written to %s\n",outfile);
    fflush(stdout);
    return 1;
    }
int boyprintvox(double ***voxels,double ***voxels_unscaled,int nglmpersub,int num_tc_frames,int *tc_frames,
    Regions_By_File *rbf,char **glm_names,int lcprint_scaled_tc,int lcprint_unscaled_tc,char *outfile,double TR,
    Files_Struct *glm_list_file,int num_wfiles,Files_Struct *wfiles,Atlas_Param *ap,char **tags_tcptr,char **tags_tc_effectsptr){
    char string[MAXNAME],string2[MAXNAME];
    int i,j,k,m,n,q,r,ii,jj,mm,nn,fidlcoor[3],atlascoor[3],count;
    size_t i1;
    double td;
    FILE *fprn;
    strcpy(string,outfile);
    if(!get_tail_sans_ext(string)) exit(-1);
    for(k=0;k<nglmpersub;k++) {
        if(nglmpersub>1) {
            sprintf(string2,"%s_%s_vox.txt",string,glm_names[k]);
            }
        else {
            sprintf(string2,"%s_vox.txt",string);
            }
        if(!(fprn=fopen_sub(string2,"w"))) return 0;
        fprintf(fprn,"TR = %4.2f\n",TR);
        if(glm_list_file) {
            fprintf(fprn,"GLM LIST = %s\n",glm_list_file->files[0]);
            for(i1=1;i1<glm_list_file->nfiles;i1++) fprintf(fprn,"           %s\n",glm_list_file->files[i1]);
            }
        for(jj=j=0;j<num_tc_frames;jj+=tc_frames[j++]) {
            fprintf(fprn,"\n%s : %s",tags_tc_effectsptr?"TIMECOURSE":"ZSTAT",tags_tcptr[jj]);
            if(tags_tc_effectsptr) for(m=jj;m<jj+tc_frames[j];m++) fprintf(fprn," %s",tags_tc_effectsptr[m]);
            fprintf(fprn,"\n");
            for(r=q=0;q<num_wfiles;q++,r+=rbf->nvoxels) {
                if(wfiles) fprintf(fprn,"WEIGHT FILE : %s\n\n",wfiles->files[q]);
                for(ii=i=0;i<rbf->nreg;ii+=rbf->nvoxels_region[i++]) {
                    fprintf(fprn,"\nREGION : %d %s %d",rbf->harolds_num[i],rbf->region_names_ptr[i],rbf->nvoxels_region[i]);
                    if(!rbf->nvoxels_region[i]) {
                        fprintf(fprn,"\n");
                        if(!j) printf("Region %s has zero voxels.\n",rbf->region_names_ptr[i]);
                        fprintf(fprn,"\n");
                        continue;
                        }
                    if(lcprint_scaled_tc) {
                        fprintf(fprn,"\n");
                        for(mm=ii,m=0;m<rbf->nvoxels_region[i];m++,mm++) {
                            nn=jj;
                            voxel_index_to_fidl_and_atlas(rbf->indices[mm],ap->xdim,ap->ydim,ap->zdim,ap->center,ap->mmppix,fidlcoor,
                                atlascoor);
                            fprintf(fprn,"%d\t%d\t%d",atlascoor[0],atlascoor[1],atlascoor[2]);
                            for(n=0;n<tc_frames[j];n++,nn++) fprintf(fprn,"\t%12.8f",voxels[mm][k][nn]);
                            fprintf(fprn,"\n");
                            }
                        }
                    if(lcprint_unscaled_tc) {
                        fprintf(fprn,"    unscaled (MR units)\n");
                        for(mm=ii,m=0;m<rbf->nvoxels_region[i];m++,mm++) {
                            nn=jj;
                            voxel_index_to_fidl_and_atlas(rbf->indices[mm],ap->xdim,ap->ydim,ap->zdim,ap->center,ap->mmppix,fidlcoor,
                                atlascoor);
                            fprintf(fprn,"%d\t%d\t%d",atlascoor[0],atlascoor[1],atlascoor[2]);
                            for(n=0;n<tc_frames[j];n++,nn++) fprintf(fprn,"\t%12.8f",voxels_unscaled[mm][k][nn]);
                            fprintf(fprn,"\n");
                            }
                        }
                    }
                }
            }
        fflush(fprn);
        fclose(fprn);
        printf("Voxel values written to %s\n",string2);
        fflush(stdout);
        }
    if(nglmpersub>1) {
        sprintf(string2,"%s_mean_vox.txt",string);
        if(!(fprn=fopen_sub(string2,"w"))) return 0;
        fprintf(fprn,"TR = %4.2f\n",TR);
        if(glm_list_file) {
            fprintf(fprn,"GLM LIST = %s\n",glm_list_file->files[0]);
            for(i1=1;i1<glm_list_file->nfiles;i1++) fprintf(fprn,"           %s\n",glm_list_file->files[i1]);
            }
        for(jj=j=0;j<num_tc_frames;jj+=tc_frames[j++]) {
            fprintf(fprn,"\n%s : %s",tags_tc_effectsptr?"TIMECOURSE":"ZSTAT",tags_tcptr[jj]);
            if(tags_tc_effectsptr) for(m=jj;m<jj+tc_frames[j];m++) fprintf(fprn," %s",tags_tc_effectsptr[m]);
            fprintf(fprn,"\n");
            for(r=q=0;q<num_wfiles;q++,r+=rbf->nvoxels) {
                if(wfiles) fprintf(fprn,"WEIGHT FILE : %s\n\n",wfiles->files[q]);
                for(ii=i=0;i<rbf->nreg;ii+=rbf->nvoxels_region[i++]) {
                    fprintf(fprn,"\nREGION : %d %s %d",rbf->harolds_num[i],rbf->region_names_ptr[i],rbf->nvoxels_region[i]);
                    if(!rbf->nvoxels_region[i]) {
                        fprintf(fprn,"\n");
                        if(!j) printf("Region %s has zero voxels.\n",rbf->region_names_ptr[i]);
                        fprintf(fprn,"\n");
                        continue;
                        }
                    if(lcprint_scaled_tc) {
                        fprintf(fprn,"\n");
                        for(mm=ii,m=0;m<rbf->nvoxels_region[i];m++,mm++) {
                            nn=jj;
                            voxel_index_to_fidl_and_atlas(rbf->indices[mm],ap->xdim,ap->ydim,ap->zdim,ap->center,ap->mmppix,fidlcoor,
                                atlascoor);
                            fprintf(fprn,"%d\t%d\t%d",atlascoor[0],atlascoor[1],atlascoor[2]);
                            for(n=0;n<tc_frames[j];n++,nn++) {
                                for(td=0.,count=k=0;k<nglmpersub;k++) {
                                    if(voxels[mm][k][nn]!=(double)UNSAMPLED_VOXEL) {td += voxels[mm][k][nn];count++;}
                                    }
                                fprintf(fprn,"\t%12.8f",td/(double)count);
                                }
                            fprintf(fprn,"\n");
                            }
                        }
                    if(lcprint_unscaled_tc) {
                        fprintf(fprn,"    unscaled (MR units)\n");
                        for(mm=ii,m=0;m<rbf->nvoxels_region[i];m++,mm++) {
                            nn=jj;
                            voxel_index_to_fidl_and_atlas(rbf->indices[mm],ap->xdim,ap->ydim,ap->zdim,ap->center,ap->mmppix,fidlcoor,
                                atlascoor);
                            fprintf(fprn,"%d\t%d\t%d",atlascoor[0],atlascoor[1],atlascoor[2]);
                            for(n=0;n<tc_frames[j];n++,nn++) {
                                for(td=0.,count=k=0;k<nglmpersub;k++)
                                    if(voxels_unscaled[mm][k][nn]!=(double)UNSAMPLED_VOXEL) 
                                        {td += voxels_unscaled[mm][k][nn];count++;}
                                fprintf(fprn,"\t%12.8f",td/(double)count);
                                }
                            fprintf(fprn,"\n");
                            }
                        }
                    }
                }
            }
        fflush(fprn);
        fclose(fprn);
        printf("Voxel values written to %s\n",string2);
        fflush(stdout);
        }
    return 1;
    }
