/* Copyright 10/31/01 Washington University.  All Rights Reserved.
   compute_pearson.c  $Revision: 1.18 $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <fidl.h>

static char rcsid[] = "$Header: /home/hannah/mcavoy/idl/clib/RCS/compute_pearson.c,v 1.18 2009/10/26 20:09:33 mcavoy Exp $";

main(int argc,char **argv)
{
char string[MAXNAME],write_back[MAXNAME],*str_ptr,*directory=NULL,string_t[MAXNAME],string_z[MAXNAME],**pstr=NULL;

int i,j,k,ii,jj,kk,skip=0,num_bold_files=0,num_target_files=0,tdim_usable,lc_4dfp_target=0,num_root=0,n_threshold_extent=0,*extent,
    lc_corrcoeff=0,lc_T_uncorrected=0,lc_Z_uncorrected=0,lc_T_monte_carlo=0,lc_Z_monte_carlo=0,lc_fisherz=0,mode,lconetarget=0,
    tdim_usable_stats,SunOS_Linux,npvalstr=0;

float *temp_float;

double *x,*y,*sumx,*sumx2,*sumy,*sumy2,*sumxy,SSxx,SSxy,SSyy,*threshold,*r,*tstat,*zstat,*df;

FILE *fp;
Interfile_header *ifh;
Memory_Map *mm_bold,*mm_target;
Af3d *af3d;
Files_Struct *bold_files,*target_files,*root,*pvalstr;
Dim_Param *dp;
Atlas_Param *ap;

print_version_number(rcsid,stdout);
if (argc < 5) {
    fprintf(stderr,"        -bold_files:   List of bold runs.\n");
    fprintf(stderr,"        -target_files: Text files with target values or bold runs.\n");
    fprintf(stderr,"        -skip:         Skip this number of initial frames.\n");
    fprintf(stderr,"        -directory:    Output storage path.\n");
    fprintf(stderr,"        -root:         Output root.\n");
    fprintf(stderr,"        -output:       Output options.\n");
    fprintf(stderr,"            corrcoeff      Correlation coefficient\n");
    fprintf(stderr,"            T_uncorrected  T = r*sqrt((n-2)/(1-r^2)) with n-2 df, where n is the number of samples.\n");
    fprintf(stderr,"            Z_uncorrected  T statistic gaussianized to a Z statistic.\n");
    fprintf(stderr,"            T_monte_carlo  T statistic corrected for multiple comparisons.\n");
    fprintf(stderr,"            Z_monte_carlo  Gaussianized T statistic corrected for multiple comparisons.\n");
    fprintf(stderr,"            fisherz        Fisher z-transformation: z=.5ln[(1+r)/(1-r)], where r = corrcoeff\n");
    fprintf(stderr,"                           Requires the assumption that the (X,Y) pairs are a random sample from a\n");
    fprintf(stderr,"                           bivariate normal distribution.\n");
    fprintf(stderr,"        -threshold_extent  Monte Carlo thresholds with spatial extent.\n");
    fprintf(stderr,"                           Ex. -threshold_extent \"3.75 18\" \"4 12\"\n");
    fprintf(stderr,"        -pval:             Associated p value for -threshold_extent. .05 is the default.\n");
    exit(-1);
    }

for(i=1;i<argc;i++) { 
    if(!strcmp(argv[i],"-bold_files") && argc > i+1) {
        for(j=1;i+j < argc && strchr(argv[i+j],'-') != argv[i+j];j++) ++num_bold_files;
        if(!(bold_files=get_files(num_bold_files,&argv[i+1]))) exit(-1);
        i += num_bold_files;
        }
    if(!strcmp(argv[i],"-target_files") && argc > i+1) {
        for(j=1;i+j < argc && strchr(argv[i+j],'-') != argv[i+j];j++) ++num_target_files;
        if(!(target_files=get_files(num_target_files,&argv[i+1]))) exit(-1);
        i += num_target_files;
        }
    if(!strcmp(argv[i],"-skip") && argc > i+1)
        skip = atoi(argv[++i]);
    if(!strcmp(argv[i],"-directory") && argc > i+1) {
        if(!(directory=malloc(sizeof*directory*(strlen(argv[i+1])+1)))) {
            printf("Error: Unable to malloc directory\n");
            exit(-1);
            }
        strcpy(directory,argv[++i]);
        }
    if(!strcmp(argv[i],"-root") && argc > i+1) {
        for(j=1;i+j < argc && strchr(argv[i+j],'-') != argv[i+j];j++) ++num_root;
        if(!(root=get_files(num_root,&argv[i+1]))) exit(-1);
        i += num_root;
        }
    if(!strcmp(argv[i],"-output") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) {
            if(!strcmp(argv[i+j],"corrcoeff")) lc_corrcoeff = 1;
            else if(!strcmp(argv[i+j],"T_uncorrected")) lc_T_uncorrected = 1;
            else if(!strcmp(argv[i+j],"Z_uncorrected")) lc_Z_uncorrected = 1;
            else if(!strcmp(argv[i+j],"T_monte_carlo")) lc_T_monte_carlo = 1;
            else if(!strcmp(argv[i+j],"Z_monte_carlo")) lc_Z_monte_carlo = 1;
            else if(!strcmp(argv[i+j],"fisherz")) lc_fisherz = 1;
            }
        i += lc_corrcoeff+lc_T_uncorrected+lc_Z_uncorrected+lc_T_monte_carlo+lc_Z_monte_carlo+lc_fisherz;
        }
    if(!strcmp(argv[i],"-threshold_extent") && argc > i+1) {
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
    if(!strcmp(argv[i],"-pval") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++npvalstr;
        if(!(pvalstr=get_files(npvalstr,&argv[i+1]))) exit(-1);
        pstr = pvalstr->files;
        i += npvalstr;
        }
    }
if(!num_bold_files) {
    fprintf(stdout,"Error: No bold files. Abort!\n");
    exit(-1);
    }
if(!num_target_files) {
    fprintf(stdout,"Error: No bold files. Abort!\n");
    exit(-1);
    }
if(npvalstr) {
    if(npvalstr!=n_threshold_extent) {
        printf("Error: -pval need to have the same number of items as -threshold_extent\n");
        exit(-1);
        }
    }
if((SunOS_Linux=checkOS())==-1) exit(-1);

if(num_bold_files != num_target_files) lconetarget = 1;

if(!(dp=dim_param(num_bold_files,bold_files->files,SunOS_Linux))) exit(-1);
if(strstr(target_files->files[0],".4dfp.img")) {
    lc_4dfp_target = 1;
    for(i=0;i<num_bold_files;i++) {
        if(!(ifh = read_ifh(target_files->files[i]))) exit(-1);
        if(ifh->dim1*ifh->dim2*ifh->dim3*ifh->dim4 != dp->vol*dp->tdim[i]) {
            printf("Dimensions of images are not equal.\n    %s\n    %s\n",bold_files->files[i],target_files->files[i]);
            exit(-1);
            }
        free_ifh(ifh,(int)FALSE);
        }
    }

if(!(tstat=malloc(sizeof*tstat*dp->vol))) {
    printf("Error: Unable to malloc tstat\n");
    exit(-1);
    }
if(!(df=malloc(sizeof*df*dp->vol))) {
    printf("Error: Unable to malloc df\n");
    exit(-1);
    }
if(!(zstat=malloc(sizeof*zstat*dp->vol))) {
    printf("Error: Unable to malloc zstat\n");
    exit(-1);
    }
if(!(temp_float=malloc(sizeof*temp_float*dp->vol))) {
    printf("Error: Unable to malloc temp_float\n");
    exit(-1);
    }
if(!(x=malloc(sizeof*x*(dp->tdim_max-skip)))) {
    printf("Error: Unable to malloc x\n");
    exit(-1);
    }
if(!(y=malloc(sizeof*y*(dp->tdim_max-skip)))) {
    printf("Error: Unable to malloc y\n");
    exit(-1);
    }

if(!(r=malloc(sizeof*r*dp->vol))) {
    printf("Error: Unable to malloc r\n");
    exit(-1);
    }
if(!(sumx=malloc(sizeof*sumx*dp->vol))) {
    printf("Error: Unable to malloc sumx\n");
    exit(-1);
    }
if(!(sumx2=malloc(sizeof*sumx2*dp->vol))) {
    printf("Error: Unable to malloc sumx2\n");
    exit(-1);
    }
if(!(sumxy=malloc(sizeof*sumxy*dp->vol))) {
    printf("Error: Unable to malloc sumxy\n");
    exit(-1);
    }
if(!(sumy=malloc(sizeof*sumx*dp->vol))) {
    printf("Error: Unable to malloc sumy\n");
    exit(-1);
    }
if(!(sumy2=malloc(sizeof*sumy2*dp->vol))) {
    printf("Error: Unable to malloc sumy2\n");
    exit(-1);
    }
for(i=0;i<dp->vol;i++) r[i]=sumx[i]=sumx2[i]=sumxy[i]=sumy[i]=sumy2[i]=0.;

if(lc_Z_monte_carlo || lc_T_monte_carlo) {
    /*mode = 0, print z; mode = 1, print t; mode = 2, print both*/
    if(lc_Z_monte_carlo) mode = 0;
    if(lc_T_monte_carlo) mode = 1;
    if(lc_Z_monte_carlo && lc_T_monte_carlo) mode = 2;
    }

