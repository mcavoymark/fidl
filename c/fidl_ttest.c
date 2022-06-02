/* Copyright 5/6/02 Washington University.  All Rights Reserved.
   fidl_ttest.c  $Revision: 1.91 $ */
#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_fit.h>
#include <gsl/gsl_cdf.h>
#include "fidl.h"
#include "read_ttest_driver.h"
#include "monte_carlo_mult_comp.h"
#include "filetype.h"
#include "subs_nifti.h"
#include "write1.h"
#include "subs_mask.h"
#include "dim_param2.h"
#include "shouldiswap.h"
#include "get_atlas_coor.h"
#include "subs_util.h"
#include "dim_param2.h"
#include "d2double.h"
#include "minmax.h"
#include "map_disk.h"
#define VAR_THRESH .0000000001 /*empirical*/
static char rcsid[] = "$Header: /home/hannah/mcavoy/idl/clib/RCS/fidl_ttest.c,v 1.91 2017/04/20 21:17:15 mcavoy Exp $";
int main(int argc,char **argv){
char string[MAXNAME],*strptr,*driver_file=NULL,*prepend=NULL,*regional_ttest_name=NULL,regional_anova_name_str[MAXNAME],
     *glm_list_file=NULL,*group_name=NULL,firststr[MAXNAME],*scratchdir=NULL,scrapstr[7],*Nimage_name=NULL,*mask_file=NULL,
     **files_ptr,string_t[MAXNAME],string_z[MAXNAME],**pstr=NULL,avgstr[MAXNAME],varstr[MAXNAME],*glmf=NULL,
     atlas[7]="",*space_str=NULL,meansemstr[MAXNAME],string2[MAXNAME],indstr[MAXNAME];
int i,j,k,l,m,r1,lenvol,*fidl_voxel=(int*)NULL,bigendian=1,swapbytes,i7,i72,
    lccleanup=0,lccleanup_only=0,num_region_names=0,how_many=0,
    lc_Z_uncorrected=0,text=0,modulo,
    num_tc=0,num_contrasts=0,argc_tc,argc_c,frames=0,
    lc_monte_carlo=(int)FALSE,lc_one_condition=(int)FALSE,*lt2subjects_voxels_idx,num_voxels_lt2subjects=0,
    lc_print_cov_matrix=(int)FALSE,n_threshold_extent=0,*extent=NULL,lc_Z_monte_carlo=0,
    lc_statview=0,t,mode=0,*df,*subtract,
    f1index,f2index,files_ptr_index,lc_T_uncorrected=(int)FALSE,
    lc_T_monte_carlo=0,*lc_test_type,loop,n4dfps,nharolds_num=0,*harolds_num=NULL,SunOS_Linux,nfspstr=0,nnull=0,lenbrain,len,
    nfiles_max,subtract_max,*n4dfps1,*n4dfps2,max1,max2,lc0,lc1,lc2,lc3,lc4,lc5,lc6,lc7,*nsubjects1,*nsubjects2,*work,
    *brnidx=NULL,filetype=IMG,nreg=0,
    ndiff=0,*idx,fpi; 
size_t i1;
float *temp_float=NULL,*temp_float2=NULL,*tf3=NULL,*tf4=NULL,**ind=NULL;
double *zstat,*dferror_stack,*threshold=NULL,*tstat,*tstat_uneqvar=NULL,*df_uneqvar=NULL,mag,mag2,difference,*avg1=NULL,avg2,
    *var1=NULL,var2,temp1,temp2,sum,sp2,*behav_var=NULL,sumx,sumx2,sumy,sumy2,sumxy,SSxx,SSxy,SSyy,*r=NULL,**cov_var=NULL,var,
    var_thresh=VAR_THRESH,*null=NULL,*data1=NULL,*data2=NULL,*Xblock=NULL,*Yblock=NULL,*dptr,*taublock=NULL,*Bblock=NULL,
    *residualblock=NULL,*XTXm1block=NULL,*XTXblock=NULL,*Xblockcopy=NULL,*LvoxLmap=NULL,*LvoxRmap=NULL,*RvoxLmap=NULL,
    *RvoxRmap=NULL,*coor=NULL,sumLvoxLmap,sumLvoxRmap,sumRvoxLmap,sumRvoxRmap,*col,*row,*slice,c0,c1LR,c1RL,cov00,cov01,cov11,
    sumsq,a1L,a2L,a1R,a2R,maga1,maga12,maga2,maga22,avga1,vara1,avga2,vara2,*sem=NULL;
Interfile_header *ifh=NULL,*ifh_out;
Memory_Map *mm,**mm1=NULL,**mm2=NULL;
Mask_Struct *ms=NULL;
FILE *fprn=NULL,*fprn1=NULL,*fprn2=NULL,*op=NULL;
Files_Struct *region_names=NULL,*fspstr,*diff=NULL;
TtestDesign *td;
Dim_Param2 *dp=NULL;
Mcmc *mcmc=NULL;
Atlas_Param *ap=NULL;
LinearModel *glm=NULL;
gsl_matrix_view Xgsl,XTXgsl,Xgslcopy;
gsl_vector_view Ygsl,taugsl,Bgsl,residualgsl,XTXm1gsl;
print_version_number(rcsid,stdout);
int64_t dims[4]={0,0,0,1};
W1 *w1=NULL;
if(argc < 3) {
    fprintf(stderr,"     -driver:              File that specifies the factor levels and associated files for the anova.\n");
    fprintf(stderr,"     -uncompress:          Specify mask file to put back into image format.\n");
    fprintf(stderr,"                           The glm_atlas_mask is used by default.\n\n");
    fprintf(stderr,"     -regions:             List of region names for regional T tests.\n");
    fprintf(stderr,"     -harolds_num:         One for each region.\n");
    fprintf(stderr,"     -prepend:             Output files for a voxel analysis will all begin with this.\n");
    fprintf(stderr,"                           If just a path, then end with /\n");
    fprintf(stderr,"     -fidl_voxel:          Three numbers from fidl: slice col row [fidl coordinates = (col,row)]\n\n");
    fprintf(stderr,"     -clean_up             Delete the files used in the anova computation.\n");
    fprintf(stderr,"     -clean_up ONLY        Don't do any computation, just delete the files used in the anova computation.\n\n");
    fprintf(stderr,"     -output:              Output options: T_uncorrected Z_uncorrected T_monte_carlo Z_monte_carlo\n\n");
    fprintf(stderr,"     -statview             Output text file for statview input. (Region is a factor.)\n");
    fprintf(stderr,"     -text                 Print out text file of regional analysis. (Region is a factor.)\n\n");
    fprintf(stderr,"     -tc:                  List of time courses for computation of box correction. The first tc is 1.\n");
    fprintf(stderr,"     -contrasts:           List of contrasts for computation of box correction. The first contrast is 1.\n");
    fprintf(stderr,"     -frames:              This option is used with -output box_correction_only\n");
    fprintf(stderr,"     -number_of_delays:    If magnitudes are being analyzed, the p-value is automatically corrected for the\n");
    fprintf(stderr,"                           number of delays. The default is set to 1.\n");
    fprintf(stderr,"     -regional_ttest_name: Output filename for a regional ttest.\n");
    fprintf(stderr,"                           Include path if you want output directed to a different directory.\n");
    fprintf(stderr,"     -Nimage_name:         Output filename for number of subjects at each voxel. Default is Nimage.4dfp.img\n");
    fprintf(stderr,"     -monte_carlo          Some processes skipped to speed up Monte Carlo simulation.\n");
    fprintf(stderr,"     -print_cov_matrix     Covariance matrix will be printed.\n");
    fprintf(stderr,"     -threshold_extent     Monte Carlo thresholds with spatial extent.\n");
    fprintf(stderr,"                           Ex. -threshold_extent \"3.75 18\" \"4 12\"\n");
    fprintf(stderr,"     -pval:                Associated p value for -threshold_extent. .05 is the default.\n");
    fprintf(stderr,"     -glm_list_file:       If present this goes into the ifh 'name of data file' field of the output files.\n");
    fprintf(stderr,"     -group_name:          Group name for subjects. Added to output filename.\n");
    fprintf(stderr,"     -scratchdir:          Location of precomputed scratchfiles. Include the backslash at the end.\n");
    fprintf(stderr,"     -var_thresh:          Variance threshold. Used for 1 and 2 sample tests. Default is 1e-10.\n");
    fprintf(stderr,"     -null:                Value of null hypothesis. Default is zero. Used for one sample and paired tests.\n");
    fprintf(stderr,"     -littleendian         Write output in little endian. Big endian is the default.\n");
    fprintf(stderr,"     -glm                  Used to extract the xml for cifti and some parameters for nifti.\n");

    //START170524
    fprintf(stderr,"     -difference:        Instead of doing the paired T test, print subject differences to file.\n");
    fprintf(stderr,"                         One filename for each T test. List is ok.\n");
    fprintf(stderr,"                         In the output the first frame is the first subject, the Nth frame is the Nth subject.\n");
    fprintf(stderr,"                         This is a hack to construct a matrix for RCUR.\n");

    exit(-1);
    }
for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-driver") && argc > i+1)
        driver_file = argv[++i];
    else if(!strcmp(argv[i],"-uncompress")) {
        if(argc > i+1 && strchr(argv[i+1],'-') != argv[i+1]) {
            mask_file = argv[++i];
            }
        else {
            printf("No mask file specified for -uncompress option.\n");
            exit(-1);
            }
        }
    else if(!strcmp(argv[i],"-regions") && argc > i+1) {
        for(j=1;i+j<argc;j++) {
            strptr = argv[i+j] + 1;
            if(strchr(argv[i+j],'-') == argv[i+j] && !isdigit((int)*strptr)) break;
            ++num_region_names;
            }
        if(!(region_names=get_files(num_region_names,&argv[i+1]))) exit(-1);
        i += num_region_names;
        }
    else if(!strcmp(argv[i],"-harolds_num") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nharolds_num;
        if(!(harolds_num=malloc(sizeof*harolds_num*nharolds_num))) {
            printf("Error: Unable to malloc harolds_num\n"); 
            exit(-1);
            }
        for(j=0;j<nharolds_num;j++) harolds_num[j] = atoi(argv[++i]);
        }
    else if(!strcmp(argv[i],"-prepend") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        prepend = argv[++i];
    else if(!strcmp(argv[i],"-fidl_voxel") && argc > i+1) {
	for(k=0,j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++k;
	if(k != 3) {
	    fprintf(stderr,"Must specify slice, col, and row for -fidl_voxel\n");
	    exit(-1);
	    }
	else {
            if(!(fidl_voxel=malloc(sizeof*fidl_voxel*3))) {
                printf("Error: Unable to malloc fidl_voxel\n");
                exit(-1);
                }
	    for(j=0;j<3;j++) fidl_voxel[j] = atoi(argv[++i]);
	    }
	}
    else if(!strcmp(argv[i],"-clean_up")) {
	lccleanup = 1;
        if(argc > i+1 && !strcmp(argv[i+1],"ONLY")) {
            lccleanup_only = 1;
            ++i;
            }
        }
    else if(!strcmp(argv[i],"-output") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) {
	    if(!strcmp(argv[i+j],"T_uncorrected")) lc_T_uncorrected = 1;
            else if(!strcmp(argv[i+j],"Z_uncorrected")) lc_Z_uncorrected = 1; 
            else if(!strcmp(argv[i+j],"T_monte_carlo")) lc_T_monte_carlo = 1; 
            else if(!strcmp(argv[i+j],"Z_monte_carlo")) lc_Z_monte_carlo = 1; 
	    }
	i += lc_T_uncorrected+lc_Z_uncorrected+lc_T_monte_carlo+lc_Z_monte_carlo;
	}
    else if(!strcmp(argv[i],"-statview"))
	lc_statview = 1;
    else if(!strcmp(argv[i],"-text"))
	text = (int)TRUE;
    else if(!strcmp(argv[i],"-tc") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_tc;
        argc_tc = i+1;
        i += num_tc;
        }
    else if(!strcmp(argv[i],"-contrasts") && argc > i+1) {
        for(j=1;i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_contrasts;
        argc_c = i+1;
        i += num_contrasts;
        }
    else if(!strcmp(argv[i],"-frames") && argc > i+1)
        frames = atoi(argv[++i]);
    else if(!strcmp(argv[i],"-monte_carlo"))
	lc_monte_carlo = (int)TRUE;
    else if(!strcmp(argv[i],"-regional_ttest_name") && argc > i+1)
        regional_ttest_name = argv[++i];
    else if(!strcmp(argv[i],"-Nimage_name") && argc > i+1)
        Nimage_name = argv[++i];
    else if(!strcmp(argv[i],"-one_condition"))
	lc_one_condition = (int)TRUE;
    else if(!strcmp(argv[i],"-print_cov_matrix"))
	lc_print_cov_matrix = (int)TRUE;
    else if(!strcmp(argv[i],"-threshold_extent") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++n_threshold_extent;
        if(!(threshold=malloc(sizeof*threshold*n_threshold_extent))) {
            printf("Error: Unable to malloc threshold\n");
            exit(-1);
            }
        if(!(extent=malloc(sizeof*extent*n_threshold_extent))) {
            printf("Error: Unable to malloc extent\n");
            exit(-1);
            }
        for(j=0;j<n_threshold_extent;j++) sscanf(argv[++i],"%lf %d",&threshold[j],&extent[j]);
        }
    else if(!strcmp(argv[i],"-pval") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nfspstr;
        if(!(fspstr=get_files(nfspstr,&argv[i+1]))) exit(-1);
        pstr = fspstr->files;
        i += nfspstr;
        }
    else if(!strcmp(argv[i],"-glm_list_file") && argc > i+1)
        glm_list_file = argv[++i];
    else if(!strcmp(argv[i],"-group_name") && argc > i+1)
        group_name = argv[++i];
    else if(!strcmp(argv[i],"-scratchdir") && argc > i+1)
        scratchdir = argv[++i];
    else if(!strcmp(argv[i],"-var_thresh") && argc > i+1)
        var_thresh = atof(argv[++i]);
    else if(!strcmp(argv[i],"-null") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nnull;
        if(!(null=malloc(sizeof*null*nnull))) {
            printf("Error: Unable to malloc null\n");
            exit(-1);
            } 
        for(j=0;j<nnull;j++) null[j] = atof(argv[++i]);
        }
    else if(!strcmp(argv[i],"-littleendian"))
        bigendian = 0;
    else if(!strcmp(argv[i],"-glm") && argc > i+1)
        glmf = argv[++i];

    //START170524
    //else if(!strcmp(argv[i],"-difference") && argc > i+1)
    //    diff=argv[++i];
    else if(!strcmp(argv[i],"-difference") && argc > i+1){
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++)++ndiff;
        if(!(diff=read_files(ndiff,&argv[i+1])))exit(-1);
        i+=ndiff;
        }


    else
        printf("%s is an unknown option.\n",argv[i]);
    }
if(!driver_file) {
    printf("Error: Need to specify -driver\n");
    fflush(stdout); exit(-1);
    }
if(nfspstr) {
    if(nfspstr!=n_threshold_extent) {
        printf("Error: -pval need to have the same number of items as -threshold_extent\n");
        exit(-1);
        }
    }
if((SunOS_Linux=checkOS())==-1) exit(-1);
swapbytes = shouldiswap(SunOS_Linux,bigendian);
printf("var_thresh=%g\n",var_thresh);
firststr[0] = 0;
if(group_name) {
    strcat(firststr,group_name);
    strcat(firststr,"_");
    }
if(Nimage_name) *strstr(Nimage_name,".4dfp.img") = 0;
if(!(td=read_ttest_driver(driver_file))) exit(-1);
printf("Successfully read %s\n",driver_file);
#if 0
/*BEGIN CHECK*/
for(k=l=m=i=0;i<td->ntests;i++) {
    printf("TTEST := %s %s\n",td->type[i],td->name[i]);
    for(j=0;j<td->nfiles1[i];j++) {
        printf("FIRST: %sEND\n",td->files1[k++]);
        }
    for(j=0;j<td->nfiles2[i];j++) {
        printf("SECOND: %sEND\n",td->files2[m++]);
        }
    printf("l=%d\n",l);
    for(j=0;j<td->ncov[i];j++) {
        printf("COVARIATES: %sEND\n",td->cov[l++]);
        }
    }
/*END CHECK*/
#endif
//for(i=0;i<td->ntests;i++) {
//    printf("td->nfiles1[%d]=%d\n",i,td->nfiles1[i]);
//    printf("td->nfiles2[%d]=%d\n",i,td->nfiles2[i]);
//    }
if(nnull) {
    if(nnull != td->ntests) {
        printf("Error: nnull=%d td->ntests=%d  Should be equal. Abort!\n",nnull,td->ntests);
        exit(-1);
        }
    }
else {
    if(!(null=malloc(sizeof*null*td->ntests))) {
        printf("Error: Unable to malloc null\n");
        exit(-1);
        }
    for(i=0;i<td->ntests;i++) null[i] = 0.;
    }
//printf("null="); for(i=0;i<td->ntests;i++) printf("%f ",null[i]); printf("\n");
//printf("td->total_nfiles=%d\n",td->total_nfiles);
if(glmf) {
    if(!(glm=read_glm(glmf,1,SunOS_Linux))){printf("fidlError: reading %s  Abort!\n",glmf);exit(-1);}
    filetype=get_filetype2(glm->ifh->glm_cifti_xmlsize,glm->ifh->glm_boldtype);

    //START160602
    if((nreg=glm->ifh->nregions)){if(!(region_names=get_files(glm->ifh->nregions,glm->ifh->region_names)))exit(-1);}

    //printf("here0 glm->ifh->region_names\n");for(i=0;i<glm->ifh->nregions;i++)printf("%s \n",glm->ifh->region_names[i]);


    }
if(!(files_ptr=malloc(sizeof*files_ptr*td->total_nfiles))) {
    printf("Error: Unable to malloc files_ptr\n");
    exit(-1);
    }
if(!(n4dfps1=malloc(sizeof*n4dfps1*td->ntests))) {
    printf("Error: Unable to malloc n4dfps1\n");
    exit(-1);
    }
for(i=0;i<td->ntests;i++) n4dfps1[i]=0;
if(!(n4dfps2=malloc(sizeof*n4dfps2*td->ntests))) {
    printf("Error: Unable to malloc n4dfps2\n");
    exit(-1);
    }
for(i=0;i<td->ntests;i++) n4dfps2[i]=0;
for(max1=max2=n4dfps=k=m=i=0;i<td->ntests;i++) {
    for(j=0;j<td->nfiles1[i];j++,k++) {
        if(strstr(td->files1[k],".4dfp.img")) {
            files_ptr[n4dfps++] = td->files1[k];
            n4dfps1[i]++;
            }
        }
    for(j=0;j<td->nfiles2[i];j++,m++) {
        if(strstr(td->files2[m],".4dfp.img")) {
            files_ptr[n4dfps++] = td->files2[m];
            n4dfps2[i]++;
            }
        }
    if(td->nfiles1[i]>max1) max1=td->nfiles1[i];
    if(td->nfiles2[i]>max2) max2=td->nfiles2[i];
    }
nfiles_max = max1>max2 ? max1 : max2;
for(i=0;i<td->ntests;i++) printf("td->nfiles1[%d]=%d n4dfps1[%d]=%d td->nfiles2[%d]=%d n4dfps2[%d]=%d null=%f\n",i,td->nfiles1[i],i,
    n4dfps1[i],i,td->nfiles2[i],i,n4dfps2[i],null[i]);
if(n4dfps) {
    if(!(dp=dim_param2(n4dfps,files_ptr,SunOS_Linux))) exit(-1);
    if(dp->filetypeall==-1){printf("fidlError: fidl_ttest All files must be a single type. Abort!\n");fflush(stdout);exit(-1);}
    if(dp->volall==-1){printf("fidlError: fidl_ttest All files must be the same size. Abort!\n");fflush(stdout);exit(-1);}
    if(dp->bigendianall==-1){
        printf("fidlError: fidl_ttest All files must have the same endianness. Abort!\n");
        printf("fidlError: You will need to process the files through fidl_bolds first.\n");fflush(stdout);exit(-1);
        }
    }

/*#if 0*/

if(!lccleanup_only) {
    if(!(df=malloc(sizeof*df*td->ntests))) {
        printf("Error: Unable to malloc df\n");
        exit(-1);
        }
    if(!(subtract=malloc(sizeof*subtract*td->ntests))) {
        printf("Error: Unable to malloc subtract\n");
        exit(-1);
        }
    if(!(lc_test_type=malloc(sizeof*lc_test_type*td->ntests))) {
        printf("Error: Unable to malloc lc_test_type\n");
        exit(-1);
        }
    for(lc0=lc1=lc2=lc3=lc4=lc5=lc6=lc7=subtract_max=i=0;i<td->ntests;i++) {
        if(!strcmp(td->type[i],"UNPAIRED_COMPARISON")) {
            df[i] = td->nfiles1[i] - 1;
            subtract[i] = 1;
            lc_test_type[i] = 0;
            lc0=1;
            }
        else if(!strcmp(td->type[i],"PAIRED_COMPARISON")) {
            df[i] = td->nfiles1[i] - 1;
            subtract[i] = 1;
            lc_test_type[i] = 1;
            lc1=1;
            }
        else if(!strcmp(td->type[i],"GROUP_COMPARISON")) {
            df[i] = td->nfiles1[i] + td->nfiles2[i] - 2;
            subtract[i] = 2;
            lc_test_type[i] = 2;
            lc2=1;
            }
        else if(!strcmp(td->type[i],"CORRELATION")) {
            df[i] = td->nfiles1[i] - 2;
            subtract[i] = 2;
            lc_test_type[i] = 3;
            lc3=1;
            }
        else if(!strcmp(td->type[i],"PARTIAL_CORRELATION")) {
            df[i] = td->nfiles1[i] - 2 - td->ncov_per_line[i];
            subtract[i] = 2 + td->ncov_per_line[i];
            lc_test_type[i] = 4;
            lc4=1;
            }
        else if(!strcmp(td->type[i],"SS_VS_GROUP")) {
            df[i] = td->nfiles2[i] - 1;
            subtract[i] = 2;
            lc_test_type[i] = 5;
            lc5=1;
            }
        else if(!strcmp(td->type[i],"TYPE_II_REGRESSION")) {
            df[i] = td->nfiles1[i] - 1;
            subtract[i] = 1;
            lc_test_type[i] = 6;
            lc6=1;
            }
        else if(!strcmp(td->type[i],"SS_TWOINDSAM")) {
            lc_test_type[i] = 7;
            lc7=1;
            }
        else {
            printf("Error: Test type has not been implemented. Abort!\n");
            exit(-1);
            }
        if(subtract[i]>subtract_max) subtract_max=subtract[i];
        }
    if(!n4dfps) {
        lenbrain=lenvol=1;
        if(!(data1=malloc(sizeof*data1*max1))) {
            printf("Error: Unable to malloc data1\n");
            exit(-1);
            }
        if(lc0||lc1||lc2||lc5||lc7) {
            if(!(data2=malloc(sizeof*data2*max2))) {
                printf("Error: Unable to malloc data2\n");
                exit(-1);
                }
            }
        }
    else {
        if(!(ifh = read_ifh(files_ptr[0],(Interfile_header*)NULL))) exit(-1);
        lenvol = dp->volall;
        #ifndef MONTE_CARLO
            if(glm_list_file) assign_glm_list_file_to_ifh(ifh,glm_list_file);
        #endif

        //if(!(ms=get_mask_struct(mask_file,lenvol,(int*)NULL,SunOS_Linux,(LinearModel*)NULL,lenvol))) exit(-1);
        //START170323
        if(!(ms=get_mask_struct(mask_file,lenvol,(int*)NULL,SunOS_Linux,(LinearModel*)NULL,0))) exit(-1);

        how_many = ms->lenvol;
        lenbrain=ms->lenbrain;
        get_atlas(how_many,atlas);
        if(atlas[0]) {

            #if 0
            if(!(ap=get_atlas_param(atlas,(Interfile_header*)NULL))) exit(-1);
            ifh->dim1 = ap->xdim;
            ifh->dim2 = ap->ydim;
            ifh->dim3 = ap->zdim;
            ifh->mmppix[0] = ap->mmppix[0];
            ifh->mmppix[1] = ap->mmppix[1];
            ifh->mmppix[2] = ap->mmppix[2];
            ifh->center[0] = ap->center[0];
            ifh->center[1] = ap->center[1];
            ifh->center[2] = ap->center[2];
            space_str=ap->str;
            #endif
            //START170323
            //if(!(ap=get_atlas_param(atlas,filetype==(int)NIFTI?ifh:NULL)))exit(-1);
            if(!(ap=get_atlas_param(atlas,filetype==(int)NIFTI?glm->ifh:NULL)))exit(-1);

            //START170324
            //if(filetype!=(int)NIFTI){

                ifh->dim1 = ap->xdim;
                ifh->dim2 = ap->ydim;
                ifh->dim3 = ap->zdim;
                ifh->mmppix[0] = ap->mmppix[0];
                ifh->mmppix[1] = ap->mmppix[1];
                ifh->mmppix[2] = ap->mmppix[2];
                ifh->center[0] = ap->center[0];
                ifh->center[1] = ap->center[1];
                ifh->center[2] = ap->center[2];

            //START170324
            //    }

            space_str=ap->str;
            }

        ifh->bigendian = bigendian;
        if(ifh->datades) free(ifh->datades);
        if(!(ifh->datades=malloc(sizeof*ifh->datades*14))) {
            printf("fidlError: Unable to malloc ifh->datades\n");
            exit(-1);
            }
        strcpy(ifh->datades,"gaussianizedT");
        if(mask_file) {
            if(ifh->mask) free(ifh->mask);
            if(!(ifh->mask=malloc(sizeof*ifh->mask*(strlen(mask_file)+1)))) {
                printf("fidlError: Unable to malloc ifh->mask\n");
                exit(-1);
                }
            strcpy(ifh->mask,mask_file);
            }
        if(ifh->nregions&&!num_region_names) {
            if(!(region_names=get_files(ifh->nregions,ifh->region_names))) exit(-1);
            }
        j = how_many > n4dfps ? how_many : n4dfps;
        if(!(temp_float=malloc(sizeof*temp_float*j))) {
            printf("Error: Unable to malloc temp_float\n");
            exit(-1);
            }
        if(!(temp_float2=malloc(sizeof*temp_float2*j))) {
            printf("Error: Unable to malloc temp_float2\n");
            exit(-1);
            }
        if(lc6) {
            if(lenvol!=ms->lenbrain) {
                brnidx = ms->brnidx;
                }
            else {
                if(!(brnidx=malloc(sizeof*brnidx*lenvol))) {
                    printf("Error: Unable to malloc brnidx\n");
                    exit(-1);
                    }
                for(i=0;i<lenvol;i++) brnidx[i]=i;
                }
            }
        else {
            if(!(mm1=malloc(sizeof*mm1*max1))) {
                printf("Error: Unable to malloc mm1\n");
                exit(-1);
                }
            if(lc0||lc1||lc2||lc5||lc7) {
                if(!(mm2=malloc(sizeof*mm2*max2))) {
                    printf("Error: Unable to malloc mm2\n");
                    exit(-1);
                    }
                }
            }
        if(lc_Z_monte_carlo||lc_T_monte_carlo) {
            if(!(mcmc=monte_carlo_mult_comp_init(ifh->dim1,ifh->dim2,ifh->dim3,lenbrain,ms->brnidx))) exit(-1);
            } 
        }
    /*printf("lenbrain=%d lenvol=%d\n",lenbrain,lenvol);fflush(stdout);*/
    if(lc3||lc4||lc7) {
        if(!(behav_var=malloc(sizeof*behav_var*max2))) {
            printf("Error: Unable to malloc behav_var\n");
            exit(-1);
            }
        }
    /*mode = 0, print z; mode = 1, print t; mode = 2, print both*/
    if(lc_Z_monte_carlo) mode = 0;
    if(lc_T_monte_carlo) mode = 1;
    if(lc_Z_monte_carlo && lc_T_monte_carlo) mode = 2;


    /*START151027*/
    if(lc0||lc1){
        if(!(sem=malloc(sizeof*sem*lenvol))) {
            printf("Error: Unable to malloc sem\n");
            exit(-1);
            }
        }

    if(lc0||lc1||lc2||lc5||lc6) {
        if(!(avg1=malloc(sizeof*avg1*lenvol))) {
            printf("Error: Unable to malloc avg1\n");
            exit(-1);
            }
        if(!(var1=malloc(sizeof*var1*lenvol))) {
            printf("Error: Unable to malloc var1\n");
            exit(-1);
            }
        }
    if(lc2||lc7) {
        if(!(tstat_uneqvar=malloc(sizeof*tstat_uneqvar*how_many))) {
            printf("Error: Unable to malloc tstat_uneqvar\n");
            exit(-1);
            }
        if(!(df_uneqvar=malloc(sizeof*df_uneqvar*how_many))) {
            printf("Error: Unable to malloc df_uneqvar\n");
            exit(-1);
            }
        }
    else if(lc3||lc4) {
        if(!(r=malloc(sizeof*r*lenvol))) {
            printf("Error: Unable to malloc r\n");
            exit(-1);
            }
        }
    if(lc4) {
        if(!(Xblock=malloc(sizeof*Xblock*nfiles_max*subtract_max))) {
            printf("Error: Unable to malloc Xblock\n");
            exit(-1);
            }
        if(!(Xblockcopy=malloc(sizeof*Xblockcopy*nfiles_max*subtract_max))) {
            printf("Error: Unable to malloc Xblockcopy\n");
            exit(-1);
            }
        if(!(Yblock=malloc(sizeof*Yblock*nfiles_max))) {
            printf("Error: Unable to malloc Yblock\n");
            exit(-1);
            }
        if(!(taublock=malloc(sizeof*taublock*subtract_max))) {
            printf("Error: Unable to malloc taublock\n");
            exit(-1);
            }
        if(!(Bblock=malloc(sizeof*Bblock*subtract_max))) {
            printf("Error: Unable to malloc Bblock\n");
            exit(-1);
            }
        if(!(residualblock=malloc(sizeof*residualblock*nfiles_max))) {
            printf("Error: Unable to malloc residualblock\n");
            exit(-1);
            }
        if(!(XTXm1block=malloc(sizeof*XTXm1block*subtract_max))) {
            printf("Error: Unable to malloc XTXm1block\n");
            exit(-1);
            }
        if(!(XTXblock=malloc(sizeof*XTXblock*subtract_max*subtract_max))) {
            printf("Error: Unable to malloc XTXblock\n");
            exit(-1);
            }
        }
    if(lc6) {
        if(!(LvoxLmap=malloc(sizeof*LvoxLmap*ms->lenbrain))) {
            printf("Error: Unable to malloc LvoxLmap\n");
            exit(-1);
            }
        if(!(LvoxRmap=malloc(sizeof*LvoxRmap*ms->lenbrain))) {
            printf("Error: Unable to malloc LvoxRmap\n");
            exit(-1);
            }
        if(!(RvoxLmap=malloc(sizeof*RvoxLmap*ms->lenbrain))) {
            printf("Error: Unable to malloc RvoxLmap\n");
            exit(-1);
            }
        if(!(RvoxRmap=malloc(sizeof*RvoxRmap*ms->lenbrain))) {
            printf("Error: Unable to malloc RvoxRmap\n");
            exit(-1);
            }
        if(!(coor=malloc(sizeof*coor*ms->lenbrain*3))) {
            printf("Error: Unable to malloc coor\n");
            exit(-1);
            }
        if(!(col=malloc(sizeof*col*ms->lenbrain))) {
            printf("Error: Unable to malloc col\n");
            exit(-1);
            }
        if(!(row=malloc(sizeof*row*ms->lenbrain))) {
            printf("Error: Unable to malloc row\n");
            exit(-1);
            }
        if(!(slice=malloc(sizeof*slice*ms->lenbrain))) {
            printf("Error: Unable to malloc slice\n");
            exit(-1);
            }
        col_row_slice(ms->lenbrain,ms->brnidx,col,row,slice,ap);
        get_atlas_coor(ms->lenbrain,col,row,slice,(double)ap->zdim,ap->center,ap->mmppix,coor);
        }
    if(lc7) {
        if(!(tf3=malloc(sizeof*tf3*lenvol))) {
            printf("Error: Unable to malloc tf3\n");
            exit(-1);
            }
        if(!(tf4=malloc(sizeof*tf4*lenvol))) {
            printf("Error: Unable to malloc tf4\n");
            exit(-1);
            }
        }
    if(regional_ttest_name) {
        strcpy(regional_anova_name_str,regional_ttest_name);
        if(!(fprn=fopen_sub(regional_anova_name_str,"w"))) exit(-1);
        strcpy(scrapstr,"region");
        modulo = 1;
        if(lc0||lc1){
            strcpy(string2,regional_ttest_name);
            if(!get_tail_sans_ext(string2))exit(-1);
            sprintf(meansemstr,"%s_meansem.txt",string2);
            if(!(fprn1=fopen_sub(meansemstr,"w")))exit(-1);
            sprintf(indstr,"%s_ind.txt",string2);
            if(!(fprn2=fopen_sub(indstr,"w")))exit(-1);
            if(!nreg){
                for(j=0;j<lenvol;j++){fprintf(fprn1,"\t%s",region_names->files[j]);fprintf(fprn2,"\t%s",region_names->files[j]);}
                fprintf(fprn1,"\n");fprintf(fprn2,"\n");
                }
            else{
                for(j=0;j<lenvol;j++){
                    strptr=strcpy(string,region_names->files[j]);
                    strtok_r(strptr," \t",&strptr);
                    fprintf(fprn1,"\t%s",strptr);fprintf(fprn2,"\t%s",strptr);
                    }
                fprintf(fprn1,"\n");fprintf(fprn2,"\n");
                }
            if(!(ind=d2float(nfiles_max,lenvol)))exit(-1);
            }
        }
    else {
        strcpy(scrapstr,"voxel");
        modulo = 20000;
        }
    if(!(dferror_stack=malloc(sizeof*dferror_stack*lenvol))) {
        printf("Error: Unable to malloc dferror_stack\n");
        exit(-1);
        }
    if(!(nsubjects1=malloc(sizeof*nsubjects1*lenvol))) {
        printf("Error: Unable to malloc nsubjects1\n");
        exit(-1);
        }
    if(!(nsubjects2=malloc(sizeof*nsubjects2*lenvol))) {
        printf("Error: Unable to malloc nsubjects2\n");
        exit(-1);
        }
    if(!(lt2subjects_voxels_idx=malloc(sizeof*lt2subjects_voxels_idx*lenvol))) {
        printf("Error: Unable to malloc lt2subjects_voxels_idx\n");
        exit(-1);
        }
    if(!(tstat=malloc(sizeof*tstat*lenvol))) {
        printf("Error: Unable to malloc tstat\n");
        exit(-1);
        }
    if(!(zstat=malloc(sizeof*zstat*lenvol))) {
        printf("Error: Unable to malloc zstat\n");
        exit(-1);
        }
    if(!(work=malloc(sizeof*work*lenvol))) {
        printf("Error: Unable to malloc work\n");
        exit(-1);
        }
    if(!(w1=write1_init()))exit(-1);
    if(glmf){w1->cifti_xmldata=glm->cifti_xmldata;w1->xmlsize=glm->ifh->glm_cifti_xmlsize;}
    w1->filetype=filetype;
    w1->how_many=how_many;
    w1->swapbytes=swapbytes;
    w1->temp_float=temp_float;
    w1->ifh=ifh;
    w1->lenbrain=lenbrain;
    if(filetype==(int)NIFTI){
        dims[0]=glm->ifh->glm_xdim;dims[1]=glm->ifh->glm_ydim;dims[2]=glm->ifh->glm_zdim;
        w1->dims=dims;
        w1->center=glm->ifh->center;
        w1->mmppix=glm->ifh->mmppix;
        }
    for(f1index=f2index=files_ptr_index=i7=i72=t=0;t<td->ntests;t++) {
        #ifndef MONTE_CARLO
            for(j=0;j<lenvol;j++) nsubjects1[j] = n4dfps1[t];
            if(lc_test_type[t]==2 || lc_test_type[t]==5 || lc_test_type[t]==7) 
                for(j=0;j<lenvol;j++) nsubjects2[j] = n4dfps2[t];
            else
                for(j=0;j<lenvol;j++) nsubjects2[j] = 0;
            if(text || lc_statview || !n4dfps) {
                for(j=0;j<lenvol;j++) dferror_stack[j] = (double)df[t];
                }
            else {
                if(lc_test_type[t]==6) {
                    /*do nothing*/
                    }
                else if(lc_test_type[t]==1) { /*PAIRED_COMPARISON*/
                    if(n4dfps1[t]!=n4dfps2[t]) {
                        printf("n4dfps1[%d]=%d n4dfps2[%d]=%d Must be equal for a PAIRED_COMPARISON\n",t,n4dfps1[t],t,n4dfps2[t]);
                        continue;
                        }
//STARTHERE


                    #if 0
                    else{
                        for(i=0;i<n4dfps1[t];i++,files_ptr_index++) {
                            if(!(mm = map_disk(files_ptr[files_ptr_index],lenvol,0,sizeof(float)))) exit(-1);
                            for(j=0;j<lenvol;j++) temp_float[j] = mm->ptr[j];
                            unmap_disk(mm);
                            if(dp->swapbytes[files_ptr_index]) swap_bytes((unsigned char *)temp_float,sizeof(float),(size_t)lenvol);
                            if(!(mm = map_disk(files_ptr[files_ptr_index+n4dfps1[t]],lenvol,0,sizeof(float)))) exit(-1);
                            for(j=0;j<lenvol;j++) temp_float2[j] = mm->ptr[j];
                            unmap_disk(mm);
                            if(dp->swapbytes[files_ptr_index+n4dfps1[t]]) 
                                swap_bytes((unsigned char *)temp_float2,sizeof(float),(size_t)lenvol);
                            for(j=0;j<lenvol;j++) {
                                if(temp_float[j] == (float)UNSAMPLED_VOXEL || isnan((double)temp_float[j]) ||
                                   temp_float2[j] == (float)UNSAMPLED_VOXEL || isnan((double)temp_float2[j])) {
                                    --nsubjects1[j];
                                    }
                                }
                            }
                        files_ptr_index += n4dfps2[t];
                        }
                    #endif
                    //START170524
                    else{
                        for(fpi=files_ptr_index,i=0;i<n4dfps1[t];i++,fpi++) {
                            if(!(mm=map_disk(files_ptr[fpi],lenvol,0,sizeof(float)))) exit(-1);
                            for(j=0;j<lenvol;j++) temp_float[j] = mm->ptr[j];
                            unmap_disk(mm);
                            if(dp->swapbytes[fpi])swap_bytes((unsigned char*)temp_float,sizeof(float),(size_t)lenvol);
                            if(!(mm=map_disk(files_ptr[fpi+n4dfps1[t]],lenvol,0,sizeof(float)))) exit(-1);
                            for(j=0;j<lenvol;j++) temp_float2[j] = mm->ptr[j];
                            unmap_disk(mm);
                            if(dp->swapbytes[fpi+n4dfps1[t]])swap_bytes((unsigned char *)temp_float2,sizeof(float),(size_t)lenvol);
                            for(j=0;j<lenvol;j++) {
                                if(temp_float[j] == (float)UNSAMPLED_VOXEL || isnan((double)temp_float[j]) ||
                                   temp_float2[j] == (float)UNSAMPLED_VOXEL || isnan((double)temp_float2[j])) {
                                    --nsubjects1[j];
                                    }
                                }
                            }
                        if(diff){
                            if(!(idx=malloc(sizeof*idx*lenvol))){
                                printf("fidlError: Unable to malloc idx\n");
                                exit(-1);
                                }
                            printf("Checking voxels\n"); 
                            for(k=j=0;j<lenvol;j++){
                                if(nsubjects1[j]<n4dfps1[t]){
                                    printf("Voxel index %d has %d subjects but needs %d\n",j,nsubjects1[j],n4dfps1[t]);
                                    }
                                else{
                                    idx[k++]=j;
                                    }
                                }
                            if(k<lenvol)printf("%d voxels will be excluded\n",lenvol-k);
                            printf("%d voxels will be included\n",k);
                            if(!(op=fopen_sub(diff->files[t],"w"))) exit(-1);
                            for(fpi=files_ptr_index,i=0;i<n4dfps1[t];i++,fpi++) {
                                if(!(mm=map_disk(files_ptr[fpi],lenvol,0,sizeof(float)))) exit(-1);
                                for(j=0;j<lenvol;j++) temp_float[j] = mm->ptr[j];
                                unmap_disk(mm);
                                if(dp->swapbytes[fpi])swap_bytes((unsigned char*)temp_float,sizeof(float),(size_t)lenvol);
                                if(!(mm=map_disk(files_ptr[fpi+n4dfps1[t]],lenvol,0,sizeof(float)))) exit(-1);
                                for(j=0;j<lenvol;j++) temp_float2[j] = mm->ptr[j];
                                unmap_disk(mm);
                                if(dp->swapbytes[fpi+n4dfps1[t]])swap_bytes((unsigned char*)temp_float2,sizeof(float),(size_t)lenvol);

                                #if 1
                                for(j=0;j<k;j++)temp_float[j]=temp_float[idx[j]]-temp_float2[idx[j]]; 
                                if(!fwrite_sub(temp_float,sizeof(float),(size_t)k,op,0)){
                                    printf("fidlError: Could not write to %s\n",diff->files[t]);
                                    exit(-1);
                                    }
                                #endif
                                //START170525
                                #if 0
                                for(j=0;j<k;j++)sem[j]=(double)temp_float[idx[j]]-(double)temp_float2[idx[j]]; 
                                if(!fwrite_sub(sem,sizeof*sem,(size_t)k,op,0)){
                                    printf("fidlError: Could not write to %s\n",diff->files[t]);
                                    exit(-1);
                                    }
                                #endif

                                }
                            fclose(op);
                            if(!(ifh_out=init_ifh(4,k,1,1,n4dfps1[t],ifh->voxel_size_1,ifh->voxel_size_2,ifh->voxel_size_3,
                                SunOS_Linux?0:1,(float*)NULL,(float*)NULL)))exit(-1);
                            if(!write_ifh(diff->files[t],ifh_out,0))exit(-1);
                            printf("Output written to %s\n",diff->files[t]);
                            free_ifh(ifh_out,0);
                            exit(-1);
                            }
                        files_ptr_index+=n4dfps1[t]+n4dfps2[t];
                        }


                    }
                else {
                    if(lc_test_type[t]==5) {
                        if(n4dfps1[t]!=1) {
                            printf("n4dfps1[%d]=%d Must be 1 for a SS_VS_GROUP test\n",t,n4dfps1[t]);
                            continue;
                            }
                        if(n4dfps2[t]<2) {
                            printf("n4dfps2[%d]=%d Must be >=2 for a SS_VS_GROUP test\n",t,n4dfps2[t]);
                            continue;
                            }
                        }
                    for(i=0;i<n4dfps1[t];i++,files_ptr_index++) {
                        if(!(mm = map_disk(files_ptr[files_ptr_index],lenvol,0,sizeof(float)))) exit(-1);
                        for(j=0;j<lenvol;j++) temp_float[j] = mm->ptr[j];
                        unmap_disk(mm);
                        if(dp->swapbytes[files_ptr_index]) swap_bytes((unsigned char *)temp_float,sizeof(float),(size_t)lenvol);
                        for(j=0;j<lenvol;j++) {
                            if(temp_float[j] == (float)UNSAMPLED_VOXEL || isnan((double)temp_float[j])) {
                                --nsubjects1[j];
                                }
                            }
                        }
                    for(i=0;i<n4dfps2[t];i++,files_ptr_index++) {
                        if(!(mm = map_disk(files_ptr[files_ptr_index],lenvol,0,sizeof(float)))) exit(-1);
                        for(j=0;j<lenvol;j++) temp_float[j] = mm->ptr[j];
                        unmap_disk(mm);
                        if(dp->swapbytes[files_ptr_index]) swap_bytes((unsigned char *)temp_float,sizeof(float),(size_t)lenvol);
                        for(j=0;j<lenvol;j++) {
                            if(temp_float[j] == (float)UNSAMPLED_VOXEL || isnan((double)temp_float[j])) {
                                --nsubjects2[j];
                                }
                            }
                        }
                    }
                if(lc_test_type[t]==2) for(j=0;j<lenvol;j++)
                    dferror_stack[j]=(nsubjects1[j]>1&&nsubjects2[j]>1)?(double)(nsubjects1[j]+nsubjects2[j]-subtract[t]):0.;
                else if(lc_test_type[t]!=7)
                    for(j=0;j<lenvol;j++)dferror_stack[j]=(temp1=(double)(nsubjects1[j]+nsubjects2[j]-subtract[t]))>= 0.?temp1:0.;
                if(lc_test_type[t]==5) {
                    for(num_voxels_lt2subjects=j=0;j<lenvol;j++) {
                        if(!nsubjects1[j]||nsubjects2[j]<2) lt2subjects_voxels_idx[num_voxels_lt2subjects++]=j;
                        }
                    }
                else if(lc_test_type[t]!=7){
                    for(num_voxels_lt2subjects=j=0;j<lenvol;j++) {
                        if(nsubjects1[j]+nsubjects2[j]<2) lt2subjects_voxels_idx[num_voxels_lt2subjects++]=j; 
                        }
                    }


                /*for(j=0;j<lenvol;j++) printf("nsubjects1[%d]=%d nsubjects2[%d]=%d subtract[%d]=%d dferror_stack[%d]=%f\n",
                    j,nsubjects1[j],j, nsubjects2[j],t,subtract[t],j,dferror_stack[j]);fflush(stdout);
                exit(-1);*/
                printf("num_voxels_lt2subjects=%d\n",num_voxels_lt2subjects);fflush(stdout);

                if(Nimage_name) {
                    for(i=0;i<(lc_test_type[t]==2?2:1);i++) {
                        for(j=0;j<how_many;j++) temp_float[j]=0.;
                        if(lc_test_type[t]==5)
                            for(j=0;j<lenbrain;j++) temp_float[ms->brnidx[j]] = (float)nsubjects2[j];
                        else if(lc_test_type[t]==2) {
                            if(!i){for(j=0;j<lenbrain;j++)temp_float[ms->brnidx[j]]=(float)nsubjects1[j];strcpy(string,"1");}
                            else  {for(j=0;j<lenbrain;j++)temp_float[ms->brnidx[j]]=(float)nsubjects2[j];strcpy(string,"2");}
                            }
                        else
                            for(j=0;j<lenbrain;j++) temp_float[ms->brnidx[j]] = (float)(nsubjects1[j]+nsubjects2[j]);
                        sprintf(regional_anova_name_str,"%s%s%s.4dfp.img",prepend?prepend:"",Nimage_name,lc_test_type[t]==2?string:"");
                        min_and_max(temp_float,how_many,&ifh->global_min,&ifh->global_max);
                        if(!writestack(regional_anova_name_str,temp_float,sizeof(float),(size_t)how_many,swapbytes)) exit(-1);
                        if(!write_ifh(regional_anova_name_str,ifh,0)) exit(-1);
                        printf("Nimage written to %s\n",regional_anova_name_str);
                        }
                    }
                }
        #else
            for(j=0;j<lenvol;j++) dferror_stack[j] = df[t];
        #endif

        //START170418
        if(fprn2)for(i=0;i<nfiles_max;i++)for(j=0;j<lenvol;j++)ind[i][j]=(float)UNSAMPLED_VOXEL;

        for(j=0;j<lenvol;j++) tstat[j]=(double)UNSAMPLED_VOXEL;
        if(lc_statview) {
            }
        else if(fidl_voxel) {
            }
        else {
            #ifndef MONTE_CARLO
                if(regional_ttest_name) {
                    strcpy(string,"region");
                    }
                else {
                    strcpy(string,"voxel");
                    }
                fprintf(stdout,"A total of %d %s(s) per scratch file will be processed.\n",lenvol,string);
            #endif
            if(lc_test_type[t]!=6) {
                if(!n4dfps) {
                    for(i=0;i<td->nfiles1[t];i++) data1[i] = atof(td->files1[f1index+i]);
                    }
                else {
                    for(i=0;i<td->nfiles1[t];i++) if(!(mm1[i] = map_disk(td->files1[f1index+i],lenvol,0,sizeof(float)))) exit(-1);
                    }
                if(!lc_test_type[t]) {
                    /*do nothing*/
                    }
                else if(lc_test_type[t]<3||lc_test_type[t]==5||lc_test_type[t]==7)  {
                    if(!n4dfps) {
                        for(i=0;i<td->nfiles2[t];i++) data2[i] = atof(td->files2[f2index+i]);
                        }
                    else {
                        for(i=0;i<td->nfiles2[t];i++) if(!(mm2[i] = map_disk(td->files2[f2index+i],lenvol,0,sizeof(float)))) exit(-1);
                        }
                    if(lc_test_type[t]==7) for(i=0;i<td->nfiles2[t];i++) behav_var[i] = atof(td->cov[f2index+i]);
                    }
                else {
                    for(i=0;i<td->nfiles2[t];i++) behav_var[i] = atof(td->files2[f2index+i]);
                    if(td->ncov[t]) {
                        if(!(cov_var=d2double(td->nfiles1[t],td->ncov_per_line[t]))) exit(-1);
                        for(i=0;i<td->nfiles1[t];i++) strings_to_double(td->cov[f1index+i],cov_var[i],td->ncov_per_line[t]);
                        }
                    }
                }
            if(lc_test_type[t]==1||lc_test_type[t]==6) {
                if(td->nfiles1[t] != td->nfiles2[t]) {
                    printf("Error: %s  FIRST has %d files. SECOND has %d files. Must be equal. Abort!\n",
                        td->type[t],td->nfiles1[t],td->nfiles2[t]);
                    exit(-1);
                    }
                }
            else if(lc_test_type[t]==3||lc_test_type[t]==4||lc_test_type[t]==7) {
                if(td->nfiles1[t] != td->nfiles2[t]) {
                    printf("Error: %s %s  Task has %d files.  %d behavioral values. Must be equal. Abort!\n",
                        td->type[t],td->name[t],td->nfiles1[t],td->nfiles2[t]);
                    exit(-1);
                    }
                if(td->ncov[t]) {
                    if(td->nfiles1[t] != td->ncov[t]) {
                        printf("Error: %s %s  Task has %d files.  %d covariate values. Must be equal. Abort!\n",
                            td->type[t],td->name[t],td->nfiles1[t],td->ncov[t]);
                        exit(-1);
                        }
                    }
                }
            if(lc_test_type[t]==2||lc_test_type[t]==7)for(i=0;i<lenvol;i++){tstat_uneqvar[i]=(double)UNSAMPLED_VOXEL;df_uneqvar[i]=0.;}
            if(lc_test_type[t]==7) {
                if(n4dfps) {
                    for(i=0;i<lenvol;i++) temp_float[i] = mm1[0]->ptr[i];
                    if(dp->swapbytes[i7++])swap_bytes((unsigned char *)temp_float,sizeof(float),(size_t)lenvol);
                    for(i=0;i<lenvol;i++) temp_float2[i] = mm1[1]->ptr[i];
                    if(dp->swapbytes[i7++])swap_bytes((unsigned char *)temp_float2,sizeof(float),(size_t)lenvol);
                    for(i=0;i<lenvol;i++) tf3[i] = mm2[0]->ptr[i];
                    if(dp->swapbytes[i72++])swap_bytes((unsigned char *)tf3,sizeof(float),(size_t)lenvol);
                    for(i=0;i<lenvol;i++) tf4[i] = mm2[1]->ptr[i];
                    if(dp->swapbytes[i72++])swap_bytes((unsigned char *)tf4,sizeof(float),(size_t)lenvol);
                    }
                for(i=0;i<lenvol;i++) {
                    #ifndef MONTE_CARLO
                        if(!(i%modulo)) fprintf(stdout,"Processing %s = %d\n",scrapstr,i);
                    #endif
                    if(nsubjects1[i]!=2||nsubjects2[i]!=2) {
                        /*printf("fidlError: nsubjects1[%d]=%d nsubjects2[%d]=%d Both must be 2. Skipping this one.\n",i,
                            nsubjects1[i],i,nsubjects2[i]);fflush(stdout);
                        printf("           temp_float[%d]=%f temp_float2[%d]=%f tf3[%d]=%f tf4[%d]=%f\n",i,temp_float[i],i,
                            temp_float2[i],i,tf3[i],i,tf4[i]);*/
                        continue;
                        } 
                    if(!n4dfps) {
                        printf("fidlError: This hasn't been coded. See McAvoy.\n");fflush(stdout);exit(-1);
                        }
                    else {
                        if(temp_float[i]!=(float)UNSAMPLED_VOXEL&&!isnan((double)temp_float[i]) &&
                            temp_float2[i]!=(float)UNSAMPLED_VOXEL&&!isnan((double)temp_float2[i]) &&
                            tf3[i]!=(float)UNSAMPLED_VOXEL&&!isnan((double)tf3[i]) &&
                            tf4[i]!=(float)UNSAMPLED_VOXEL&&!isnan((double)tf4[i])) {
                            temp1 = (double)tf3[i];
                            temp2 = (double)tf4[i];
                            sum = temp1 + temp2;
                            dferror_stack[i] = behav_var[0]+behav_var[1];
                            sp2 = (behav_var[0]*temp1 + behav_var[1]*temp2)/(behav_var[0]+behav_var[1]);
                            tstat[i] = sp2 < var_thresh ? (double)UNSAMPLED_VOXEL :
                                ((double)temp_float[i]-(double)temp_float2[i])/sqrt(sp2);
                            df_uneqvar[i] = sum*sum/(temp1*temp1/behav_var[0] + temp2*temp2/behav_var[1]);
                            tstat_uneqvar[i] = sum < var_thresh ? (double)UNSAMPLED_VOXEL:
                                ((double)temp_float[i]-(double)temp_float2[i])/sqrt(sum);
                            /*printf("temp_float[%d]=%f temp_float2[%d]=%f tf3[%d]=%f tf4[%d]=%f tstat=%f dferror_stack=%f\n",
                                i,temp_float[i],i,temp_float2[i],i,tf3[i],i,tf4[i],tstat[i],dferror_stack[i]);*/
                            }
                        }

                    }
                }
            else if(lc_test_type[t] == 6) {
                for(maga1=maga12=maga2=maga22=0.,i=0;i<n4dfps1[t];i++,files_ptr_index++) {
                    if(!(mm = map_disk(files_ptr[files_ptr_index],lenvol,0,sizeof(float)))) exit(-1);
                    for(j=0;j<lenvol;j++) temp_float[j] = mm->ptr[j];
                    unmap_disk(mm);
                    if(dp->swapbytes[files_ptr_index]) 
                        swap_bytes((unsigned char *)temp_float,sizeof(float),(size_t)lenvol);
                    if(!(mm = map_disk(files_ptr[files_ptr_index+n4dfps1[t]],lenvol,0,sizeof(float)))) exit(-1);
                    for(j=0;j<lenvol;j++) temp_float2[j] = mm->ptr[j];
                    unmap_disk(mm);
                    if(dp->swapbytes[files_ptr_index+n4dfps1[t]])
                        swap_bytes((unsigned char *)temp_float2,sizeof(float),(size_t)lenvol);
                    for(sumLvoxLmap=sumLvoxRmap=sumRvoxLmap=sumRvoxRmap=0.,dptr=coor,l=r1=j=0;j<lenvol;j++,dptr+=3) {
                        if(temp_float[j] == (float)UNSAMPLED_VOXEL || isnan((double)temp_float[j]) ||
                           temp_float2[j] == (float)UNSAMPLED_VOXEL || isnan((double)temp_float2[j])) {
                            /*do nothing*/
                            }
                        else {
                            if((*dptr)<0) {
                                sumLvoxLmap += LvoxLmap[l] = (double)temp_float[brnidx[j]];
                                sumLvoxRmap += LvoxRmap[l] = (double)temp_float2[brnidx[j]];
                                l++;
                                }
                            else {
                                sumRvoxLmap += RvoxLmap[r1] = (double)temp_float[brnidx[j]];
                                sumRvoxRmap += RvoxRmap[r1] = (double)temp_float2[brnidx[j]];
                                r1++;
                                }
                            }
                        }
                    gsl_fit_linear(LvoxRmap,1,LvoxLmap,1,l,&c0,&c1LR,&cov00,&cov01,&cov11,&sumsq);
                    gsl_fit_linear(LvoxLmap,1,LvoxRmap,1,l,&c0,&c1RL,&cov00,&cov01,&cov11,&sumsq);
                    a2L = sqrt(c1LR/c1RL);
                    a1L = sumLvoxLmap/(double)l - sumLvoxRmap/(double)l*a2L;
                    gsl_fit_linear(RvoxRmap,1,RvoxLmap,1,l,&c0,&c1LR,&cov00,&cov01,&cov11,&sumsq);
                    gsl_fit_linear(RvoxLmap,1,RvoxRmap,1,l,&c0,&c1RL,&cov00,&cov01,&cov11,&sumsq);
                    a2R = sqrt(c1LR/c1RL);
                    a1R = sumRvoxLmap/(double)r1 - sumRvoxRmap/(double)r1*a2R;
                    fprintf(fprn,"%d a1L=%f a1R=%f a2L=%f a2R=%f\n",i,a1L,a1R,a2L,a2R);
                    difference = a1L - a1R;
                    maga1 += difference;
                    maga12 += difference*difference; 
                    difference = a2L - a2R;
                    maga2 += difference;
                    maga22 += difference*difference; 
                    }
                avga1 = maga1/(double)n4dfps1[t];
                vara1 = (maga12-maga1*maga1/(double)n4dfps1[t])/((double)n4dfps1[t]-1.);
                avga2 = maga2/(double)n4dfps1[t];
                vara2 = (maga22-maga2*maga2/(double)n4dfps1[t])/((double)n4dfps1[t]-1.);
                tstat[0] = vara1<var_thresh ? (double)UNSAMPLED_VOXEL : (avga1-null[t])/sqrt(vara1/(double)n4dfps1[t]);
                tstat[1] = vara2<var_thresh ? (double)UNSAMPLED_VOXEL : (avga2-null[t])/sqrt(vara2/(double)n4dfps1[t]);
                for(i=0;i<2;i++) dferror_stack[i] = (double)df[i];
                t_to_z(tstat,zstat,2,dferror_stack,work);
                fprintf(fprn,"avga1=%f\tvara1=%f\tavga2=%f\tvara2=%f\n",avga1,vara1,avga2,vara2);
                fprintf(fprn,"\t\t\tT\t\tZ\t\tsigprob\n");

                #if 0
                fprintf(fprn,"%s\ta1L-a1R\t%f\t%f\t%g\n",td->name[t],tstat[0],zstat[0],gsl_cdf_tdist_Q(tstat[0],dferror_stack[0]));
                fprintf(fprn,"%s\ta2L-a2R\t%f\t%f\t%g\n\n",td->name[t],tstat[1],zstat[1],
                    gsl_cdf_tdist_Q(tstat[1],dferror_stack[1]));
                #endif
                //START160518
                fprintf(fprn,"%s\ta1L-a1R\t%f\t%f\t%g\n",td->name[t],tstat[0],zstat[0],tstat[0]>0.?
                    gsl_cdf_tdist_Q(tstat[0],dferror_stack[0]):gsl_cdf_tdist_P(tstat[0],dferror_stack[0]));
                fprintf(fprn,"%s\ta2L-a2R\t%f\t%f\t%g\n\n",td->name[t],tstat[1],zstat[1],tstat[1]>0.?
                    gsl_cdf_tdist_Q(tstat[1],dferror_stack[1]):gsl_cdf_tdist_P(tstat[1],dferror_stack[1]));

                files_ptr_index += n4dfps2[t];
                }
            else if(lc_test_type[t] == 4) {
                for(i=0;i<lenvol;i++) {
                    #ifndef MONTE_CARLO
                        if(!(i%modulo)) fprintf(stdout,"Processing %s = %d\n",scrapstr,i);
                    #endif
                    if(nsubjects1[i]) {
                        if(nsubjects1[i] > subtract[t]) {
                            Xgsl = gsl_matrix_view_array(Xblock,nsubjects1[i],subtract[t]);
                            Xgslcopy = gsl_matrix_view_array(Xblockcopy,nsubjects1[i],subtract[t]);
                            Ygsl = gsl_vector_view_array(Yblock,nsubjects1[i]);
                            taugsl = gsl_vector_view_array(taublock,subtract[t]);
                            Bgsl = gsl_vector_view_array(Bblock,subtract[t]);
                            residualgsl = gsl_vector_view_array(residualblock,nsubjects1[i]);
                            XTXgsl = gsl_matrix_view_array(XTXblock,subtract[t],subtract[t]);
                            XTXm1gsl = gsl_vector_view_array(XTXm1block,subtract[t]);
                            for(j=0;j<td->nfiles1[t];j++) temp_float[j] = mm1[j]->ptr[i];
                            if(dp->swapbytes[0]) swap_bytes((unsigned char *)temp_float,sizeof(float),(size_t)td->nfiles1[t]);
                            for(dptr=Xblock,k=j=0;j<td->nfiles1[t];j++) {
                                if(temp_float[j]!=(float)UNSAMPLED_VOXEL && !isnan((double)temp_float[j])) {
                                    *dptr++ = 1.;
                                    *dptr++ = behav_var[j];
                                    for(l=0;l<td->ncov_per_line[t];l++) *dptr++ = cov_var[j][l];
                                    Yblock[k++] = (double)temp_float[j];
                                    }
                                }
                            if(k != nsubjects1[i]) {
                                printf("Error: Problem0. k=%d nsubjects=%d. Should be equal.\n",k,nsubjects1[i]);
                                exit(-1);
                                }
                            gsl_matrix_memcpy(&Xgslcopy.matrix,&Xgsl.matrix);
                            gsl_linalg_QR_decomp(&Xgsl.matrix,&taugsl.vector);
                            gsl_linalg_QR_lssolve(&Xgsl.matrix,&taugsl.vector,&Ygsl.vector,&Bgsl.vector,&residualgsl.vector);
                            for(var=0.,j=0;j<nsubjects1[i];j++) var+=residualblock[j]*residualblock[j];
                            var/=(double)(nsubjects1[i]-subtract[t]);
                            for(j=0;j<subtract[t];j++) XTXm1block[j]=0.; XTXm1block[1]=1.;
                            gsl_blas_dgemm(CblasTrans,CblasNoTrans,1.0,&Xgslcopy.matrix,&Xgslcopy.matrix,0.0,&XTXgsl.matrix);
                            gsl_linalg_cholesky_decomp(&XTXgsl.matrix);
                            gsl_linalg_cholesky_svx(&XTXgsl.matrix,&XTXm1gsl.vector);
                            tstat[i] = Bblock[1]/sqrt(var*XTXm1block[1]);
                            r[i] = tstat[i]/sqrt(tstat[i]*tstat[i]+dferror_stack[i]);
                            }
                        else {
                            tstat[i] = (double)UNSAMPLED_VOXEL;
                            }
                        }
                    }
                }
            else if(lc_test_type[t] == 3) {
                for(i=0;i<lenvol;i++) {
                    #ifndef MONTE_CARLO
                        if(!(i%modulo)) fprintf(stdout,"Processing %s = %d\n",scrapstr,i);
                    #endif
                    if(nsubjects1[i]) {
                        if(nsubjects1[i] > 2) {
                            for(j=0;j<td->nfiles1[t];j++) temp_float[j] = mm1[j]->ptr[i];
                            if(dp->swapbytes[0]) swap_bytes((unsigned char *)temp_float,sizeof(float),(size_t)td->nfiles1[t]);
                            for(sumx=sumx2=sumy=sumy2=sumxy=j=0;j<td->nfiles1[t];j++) {
                                if(temp_float[j]!=(float)UNSAMPLED_VOXEL && !isnan((double)temp_float[j])) {
                                    sumx += (double)temp_float[j];
                                    sumx2 += (double)temp_float[j]*(double)temp_float[j];
                                    sumy += (double)behav_var[j];
                                    sumy2 += (double)behav_var[j]*(double)behav_var[j];
                                    sumxy += (double)temp_float[j]*(double)behav_var[j];
                                    }
                                }
                            SSxx = sumx2 - sumx*sumx/(double)nsubjects1[i];
                            SSxy = sumxy - sumx*sumy/(double)nsubjects1[i];
                            SSyy = sumy2 - sumy*sumy/(double)nsubjects1[i];
                            /*printf("SSxx=%f SSxy=%f SSyy=%f dferror_stack[%d]=%f tstat[%d]=%f\n",SSxx,SSxy,SSyy,i,dferror_stack[i],
                                i,tstat[i]);*/
                            r[i] = SSxy/sqrt(SSxx*SSyy);
                            tstat[i] =  r[i]*sqrt(dferror_stack[i]/(1-r[i]*r[i]));
                            }
                        else {
                            r[i] = tstat[i] = (double)UNSAMPLED_VOXEL;
                            }
                        }
                    }
                }
            else {
                for(i=0;i<lenvol;i++) {
                    #ifndef MONTE_CARLO
                        if(!(i%modulo)) fprintf(stdout,"Processing %s = %d\n",scrapstr,i);
                    #endif
                    if(nsubjects1[i]>1||lc_test_type[t]==5) {
                        if(!n4dfps) {
                            for(mag=mag2=j=0;j<td->nfiles1[t];j++) {
                                difference = lc_test_type[t]==1 ? data1[j]-data2[j] : data1[j];
                                mag += difference;
                                mag2 += difference*difference;
                                }
                            }
                        else {
                            for(j=0;j<td->nfiles1[t];j++) temp_float[j] = mm1[j]->ptr[i];
                            if(dp->swapbytes[0]) swap_bytes((unsigned char *)temp_float,sizeof(float),(size_t)td->nfiles1[t]);
                            if(lc_test_type[t]==1) {
                                for(j=0;j<td->nfiles1[t];j++) temp_float2[j] = mm2[j]->ptr[i];
                                if(dp->swapbytes[0]) swap_bytes((unsigned char *)temp_float2,sizeof(float),(size_t)td->nfiles1[t]);
                                for(mag=mag2=j=0;j<td->nfiles1[t];j++) {
                                    if(temp_float[j]!=(float)UNSAMPLED_VOXEL&&!isnan((double)temp_float[j]) &&
                                        temp_float2[j]!=(float)UNSAMPLED_VOXEL&&!isnan((double)temp_float2[j])) {
                                        difference = (double)temp_float[j]-(double)temp_float2[j];
                                        mag += difference;
                                        mag2 += difference*difference;
                                        if(fprn2)ind[j][i]=(float)difference;
                                        }
                                    }
                                }
                            else {
                                for(mag=mag2=j=0;j<td->nfiles1[t];j++) {
                                    if(temp_float[j]!=(float)UNSAMPLED_VOXEL&&!isnan((double)temp_float[j])) {
                                        difference = (double)temp_float[j];
                                        mag += difference;
                                        mag2 += difference*difference;
                                        }
                                    }
                                }
                            }

                        //printf("here0 mag=%f nsubjects1[%d]=%d\n",mag,i,nsubjects1[i]);

                        avg1[i] = mag/(double)nsubjects1[i];
                        var1[i] = nsubjects1[i]>1 ? (mag2-mag*mag/nsubjects1[i])/(nsubjects1[i]-1.) : 0.;
                        if(lc_test_type[t] < 2) {

                            #if 0
                            tstat[i] = var1[i]<var_thresh ? (double)UNSAMPLED_VOXEL : (avg1[i]-null[t])/sqrt(var1[i]/nsubjects1[i]);
                            #endif
                            /*START151027*/
                            tstat[i]=var1[i]<var_thresh?(double)UNSAMPLED_VOXEL:(avg1[i]-null[t])/(sem[i]=sqrt(var1[i]/nsubjects1[i]));

                            }
                        else if(nsubjects2[i] > 1) { /*GROUP*/

                            if(!nsubjects1[i]||!nsubjects2[i])
                                printf("here0 nsubjects1[%d]=%d nsubjects2[%d]=%d\n",i,nsubjects1[i],i,nsubjects2[i]);fflush(stdout);

                            if(!n4dfps) {
                                for(mag=mag2=j=0;j<td->nfiles2[t];j++) {
                                    difference = data2[j];
                                    mag += difference;
                                    mag2 += difference*difference;
                                    }
                                }
                            else {
                                for(j=0;j<td->nfiles2[t];j++) temp_float[j] = mm2[j]->ptr[i];
                                if(dp->swapbytes[0]) swap_bytes((unsigned char *)temp_float,sizeof(float),(size_t)td->nfiles2[t]);
                                for(mag=mag2=j=0;j<td->nfiles2[t];j++) {
                                    if(temp_float[j]!=(float)UNSAMPLED_VOXEL&&!isnan((double)temp_float[j])) {
                                        difference = (double)temp_float[j];
                                        mag += difference;
                                        mag2 += difference*difference;
                                        }
                                    }
                                }
                            avg2 = mag/(double)nsubjects2[i];
                            var2 = (mag2-mag*mag/nsubjects2[i])/(nsubjects2[i]-1);
                            sp2 = ((nsubjects1[i]-1)*var1[i] + (nsubjects2[i]-1)*var2)/(nsubjects1[i]+nsubjects2[i]-2);
                            tstat[i] = sp2 < var_thresh ? (double)UNSAMPLED_VOXEL :
                                (avg1[i]-avg2)/sqrt(sp2*(double)(1./nsubjects1[i]+1./nsubjects2[i]));
                            /*printf("tstat[%d]=%f dferror_stack[%d]=%f\n",i,tstat[i],i,dferror_stack[i]);fflush(stdout);*/
                            if(nsubjects1[i]>1) {
                                temp1 = var1[i]/(double)nsubjects1[i];
                                temp2 = var2/(double)nsubjects2[i];
                                sum = temp1 + temp2;
                                df_uneqvar[i] = sum*sum/(temp1*temp1/(double)(nsubjects1[i]-1) +
                                    temp2*temp2/(double)(nsubjects2[i]-1));
                                tstat_uneqvar[i] = (var1[i]+var2) < var_thresh ? (double)UNSAMPLED_VOXEL:(avg1[i]-avg2)/sqrt(sum);
                                }
                            }
                        }
                    }
                }
            if(lc_test_type[t]==6) continue;
            loop = (lc_test_type[t]==2||lc_test_type[t]==7) ? 2 : 1;
            for(i=0;i<loop;i++) {
                if(i == 1) {
                    for(j=0;j<lenvol;j++) {
                        tstat[j] = tstat_uneqvar[j];
                        dferror_stack[j] = df_uneqvar[j];
                        }
                    }
                t_to_z(tstat,zstat,lenvol,dferror_stack,work);
                /*for(j=0;j<lenvol;j++) printf("tstat[%d]=%f dferror_stack[%d]=%f zstat[%d]=%f\n",j,tstat[j],j,dferror_stack[j],j,
                    zstat[j]);fflush(stdout);*/
                if(!fprn) {
                    ifh->df1 = (float)df[t];
                    string[0] = 0;
                    if(prepend) sprintf(string,"%s",prepend);
                    strcat(string,firststr);
                    strcat(string,td->name[t]);
                    regional_anova_name_str[0] = 0;
                    if(lc_test_type[t]==2||lc_test_type[t]==5||lc_test_type[t]==7) {
                        i==0 ? strcat(regional_anova_name_str,"_eqvar") : strcat(regional_anova_name_str,"_uneqvar");
                        }
                    if(space_str)strcat(regional_anova_name_str,space_str);
                    for(j=0;j<how_many;j++) temp_float[j]=0.;
                    if(lc_test_type[t]==3||lc_test_type[t]==4) {
                        for(j=0;j<lenbrain;j++) temp_float[ms->brnidx[j]] = (float)r[j];
                        sprintf(string_t,"%s_corcoeff%s%s",string,regional_anova_name_str,Fileext[filetype]);
                        w1->temp_double=r;
                        if(!write1(string_t,w1))exit(-1);
                        printf("Correlation coefficient written to %s\n",string_t);
                        }
                    if(lc_T_uncorrected || lc_T_monte_carlo) {
                        for(j=0;j<num_voxels_lt2subjects;j++) tstat[lt2subjects_voxels_idx[j]] = (double)UNSAMPLED_VOXEL;
                        sprintf(string_t,"%s_tstat%s%s",string,regional_anova_name_str,Fileext[filetype]);
                        }
                    if(lc_T_uncorrected) {
                        for(j=0;j<lenbrain;j++) temp_float[ms->brnidx[j]] = (float)tstat[j];
                        w1->temp_double=tstat;
                        if(!write1(string_t,w1))exit(-1);
                        printf("T statistics written to %s\n",string_t);
                        }
                    if(lc_Z_uncorrected || lc_Z_monte_carlo) {
                        for(j=0;j<num_voxels_lt2subjects;j++) zstat[lt2subjects_voxels_idx[j]] = (double)UNSAMPLED_VOXEL;
                        sprintf(string_z,"%s_zstat%s%s",string,regional_anova_name_str,Fileext[filetype]);
                        if(lc_test_type[t]==0||lc_test_type[t]==1) {
                            for(j=0;j<num_voxels_lt2subjects;j++) avg1[lt2subjects_voxels_idx[j]] = (double)UNSAMPLED_VOXEL;
                            sprintf(avgstr,"%s_avg%s%s",string,regional_anova_name_str,Fileext[filetype]);
                            for(j=0;j<num_voxels_lt2subjects;j++) var1[lt2subjects_voxels_idx[j]] = (double)UNSAMPLED_VOXEL;
                            sprintf(varstr,"%s_var%s%s",string,regional_anova_name_str,Fileext[filetype]);
                            }
                        }
                    if(lc_Z_uncorrected) {
                        for(j=0;j<lenbrain;j++) temp_float[ms->brnidx[j]] = (float)zstat[j];
                        w1->temp_double=zstat;
                        if(!write1(string_z,w1))exit(-1);
                        printf("Z statistics written to %s\n",string_z);

                        #if 0
                        //START170323
                        if(filetype==(int)NIFTI){
                            sprintf(string_z0,"%s_zstat%s.4dfp.img",string,regional_anova_name_str);
                            w1->filetype=(int)IMG;
                            if(!write1(string_z,w1))exit(-1);
                            printf("Z statistics written to %s\n",string_z0);
                            w1->filetype=(int)NIFTI;
                            }
                        #endif


                        if(lc_test_type[t]==0||lc_test_type[t]==1) {
                            for(j=0;j<lenbrain;j++) temp_float[ms->brnidx[j]] = (float)avg1[j];
                            w1->temp_double=avg1;
                            if(!write1(avgstr,w1))exit(-1);
                            printf("Numerator of T test written to %s\n",avgstr);
                            for(j=0;j<lenbrain;j++) temp_float[ms->brnidx[j]] = (float)var1[j];
                            w1->temp_double=var1;
                            if(!write1(varstr,w1))exit(-1);
                            printf("Variance of T test written to %s\n",varstr);
                            }
                        }
                    if((lc_Z_monte_carlo||lc_T_monte_carlo)&&filetype==(int)IMG) {
                        if(!monte_carlo_mult_comp(zstat,threshold,extent,n_threshold_extent,string_z,ifh,mode,tstat,string_t,swapbytes,
                            pstr,lc_test_type[t]==0?avg1:(double*)NULL,avgstr,mcmc,ap))exit(-1);
                        }
                    if(lc_T_uncorrected || lc_T_monte_carlo) {
                        for(j=0;j<lenbrain;j++) temp_float[ms->brnidx[j]] = (float)dferror_stack[j];
                        sprintf(string_t,"%s_df%s%s",string,regional_anova_name_str,Fileext[filetype]);
                        w1->temp_double=dferror_stack;
                        if(!write1(string_t,w1))exit(-1);
                        printf("Degrees of freedom written to %s\n",string_t);
                        }
                    }
                else {
                    if(!nharolds_num) {
                        fprintf(fprn,"%s %s df = %d\n",td->type[t],td->name[t],df[t]);
                        if(lc_test_type[t] == 2) {
                            i==0 ? fprintf(fprn,"EQUAL VARIANCES\n") : fprintf(fprn,"UNEQUAL VARIANCES\n");
                            }
                        strcpy(string,"REGION");
                        len = region_names?region_names->max_length+6:0;
                        fprintf(fprn,"%-*sT        Z     p (two-tailed)\n",len,len?"REGION":"");
                        for(k=0;k<len;k++) fprintf(fprn,"=");
                        fprintf(fprn,"=============================\n");
                        if(region_names) {
                            for(j=0;j<lenvol;j++) {

                                #if 0
                                fprintf(fprn,"%s%*.4f %8.4f %g\n",region_names->files[j],(int)(region_names->max_length-
                                    region_names->strlen_files[j]+9),tstat[j],zstat[j],gsl_cdf_tdist_Q(tstat[j],
                                    dferror_stack[j]));
                                #endif
                                //START160518
                                fprintf(fprn,"%s%*.4f %8.4f %g\n",region_names->files[j],(int)(region_names->max_length-
                                    region_names->strlen_files[j]+9),tstat[j],zstat[j],tstat[j]>0.?
                                    gsl_cdf_tdist_Q(tstat[j],dferror_stack[j]):gsl_cdf_tdist_P(tstat[j],dferror_stack[j]));

                                }
                            }
                        else {
                            for(j=0;j<lenvol;j++) {

                                //fprintf(fprn,"%.4f %8.4f %g\n",tstat[j],zstat[j],gsl_cdf_tdist_Q(tstat[j],dferror_stack[j]));
                                //START160518
                                fprintf(fprn,"%.4f %8.4f %g\n",tstat[j],zstat[j],tstat[j]>0.?
                                    gsl_cdf_tdist_Q(tstat[j],dferror_stack[j]):gsl_cdf_tdist_P(tstat[j],dferror_stack[j]));

                                }
                            }
                        fprintf(fprn,"\n\n");
                        }
                    else {
                        fprintf(fprn,"%s %s\n",td->type[t],td->name[t]);
                        if(lc_test_type[t] == 2) {
                            i==0 ? fprintf(fprn,"EQUAL VARIANCES\n") : fprintf(fprn,"UNEQUAL VARIANCES\n");
                            }
                        if(lc_test_type[t] >= 3) {
                            strcpy(string,"REGION");
                            if(dferror_stack[0]<10.) k=1;
                            else if(dferror_stack[0]<100.) k=2;
                            else if(dferror_stack[0]<1000.) k=3; 
                            else k=4;
                            fprintf(fprn,"%-*sr        T(df=%d)%*s       p(two-tailed)\n",(int)(region_names->max_length+6),string,
                                (int)dferror_stack[0],k,"Z");
                            for(i1=0;i1<region_names->max_length+6;i1++)fprintf(fprn,"=");
                            fprintf(fprn,"=======================================\n");
                            for(j=0;j<lenvol;j++) {
                                if(nsubjects1[j]) {

                                    #if 0
                                    fprintf(fprn,"%-3d %s%*.4f %8.4f %8.4f  %g\n",harolds_num[j],region_names->files[j],
                                        (int)(region_names->max_length-region_names->strlen_files[j]+9),r[j],tstat[j],zstat[j],
                                        gsl_cdf_tdist_Q(tstat[j],dferror_stack[j]));
                                    #endif
                                    //START160518
                                    fprintf(fprn,"%-3d %s%*.4f %8.4f %8.4f  %g\n",harolds_num[j],region_names->files[j],
                                        (int)(region_names->max_length-region_names->strlen_files[j]+9),r[j],tstat[j],zstat[j],
                                        tstat[j]>0.?gsl_cdf_tdist_Q(tstat[j],dferror_stack[j]):
                                        gsl_cdf_tdist_P(tstat[j],dferror_stack[j]));

                                    }
                                else {
                                    fprintf(fprn,"%-3d %s   Region has zero voxels.\n",harolds_num[j],region_names->files[j]);
                                    }
                                }
                            fprintf(fprn,"\n\n");
                            }
                        else {
                            strcpy(string,"REGION");

                            #if 0
                            fprintf(fprn,"%-*smean     sd       T        Z        p (two-tailed)\n",
                                (int)(region_names->max_length+6),string);
                            #endif
                            //START160518
                            fprintf(fprn,"%-*smean     sem      T        Z        p (two-tailed)\n",
                                (int)(region_names->max_length+6),string);

                            for(i1=0;i1<region_names->max_length+6;i1++) fprintf(fprn,"=");
                            fprintf(fprn,"===================================================\n");
                            for(j=0;j<lenvol;j++) {

                                #if 0
                                fprintf(fprn,"%-3d %s%*.4f %8.4f %8.4f %8.4f %g\n",harolds_num[j],region_names->files[j],
                                    (int)(region_names->max_length-region_names->strlen_files[j]+9),avg1[j],sqrt(var1[j]),tstat[j],
                                    zstat[j],gsl_cdf_tdist_Q(tstat[j],dferror_stack[j]));
                                #endif
                                //START160518
                                fprintf(fprn,"%-3d %s%*.4f %8.4f %8.4f %8.4f %g\n",harolds_num[j],region_names->files[j],
                                    (int)(region_names->max_length-region_names->strlen_files[j]+9),avg1[j],sem[j],tstat[j],
                                    zstat[j],tstat[j]>0.?gsl_cdf_tdist_Q(tstat[j],dferror_stack[j]):
                                    gsl_cdf_tdist_P(tstat[j],dferror_stack[j]));


                                }
                            }
                        }
                    if(lc_test_type[t]<2){
                        fprintf(fprn1,"%s",td->name[t]);
                        for(j=0;j<lenvol;j++)fprintf(fprn1,"\t%.6f %.6f",avg1[j],sem[j]);
                        fprintf(fprn1,"\n");
                        }

                    }
                }
            }
        if(n4dfps) {
            for(i=0;i<td->nfiles1[t];i++) unmap_disk(mm1[i]); 
            if(lc_test_type[t] < 3) for(i=0;i<td->nfiles2[t];i++) unmap_disk(mm2[i]); 
            }
        f1index += td->nfiles1[t];
        f2index += td->nfiles2[t];
        #ifndef MONTE_CARLO
            fprintf(stdout,"\n");
        #endif
        if(lc_test_type[t] == 4) free_d2double(cov_var);

        //START170418
        if(fprn2){
            fprintf(fprn2,"%s\n",td->name[t]);
            for(i=0;i<td->nfiles1[t];i++){
                fprintf(fprn2,"REPLACE");
                for(j=0;j<lenvol;j++)
                    if(ind[i][j]!=(float)UNSAMPLED_VOXEL){fprintf(fprn2,"\t%f",ind[i][j]);}else{fprintf(fprn2,"\tNA");}
                fprintf(fprn2,"\n");
                }
            } 

        } /*for(fiindex=f2index=f1=f2=files_ptr_index=t=0;t<td->ntests;t++)*/
    } /*if(!lccleanup_only)*/
if(fprn) {
    fclose(fprn);
    printf("T-test regional output written to %s\n",regional_anova_name_str);
    }
if(fprn1){
    fclose(fprn1);
    printf("T-test regional output written to %s\n",meansemstr);
    }

//START170418
if(fprn2){
    fclose(fprn2);
    printf("T-test regional output written to %s\n",indstr);
    }

if(lccleanup||lccleanup_only) {
    if(scratchdir) {
        sprintf(string,"rm -r %s",scratchdir);
        if(system(string) == -1) printf("Error: unable to %s\n",string);
        }
    else {
        delete_scratch_files(files_ptr,n4dfps); 
        }
    }
}
