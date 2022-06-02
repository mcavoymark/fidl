/* copyright 08/05/01 Washington University.  All Rights Reserved.
   fidl_fano.c  $Revision: 1.23 $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_multimin.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_errno.h>

/*#ifndef LINUX*/
#ifdef __sun__

    #include <nan.h>
#endif

static char rcsid[] = "$Header: /home/hannah/mcavoy/idl/clib/RCS/fidl_fano.c,v 1.23 2010/07/20 18:31:15 mcavoy Exp $";

/*double my_f(const gsl_vector *xx,void *params);*/
double fano_f(const gsl_vector *xx,void *params);
double cv_f(const gsl_vector *xx,void *params);

main(int argc,char **argv)
{
char *event_file=NULL,*conc_file,*frames_file=NULL,*mask_file=NULL,*regional_name=NULL,*xform_file=NULL,*scratchdir,string[MAXNAME],
    *strptr,filename[MAXNAME],*root=NULL,*strptr1,*strptr2,string2[MAXNAME];
int i,j,k,l,kk,ll,lll,n,num_tc_names=0,num_tc_cond=0,argc_tc_cond,ntc_file=0,nvar_file=0,num_region_files=0,num_regions=0,*roi,
    lcscratch=1,lccleanup=0,SunOS_Linux,num_bold_files=0,A_or_B_or_U,atlas,*nframes,lengthX,*sindex,num_wfiles=1,*neach_tc_cond,
    *y,modulo=20000,ncol=2,nparam,status,bigendian=1,swapbytesout,flag=0,maxiter,*sbmean,*sbvar,idx,argc_tc,num_tc=0,lccoef_of_var=0,
    lccat=0,nfiles;
size_t iter,*ieach_tc_cond;
float *t4,*temp_float,*temp_float2,*tfptr,*tf2ptr;
double *weights,*Astack,*dptr,*dptr2,*dptr3,*bstack,bsum,tol,size,*xxstack,*fanovar,*fanofactor,*fanomeans,td;
FILE *fp;
Files_Struct *tc_names,*tc_file,*var_file,*region_files,*bold_files,*wfiles=NULL;
TC *tc_cond,*tcs;
FS *fs;
Dim_Param *dp,*dpmean,*dpvar;
Atlas_Param *ap;
Regions **reg;
Regions_By_File *rbf=NULL,*fbf;
Mask_Struct *ms;
Scratch *scratch;
Files_Struct_new *sn;
Memory_Map **mmmean,**mmvar;
Interfile_header *ifh;

if(argc < 13) {
    fprintf(stderr,"        -tc_names:            Timcourse identifiers.\n");
    fprintf(stderr,"        -tc_cond:             Identifier from second column of event file.\n");
    fprintf(stderr,"        -event_file           Used for documentation.\n");
    fprintf(stderr,"        -bold_files           BOLD data.\n");
    fprintf(stderr,"        -conc_file            BOLD data.\n");
    fprintf(stderr,"        -cond_and_frames      File identifying which frames to extract from the conc.\n");
    fprintf(stderr,"                              Identifier from second column of event file, frames.\n");
    fprintf(stderr,"                              Need not be comma separated, but easier on the eyes.\n");
    fprintf(stderr,"        -mask:                Only voxels in the mask are analyzed.\n");
    fprintf(stderr,"        -tc_file:             Extracted timecourse files. One for each -tc_names\n");
    fprintf(stderr,"        -var_file:            Residual variance files. One for each -tc_names\n");
    fprintf(stderr,"                              The square root is taken with the use of -coef_of_var\n");
    fprintf(stderr,"        -region_file:         *.4dfp.img file(s) that specifies regions of interest.\n");
    fprintf(stderr,"                              Timecourses are averaged over the region.\n");
    fprintf(stderr,"        -regions_of_interest: Compute timecourses for selected regions in the region file(s).\n");
    fprintf(stderr,"                              First region is one.\n");
    fprintf(stderr,"        -regional_name:       Output filename for regional timecourses. Default is fidl_tc_ss.txt\n");
    fprintf(stderr,"        -xform_file:          Name of t4 file defining the transform to atlas space.\n");
    fprintf(stderr,"        -scratchdir:          Location of precomputed scratchfiles. Include the backslash at the end.\n");
    fprintf(stderr,"        -clean_up             Delete the scratch files and directory.\n");
    fprintf(stderr,"        -littleendian         Write output in little endian. Big endian is the default.\n");
    fprintf(stderr,"        -coef_of_var          Estimate coefficient of variation instead of the fano factor.\n");
    fprintf(stderr,"        -concatenate          Concatenate tc and var files to match event file.\n");
    fprintf(stderr,"        -root                 OUtput root.\n");
    exit(-1);
    }
for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-tc_names") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_tc_names;
        if(!(tc_names=get_files(num_tc_names,&argv[i+1]))) exit(-1);
        i += num_tc_names;
        }
    if(!strcmp(argv[i],"-tc_cond") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_tc_cond;
        argc_tc_cond = i+1;
        i += num_tc_cond;
        }
    if(!strcmp(argv[i],"-tc") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_tc;
        argc_tc = i+1;
        i += num_tc;
        }
    if(!strcmp(argv[i],"-event_file") && argc > i+1)
        event_file = argv[++i];
    if(!strcmp(argv[i],"-bold_files") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_bold_files;
        if(!(bold_files=get_files(num_bold_files,&argv[i+1]))) exit(-1);
        i += num_bold_files;
        }
    if(!strcmp(argv[i],"-conc_file") && argc > i+1)
        conc_file = argv[++i];
    if(!strcmp(argv[i],"-cond_and_frames") && argc > i+1)
        frames_file = argv[++i];
    if(!strcmp(argv[i],"-mask") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        mask_file = argv[++i];
    if(!strcmp(argv[i],"-tc_file") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++ntc_file;
        if(!(tc_file=get_files(ntc_file,&argv[i+1]))) exit(-1);
        i += ntc_file;
        }
    if(!strcmp(argv[i],"-var_file") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nvar_file;
        if(!(var_file=get_files(nvar_file,&argv[i+1]))) exit(-1);
        i += nvar_file;
        }
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
    if(!strcmp(argv[i],"-regional_name") && argc > i+1)
        regional_name = argv[++i];
    if(!strcmp(argv[i],"-xform_file") && argc > i+1)
        xform_file = argv[++i];
    if(!strcmp(argv[i],"-scratchdir") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1]) {
        scratchdir = argv[++i];
        lcscratch = 0;
        }
    if(!strcmp(argv[i],"-clean_up"))
        lccleanup = 1;
    if(!strcmp(argv[i],"-littleendian"))
        bigendian = 0;
    if(!strcmp(argv[i],"-coef_of_var"))
        lccoef_of_var = 1;
    if(!strcmp(argv[i],"-concatenate"))
        lccat = 1;
    if(!strcmp(argv[i],"-root") && argc > i+1)
        root = argv[++i];
    }
