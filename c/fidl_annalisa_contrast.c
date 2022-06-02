/* Copyright 7/07/09 Washington University.  All Rights Reserved.
   fidl_annalisa_contrast.c  $Revision: 1.5 $*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_cdf.h>
#include <anova_header_new.h>

enum{DATA_SPACE_X = 64,
     DATA_SPACE_Y = 64,
     DATA_SPACE_Z = 16,
     LEN_DATA_SPACE = DATA_SPACE_X*DATA_SPACE_Y*DATA_SPACE_Z};

#define VAR_THRESH .0000000001 /*empirical*/

int read_confile(char *confile,int how_many,char **region_names_ptr,int num_tc_frames,char **tcstrptr,int *tc_frames,
    double *contrasts,int num_tc);

main(int argc,char **argv)
{
char *glm_list_file=NULL,*root=NULL,filename[MAXNAME],*strptr;
int i,j,k,l,m,n,jj,jjj,ll,mm,m1,nn,vol,num_glm_files=0,num_contrasts=0,argc_c,num_tc=0,argc_tc,num_xform_files=0,atlas=222,bigendian=1,
    SunOS_Linux,*A_or_B_or_U,swapbytesout,nL=0,nR=0,nnum=0,nden=0,ntc_frames=0,*tc_frames,tc_frames_max,*fidlcoor,*atlascoor,
    *contrast_nsub,*effect_length,effect_length_total,*temp_int,eff,*avgtc_nsub,col,num_region_files=0,num_regions=0,*roi,volreg,
    *count;
long start_b,offset;
float *temp_float,min,max,sdmin,sdmax,semmin,semmax,*t4,fwhm=0,**time_courses_avg,**time_courses_sd,TR,delay_inc=1,init_delay=0,dt,
      t,*c,**accumulate_mag,smoothness_avg,min_dly,max_dly,min_unscaled,max_unscaled,sdmin_unscaled,sdmax_unscaled,semmin_unscaled,
      semmax_unscaled,*dlymaxmin,**regavg,**regsem,**regavg_see,**regsem_see,min_of,max_of,***accumulate_z,
      **time_courses_avg_unscaled,**time_courses_sd_unscaled,*accumulate_m,*accumulate_sd,minphase_unscaled,maxphase_unscaled,
      **tcavgphase_unscaled;
double *temp_double,*L,*R,*num,*den,*contrast,*idxnum,*idxden,*mags,*summags,*avgtc,mean,*stat,*sum,*sum2,var,var_thresh=VAR_THRESH,*nsub;
FILE *fp;
LinearModel *glm;
Interfile_header *ifh;
TC *tcs;
Atlas_Param *ap;
Files_Struct *glm_files,*xform_files,*region_files;
TCnew *con;
Dim_Param *dp;
Grand_Mean_Struct *gms;
Regions **reg;
Regions_By_File *rbf;
if(argc < 16) {
    fprintf(stderr,"    -glm_files:           *.glm\n");
    fprintf(stderr,"    -contrasts:           Identify conditions used for magnitude and index (ie -num and -den) computation.\n");
    fprintf(stderr,"                          The first contrast is 1.\n");
    fprintf(stderr,"    -tc:                  Identify estimates (ie timecourses) for contrast computation. Two tc, L and R.\n");
    fprintf(stderr,"    -tc_frames:           The first number is the number of frames for the L timecourse. The second number is the number of frames for the R timecourse.\n");
    fprintf(stderr,"    -glm_list_file:       If present this goes into the ifh 'name of data file' field of the output files.\n");
    fprintf(stderr,"    -root:                Output root.\n");
    fprintf(stderr,"    -L:                   Left hemisphere contrast weights.\n");
    fprintf(stderr,"    -R:                   Right hemisphere contrast weights.\n");
    fprintf(stderr,"    -num:                 Numerator index weights.\n");
    fprintf(stderr,"    -den:                 Denominator index weights.\n");
    fprintf(stderr,"    -xform_files:         List of t4 files defining the transform to atlas space.\n");
    fprintf(stderr,"                          If *.glm files are not in atlas space, then one is needed for each *.glm file.\n");
    fprintf(stderr,"    -atlas:               Either 111, 222 or 333. Default is 222. Used with -xform_files option.\n");
    fprintf(stderr,"    -littleendian         Write output in little endian. Big endian is the default.\n");
    fprintf(stderr,"    -region_file:         Fidl region file.\n");
    fprintf(stderr,"    -regions_of_interest: First region is one.\n");
    fprintf(stderr,"    -var_thresh:          Variance threshold. Default is 1e-10.\n");
    exit(-1);
    }
for(i=1;i<argc;i++) { 
    if(!strcmp(argv[i],"-glm_files") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_glm_files;
        if(!(glm_files=get_files(num_glm_files,&argv[i+1]))) exit(-1);
        i += num_glm_files;
        }
    if(!strcmp(argv[i],"-contrasts") && argc > i+1) {
        for(j=1;i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_contrasts;
	argc_c = i+1;
        i += num_contrasts;
        }
    if(!strcmp(argv[i],"-tc") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_tc;
	argc_tc = i+1;
        i += num_tc;
        }
    if(!strcmp(argv[i],"-glm_list_file") && argc > i+1)
        glm_list_file = argv[++i];
    if(!strcmp(argv[i],"-root") && argc > i+1)
        root = argv[++i];
    if(!strcmp(argv[i],"-L") && argc > i+1) {
        for(j=1;i+j<argc;j++) {
            strptr = argv[i+j] + 1;
            if(strchr(argv[i+j],'-') == argv[i+j] && !isdigit(*strptr) && !strchr(strptr,'.')) break;
            ++nL;
            }
        if(!(L=malloc(sizeof*L*nL))) {
            printf("Error: Unable to malloc L\n");
            exit(-1);
            }
        for(j=0;j<nL;j++) L[j] = atof(argv[++i]);
        }
    if(!strcmp(argv[i],"-R") && argc > i+1) {
        for(j=1;i+j<argc;j++) {
            strptr = argv[i+j] + 1;
            if(strchr(argv[i+j],'-') == argv[i+j] && !isdigit(*strptr) && !strchr(strptr,'.')) break;
            ++nR;
            }
        if(!(R=malloc(sizeof*R*nR))) {
            printf("Error: Unable to malloc R\n");
            exit(-1);
            }
        for(j=0;j<nR;j++) R[j] = atof(argv[++i]);
        }
    if(!strcmp(argv[i],"-num") && argc > i+1) {
        for(j=1;i+j<argc;j++) {
            strptr = argv[i+j] + 1;
            if(strchr(argv[i+j],'-') == argv[i+j] && !isdigit(*strptr) && !strchr(strptr,'.')) break;
            ++nnum;
            }
        if(!(num=malloc(sizeof*num*nnum))) {
            printf("Error: Unable to malloc num\n");
            exit(-1);
            }
        for(j=0;j<nnum;j++) num[j] = atof(argv[++i]);
        }
    if(!strcmp(argv[i],"-den") && argc > i+1) {
        for(j=1;i+j<argc;j++) {
            strptr = argv[i+j] + 1;
            if(strchr(argv[i+j],'-') == argv[i+j] && !isdigit(*strptr) && !strchr(strptr,'.')) break;
            ++nden;
            }
        if(!(den=malloc(sizeof*den*nden))) {
            printf("Error: Unable to malloc den\n");
            exit(-1);
            }
        for(j=0;j<nden;j++) den[j] = atof(argv[++i]);
        }
    if(!strcmp(argv[i],"-tc_frames") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++ntc_frames;
        if(!(tc_frames=malloc(sizeof*tc_frames*ntc_frames))) {
            printf("Error: Unable to malloc tc_frames\n");
            exit(-1);
            }
        for(tc_frames_max=j=0;j<ntc_frames;j++) {
            if((tc_frames[j]=atof(argv[++i])) > tc_frames_max) tc_frames_max = tc_frames[j];
            }
        }
    if(!strcmp(argv[i],"-xform_files") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_xform_files;
        if(!(xform_files=get_files(num_xform_files,&argv[i+1]))) exit(-1);
        i += num_xform_files;
        }
    if(!strcmp(argv[i],"-atlas") && argc > i+1)
        atlas = atoi(argv[++i]);
    if(!strcmp(argv[i],"-littleendian"))
        bigendian = 0;
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
    if(!strcmp(argv[i],"-var_thresh") && argc > i+1)
        var_thresh = atof(argv[++i]);
    }
/*printf("num_regions=%d\n",num_regions);*/
if((SunOS_Linux=checkOS())==-1) exit(-1);

if(!(temp_int=malloc(sizeof*temp_int*num_tc))) {
    printf("Error: Unable to malloc temp_int\n");
    exit(-1);
    }
for(i=0;i<num_tc;i++) temp_int[i] = num_glm_files;
tcs = read_tc_string_new(num_tc,temp_int,argc_tc,argv);
free(temp_int);
#if 0
printf("tcs->num_tc=%d num_tc=%d\n",tcs->num_tc,num_tc);
printf("tcs->num_tc_to_sum="); for(i=0;i<tcs->num_tc;i++) printf("%d ",tcs->num_tc_to_sum[i][0]); printf("\n");
for(i=0;i<tcs->num_tc;i++) {
    printf("tcs->each[%d]=%d\n",i,tcs->each[i]);
    for(j=0;j<tcs->num_tc_to_sum[i][0];j++) printf("%d ",(int)tcs->tc[i][0][j]);
    printf("\n");
    }
printf("tcs->total=%d\n",tcs->total);
#endif

/*START27*/
if(num_contrasts!=(nnum+nden)) {
    printf("num_contrasts=%d nnum=%d nden=%d nnum+nden must equal num_contrasts\n",num_contrasts,nnum,nden);
    exit(-1);
    }

if(!(temp_int=malloc(sizeof*temp_int*num_contrasts))) {
    printf("Error: Unable to malloc temp_int\n");
    exit(-1);
    }
for(i=0;i<num_contrasts;i++) temp_int[i] = num_glm_files;
if(!(con=read_tc_string_TCnew(num_contrasts,temp_int,argc_c,argv,'+'))) exit(-1);
#if 0
for(i=0;i<num_contrasts;i++) {
    /*printf("con->tc[con->eachi[%d]]-1=%d %s\n",i,(int)con->tc[con->eachi[i]]-1,
        glm_stack[0]->contrast_labels[(int)con->tc[con->eachi[i]]-1]);*/
    temp_int[i]=strlen(glm_stack[0]->contrast_labels[(int)con->tc[con->eachi[i]]-1])+strlen(appendstr)+1;
    }
if(!(tags=d2charvar(num_contrasts,temp_int))) exit(-1);
for(i=0;i<num_contrasts;i++) sprintf(tags[i],"%s%s",glm_stack[0]->contrast_labels[(int)con->tc[con->eachi[i]]-1],appendstr);
#endif
free(temp_int);

if(!(effect_length=malloc(sizeof*effect_length*num_contrasts))) {
    printf("Error: Unable to malloc effect_length\n");
    exit(-1);
    }
for(m=0;m<num_glm_files;m++) {
    if(!(glm=read_glm(glm_files->files[m],(int)SMALL,SunOS_Linux))) exit(-1);
    if(!m) {
        vol = glm->ifh->glm_xdim*glm->ifh->glm_ydim*glm->ifh->glm_zdim;
        for(effect_length_total=l=0;l<num_contrasts;l++) {
            eff = (int)con->tc[con->eachi[l]+m]-1;
            effect_length_total+=effect_length[l]=glm->ifh->glm_effect_length[eff];
            }
        }
    else {
        if(vol != (l=glm->ifh->glm_xdim*glm->ifh->glm_ydim*glm->ifh->glm_zdim)) {
            printf("%s l=%d Must equal vol=%d\n",glm_files->files[m],l,vol);
            exit(-1);
            }
        for(l=0;l<num_contrasts;l++) {
            eff = (int)con->tc[con->eachi[l]+m]-1;
            if(effect_length[l]!=glm->ifh->glm_effect_length[eff]) {
                printf("Error: effect_length[%d]=%d\n",l,effect_length[l]);
                printf("Error: %s glm->ifh->glm_effect_length[%d]=%d\n",glm_files->files[m],eff,glm->ifh->glm_effect_length[eff]);
                printf("Error: Must be equal.\n");
                exit(-1);
                } 
            }
        }
    free_glm(glm,(int)SMALL);
    }

if(num_region_files) {
    if(!num_regions) {
        printf("Error: Need to specify -regions_of_interest\n");
        fflush(stdout);
        exit(-1);
        }
    if(!(ifh = read_ifh(region_files->files[0]))) exit(-1);
    if((volreg=ifh->dim1*ifh->dim2*ifh->dim3)!=vol) {
        printf("volreg=%d vol=%d Must be equal.\n",volreg,vol);
        exit(-1);
        }
    free_ifh(ifh,(int)FALSE);
    if(!check_dimensions(num_region_files,region_files->files,volreg)) exit(-1);
    if(!(reg=malloc(sizeof*reg*num_region_files))) {
        printf("Error: Unable to malloc reg\n");
        exit(-1);
        }
    for(m=0;m<num_region_files;m++) {
        if(!(reg[m]=extract_regions(region_files->files[m],0,0,(float*)NULL,0,SunOS_Linux,(char**)NULL))) exit(-1);
        }
    if(!(rbf=find_regions_by_file_cover(num_region_files,num_regions,reg,roi))) exit(-1);
    }
if(!(idxnum=malloc(sizeof*idxnum*vol*num_glm_files))) {
    printf("Error: Unable to malloc idxnum\n");
    exit(-1);
    }
for(m=0;m<vol*num_glm_files;m++) idxnum[m]=0.;
if(!(idxden=malloc(sizeof*idxden*vol))) {
    printf("Error: Unable to malloc idxden\n");
    exit(-1);
    }
if(!(mags=malloc(sizeof*mags*vol*num_contrasts))) {
    printf("Error: Unable to malloc mags\n");
    exit(-1);
    }


#if 0
if(!(summags=malloc(sizeof*summags*vol*num_contrasts))) {
    printf("Error: Unable to malloc summags\n");
    exit(-1);
    }
for(m=0;m<vol*num_contrasts;m++) summags[m]=0.;
if(!(avgtc=malloc(sizeof*avgtc*vol*effect_length_total))) {
    printf("Error: Unable to malloc avgtc\n");
    exit(-1);
    }
for(m=0;m<vol*effect_length_total;m++) avgtc[m]=0.;
if(!(avgtc_nsub=malloc(sizeof*avgtc_nsub*vol*effect_length_total))) {
    printf("Error: Unable to malloc avgtc_nsub\n");
    exit(-1);
    }
for(m=0;m<vol*effect_length_total;m++) avgtc_nsub[m]=0;
#endif

if(!(contrast=malloc(sizeof*contrast*vol*tc_frames_max))) {
    printf("Error: Unable to malloc contrast\n");
    exit(-1);
    }
for(m=0;m<vol*tc_frames_max;m++) contrast[m]=0.;
if(!(contrast_nsub=malloc(sizeof*contrast_nsub*vol*tc_frames_max))) {
    printf("Error: Unable to malloc contrast_nsub\n");
    exit(-1);
    }
for(m=0;m<vol*tc_frames_max;m++) contrast_nsub[m]=0;
if(!(temp_double=malloc(sizeof*temp_double*vol*tc_frames_max))) {
    printf("Error: Unable to malloc temp_double\n");
    exit(-1);
    }

/*START27*/
if(!(count=malloc(sizeof*count*vol))) {
    printf("Error: Unable to malloc count\n");
    exit(-1);
    }
if(!(nsub=malloc(sizeof*nsub*vol))) {
    printf("Error: Unable to malloc nsub\n");
    exit(-1);
    }
for(m=0;m<vol;m++) nsub[m]=0.;
if(!(sum=malloc(sizeof*sum*vol))) {
    printf("Error: Unable to malloc sum\n");
    exit(-1);
    }
for(m=0;m<vol;m++) sum[m]=0.;
if(!(sum2=malloc(sizeof*sum2*vol))) {
    printf("Error: Unable to malloc sum2\n");
    exit(-1);
    }
for(m=0;m<vol;m++) sum2[m]=0.;


/*START25*/
if(num_regions) {
    if(!(stat=malloc(sizeof*stat*num_regions*tc_frames_max))) {
        printf("Error: Unable to malloc stat\n");
        exit(-1);
        }
    }

if(!(temp_float=malloc(sizeof*temp_float*vol))) {
    printf("Error: Unable to malloc temp_float\n");
    exit(-1);
    }
if(!(atlascoor=malloc(sizeof*atlascoor*3))) {
    printf("Error: Unable to malloc atlascoor\n");
    exit(-1);
    }
if(!(fidlcoor=malloc(sizeof*fidlcoor*3))) {
    printf("Error: Unable to malloc fidlcoor\n");
    exit(-1);
    }

if(!num_xform_files) {
    atlas = (int)get_atlas(vol);
    }
else {
    if(!(t4=malloc(sizeof*t4*(size_t)T4SIZE*num_glm_files))) {
        printf("Error: Unable to malloc t4\n");
        exit(-1);
        }
    if(!(A_or_B_or_U=malloc(sizeof*A_or_B_or_U*num_glm_files))) {
        printf("Error: Unable to malloc A_or_B_or_U\n");
        exit(-1);
        }
    for(i=0;i<num_glm_files;i++) {
        if(!read_xform(xform_files->files[i],&t4[i*(int)T4SIZE])) exit(-1);
        if((A_or_B_or_U[i]=twoA_or_twoB(xform_files->files[i])) == 2) exit(-1);
        }
    }
if(!(ap=get_atlas_param(atlas,(Interfile_header*)NULL))) exit(-1);
if(!atlas) {
    ap->vol = vol;
    ap->xdim = glm->ifh->glm_xdim;
    ap->ydim = glm->ifh->glm_ydim;
    ap->zdim = glm->ifh->glm_zdim;
    ap->voxel_size[0] = glm->ifh->glm_dxdy;
    ap->voxel_size[1] = glm->ifh->glm_dxdy;
    ap->voxel_size[2] = glm->ifh->glm_dz;
    }
swapbytesout = shouldiswap(SunOS_Linux,bigendian);
if(!(ifh=init_ifh(4,ap->xdim,ap->ydim,ap->zdim,1,ap->voxel_size[0],ap->voxel_size[1],ap->voxel_size[2],bigendian))) exit(-1);
printf("Computing contrast. Please be patient.\n");
for(m=0;m<num_glm_files;m++) {
    printf("    Processing %s\n",glm_files->files[m]);
    fflush(stdout);
    if(!(glm=read_glm(glm_files->files[m],(int)SMALL,SunOS_Linux))) exit(-1);
    if(vol != (l=glm->ifh->glm_xdim*glm->ifh->glm_ydim*glm->ifh->glm_zdim)) {
        printf("%s l=%d Must equal vol=%d\n",glm_files->files[m],l,vol);
        exit(-1);
        }
    if(!(gms=get_grand_mean_struct(glm->grand_mean,vol,glm_files->files[m]))) exit(-1);
    start_b = find_b(glm);
    if(!(fp=fopen_sub(glm_files->files[m],"r"))) exit(-1);
    for(l=0;l<vol*tc_frames_max;l++) temp_double[l] = 0.;
    for(k=l=0;l<ntc_frames;l++) {
        for(nn=n=0;n<tc_frames[l];n++,k++,nn+=vol) {
            for(i=0;i<tcs->num_tc_to_sum[k][m];i++) {
                offset = start_b+(long)(sizeof(float)*vol*((int)tcs->tc[k][m][i]-1));
                if(fseek(fp,offset,(int)SEEK_SET)) {
                    printf("Error: occured while seeking to %d in %s.\n",offset,glm_files->files[m]);
                    exit(-1);
                    }
                if(!fread_sub(temp_float,sizeof(float),(size_t)vol,fp,SunOS_Linux,glm_files->files[m])) {
                    printf("Error reading parameter estimates from %s.\n",glm_files->files[m]);
                    exit(-1);
                    }
                for(j=0;j<vol;j++) {
                    if(temp_double[nn+j]==(double)UNSAMPLED_VOXEL) continue;
                    if(temp_float[j]==(float)UNSAMPLED_VOXEL) {
                        temp_double[nn+j] = (double)UNSAMPLED_VOXEL;
                        continue;
                        }
                    voxel_index_to_fidl_and_atlas(j,ap->xdim,ap->ydim,ap->zdim,ap->center,ap->mmppix,fidlcoor,atlascoor);
                    if(!l&&atlascoor[0]<0) temp_double[nn+j] += (double)temp_float[j] * L[i];
                    else if(l&&atlascoor[0]>=0) temp_double[nn+j] += (double)temp_float[j] * R[i];
                    }
                }
            }
        }
    for(ll=l=0;l<tc_frames_max;l++) {
        for(n=0;n<vol;n++,ll++) {
            if(temp_double[ll]==(double)UNSAMPLED_VOXEL) continue;
            if(gms->grand_mean[n]<gms->grand_mean_thresh) {
                temp_double[ll] = (double)UNSAMPLED_VOXEL;
                continue;
                }
            temp_double[ll] *= gms->pct_chng_scl/gms->grand_mean[n];
            contrast[ll] += temp_double[ll];
            contrast_nsub[ll]++;
            }
        }
    fclose(fp);
    free_grand_mean_struct(gms);
    free_glm(glm,(int)SMALL);
    }
for(m=0;m<vol*tc_frames_max;m++) if(contrast_nsub[m]) contrast[m]/=(double)contrast_nsub[m];
for(m=0;m<vol;m++) {
    for(mean=0.,ll=m,l=0;l<tc_frames_max;l++,ll+=vol) mean += contrast[ll];
    mean /= (double)tc_frames_max;
    for(ll=m,l=0;l<tc_frames_max;l++,ll+=vol) contrast[ll] -= mean;
    }
if(num_regions) {
    for(ll=l=0;l<tc_frames_max;l++,ll+=vol) {
        if(!(crs(&contrast[ll],stat,rbf,(char*)NULL))) exit(-1);
        for(i=0;i<num_regions;i++) printf("contrast l=%d stat[%d]=%f\n",l,i,stat[i]);
        }
    }














/*START27*/
printf("Computing index. Please be patient.\n");
for(m1=m=0;m<num_glm_files;m++,m1+=vol) {

    for(l=0;l<vol*num_contrasts;l++) mags[l]=0.;
    for(l=0;l<vol;l++) count[l]=0;

    printf("    Processing %s\n",glm_files->files[m]);
    fflush(stdout);
    if(!(glm=read_glm(glm_files->files[m],(int)SMALL,SunOS_Linux))) exit(-1);
    if(vol != (l=glm->ifh->glm_xdim*glm->ifh->glm_ydim*glm->ifh->glm_zdim)) {
        printf("%s l=%d Must equal vol=%d\n",glm_files->files[m],l,vol);
        exit(-1);
        }
    if(!(gms=get_grand_mean_struct(glm->grand_mean,vol,glm_files->files[m]))) exit(-1);
    start_b = find_b(glm);
    if(!(fp=fopen_sub(glm_files->files[m],"r"))) exit(-1);
    for(jj=ll=l=0;l<num_contrasts;l++,ll+=vol) {
        eff = (int)con->tc[con->eachi[l]+m]-1;
        col = glm->ifh->glm_effect_column[eff];
        offset = start_b+(long)(sizeof(float)*vol*col);
        if(fseek(fp,offset,(int)SEEK_SET)) {
            printf("Error: occured while seeking to %d in %s.\n",offset,glm_files->files[m]);
            exit(-1);
            }
        for(nn=n=0;n<glm->ifh->glm_effect_length[eff];n++) {
            if(!fread_sub(temp_float,sizeof(float),(size_t)vol,fp,SunOS_Linux,glm_files->files[m])) {
                printf("Error reading parameter estimates from %s.\n",glm_files->files[m]);
                exit(-1);
                }
            for(jjj=ll,j=0;j<vol;j++,jj++,jjj++,nn++) {
                if(temp_float[j]==(float)UNSAMPLED_VOXEL) continue;
                if(gms->grand_mean[j]<gms->grand_mean_thresh) continue;
                if(temp_float[j]==(float)UNSAMPLED_VOXEL) {
                    mags[jjj] = (double)UNSAMPLED_VOXEL;
                    continue;
                    }
                else if(mags[jjj]==(double)UNSAMPLED_VOXEL) continue;
                else if(gms->grand_mean[j]<gms->grand_mean_thresh) continue;
                mags[jjj] += (double)temp_float[j] * gms->pct_chng_scl/gms->grand_mean[j] * contrast[nn]; 


                #if 0
                avgtc[jj] += (double)temp_float[j] * gms->pct_chng_scl/gms->grand_mean[j];
                avgtc_nsub[jj]++;
                summags[jjj] += (double)temp_float[j] * gms->pct_chng_scl/gms->grand_mean[j] * contrast[nn]; 
                #endif

                }
            }
        }
    fclose(fp);
    free_grand_mean_struct(gms);
    free_glm(glm,(int)SMALL);

    for(k=l=0;l<num_contrasts;l++) for(n=0;n<vol;n++,k++) if(mags[k]!=(double)UNSAMPLED_VOXEL) count[n]++; 
    for(l=0;l<vol;l++) if(count[l]==num_contrasts) nsub[l]++;
    for(k=l=0;l<nnum;l++) for(nn=m1,n=0;n<vol;n++,k++,nn++) if(count[n]==num_contrasts) idxnum[nn] += mags[k]*num[l];

    /*for(k=nnum*vol,l=0;l<nnum;l++) for(nn=m1,n=0;n<vol;n++,k++,nn++) if(count[n]==num_contrasts) idxnum[nn] += mags[k]*den[l];*/

    #if 1
    if(nden) {
        for(l=0;l<vol;l++) idxden[l]=0.;
        /*for(l=0;l<nden;l++) for(n=0;n<vol;n++,k++) if(count[n]==num_contrasts) idxden[n] += mags[k]*den[l];*/
        for(k=nnum*vol,l=0;l<nden;l++) for(n=0;n<vol;n++,k++) if(count[n]==num_contrasts) idxden[n] += mags[k]*den[l];
        for(ll=m1,l=0;l<vol;l++,ll++) {
            if(count[l]==num_contrasts) idxnum[ll]=fabs(idxden[l])>(double)UNSAMPLED_VOXEL?idxnum[ll]/idxden[l]:0.;
            /*if(count[l]==num_contrasts) idxnum[ll]/=idxden[l];*/
            }
        }
    #endif

    /*for(n=0;n<vol;n++) printf("count[%d]=%d\n",n,count[n]);*/
    }

#if 0
if(num_regions) {
    if(!(crs(idxnum,stat,rbf,(char*)NULL))) exit(-1);
    for(i=0;i<num_regions;i++) printf("new idxnum[%d]=%f\n",i,stat[i]);
    if(!(crs(idxden,stat,rbf,(char*)NULL))) exit(-1);
    for(i=0;i<num_regions;i++) printf("new idxden[%d]=%f\n",i,stat[i]);
    }
#endif


/*for(m=0;m<vol*effect_length_total;m++) if(avgtc_nsub[m]) avgtc[m]/=(double)avgtc_nsub[m];*/
#if 0
if(num_regions) {
    for(ll=l=0;l<effect_length_total;l++,ll+=vol) {
        if(!(crs(&avgtc[ll],stat,rbf,(char*)NULL))) exit(-1);
        for(i=0;i<num_regions;i++) printf("avgtc l=%d stat[%d]=%f\n",l,i,stat[i]);
        }
    }
#endif

#if 0
for(l=0;l<vol*num_contrasts;l++) mags[l]=0.;
for(jj=mm=m=0;m<num_contrasts;m++,mm+=vol) {
    for(ll=l=0;l<effect_length[m];l++) {
        for(nn=mm,j=0;j<vol;j++,jj++,nn++,ll++) {
            mags[nn]+=avgtc[jj]*contrast[ll];
            }
        }
    }
#endif
#if 0
if(num_regions) {
    for(ll=l=0;l<num_contrasts;l++,ll+=vol) {
        if(!(crs(&mags[ll],stat,rbf,(char*)NULL))) exit(-1);
        if(!(crs(&summags[ll],temp_double,rbf,(char*)NULL))) exit(-1);
        for(i=0;i<num_regions;i++) printf("l=%d mags[%d]=%f summags[%d]=%f\n",l,i,stat[i],i,temp_double[i]);
        }
    }
#endif



for(k=m=0;m<num_glm_files;m++) {
    for(l=0;l<vol;l++,k++) {
        sum[l] += idxnum[k];
        sum2[l] += idxnum[k]*idxnum[k];
        }
    }

/*for(l=0;l<vol;l++) printf("nsub[%d]=%f\n",l,nsub[l]);*/
/*for(k=m=0;m<num_glm_files;m++) for(l=0;l<vol;l++,k++) printf("%f ",idxnum[k]); printf("\n");*/
/*for(m=0;m<vol;m++) printf("sum[%d]=%f\n",m,sum[m]); printf("\n");*/

for(m=0;m<vol;m++) temp_float[m] = nsub[m]>=1. ? (float)(sum[m]/nsub[m]) : (float)UNSAMPLED_VOXEL;
sprintf(filename,"%s_index.4dfp.img",root);
if(!writestack(filename,temp_float,sizeof(float),(size_t)vol,swapbytesout)) exit(-1);
min_and_max_init(&ifh->global_min,&ifh->global_max);
min_and_max_floatstack(temp_float,vol,&ifh->global_min,&ifh->global_max);
if(glm_list_file) assign_glm_list_file_to_ifh(ifh,glm_list_file);
if(!write_ifh(filename,ifh,(int)FALSE)) exit(-1);
printf("Index written to %s\n",filename);
if(num_regions) {
    for(m=0;m<vol;m++) temp_double[m] = nsub[m]>=1. ? sum[m]/nsub[m] : (double)UNSAMPLED_VOXEL;
    if(!(crs(temp_double,stat,rbf,(char*)NULL))) exit(-1);
    for(i=0;i<num_regions;i++) printf("index stat[%d]=%f\n",i,stat[i]);
    }


#if 0
for(m=0;m<vol;m++) idxnum[m]=idxden[m]=0.;
k=0;
for(m=0;m<nnum;m++) for(l=0;l<vol;l++,k++) idxnum[l] += mags[k]*num[m];
for(m=0;m<nden;m++) for(l=0;l<vol;l++,k++) idxden[l] += mags[k]*den[m];
for(m=0;m<vol;m++) temp_double[m] = fabs(idxden[m])>(double)UNSAMPLED_VOXEL ? idxnum[m]/idxden[m] : 0.;
if(num_regions) {
    if(!(crs(idxnum,stat,rbf,(char*)NULL))) exit(-1);
    for(i=0;i<num_regions;i++) printf("old idxnum[%d]=%f\n",i,stat[i]);
    if(!(crs(idxden,stat,rbf,(char*)NULL))) exit(-1);
    for(i=0;i<num_regions;i++) printf("old idxden[%d]=%f\n",i,stat[i]);
    if(!(crs(temp_double,stat,rbf,(char*)NULL))) exit(-1);
    for(i=0;i<num_regions;i++) printf("old index[%d]=%f\n",i,stat[i]);
    }
#endif







if(!nden) {
    for(m=0;m<vol;m++) {
        /*if(nsub[m]<2.) {*/
        if(nsub[m]<2.||sum[m]<=(double)UNSAMPLED_VOXEL) {
            sum2[m] = 0.;
            }
        else {
            var=(sum2[m]-sum[m]*sum[m]/nsub[m])/(nsub[m]-1);

            /*printf("before sum2[%d]=%f sum[%d]=%f nsub[%d]=%f\n",m,sum2[m],m,sum[m],m,nsub[m]);*/

            sum2[m] = var<var_thresh ? (double)UNSAMPLED_VOXEL : sum[m]/nsub[m]/sqrt(var/nsub[m]);

            /*printf("sum2[%d]=%f sum[%d]=%f nsub[%d]=%f\n",m,sum2[m],m,sum[m],m,nsub[m]);*/

            }
        }    
    
    /*for(m=0;m<vol;m++) printf("sum[%d]=%f\n",m,sum[m]);*/
    
    t_to_z(sum2,sum,vol,nsub);
    for(m=0;m<vol;m++) temp_float[m] = sum[m]!=(double)UNSAMPLED_VOXEL ? (float)sum[m] : (float)UNSAMPLED_VOXEL;
    sprintf(filename,"%s_zstat.4dfp.img",root);
    if(!writestack(filename,temp_float,sizeof(float),(size_t)vol,swapbytesout)) exit(-1);
    min_and_max_init(&ifh->global_min,&ifh->global_max);
    min_and_max_floatstack(temp_float,vol,&ifh->global_min,&ifh->global_max);
    if(glm_list_file) assign_glm_list_file_to_ifh(ifh,glm_list_file);
    if(!write_ifh(filename,ifh,(int)FALSE)) exit(-1);
    printf("zstat written to %s\n",filename);
    }


#if 0
if(num_regions) {
    for(ll=l=0;l<tc_frames_max;l++,ll+=vol) { 
        if(!(crs(&contrast[ll],stat,rbf,(char*)NULL))) exit(-1);
        for(i=0;i<num_regions;i++) printf("contrast l=%d stat[%d]=%f\n",l,i,stat[i]);
        }
    for(jj=l=0;l<num_contrasts;l++) {
        eff = (int)con->tc[con->eachi[l]]-1;
        printf("avgtc %s\n",glm->contrast_labels[(int)con->tc[con->eachi[l]]-1]);
        for(n=0;n<glm->ifh->glm_effect_length[eff];n++,jj+=vol) {
            if(!(crs(&avgtc[jj],stat,rbf,(char*)NULL))) exit(-1);
            for(i=0;i<num_regions;i++) printf("%f ",stat[i]);
            printf("\n");
            }
        }
    }
#endif
}
