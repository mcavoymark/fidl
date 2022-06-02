/* Copyright 11/15/00 Washington University.  All Rights Reserved.
   compute_region_clusters.c  $Revision: 1.20 $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <math.h>

//START160107
#include <ctype.h>

#include "fidl.h"
#include "assign_region_names2.h"

//START160107
#include "get_atlas_param.h"
#include "shouldiswap.h"
#include "subs_util.h"

int main(int argc,char **argv)
{
char *string_ptr,*cluster_file=NULL,atlas[7]="";
int i,j,k,m,lenvol,argc_clusters=0,num_region_files=0,num_clusters=0,num_cluster_names=0,*num_voxels_per_cluster,
    *roi,*voxnum,SunOS_Linux,*cluster,swapbytes;
float *temp_float;
Interfile_header *ifh;
Regions **reg;
Regions_By_File *rbf;
Files_Struct *region_files=NULL,*cluster_names=NULL;
TC *tcs;
assign_region_names2struct *arn;
Atlas_Param *ap;
if(argc < 5) {
    fprintf(stderr,"        -region_file:         *.4dfp.img file(s) that specifies regions of interest.\n");
    fprintf(stderr,"        -regions_of_interest: Calculate statistics for selected regions in the region file(s).\n"); 
    fprintf(stderr,"                              First region is one.\n"); 
    fprintf(stderr,"        -cluster_file:        Clusters are written to this *.4dfp.img file.\n");
    fprintf(stderr,"        -clusters:            List of names to be assigned to the clusters.\n");
    exit(-1);
    }

for(i=1;i<argc;i++) { 
    if(!strcmp(argv[i],"-region_file") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_region_files;
        if(!(region_files=get_files(num_region_files,&argv[i+1]))) exit(-1);
        i += num_region_files;
        }
    if(!strcmp(argv[i],"-regions_of_interest") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_clusters;
        argc_clusters = i+1;
        i += num_clusters;
        }
    if(!strcmp(argv[i],"-cluster_file") && argc > i+1)
        cluster_file = argv[++i];
    if(!strcmp(argv[i],"-clusters") && argc > i+1) {
        for(j=1;i+j<argc;j++) {
            string_ptr = argv[i+j] + 1;
            if(strchr(argv[i+j],'-') == argv[i+j] && !isdigit(*string_ptr)) break;
            ++num_cluster_names;
            }
        if(!(cluster_names=get_files(num_cluster_names,&argv[i+1]))) exit(-1);
        i += num_cluster_names;
        }
    }
if((SunOS_Linux=checkOS())==-1) exit(-1);
if(!num_region_files) {
    printf("Error: No region files specified with -region_file option.\n");
    exit(-1);
    }
if(!num_clusters) {
    printf("Error: No clusters specified with -regions_of_interest option.\n");
    exit(-1);
    }
else if(num_clusters != num_cluster_names) {
    printf("Error: num_clusters = %d num_cluster_names = %d This is a discrepancy. Abort!\n",num_clusters,num_cluster_names);
    exit(-1);
    }

if(!(ifh = read_ifh(region_files->files[num_region_files-1],(Interfile_header*)NULL))) exit(-1);
free_ifh(ifh,1);
lenvol = ifh->dim1*ifh->dim2*ifh->dim3;
if(!(temp_float=malloc(sizeof*temp_float*lenvol))) {
    printf("Error: Unable to malloc temp_float\n");
    exit(-1);
    }
if(!check_dimensions(num_region_files,region_files->files,lenvol)) exit(-1);
if(!(reg=malloc(sizeof*reg*num_region_files))) {
    printf("Error: Unable to malloc reg\n");
    exit(-1);
    }
for(m=0;m<num_region_files;m++) if(!(reg[m] = extract_regions(region_files->files[m],0,0,(float*)NULL,0,SunOS_Linux,(char**)NULL))) 
    exit(-1);


if(!(tcs=read_tc_string_new(num_clusters,(int*)NULL,argc_clusters,argv))) exit(-1);
#if 0
    printf("num_clusters=%d\n",num_clusters);
    printf("tcs->total=%d\n",tcs->total);
    printf("tcs->num_tc=%d\n",tcs->num_tc);
    printf("tcs->num_tc_to_sum="); for(i=0;i<tcs->num_tc;i++) printf("%d ",tcs->num_tc_to_sum[i][0]); printf("\n");
    printf("tcs->tc=\n");
    for(i=0;i<tcs->num_tc;i++) {
        printf("tcs->each[%d]=%d\n",i,tcs->each[i]);
        for(k=0;k<tcs->each[i];k++) {
            for(j=0;j<tcs->num_tc_to_sum[i][k];j++) printf("%d ",(int)tcs->tc[i][k][j]);
            printf("    ");
            }
        printf("\n");
        }
#endif

if(!(roi=malloc(sizeof*roi*tcs->total))) {
    printf("Error: Unable to malloc roi\n");
    exit(-1);
    }
if(!(cluster=malloc(sizeof*cluster*tcs->total))) {
    printf("Error: Unable to malloc roi\n");
    exit(-1);
    }
if(!(voxnum=malloc(sizeof*voxnum*tcs->total))) {
    printf("Error: Unable to malloc roi\n");
    exit(-1);
    }
if(!(num_voxels_per_cluster=malloc(sizeof*num_voxels_per_cluster*num_clusters))) {
    printf("Error: Unable to malloc num_voxels_per_cluster\n");
    exit(-1);
    }
for(i=0;i<num_clusters;i++) num_voxels_per_cluster[i] = 0;

/*START150408*/
#if 0
if(!(temp_int=malloc(sizeof*temp_int*num_clusters))) {
    printf("Error: Unalble to malloc temp_int\n");
    exit(-1);
    }