print_version_number(rcsid,stdout);
if((SunOS_Linux=checkOS())==-1) exit(-1);
if(!ntc_file) {
    printf("Error: Need to specify extracted timecourse file(s) with -tc_file\n");
    exit(-1);
    }
if(!nvar_file) {
    printf("Error: Need to specify residual variance file(s) with -var_file\n");
    exit(-1);
    }
if(ntc_file!=nvar_file) {
    printf("Error: ntc_file=%d nvar_file=%d  Must be equal. Abort!\n",ntc_file,nvar_file);
    exit(-1);
    }
if(!frames_file) {
    printf("Error: Need to specify -cond_and_frames file.\n");
    exit(-1);
    }
if(!num_tc_names) {
    printf("Error: Need to specify -tc_names\n");
    exit(-1);
    }
if(!num_tc_cond) {
    printf("Error: Need to specify -tc_cond\n");
    exit(-1);
    }
if(num_tc_names!=num_tc_cond) {
    printf("Error: num_tc_names=%d num_tc_cond=%d  Must be equal. Abort!\n",num_tc_names,num_tc_cond);
    exit(-1);
    }
if(ntc_file!=num_tc_cond) {
    printf("Error: ntc_file=%d num_tc_cond=%d  Must be equal. Abort!\n",ntc_file,num_tc_cond);
    exit(-1);
    }
if(!num_bold_files) {
    if(!conc_file) {
        printf("Error: No bold files. Abort!\n");
        exit(-1);
        }
    else {
        if(!(bold_files=read_conc(conc_file))) exit(-1);
        }
    }
if(lcscratch) if(!(scratchdir=make_scratchdir())) exit(-1);
if(!(tc_cond=read_tc_string_new(num_tc_cond,(int*)NULL,argc_tc_cond,argv))) exit(-1);
#if 1
    printf("num_tc_cond=%d\n",num_tc_cond);
    printf("tc_cond->num_tc=%d\n",tc_cond->num_tc);
    printf("tc_cond->num_tc_to_sum="); for(i=0;i<tc_cond->num_tc;i++) printf("%d ",tc_cond->num_tc_to_sum[i][0]); printf("\n");
    printf("tc_cond->tc=\n");
    for(i=0;i<tc_cond->num_tc;i++) {
        for(k=0;k<tc_cond->each[i];k++) {
            for(j=0;j<tc_cond->num_tc_to_sum[i][k];j++) printf("%d ",(int)tc_cond->tc[i][k][j]);
            printf("    ");
            }
        printf("\n");
        }
#endif

if(!(tcs=read_tc_string_new(num_tc,(int*)NULL,argc_tc,argv))) exit(-1);
#if 1
    printf("num_tc=%d\n",num_tc);
    printf("tcs->num_tc=%d\n",tcs->num_tc);
    printf("tcs->num_tc_to_sum="); for(i=0;i<tcs->num_tc;i++) printf("%d ",tcs->num_tc_to_sum[i][0]); printf("\n");
    printf("tcs->tc=\n");
    for(i=0;i<tcs->num_tc;i++) {
        for(k=0;k<tcs->each[i];k++) {
            for(j=0;j<tcs->num_tc_to_sum[i][k];j++) printf("%d ",(int)tcs->tc[i][k][j]);
            printf("    ");
            }
        printf("\n");
        }
