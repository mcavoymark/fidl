/* Copyright 12/31/99 Washington University.  All Rights Reserved.
   compute_region_stats.c  $Revision: 1.28 $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "fidl.h"
#include "center_of_mass2.h"
main(int argc,char **argv)
{
char *statistical_image=NULL,*region_center_of_mass_name="region_center_of_mass.txt",*date=NULL,atlas[7];
int i,j,k,vol,SunOS_Linux,num_region_files=0,num_regions=0,*roi;
float *stat,*peakval; /* *peakval */
/*double *coor,*peakcoor;*/
FILE *fp;
Interfile_header *ifh;
Atlas_Param *ap;
Regions **reg;
Regions_By_File *rbf;
Files_Struct *region_files;
center_of_mass2struct *com;
if (argc < 5) {
    fprintf(stderr,"Calculate the center of mass and peak for selected regions.\n\n");
    fprintf(stderr,"compute_region_stats -statistical_image all_cond_slice_ave_zstat_222_mcomp.4dfp.img \\\n");
    fprintf(stderr,"                     -region_file all_cond_good_regionsA.4dfp.img \\\n");
    fprintf(stderr,"                     -regions_of_interest 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17\n\n");
    fprintf(stderr,"        -statistical_image:          Z-statistic or magnitude image\n");
    fprintf(stderr,"        -region_center_of_mass_name: Name of output. Default is region_center_of_mass.txt\n");
    fprintf(stderr,"        -date:                       Printed at top of output.\n\n");
    fprintf(stderr,"        -region_file:                *.4dfp.img file(s) that specifies regions of interest.\n");
    fprintf(stderr,"        -regions_of_interest:        Calculate statistics for selected regions in the region file.\n");
    fprintf(stderr,"                                     First region is one.\n");
    exit(-1);
    }
for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-statistical_image") && argc > i+1)
        statistical_image = argv[++i];
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
    if(!strcmp(argv[i],"-region_center_of_mass_name") && argc > i+1)
        region_center_of_mass_name = argv[++i];
    if(!strcmp(argv[i],"-date") && argc > i+1)
        date = argv[++i];
    }
if(!statistical_image) {
    printf("Error: Need to specify -statistical_image\n");
    exit(-1);
    }
if(!num_region_files) {
    printf("Error: Need to specify -region_file\n");
    exit(-1);
    }
if((SunOS_Linux=checkOS())==-1) exit(-1);
if(!(ifh = read_ifh(statistical_image,(Interfile_header*)NULL))) exit(-1);
vol = ifh->dim1*ifh->dim2*ifh->dim3;

/*START150408*/
/*free_ifh(ifh,(int)FALSE);*/

if(!check_dimensions(num_region_files,region_files->files,vol)) exit(-1);
if(!(reg=malloc(sizeof*reg*num_region_files))) {
    printf("Error: Unable to malloc reg\n");
    exit(-1);
    }

for(i=0;i<num_region_files;i++) if(!(reg[i]=extract_regions(region_files->files[i],0,0,(float*)NULL,0,SunOS_Linux,(char**)NULL))) 
    exit(-1);

if(!num_regions) for(i=0;i<num_region_files;i++) num_regions += reg[i]->nregions;
if(!(rbf=find_regions_by_file_cover(num_region_files,num_regions,reg,roi))) exit(-1);

if(!(stat=malloc(sizeof*stat*vol))) {
    printf("Error: Unable to malloc stat\n");
    exit(-1);
    }
get_atlas(vol,atlas);
if(!(ap=get_atlas_param(atlas,(Interfile_header*)NULL))) exit(-1);
if(!readstack(statistical_image,(float*)stat,sizeof(float),(size_t)ap->vol,SunOS_Linux,ifh->bigendian)) exit(-1);

#if 0
if(!(coor=malloc(sizeof*coor*num_regions*3))) {
    printf("Error: Unable to malloc coor in fidl_threshold_cluster\n");
    exit(-1);
    }
if(!(peakcoor=malloc(sizeof*peakcoor*num_regions*3))) {
    printf("Error: Unable to malloc peakcoor in fidl_threshold_cluster\n");
    exit(-1);
    }
if(!(peakval=malloc(sizeof*peakval*num_regions))) {
    printf("Error: Unable to malloc peakval in fidl_threshold_cluster\n");
    exit(-1);
    }
if(!center_of_mass(stat,rbf,coor,ap,peakcoor,peakval)) exit(-1);
#endif
/*START150408*/
if(!(com=center_of_mass2init(rbf->nvoxels,num_regions))) exit(-1);
center_of_mass2(stat,com,rbf->nvoxels,rbf->indices,num_regions,rbf->nvoxels_region,ap);

if(!(fp=fopen_sub(region_center_of_mass_name,"w"))) exit(-1);
if(date) fprintf(fp,"%s\n",date);
fprintf(fp,"statistical image: %s\n",statistical_image);
fprintf(fp,"region file: %s\n",region_files->files[0]);
for(i=1;i<num_region_files;i++) fprintf(fp,"             %s\n",region_files->files[i]);
fprintf(fp,"\n");
for(j=k=i=0;i<num_regions;i++,j+=3,k+=3) {
    fprintf(fp,"REGION : %s %d\n",rbf->region_names_ptr[i],rbf->nvoxels_region[i]);
    fprintf(fp,"CENTER OF MASS\n");

    #if 0
    fprintf(fp,"    atlas coordinates: %.1f %.1f %.1f\n",coor[j],coor[j+1],coor[j+2]);
    if(statistical_image) {
        fprintf(fp,"PEAK : %f\n",peakval[i]);
        fprintf(fp,"    atlas coordinates: %.1f %.1f %.1f\n",peakcoor[k],peakcoor[k+1],peakcoor[k+2]);
        }
    #endif
    /*START150408*/
    fprintf(fp,"    atlas coordinates: %.1f %.1f %.1f\n",com->coor[j],com->coor[j+1],com->coor[j+2]);
    if(statistical_image) {
        fprintf(fp,"PEAK : %f\n",com->peakval[i]);
        fprintf(fp,"    atlas coordinates: %.1f %.1f %.1f\n",com->peakcoor[k],com->peakcoor[k+1],com->peakcoor[k+2]);
        }

    fprintf(fp,"\n");
    }
fclose(fp);
printf("Output witten to %s\n",region_center_of_mass_name);fflush(stdout);
}