#endif

for(m=i=0;i<tcs->num_tc;i++) {
    for(j=0;j<tcs->num_tc_to_sum[i][0];j++,m++) {
        roi[m] = tcs->tc[i][0][j]-1;
        cluster[m] = i;
        voxnum[m] = i+2;
        }
    }

#if 0
printf("roi="); for(i=0;i<tcs->total;i++) printf("%d ",roi[i]); printf("\n");
printf("unsorted cluster="); for(i=0;i<tcs->total;i++) printf("%d ",cluster[i]); printf("\n");
printf("sorted cluster="); for(i=0;i<tcs->total;i++) printf("%d ",cluster[indx[i]]); printf("\n");
printf("voxnum="); for(i=0;i<tcs->total;i++) printf("%d ",voxnum[i]); printf("\n");
#endif

if(!(rbf=find_regions_by_file_cover(num_region_files,tcs->total,reg,roi))) exit(-1);
for(i=0;i<lenvol;i++) temp_float[i] = 0.;
for(k=i=0;i<tcs->total;i++) {
    num_voxels_per_cluster[cluster[i]] += rbf->nvoxels_region[i];
    for(j=0;j<rbf->nvoxels_region[i];j++,k++) temp_float[rbf->indices[k]] = (float)voxnum[i];
    }
if(!(arn=assign_region_names2init(num_clusters))) exit(-1);
if(!assign_region_names2(num_clusters,arn,ifh,num_voxels_per_cluster,(double*)NULL,(double*)NULL,(float*)NULL,
    cluster_names->files)) exit(-1);
swapbytes = shouldiswap(SunOS_Linux,ifh->bigendian);
if(get_atlas(lenvol,atlas)){
    if(!(ap=get_atlas_param(atlas,(Interfile_header*)NULL)))exit(-1);
    ifh->dim1 = ap->xdim;
    ifh->dim2 = ap->ydim;
    ifh->dim3 = ap->zdim;
    ifh->mmppix[0] = ap->mmppix[0];
    ifh->mmppix[1] = ap->mmppix[1];
    ifh->mmppix[2] = ap->mmppix[2];
    ifh->center[0] = ap->center[0];
    ifh->center[1] = ap->center[1];
    ifh->center[2] = ap->center[2];
    }
if(!writestack(cluster_file,temp_float,sizeof*temp_float,(size_t)lenvol,swapbytes)) exit(-1);
if(!write_ifh(cluster_file,ifh,(int)FALSE)) exit(-1);
printf("Output written to %s\n",cluster_file);
}