#endif

if(!(fs=read_frames_file(frames_file,1,0,0))) exit(-1);
if(!(nframes=malloc(sizeof*nframes*num_tc_cond))) {
    printf("Error: Unable to malloc nframes\n");
    exit(-1);
    }
for(i=0;i<tc_cond->num_tc;i++) nframes[i]=0;
if(!(neach_tc_cond=malloc(sizeof*neach_tc_cond*tc_cond->num_tc))) {
    printf("Error: Unable to malloc neach_tc_cond\n");
    exit(-1);
    }
for(i=0;i<tc_cond->num_tc;i++) neach_tc_cond[i]=0;
if(!(y=malloc(sizeof*y*fs->nlines))) {
    printf("Error: Unable to malloc y\n");
    exit(-1);
    }
for(i=0;i<fs->nlines;i++) {
    for(j=0;j<tc_cond->num_tc;j++) {
        for(k=0;k<tc_cond->each[j];k++) {
            if(fs->frames_cond[i] == tc_cond->tc[j][k][0]) {
                if(nframes[j] < fs->frames_per_line[i]) nframes[j] = fs->frames_per_line[i];
                y[i]=j;
                neach_tc_cond[j]++;
                break;
                }
            }
        }
    }
for(lengthX=nframes[0],j=0,i=1;i<tc_cond->num_tc;i++) if(nframes[i]!=lengthX) j=1;
if(j) {
    for(i=0;i<tc_cond->num_tc;i++) {
        printf("Error: tc_cond %s has %d timepoints.\n", argv[argc_tc_cond+i],nframes[i]);
        printf("Error: All must be equal. Abort!\n");
        exit(-1);
        }
    }
printf("y="); for(i=0;i<fs->nlines;i++) printf("%d ",y[i]); printf("\n");
printf("neach_tc_cond="); for(i=0;i<tc_cond->num_tc;i++) printf("%d ",neach_tc_cond[i]); printf("\n");
printf("tc_cond="); for(i=0;i<num_tc_cond;i++) printf("%s ",argv[argc_tc_cond+i]); printf("\n");
printf("nframes="); for(i=0;i<num_tc_cond;i++) printf("%d ",nframes[i]); printf("\n");


/*START39*/
if(!(ieach_tc_cond=malloc(sizeof*ieach_tc_cond*fs->nlines))) {
    printf("Error: Unable to malloc ieach_tc_cond\n");
    exit(-1);
    }
for(k=j=0;j<tc_cond->num_tc;j++) {
    for(l=i=0;i<fs->nlines;i++) if(y[i]==j) {ieach_tc_cond[k++]=i;l++;} 
    if(l!=neach_tc_cond[j]) {
        printf("Error: l=%d neach_tc_cond[%d]=%d  Must be equal.\n",l,j,neach_tc_cond[j]);
        exit(-1);
        }
    }



#if 0
for(i=0;i<tcs->num_tc;i++) {
    if(lengthX!=tcs->each[i]) {
        printf("Error: lengthX=%d tcs->each[%d]=%d\n",lengthX,i,tcs->each[i]);
        exit(-1);
        }
    }
#endif
/*START39*/
if(!lccat) {
    for(i=0;i<tcs->num_tc;i++) {
        if(lengthX!=tcs->each[i]) {
            printf("Error: lengthX=%d tcs->each[%d]=%d\n",lengthX,i,tcs->each[i]);
            exit(-1);
            }
        }
    }
else {
    if(tcs->num_tc!=2) {
        printf("Error: tcs->num_tc=%d For -concatenate, tcs->num_tc must be 2.\n",tcs->num_tc);
        exit(-1);
        }
    if(lengthX!=(tcs->each[0]+tcs->each[1])) {
        printf("Error: lengthX=%d tcs->each[0]=%d tcs->each[1]=%d  tcs->each must sum to lengthX\n",lengthX,tcs->each[0],
            tcs->each[1]);
        exit(-1);
        }
    }



if(tcs->num_tc!=tc_file->nfiles) {
    printf("Error: tcs->num_tc=%d tc_file->nfiles=%d  Must be equal.\n",tcs->num_tc,tc_file->nfiles);
    exit(-1);
    }

if(!(sindex=malloc(sizeof*sindex*fs->nlines))) {
    printf("Error: Unable to malloc sindex\n");
    exit(-1);
    }
for(i=0;i<fs->nlines;i++) sindex[i]=0;
if(!(dp=dim_param(bold_files->nfiles,bold_files->files,SunOS_Linux))) exit(-1);
if(xform_file) {
    if(!(t4=malloc(sizeof*t4*(size_t)T4SIZE))) {
        printf("Error: Unable to malloc t4\n");
        exit(-1);
        }
    if(!read_xform(xform_file,t4)) exit(-1);
    if((A_or_B_or_U=twoA_or_twoB(xform_file)) == 2) {
        printf("Error: Illegally named t4 file.\n");
        exit(-1);
        }
    atlas = 222;
    }
