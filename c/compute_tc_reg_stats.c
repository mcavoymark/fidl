/* Copyright 9/13/02 Washington University.  All Rights Reserved.
   compute_tc_reg_stats.c  $Revision: 1.26 $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "fidl.h"
#include "dim_param2.h"
#include "get_atlas_param.h"
#include "shouldiswap.h"
#include "subs_util.h"
#include "t4_atlas.h"
int main(int argc,char **argv)
{
char *xform_file=NULL,*output="compute_tc_reg_stats.txt",atlas[7]="";
int i,j,k,l,m,num_regions=0,num_region_files=0,num_tc_files=0,*roi=NULL,count,A_or_B_or_U=-1,lcugly=0,lcmoric=0,SunOS_Linux,swapbytes;
float *t4=NULL,*temp_float;
double *temp_double,*stat,sum,sum2,sd,min,max;
FILE *op;
Regions **reg;
Regions_By_File *rbf;
Region_Stats **regstat;
Dim_Param2 *dp;
Atlas_Param *ap;
Memory_Map *mm;
Files_Struct *tc_files=NULL,*region_files=NULL;
if(argc < 5) {
    fprintf(stderr,"        -tc_files:                  4dfp timecourse files.\n");
    fprintf(stderr,"        -region_file:               *.4dfp.img file(s) that specifies regions of interest.\n");
    fprintf(stderr,"        -regions_of_interest:       First region is one.\n");
    fprintf(stderr,"        -xform_file:                t4 file defining the transform to atlas space.\n");
    fprintf(stderr,"        -output:                    Name of text file that regional z statistics are written to.\n");
    fprintf(stderr,"                                    Default is regz.txt\n");
    fprintf(stderr,"        -ugly                       Output with more decimal places.\n");
    fprintf(stderr,"        -mean_only_runs_in_columns  Just like it says.\n"); 
    fprintf(stderr,"        -mean_only_each_regtc_a_col Just like it says.\n"); 
    exit(-1);
    }
for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-tc_files") && argc > i+1) {
        for(j=1;i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_tc_files;
        if(!(tc_files=get_files(num_tc_files,&argv[i+1]))) exit(-1);
        i += num_tc_files;
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
    if(!strcmp(argv[i],"-xform_file") && argc > i+1)
        xform_file = argv[++i];
    if(!strcmp(argv[i],"-output") && argc > i+1)
        output = argv[++i];
    if(!strcmp(argv[i],"-ugly"))
        lcugly = 1;
    if(!strcmp(argv[i],"-mean_only_runs_in_columns"))
        lcmoric = 1;
    if(!strcmp(argv[i],"-mean_only_each_regtc_a_col"))
        lcmoric = 2;
    }
if(!num_tc_files) {
    printf("No timecourse files. Abort!\n");
    exit(-1);
    }
if(!num_region_files) {
    printf("No region files. Abort!\n");
    exit(-1);
    }
if((SunOS_Linux=checkOS())==-1) exit(-1);
printf("SunOS_Linux=%d\n",SunOS_Linux);

//if(!(dp=dim_param(num_tc_files,tc_files->files,SunOS_Linux,0,(int*)NULL,(int*)NULL))) exit(-1);
//START160803
if(!(dp=dim_param2(num_tc_files,tc_files->files,SunOS_Linux))) exit(-1);

if(xform_file) {
    if(!(t4=malloc(sizeof*t4*(size_t)T4SIZE))) {
        printf("Error: Unable to malloc t4\n");
        exit(-1);
        }
    if(!read_xform(xform_file,t4)) exit(-1);
    if((A_or_B_or_U=twoA_or_twoB(xform_file)) == 2) {
        fprintf(stdout,"Error: Illegally named t4 file.\n");
        exit(-1);
        }
    }
else {

    //get_atlas(dp->vol,atlas);
    //START160803
    get_atlas(dp->vol[0],atlas);

    }
if(!(ap=get_atlas_param(atlas,(Interfile_header*)NULL))) exit(-1);

//if(!xform_file) ap->vol = dp->vol;
//START160803
if(!xform_file) ap->vol = dp->vol[0];

if(!atlas[0]) {

    #if 0
    ap->xdim = dp->xdim;
    ap->ydim = dp->ydim;
    ap->zdim = dp->zdim;
    ap->voxel_size[0] = dp->dxdy;
    ap->voxel_size[1] = dp->dxdy;
    ap->voxel_size[2] = dp->dz;
    #endif
    //START160803
    ap->xdim = dp->xdim[0];
    ap->ydim = dp->ydim[0];
    ap->zdim = dp->zdim[0];
    ap->voxel_size[0] = dp->dxdy[0];
    ap->voxel_size[1] = dp->dxdy[0];
    ap->voxel_size[2] = dp->dz[0];

    }
if(!check_dimensions(num_region_files,region_files->files,ap->vol)) exit(-1);
if(!(reg=malloc(sizeof*reg*num_region_files))) {
    printf("Error: Unable to malloc reg\n"); 
    exit(-1);
    }
for(i=0;i<num_region_files;i++) if(!(reg[i]=extract_regions(region_files->files[i],0,0,(float*)NULL,0,SunOS_Linux,
    (char**)NULL))) exit(-1);
if(!num_regions) for(m=0;m<num_region_files;m++) num_regions += reg[m]->nregions;
if(!(rbf=find_regions_by_file_cover(num_region_files,num_regions,reg,roi))) exit(-1);
if(!(regstat=malloc(sizeof*regstat*dp->tdim_total))) {
    printf("Error: Unable to malloc regstat\n");
    exit(-1);
    }
if(!(temp_float=malloc(sizeof*temp_float*ap->vol))) {
    printf("Error: Unable to malloc temp_float\n"); 
    exit(-1);
    }
if(!(temp_double=malloc(sizeof*temp_double*ap->vol))) {
    printf("Error: Unable to malloc temp_double\n");
    exit(-1);
    }
if(!(stat=malloc(sizeof*stat*ap->vol))) {
    printf("Error: Unable to malloc stat\n");
    exit(-1);
    }

for(l=i=0;i<num_tc_files;i++) {

    //if(!(mm = map_disk(tc_files->files[i],dp->vol*dp->tdim[i],0,sizeof(float)))) exit(-1);
    //START160803
    if(!(mm = map_disk(tc_files->files[i],dp->vol[i]*dp->tdim[i],0,sizeof(float)))) exit(-1);

    printf("Processing %s\n",tc_files->files[i]);
    swapbytes = shouldiswap(SunOS_Linux,dp->bigendian[i]);
    for(j=0;j<dp->tdim[i];j++,l++) {
        if(!swapbytes) {

            //for(k=0;k<dp->vol;k++) temp_double[k] = mm->ptr[j*dp->vol+k]==(float)UNSAMPLED_VOXEL ?
            //    (double)UNSAMPLED_VOXEL : (double)mm->ptr[j*dp->vol+k];
            //START160803
            for(k=0;k<dp->vol[i];k++) temp_double[k] = mm->ptr[j*dp->vol[i]+k]==(float)UNSAMPLED_VOXEL ?
                (double)UNSAMPLED_VOXEL : (double)mm->ptr[j*dp->vol[i]+k];

            }
        else {

            #if 0
            for(k=0;k<dp->vol;k++) temp_float[k] = mm->ptr[j*dp->vol+k];
            swap_bytes((unsigned char *)temp_float,sizeof(float),(size_t)dp->vol);
            for(k=0;k<dp->vol;k++) temp_double[k] = temp_float[k]==(float)UNSAMPLED_VOXEL ?
                (double)UNSAMPLED_VOXEL : (double)temp_float[k];
            #endif
            //START160803
            for(k=0;k<dp->vol[i];k++) temp_float[k] = mm->ptr[j*dp->vol[i]+k];
            swap_bytes((unsigned char *)temp_float,sizeof(float),(size_t)dp->vol[i]);
            for(k=0;k<dp->vol[i];k++) temp_double[k] = temp_float[k]==(float)UNSAMPLED_VOXEL ?
                (double)UNSAMPLED_VOXEL : (double)temp_float[k];

            }
        if(xform_file) {

            //if(!t4_atlas(temp_double,stat,t4,dp->xdim,dp->ydim,dp->zdim,dp->dxdy,dp->dz,A_or_B_or_U,dp->orientation,ap,
            //    (double*)NULL)) exit(-1);
            //START160803
            if(!t4_atlas(temp_double,stat,t4,dp->xdim[i],dp->ydim[i],dp->zdim[i],dp->dxdy[i],dp->dz[i],A_or_B_or_U,dp->orientation[i],
                ap,(double*)NULL)) exit(-1);

            for(k=0;k<ap->vol;k++) temp_double[k] = stat[k];
            }
        if(!(regstat[l]=rs(temp_double,rbf))) exit(-1);
        }
    if(!unmap_disk(mm)) exit(-1);
    }

if(!(op = fopen_sub(output,"w"))) exit(-1);
if(!lcmoric) {
    for(l=i=0;i<num_tc_files;l+=dp->tdim[i++]) {
        fprintf(op,"TIMECOURSE: %s\n\n",tc_files->files[i]);
        for(k=0;k<num_regions;k++) {
            fprintf(op,"REGION: %s NVOXELS: %d\n",rbf->region_names_ptr[k],rbf->nvoxels_region[k]);
            fprintf(op,"frame mean     sd      sem      min     max    nsampledvoxels\n");
            fprintf(op,"-------------------------------------------------------------\n");
            for(j=0;j<dp->tdim[i];j++) {
                m = l+j;
                if(!lcugly) {
                    fprintf(op,"%3d %7.2f %7.2f %7.2f %7.2f %7.2f %7d\n",j+1,regstat[m]->mean[k],regstat[m]->sd[k],
                        regstat[m]->sd[k]/sqrt((double)regstat[m]->nsampledvoxels[k]),regstat[m]->min[k],regstat[m]->max[k],
                        regstat[m]->nsampledvoxels[k]);
                    }
                else {
                    fprintf(op,"%3d %f %f %f %f %f %7d\n",j+1,regstat[m]->mean[k],regstat[m]->sd[k],
                        regstat[m]->sd[k]/sqrt((double)regstat[m]->nsampledvoxels[k]),regstat[m]->min[k],regstat[m]->max[k],
                        regstat[m]->nsampledvoxels[k]);
                    }
                }
            for(min=1.e20,max=-1.e20,sum=sum2=count=j=0;j<dp->tdim[i];j++) {
                m = l+j;
                sum += regstat[m]->sum[k];
                sum2 += regstat[m]->sum2[k];
                count += regstat[m]->nsampledvoxels[k];
                if(regstat[m]->max[k] > max) max = regstat[m]->max[k];
                if(regstat[m]->min[k] < min) min = regstat[m]->min[k];
                }
            sd = sqrt((sum2 - (sum*sum)/(double)count)/(double)(count-1));
            fprintf(op,"all %7.2f %7.2f %7.2f %7.2f %7.2f %7d\n",sum/(double)count,sd,sd/sqrt((double)count),min,max,count);
            fprintf(op,"\n");
            }
        fprintf(op,"\n");
        }
    }
else if(lcmoric==1) {
    fprintf(op,"TIMECOURSE:   1 %s\n",tc_files->files[0]);
    for(i=1;i<num_tc_files;i++) fprintf(op,"            %3d %s\n",i+1,tc_files->files[i]);
    fprintf(op,"\n");
    for(k=0;k<num_regions;k++) {
        fprintf(op,"%s %d\n",rbf->region_names_ptr[k],rbf->nvoxels_region[k]);
        for(i=0;i<num_tc_files;i++) fprintf(op,"\trun%-4d",i+1);
        for(j=0;j<dp->tdim_max;j++) {
            fprintf(op,"\n%3d",j+1);
            for(l=i=0;i<num_tc_files;l+=dp->tdim[i++]) {
                if(j<dp->tdim[i]) {
                    fprintf(op,"\t%7.2f",regstat[l+j]->mean[k]);
                    }
                else {
                    fprintf(op,"\t       ");
                    }
                }
            }
        fprintf(op,"\n\n");
        }
    }
else if(lcmoric==2) {
    for(l=i=0;i<num_tc_files;l+=dp->tdim[i++]) {
        fprintf(op,"TIMECOURSE: %s\n",tc_files->files[i]);
        for(k=0;k<num_regions;k++) fprintf(op,"\t%s %d",rbf->region_names_ptr[k],rbf->nvoxels_region[k]); fprintf(op,"\n");
        for(j=0;j<dp->tdim[i];j++) {
            fprintf(op,"%3d\t",j+1);
            for(k=0;k<num_regions;k++) fprintf(op,"%7.4f\t",regstat[l+j]->mean[k]); fprintf(op,"\n");
            }
        }
    }
fflush(op);
fclose(op);
printf("Regional statistics written to %s\n",output);
}
