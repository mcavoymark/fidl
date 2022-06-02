/* Copyright 2/27/02 Washington University.  All Rights Reserved.
   delete_regions.c  $Revision: 1.16 $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>
main(int argc,char **argv)
{
char string[MAXNAME],*output_file;
int i,j,k,m,n,vol,num_region_files=0,num_delete=0,*roi_delete,*roi,ntotal_regions,num_regions,*temp_int,SunOS_Linux,swapbytes;
float *temp_float;
Interfile_header *ifh;
Regions **reg;
Regions_By_File *rbf;
Files_Struct *region_files;
Length_And_Max *lam;
if(argc < 5) {
    fprintf(stderr,"        -region_file: *.4dfp.img file that specifies regions of interest.\n");
    fprintf(stderr,"        -delete:      Regions to be deleted. First region is one.\n"); 
    fprintf(stderr,"        -output_file: Undeleted regions are written to this *.4dfp.img file.\n");
    exit(-1);
    }
for(i=1;i<argc;i++) { 
    if(!strcmp(argv[i],"-region_file") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_region_files;
        if(!(region_files=get_files(num_region_files,&argv[i+1]))) exit(-1);
        i += num_region_files;
        }
    if(!strcmp(argv[i],"-delete") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_delete;
        if(!(roi_delete=malloc(sizeof*roi_delete*num_delete))) {
            printf("Error: Unable to malloc roi_delete in delete_regions\n");
            exit(-1);
            }
        for(j=0;j<num_delete;j++) roi_delete[j] = atoi(argv[++i]) - 1;
        }
    if(!strcmp(argv[i],"-output_file") && argc > i+1)
        output_file = argv[++i];
    }
if(!num_region_files) {
    fprintf(stdout,"Error: No region files specified with -region_file option.\n");
    exit(-1);
    }
if(!num_delete) {
    fprintf(stdout,"Error: No regions specified to be deleted with -regions_of_interest option.\n");
    exit(-1);
    }
if((SunOS_Linux=checkOS())==-1) exit(-1);
if(!(reg=malloc(sizeof*reg*num_region_files))) {
    printf("Error: Unable to malloc reg in delete_regions\n");
    exit(-1);
    }
for(ntotal_regions=m=0;m<num_region_files;m++) {
    if(!(reg[m]=extract_regions(region_files->files[m],0,0,(float*)NULL,0,SunOS_Linux,(char**)NULL))) exit(-1);
    ntotal_regions += reg[m]->nregions;
    }
if(!(ifh=read_ifh(region_files->files[num_region_files-1],(Interfile_header*)NULL))) exit(-1);
vol = ifh->dim1*ifh->dim2*ifh->dim3;

/*START110916*/
swapbytes = shouldiswap(SunOS_Linux,ifh->bigendian);

free_ifhregnames(ifh);
num_regions = ntotal_regions - num_delete;
if(!(roi=malloc(sizeof*roi*num_regions))) {
    printf("Error: Unable to malloc roi in delete_regions\n");
    exit(-1);
    } 
for(j=k=m=0;m<ntotal_regions;m++) {
    if(m != roi_delete[k]) {
        roi[j++] = m;
        }
    else {
        if(k!=(num_delete-1)) k++;
        }
    }
if(!(temp_float=malloc(sizeof*temp_float*vol))) {
    printf("Error: Unable to malloc temp_float in delete_regions\n");
    exit(-1);
    }
for(i=0;i<vol;i++) temp_float[i]=0.;

#if 0
printf("ntotal_regions=%d\n",ntotal_regions);
printf("roi_delete="); for(i=0;i<num_delete;i++) printf("%d ",roi_delete[i]); printf("\n");
printf("roi="); for(i=0;i<num_regions;i++) printf("%d ",roi[i]); printf("\n");
#endif

if(!(rbf=find_regions_by_file_cover(num_region_files,num_regions,reg,roi))) exit(-1);
for(k=i=0;i<num_regions;i++) for(j=0;j<rbf->nvoxels_region[i];j++,k++) temp_float[rbf->indices[k]] = (float)(i+2);
if(!(temp_int=malloc(sizeof*temp_int*num_regions))) {
    printf("Error: Unable to malloc temp_int\n");
    exit(-1);
    }
for(i=0;i<num_regions;i++) {
    sprintf(string,"%d %s %d",i,rbf->region_names_ptr[i],rbf->nvoxels_region[i]);
    temp_int[i] = strlen(string) + 1;
    }
if(!(ifh->region_names=d2charvar(num_regions,temp_int))) return 0;
ifh->nregions = num_regions;
for(i=0;i<num_regions;i++) sprintf(ifh->region_names[i],"%d %s %d",i,rbf->region_names_ptr[i],rbf->nvoxels_region[i]);
free(temp_int);

/*if(!(writestack(output_file,(float*)temp_float,sizeof(float),(size_t)vol,SunOS_Linux))) exit(-1);*/
/*START110916*/
if(!(writestack(output_file,(float*)temp_float,sizeof(float),(size_t)vol,swapbytes))) exit(-1);

ifh->global_max = (float)(num_regions+1);
ifh->global_min = 0.;
ifh->smoothness = (float)UNSAMPLED_VOXEL;
ifh->box_correction = (float)UNSAMPLED_VOXEL;
ifh->number_format = (int)FLOAT_IF;

free(ifh->file_name);
for(j=i=0;i<region_files->nfiles;i++) j += region_files->strlen_files[i]+1;
j++;
if(!(ifh->file_name=malloc(sizeof*ifh->file_name*j))) {
    printf("Error: Unable to malloc ifh->file_name in delete_regions\n");
    exit(-1);
    }
ifh->file_name[0] = 0;
for(i=0;i<region_files->nfiles;i++) {
    strcat(ifh->file_name,region_files->files[i]);
    strcat(ifh->file_name," "); 
    }
if(!write_ifh(output_file,ifh,FALSE)) exit(-1); 
printf("Output written to %s\n",output_file);
}