else {
    atlas = get_atlas(dp->vol);
    }
if(!(ap=get_atlas_param(atlas,(Interfile_header*)NULL))) exit(-1);
if(!xform_file) ap->vol = dp->vol;
if(!atlas) {
    ap->xdim = dp->xdim;
    ap->ydim = dp->ydim;
    ap->zdim = dp->zdim;
    ap->voxel_size[0] = dp->dxdy;
    ap->voxel_size[1] = dp->dxdy;
    ap->voxel_size[2] = dp->dz;
    }
if(num_region_files) {
    if(!check_dimensions(num_region_files,region_files->files,ap->vol)) exit(-1);
    if(!(reg=malloc(sizeof*reg*num_region_files))) {
        printf("Error: Unable to malloc reg\n");
        exit(-1);
        }
    for(i=0;i<num_region_files;i++) if(!(reg[i] = extract_regions(region_files->files[i],0,0,(float*)NULL,0,SunOS_Linux,
        (char**)NULL))) exit(-1);
    if(!num_regions) for(i=0;i<num_region_files;i++) num_regions += reg[i]->nregions;
    if(!(rbf=find_regions_by_file_cover(num_region_files,num_regions,reg,roi))) exit(-1);
    if(!(weights=malloc(sizeof*weights*ap->vol))) {
        printf("Error: Unable to malloc weights\n");
        exit(-1);
        }
    for(i=0;i<ap->vol;i++) weights[i] = 1;
    }
else {
    if(!(ms=get_mask_struct(mask_file,ap->vol,(int*)NULL,SunOS_Linux,(LinearModel*)NULL))) exit(-1);
    num_regions = ms->lenbrain;
    }
printf("num_regions=%d ap->vol=%d fs->nlines=%d\n",num_regions,ap->vol,fs->nlines);
if(!(scratch=malloc(sizeof*scratch))) {
    printf("Error: Unable to malloc scratch\n");
    exit(-1);
    }

if(!(fbf=find_regions_by_file(bold_files->nfiles,fs->nframes,dp->tdim,fs->frames))) exit(-1);
if(!(sn=create_scratchnames(bold_files,fbf,scratchdir,"_fano"))) exit(-1);
if(lcscratch) {
    if(!(create_scratchfiles(bold_files,fbf,dp,sn,SunOS_Linux,xform_file,ap,num_wfiles,rbf,num_regions,ms,wfiles,fs->nframes,0.)))
        exit(-1);
    }

if(!(dpmean=dim_param(tc_file->nfiles,tc_file->files,SunOS_Linux))) exit(-1);
if(!(dpvar=dim_param(var_file->nfiles,var_file->files,SunOS_Linux))) exit(-1);
if(dpmean->vol!=dpvar->vol) {
    printf("Error: dpmean->vol=%d dpvar->vol=%d  Must be equal. Abort!\n",dpmean->vol,dpvar->vol);
    exit(-1);
    }
if(dpmean->vol!=dp->vol) {
    printf("Error: dpmean->vol=%d dp->vol=%d  Must be equal. Abort!\n",dpmean->vol,dp->vol);
    exit(-1);
    }
if(!dpmean->all_tdim_same) {
    printf("Error: dpmean->all_tdim_same=%d  Must be 1. Abort!\n",dpmean->all_tdim_same);
    exit(-1);
    }
if(!dpvar->all_tdim_same) {
    printf("Error: dpvar->all_tdim_same=%d  Must be 1. Abort!\n",dpvar->all_tdim_same);
    exit(-1);
    }
if(!(mmmean=malloc(sizeof*mmmean*tc_file->nfiles))) {
    printf("Error: Unable to malloc mmmean\n");
    exit(-1);
    }
if(!(mmvar=malloc(sizeof*mmvar*tc_file->nfiles))) {
    printf("Error: Unable to malloc mmvar\n");
    exit(-1);
    }
if(!(sbmean=malloc(sizeof*sbmean*tc_file->nfiles))) {
    printf("Error: Unable to malloc sbmean\n");
    exit(-1);
    }
if(!(sbvar=malloc(sizeof*sbvar*tc_file->nfiles))) {
    printf("Error: Unable to malloc sbvar\n");
    exit(-1);
    }

/*START41*/
if(lccat) {
    if((dpmean->tdim[0]+dpmean->tdim[1])!=lengthX) {
        printf("Error: dpmean->tdim[0]=%d dpmean->tdim[1]=%d lengthX=%d  Must sum to lengthX\n",dpmean->tdim[0],dpmean->tdim[1]);
        exit(-1);
        }
    if((dpvar->tdim[0]+dpvar->tdim[1])!=lengthX) {
        printf("Error: dpvar->tdim[0]=%d dpvar->tdim[1]=%d lengthX=%d  Must sum to lengthX\n",dpvar->tdim[0],dpvar->tdim[1]);
        exit(-1);
        }
    }