if(!(ap=get_atlas_param(222,(Interfile_header*)NULL))) exit(-1);
if(!lc_4dfp_target && lconetarget) {
    if(!(af3d=read_af3d(target_files->files[0],ap,(float)UNSAMPLED_VOXEL,"other"))) exit(-1);
    if(af3d->nindex != dp->tdim_total) {
        printf("Error: Your bold files have %d total timepoints but %s has %d timepoints. Must be equal. Abort!\n",dp->tdim_total,
            target_files->files[0],af3d->nindex);
        exit(-1);
        }
    }

tdim_usable_stats = dp->tdim_total - num_bold_files*skip;
for(kk=i=0;i<num_bold_files;i++) {
    /*printf("kk=%d\n",kk);*/
    tdim_usable = dp->tdim[i] - skip;

    if(!(mm_bold = map_disk(bold_files->files[i],dp->tdim[i]*dp->vol,0))) exit(-1);
    if(lc_4dfp_target) {
        if(!(mm_target = map_disk(target_files->files[i],dp->tdim[i]*dp->vol,0))) exit(-1); 
        }
    else {
        if(!lconetarget) {
            if(!(af3d=read_af3d(target_files->files[i],ap,(float)UNSAMPLED_VOXEL,"other"))) exit(-1);
            if(af3d->nindex != dp->tdim[i]) {
                printf("Error: %s has %d timepoints. %s has %d timepoints. Must be equal. Abort!\n",bold_files->files[i],dp->tdim[i],
                    target_files->files[i],af3d->nindex);
                exit(-1);
                }
            kk=0;
            sumy[0]=sumy2[0]=0.; 
            }
        for(kk+=skip,j=0;j<tdim_usable;j++,kk++) {
            y[j] = af3d->coordinates[kk][af3d->ncoordinates[kk]-1];
            sumy[0] += y[j];
            sumy2[0] += y[j]*y[j];
            }
        }

    if(!lconetarget) {
        for(k=0;k<dp->vol;k++) r[k]=sumx[k]=sumx2[k]=sumxy[k]=0.; 
        if(lc_4dfp_target) for(k=0;k<dp->vol;k++) sumy[k]=sumy2[k]=0.;
        }

    printf("Processing %s\n",bold_files->files[i]);
    if(lc_4dfp_target && !lconetarget) printf("Processing %s\n",target_files->files[i]);
    for(k=0;k<dp->vol;k++) {
        for(j=0;j<tdim_usable;j++) temp_float[j] = mm_bold->ptr[dp->vol*(j+skip)+k];
        if(SunOS_Linux) swap_bytes((unsigned char *)temp_float,sizeof(float),(size_t)tdim_usable);
        for(j=0;j<tdim_usable;j++) {
            if(temp_float[j]==(float)UNSAMPLED_VOXEL) {
                r[k]=(double)UNSAMPLED_VOXEL;
                break;
                }
            else {
                x[j]=(double)temp_float[j];
                }
            }
        if(r[k]!=(double)UNSAMPLED_VOXEL) {
            if(!lc_4dfp_target) {
                sumy[k] = sumy[0];
                sumy2[k] = sumy2[0];
                }
            else {
                for(j=0;j<tdim_usable;j++) temp_float[j] = mm_target->ptr[dp->vol*(j+skip)+k];
                swap_bytes((unsigned char *)temp_float,sizeof(float),(size_t)tdim_usable);
                for(j=0;j<tdim_usable;j++) {
                    if(temp_float[j]==(float)UNSAMPLED_VOXEL) {
                        r[k]=(double)UNSAMPLED_VOXEL;
                        break;
                        }
                    else {
                        y[j]=(double)temp_float[j];
                        }
                    } 
                if(r[k]!=(double)UNSAMPLED_VOXEL) {
                    for(j=0;j<tdim_usable;j++) {
                        sumy[k] += y[j];
                        sumy2[k] += y[j]*y[j];
                        }
                    }
                }
            }
        if(r[k]!=(double)UNSAMPLED_VOXEL) {
            for(j=0;j<tdim_usable;j++) {
                sumx[k] += x[j];
                sumx2[k] += x[j]*x[j];
                sumxy[k] += x[j]*y[j];
                }
            }
        }

    if(!lconetarget || i==(num_bold_files-1)) {
        ii = !lconetarget ? i : 0; 
        tdim_usable_stats = !lconetarget ? tdim_usable : tdim_usable_stats;

        for(k=0;k<dp->vol;k++) {
            if(r[k]!=(double)UNSAMPLED_VOXEL) {
                SSxx = sumx2[k] - sumx[k]*sumx[k]/(double)tdim_usable_stats;
                SSxy = sumxy[k] - sumx[k]*sumy[k]/(double)tdim_usable_stats;
                SSyy = sumy2[k] - sumy[k]*sumy[k]/(double)tdim_usable_stats;
                r[k] = SSxy/sqrt(SSxx*SSyy);
                }
            }

        if(!(str_ptr=get_tail_sans_ext(root->files[ii]))) exit(-1);
        string[0] = 0;
        if(directory) strcat(string,directory);
        strcat(string,str_ptr);

        if(!(ifh=read_ifh(bold_files->files[i]))) exit(-1);
        ifh->dim4 = 1;
        ifh->dof_condition = (float)(tdim_usable_stats - 2);

        if(lc_corrcoeff) {
            for(k=0;k<dp->vol;k++) temp_float[k] = (float)r[k]; 
            sprintf(write_back,"%s_corrcoeff.4dfp.img",string);
            if(!writestack(write_back,temp_float,sizeof(float),(size_t)dp->vol,SunOS_Linux)) exit(-1);
            printf("Output written to %s\n",write_back);
            min_and_max(temp_float,dp->vol,&ifh->global_min,&ifh->global_max);
            if(!write_ifh(write_back,ifh,(int)FALSE)) exit(-1);
            }
        if(lc_T_uncorrected+lc_Z_uncorrected+lc_T_monte_carlo+lc_Z_monte_carlo) {
            for(k=0;k<dp->vol;k++) {
                tstat[k] = r[k]==(double)UNSAMPLED_VOXEL ? (double)UNSAMPLED_VOXEL :
                    r[k]*sqrt((double)(tdim_usable_stats-2)/(1-r[k]*r[k]));
                }
            if(lc_T_uncorrected || lc_T_monte_carlo) { 
                sprintf(string_t,"%s_tstat.4dfp.img",string);
                }
            if(lc_Z_uncorrected || lc_Z_monte_carlo) {
                sprintf(string_z,"%s_zstat.4dfp.img",string);
                for(k=0;k<dp->vol;k++) df[k]=(double)tdim_usable_stats-2;
                t_to_z(tstat,zstat,dp->vol,df);
                }
            if(lc_T_uncorrected) {
                for(k=0;k<dp->vol;k++) temp_float[k] = (float)tstat[k];
                if(!writestack(string_t,temp_float,sizeof(float),(size_t)dp->vol,SunOS_Linux)) exit(-1);
                printf("Output written to %s\n",string_t);
                min_and_max(temp_float,dp->vol,&ifh->global_min,&ifh->global_max);
                if(!write_ifh(string_t,ifh,(int)FALSE)) exit(-1);
                }
            if(lc_Z_uncorrected) {
                for(k=0;k<dp->vol;k++) temp_float[k] = (float)zstat[k];
                if(!writestack(string_z,temp_float,sizeof(float),(size_t)dp->vol,SunOS_Linux)) exit(-1);
                printf("Output written to %s\n",string_z);
                min_and_max(temp_float,dp->vol,&ifh->global_min,&ifh->global_max);
                if(!write_ifh(string_z,ifh,(int)FALSE)) exit(-1);
                }
            if(lc_Z_monte_carlo || lc_T_monte_carlo) {
                if(!monte_carlo_mult_comp(zstat,dp->vol,(int*)NULL,ifh->dim1,ifh->dim2,ifh->dim3,threshold,extent,
                    n_threshold_extent,string_z,ifh,mode,tstat,string_t,SunOS_Linux,pstr,(double*)NULL,(char*)NULL)) exit(-1);
                }
            }
        if(lc_fisherz) {
            for(k=0;k<dp->vol;k++) {
                temp_float[k] = r[k]==(double)UNSAMPLED_VOXEL ? (float)UNSAMPLED_VOXEL : 
                    (float)(.5*log((1+r[k])/(1-r[k])) - r[k]/(.2*(tdim_usable_stats-1)));
                }
            sprintf(write_back,"%s_fisherz.4dfp.img",string);
            if(!writestack(write_back,temp_float,sizeof(float),(size_t)dp->vol,SunOS_Linux)) exit(-1);
            printf("Output written to %s\n",write_back);
            min_and_max(temp_float,dp->vol,&ifh->global_min,&ifh->global_max);
            if(!write_ifh(write_back,ifh,(int)FALSE)) exit(-1);
            }
        free_ifh(ifh,(int)FALSE);
        !lc_4dfp_target ? free_af3d(af3d) : unmap_disk(mm_target);
        }
    unmap_disk(mm_bold);
    }
}
