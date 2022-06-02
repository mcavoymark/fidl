/* Copyright 11/4/03 Washington University.  All Rights Reserved.
   fidl_reg_ass.c  $Revision: 1.8 $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>
main(int argc,char **argv)
{
char *outfile,*strptr;
int i,j,m,n,vol,num_region_files=0,num_regions=0,num_val=0,*roi,SunOS_Linux,swapbytes;
float *temp_float,*val; 
Interfile_header *ifh;
Regions **reg;
Regions_By_File *rbf;
Files_Struct *region_files;
if(argc < 9) {
    fprintf(stderr,"        -region_file:         *.4dfp.img file that specifies regions of interest.\n");
    fprintf(stderr,"        -regions_of_interest: First regions is 1. Assign values to these regions. The rest are zero.\n");
    fprintf(stderr,"        -val:                 Values to assign to regions of interest.\n");
    fprintf(stderr,"        -output:              Name of output file.\n");
    exit(-1);
    }
for(i=1;i<argc;i++) {
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

    #if 0
    if(!strcmp(argv[i],"-val") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_val;
        if(!(val=malloc(sizeof*val*num_val))) {
            printf("Error: Unable to malloc val\n");
            exit(-1);
            }
        for(j=0;j<num_val;j++) val[j] = (float)atof(argv[++i]);
        }
    #endif
    /*START130517*/
    if(!strcmp(argv[i],"-val") && argc > i+1) {
        for(j=1;i+j<argc;j++) {
            strptr = argv[i+j] + 1;
            if(*strptr == '.') strptr++;
            if(strchr(argv[i+j],'-') == argv[i+j] && !isdigit(*strptr)) break;
            ++num_val;
            }
        if(!(val=malloc(sizeof*val*num_val))) {
            printf("Error: Unable to malloc val\n");
            exit(-1);
            }
        for(j=0;j<num_val;j++) val[j] = (float)atof(argv[++i]);
        }


    if(!strcmp(argv[i],"-output") && argc > i+1)
        outfile = argv[++i];
    }
if(!num_regions) {
    printf("Error: No regions specified with -regions_of_interest option. Abort!\n");
    exit(-1);
    }
if(num_regions != num_val) {
    printf("Error: num_regions = %d num_val=%d. Must be equal. Abort!\n",num_regions,num_val);
    exit(-1);
    }
if((SunOS_Linux=checkOS())==-1) exit(-1);

if(!(ifh = read_ifh(region_files->files[0],(Interfile_header*)NULL))) exit(-1);
free_ifhregnames(ifh);
vol = ifh->dim1*ifh->dim2*ifh->dim3;
if(!check_dimensions(num_region_files,region_files->files,vol)) exit(-1);
if(!(reg=malloc(sizeof*reg*num_region_files))) {
    printf("Error: Unable to malloc reg\n");
    exit(-1);
    }
for(m=0;m<num_region_files;m++) if(!(reg[m] = extract_regions(region_files->files[m],0,0,(float*)NULL,0,SunOS_Linux,(char**)NULL))) 
    exit(-1);
if(!(rbf=find_regions_by_file_cover(num_region_files,num_regions,reg,roi))) exit(-1);;

if(!(temp_float=malloc(sizeof*temp_float*vol))) {
    printf("Error: Unable to malloc temp_float\n");
    exit(-1);
    }
for(i=0;i<vol;i++) temp_float[i] = 0.;

for(n=m=0;m<num_region_files;m++) {
    for(i=0;i<rbf->num_regions_by_file[m];i++,n++) {
        for(j=0;j<reg[m]->nvoxels_region[rbf->roi_by_file[m][i]];j++) {
            temp_float[reg[m]->voxel_indices[rbf->roi_by_file[m][i]][j]] = val[n];
            }
        }
    }

swapbytes=shouldiswap(SunOS_Linux,ifh->bigendian);
if(!writestack(outfile,temp_float,sizeof(float),(size_t)vol,swapbytes)) exit(-1);
printf("Output written to %s\n",outfile);
if(!write_ifh(outfile,ifh,(int)FALSE)) exit(-1);
exit(0);
}