for(i=0;i<tc_file->nfiles;i++) {

    #if 0
    if(dpmean->tdim[i]<lengthX) {
        printf("Error: dpmean->tdim[%d]=%d lengthX=%d  Must be greater than or equal. Abort!\n",i,dpmean->tdim[i],lengthX);
        exit(-1);
        }
    if(dpvar->tdim_max<lengthX) {
        printf("Error: dpvar->tdim[%d]=%d lengthX=%d  Must be greater than or equal. Abort!\n",dpvar->tdim[i],lengthX);
        exit(-1);
        }
    #endif
    /*START41*/
    if(!lccat) {
        if(dpmean->tdim[i]<lengthX) {
            printf("Error: dpmean->tdim[%d]=%d lengthX=%d  Must be greater than or equal. Abort!\n",i,dpmean->tdim[i],lengthX);
            exit(-1);
            }
        if(dpvar->tdim[i]<lengthX) {
            printf("Error: dpvar->tdim[%d]=%d lengthX=%d  Must be greater than or equal. Abort!\n",dpvar->tdim[i],lengthX);
            exit(-1);
            }
        }



    if(!(mmmean[i] = map_disk(tc_file->files[i],dp->vol*dpmean->tdim[i],0))) exit(-1);
    if(!(mmvar[i] = map_disk(var_file->files[i],dp->vol*dpvar->tdim[i],0))) exit(-1);
    sbmean[i] = shouldiswap(SunOS_Linux,dpmean->bigendian[i]);
    sbvar[i] = shouldiswap(SunOS_Linux,dpvar->bigendian[i]);
    }
if(!(Astack=malloc(sizeof*Astack*lengthX*ncol))) {
    printf("Error: Unable to malloc Astack\n");
    exit(-1);
    }
if(!(bstack=malloc(sizeof*bstack*lengthX))) {
    printf("Error: Unable to malloc bstack\n");
    exit(-1);
    }
gsl_matrix_view m = gsl_matrix_view_array(Astack,lengthX,ncol);
gsl_vector_view b = gsl_vector_view_array(bstack,lengthX);
gsl_vector *tau = gsl_vector_alloc(ncol);
gsl_vector *x = gsl_vector_alloc(ncol);
gsl_vector *residual = gsl_vector_alloc(lengthX);

/*gsl_matrix *X = gsl_matrix_alloc(lengthX,fs->nlines);*/
/*START39*/
gsl_matrix *X1 = gsl_matrix_alloc(lengthX,fs->nlines);
gsl_matrix *X;

nparam = ncol+lengthX;
maxiter = 200*nparam;

if(!(xxstack=malloc(sizeof*xxstack*nparam))) {
    printf("Error: Unable to malloc xxstack\n");
    exit(-1);
    }
gsl_vector_view xx = gsl_vector_view_array(xxstack,nparam);

const gsl_multimin_fminimizer_type *T = gsl_multimin_fminimizer_nmsimplex;
gsl_multimin_fminimizer *s = NULL;
gsl_vector *ss = gsl_vector_alloc(nparam);

gsl_multimin_function minex_func;
minex_func.n = nparam;

/*minex_func.f = &my_f;*/
minex_func.f = lccoef_of_var ? &cv_f : &fano_f;

s = gsl_multimin_fminimizer_alloc(T,nparam);

if(!(fanovar=malloc(sizeof*fanovar*num_regions*tc_file->nfiles))) {
    printf("Error: Unable to malloc fanovar\n");
    exit(-1);
    }
for(i=0;i<num_regions*tc_file->nfiles;i++) fanovar[i]=(double)UNSAMPLED_VOXEL;
if(!(fanofactor=malloc(sizeof*fanofactor*num_regions*tc_file->nfiles))) {
    printf("Error: Unable to malloc fanofactor\n");
    exit(-1);
    }
for(i=0;i<num_regions*tc_file->nfiles;i++) fanofactor[i]=(double)UNSAMPLED_VOXEL;
if(!(fanomeans=malloc(sizeof*fanomeans*num_regions*lengthX*tc_file->nfiles))) {
    printf("Error: Unable to malloc fanomeans\n");
    exit(-1);
    }
for(i=0;i<num_regions*lengthX*tc_file->nfiles;i++) fanomeans[i]=(double)UNSAMPLED_VOXEL;
if(!(temp_float=malloc(sizeof*temp_float*ap->vol))) {
    printf("Error: Unable to malloc temp_float\n");
    exit(-1);
    }
if(!(temp_float2=malloc(sizeof*temp_float2*lengthX))) {
    printf("Error: Unable to malloc temp_float2\n");
    exit(-1);
    }
tol=lengthX*(double)UNSAMPLED_VOXEL;
gsl_set_error_handler_off();



if(!lccat) {
    for(j=0;j<num_regions;j++) {
        if(!(j%modulo)) fprintf(stdout,"Processing voxel %d\n",j);
        for(flag=n=lll=ll=l=0;l<tc_file->nfiles;l++,ll+=num_regions,lll+=num_regions*lengthX) {

            /*for(dptr=Astack,k=0;k<lengthX;k++,dptr+=2) *dptr=1.;*/
            /*START33*/
            for(dptr=Astack,k=0;k<lengthX;k++,dptr+=ncol) *dptr=1.;

            for(k=0;k<lengthX;k++) {
                idx = ms->brnidx[j]+dp->vol*((int)tcs->tc[l][k][0]-1);
                temp_float[k] = mmmean[l]->ptr[idx];
                temp_float2[k] = mmvar[l]->ptr[idx];
                }
            if(sbmean[l]) swap_bytes((unsigned char *)temp_float,sizeof(float),(size_t)lengthX);
            if(sbvar[l]) swap_bytes((unsigned char *)temp_float2,sizeof(float),(size_t)lengthX);
            for(bsum=0.,dptr=Astack,dptr++,k=0;k<lengthX;k++,dptr+=2) {
                if(temp_float[k]==(float)UNSAMPLED_VOXEL || temp_float2[k]==(float)UNSAMPLED_VOXEL) {
                    bsum=0.;
                    break;
                    }
                *dptr = (double)temp_float[k];
                bsum += bstack[k] = lccoef_of_var ? sqrt((double)temp_float2[k]) : (double)temp_float2[k];
                }
            if(bsum>tol) {
                gsl_linalg_QR_decomp(&m.matrix,tau);
                gsl_linalg_QR_lssolve(&m.matrix,tau,&b.vector,x,residual);
                if(!flag) {
                    flag=1;
                    assignXgsl(bold_files->nfiles,fbf->num_regions_by_file,sn,num_regions,num_wfiles,0,j,0,sindex,fs,X1,scratch);
                    }
                if(!scratch->uns_count) {
                    X = gsl_matrix_alloc(lengthX,neach_tc_cond[l]);
                    minex_func.params = (void *)X;
                    for(k=0;k<neach_tc_cond[l];k++,n++) {
                        gsl_matrix_get_col(residual,(const gsl_matrix*)X1,ieach_tc_cond[n]);
                        gsl_matrix_set_col(X,(size_t)k,(const gsl_vector*)residual);
                        }
                    for(dptr2=xxstack,k=0;k<ncol;k++) *dptr2++=gsl_vector_get(x,k);
                    for(dptr=Astack,dptr++,k=0;k<lengthX;k++,dptr+=2) *dptr2++=*dptr;
                    for(dptr=xxstack+2,k=0;k<lengthX;k++,dptr++) {
                        td=xxstack[0]+xxstack[1]*(*dptr);
                        if(td<0.) break;
                        }
                    if(td<0.) continue;
                    gsl_vector_set_all(ss,.1);
                    if(gsl_multimin_fminimizer_set(s,&minex_func,&xx.vector,ss)) continue;
                    iter=0;
                    do {
                        iter++;
                        status = gsl_multimin_fminimizer_iterate(s);
                        if (status) break;
                        size = gsl_multimin_fminimizer_size(s);
                        status = gsl_multimin_test_size(size,1e-3);
                        } while(status == GSL_CONTINUE && iter < maxiter);
                    fanovar[ll+j] = gsl_vector_get(s->x,0);
                    fanofactor[ll+j] = gsl_vector_get(s->x,1);
                    for(kk=lll,i=2,k=0;k<lengthX;k++,i++,kk+=num_regions) fanomeans[kk+j] = gsl_vector_get(s->x,i);
                    gsl_matrix_free(X);
                    }
                }
            }
        }
    }
else {
    gsl_matrix *X = gsl_matrix_alloc(lengthX,fs->nlines);
    minex_func.params = (void *)X;
    for(j=0;j<num_regions;j++) {
        if(!(j%modulo)) fprintf(stdout,"Processing voxel %d\n",j);
        for(dptr=Astack,k=0;k<lengthX;k++,dptr+=2) *dptr=1.;
        for(tfptr=temp_float,tf2ptr=temp_float2,kk=l=0;l<tc_file->nfiles;l++) {
            for(k=0;k<tcs->each[l];k++,kk++) {
                idx = ms->brnidx[j]+dp->vol*((int)tcs->tc[l][k][0]-1);
                temp_float[kk] = mmmean[l]->ptr[idx];
                temp_float2[kk] = mmvar[l]->ptr[idx];
                }
            if(sbmean[l]) swap_bytes((unsigned char *)tfptr,sizeof(float),(size_t)tcs->each[l]);
            if(sbvar[l]) swap_bytes((unsigned char *)tf2ptr,sizeof(float),(size_t)tcs->each[l]);
            tfptr+=tcs->each[l];tf2ptr+=tcs->each[l];
            }
        for(bsum=0.,dptr=Astack,dptr++,k=0;k<lengthX;k++,dptr+=2) {
            if(temp_float[k]==(float)UNSAMPLED_VOXEL || temp_float2[k]==(float)UNSAMPLED_VOXEL) {
                bsum=0.;
                break;
                }
            *dptr = (double)temp_float[k];
            bsum += bstack[k] = lccoef_of_var ? sqrt((double)temp_float2[k]) : (double)temp_float2[k];
            }
        if(bsum>tol) {
            gsl_linalg_QR_decomp(&m.matrix,tau);
            gsl_linalg_QR_lssolve(&m.matrix,tau,&b.vector,x,residual);
            assignXgsl(bold_files->nfiles,fbf->num_regions_by_file,sn,num_regions,num_wfiles,0,j,0,sindex,fs,X,scratch);
            if(!scratch->uns_count) {
                for(n=neach_tc_cond[0],k=0;k<neach_tc_cond[1];k++,n++) {
                    gsl_matrix_get_col(residual,(const gsl_matrix*)X,ieach_tc_cond[n]);
                    dptr = gsl_vector_ptr(residual,(size_t)0);
                    for(l=tcs->each[0],i=0;i<tcs->each[1];i++,l++) gsl_matrix_set(X,(size_t)l,(size_t)ieach_tc_cond[n],*dptr++);
                    for(l=i=0;i<tcs->each[0];i++,l++) gsl_matrix_set(X,(size_t)l,(size_t)ieach_tc_cond[n],*dptr++);
                    }
                for(dptr2=xxstack,k=0;k<ncol;k++) *dptr2++=gsl_vector_get(x,k);
                for(dptr=Astack,dptr++,k=0;k<lengthX;k++,dptr+=2) *dptr2++=*dptr;
                for(dptr=xxstack+2,k=0;k<lengthX;k++,dptr++) {
                    td=xxstack[0]+xxstack[1]*(*dptr);
                    if(td<0.) break;
                    }
                if(td<0.) continue;
                gsl_vector_set_all(ss,.1);
                if(gsl_multimin_fminimizer_set(s,&minex_func,&xx.vector,ss)) continue;
                iter=0;
                do {
                    iter++;
                    status = gsl_multimin_fminimizer_iterate(s);
                    if (status) break;
                    size = gsl_multimin_fminimizer_size(s);
                    status = gsl_multimin_test_size(size,1e-3);
                    } while(status == GSL_CONTINUE && iter < maxiter);
                fflush(stdout);

                /*fanovar[ll+j] = gsl_vector_get(s->x,0);*/
                fanovar[j] = gsl_vector_get(s->x,0);

                /*fanofactor[ll+j] = gsl_vector_get(s->x,1);*/
                fanofactor[j] = gsl_vector_get(s->x,1);


                /*for(kk=lll,i=2,k=0;k<lengthX;k++,i++,kk+=num_regions) fanomeans[kk+j] = gsl_vector_get(s->x,i);*/
                for(kk=0,i=2,k=0;k<lengthX;k++,i++,kk+=num_regions) fanomeans[kk+j] = gsl_vector_get(s->x,i);
                }
            }
        }
    }

nfiles = !lccat ? tc_file->nfiles : 1;
swapbytesout = shouldiswap(SunOS_Linux,bigendian);
if(!(ifh=init_ifh(4,ap->xdim,ap->ydim,ap->zdim,1,ap->voxel_size[0],ap->voxel_size[1],ap->voxel_size[2],bigendian))) exit(-1);
for(j=0;j<ap->vol;j++) temp_float[j]=0.;
for(dptr=fanovar,dptr2=fanofactor,dptr3=fanomeans,lll=ll=l=0;l<nfiles;l++,ll+=num_regions,lll+=num_regions*lengthX) {
    if(!root) {
        strcpy(string,tc_file->files[l]);
        if(!(strptr=get_tail_sans_ext(string))) exit(-1);
        }
    else {
        if(!lccat) {
            strcpy(string,tc_file->files[l]);
            if(!(strptr1=get_tail_sans_ext(string))) exit(-1);
            sprintf(string,"%s_%s",root,strptr1);
            strptr = string;
            }
        else {
            strcpy(string,tc_file->files[0]);
            if(!(strptr1=get_tail_sans_ext(string))) exit(-1);
            strcpy(string2,tc_file->files[1]);
            if(!(strptr2=get_tail_sans_ext(string2))) exit(-1);
            sprintf(string,"%s_%s_%s",root,strptr1,strptr2);
            strptr = string;
            }

        }

    sprintf(filename,"%s_%s.4dfp.img",strptr,lccoef_of_var?"cvsd":"fanovar");
    for(j=0;j<num_regions;j++) temp_float[ms->brnidx[j]] = (float)(*dptr++);
    if(!writestack(filename,temp_float,sizeof(float),(size_t)ap->vol,swapbytesout)) exit(-1);
    min_and_max_init(&ifh->global_min,&ifh->global_max);
    min_and_max_doublestack(&fanovar[ll],num_regions,&ifh->global_min,&ifh->global_max);
    ifh->dim4 = 1;
    if(!write_ifh(filename,ifh,(int)FALSE)) exit(-1);
    printf("Output written to %s\n",filename);

    sprintf(filename,"%s_%s.4dfp.img",strptr,lccoef_of_var?"cv":"fanofactor");
    for(j=0;j<num_regions;j++) temp_float[ms->brnidx[j]] = (float)(*dptr2++);
    if(!writestack(filename,temp_float,sizeof(float),(size_t)ap->vol,swapbytesout)) exit(-1);
    min_and_max_init(&ifh->global_min,&ifh->global_max);
    min_and_max_doublestack(&fanofactor[ll],num_regions,&ifh->global_min,&ifh->global_max);
    ifh->dim4 = 1;
    if(!write_ifh(filename,ifh,(int)FALSE)) exit(-1);
    printf("Output written to %s\n",filename);

    sprintf(filename,"%s_%s.4dfp.img",strptr,lccoef_of_var?"cvmeans":"fanomeans");
    if(!(fp=fopen_sub(filename,"w"))) exit(-1);
    for(k=0;k<lengthX;k++) {
        for(j=0;j<num_regions;j++) temp_float[ms->brnidx[j]] = (float)(*dptr3++);
        if(!fwrite_sub(temp_float,sizeof(float),(size_t)ap->vol,fp,swapbytesout)) exit(-1);
        }
    fclose(fp);
    min_and_max_init(&ifh->global_min,&ifh->global_max);
    min_and_max_doublestack(&fanofactor[lll],num_regions,&ifh->global_min,&ifh->global_max);
    ifh->dim4 = lengthX;
    if(!write_ifh(filename,ifh,(int)FALSE)) exit(-1);
    printf("Output written to %s\n",filename);
    }
}

#if 0
double my_f(const gsl_vector *xx,void *params)
{
    int i,j;
    double logp;
    double mu = gsl_vector_get(xx,0);
    double fano = gsl_vector_get(xx,1);
    const double *means = gsl_vector_const_ptr(xx,2);
    gsl_matrix *X = (gsl_matrix*)params;

    for(logp=0.,i=0;i<X->size2;i++) {
        for(j=0;j<X->size1;j++) {
            logp-=log(gsl_ran_gaussian_pdf(gsl_matrix_get(X,j,i)-means[j],sqrt(mu+fano*means[j])));
            /*printf("gsl_matrix_get(X,%d,%d)=%f mu=%f fano=%f means[j]=%f gsl_ran_gaussian_pdf=%f log=%f logp=%f\n",j,i,
                gsl_matrix_get(X,j,i),mu,fano,means[j],gsl_ran_gaussian_pdf(gsl_matrix_get(X,j,i)-means[j],sqrt(mu+fano*means[j])),
                log(gsl_ran_gaussian_pdf(gsl_matrix_get(X,j,i)-means[j],sqrt(mu+fano*means[j]))),logp);*/

            #ifndef LINUX
                if(IsNANorINF(logp)) {
            #else
                /*if(!isfinite(logp)) {*/  /*undefined reference to `isfinite'*/
                if(isnan(logp) || isinf(logp)) {
            #endif
                    return logp;
                    }
              

            }
        }
    /*printf("logp=%f\n",logp);*/

    return logp;
}
#endif
double fano_f(const gsl_vector *xx,void *params)
{
    int i,j;
    double logp;
    double mu = gsl_vector_get(xx,0);
    double fano = gsl_vector_get(xx,1);
    const double *means = gsl_vector_const_ptr(xx,2);
    gsl_matrix *X = (gsl_matrix*)params;
    for(logp=0.,i=0;i<X->size2;i++) {
        for(j=0;j<X->size1;j++) {
            logp-=log(gsl_ran_gaussian_pdf(gsl_matrix_get(X,j,i)-means[j],sqrt(mu+fano*means[j])));

            /*#ifndef LINUX*/
            #ifdef __sun__

                if(IsNANorINF(logp)) {
            #else
                /*if(!isfinite(logp)) {*/  /*undefined reference to `isfinite'*/
                if(isnan(logp) || isinf(logp)) {
            #endif
                    return logp;
                    }
            }
        }
    return logp;
}
double cv_f(const gsl_vector *xx,void *params)
{
    int i,j;
    double logp;
    double mu = gsl_vector_get(xx,0);
    double fano = gsl_vector_get(xx,1);
    const double *means = gsl_vector_const_ptr(xx,2);
    gsl_matrix *X = (gsl_matrix*)params;
    for(logp=0.,i=0;i<X->size2;i++) {
        for(j=0;j<X->size1;j++) {
            logp-=log(gsl_ran_gaussian_pdf(gsl_matrix_get(X,j,i)-means[j],mu+fano*means[j]));

            /*#ifndef LINUX*/
            #ifdef __sun__

                if(IsNANorINF(logp)) {
            #else
                /*if(!isfinite(logp)) {*/  /*undefined reference to `isfinite'*/
                if(isnan(logp) || isinf(logp)) {
            #endif
                    return logp;
                    }
            }
        }
    return logp;
}
