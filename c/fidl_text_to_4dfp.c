/* Copyright 6/25/10 Washington University.  All Rights Reserved.
   fidl_text_to_4dfp.c  $Revision: 1.4 $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>

static char rcsid[] = "$Header: /home/hannah/mcavoy/idl/clib/RCS/fidl_text_to_4dfp.c,v 1.4 2010/11/24 21:02:05 mcavoy Exp $";

main(int argc,char **argv)
{
char *file=NULL,*out=NULL,line[10000],*strptr;
int i,j,k,nfiles=0,SunOS_Linux,num_region_files=0,num_regions=0,*roi,volreg,swapbytes,firstlinelabels;
float *temp_float,max,min;
Files_Struct *region_files;
Regions **reg;
Regions_By_File *rbf;
Data *data;
Interfile_header *ifh;
FILE *fp;

print_version_number(rcsid,stdout);
if(argc < 7) {
    fprintf(stderr,"    -file:                Text file. Each col is a region.\n");
    fprintf(stderr,"    -region_file:         Voxels were pulled from regions in this file.\n"); 
    fprintf(stderr,"    -regions_of_interest: First region is one.\n");
    fprintf(stderr,"    -output:              Output filename.\n"); 
    exit(-1);
    }
for(i=1;i<argc;i++) { 
    if(!strcmp(argv[i],"-file") && argc > i+1)
        file = argv[++i];
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
    if(!strcmp(argv[i],"-out") && argc > i+1)
        out = argv[++i];
    }
if(!file) {
    printf("Error: No text file specified with -file option. Abort!\n");
    exit(-1);
    }
if(!num_region_files) {
    printf("Error: No region file specified with -region_file option. Abort!\n");
    exit(-1);
    }
if(!num_regions) {
    printf("Error: No regions specified with -regions_of_interest option. Abort!\n");
    exit(-1);
    }
if(!out) {
    printf("Error: No output file specified with -out option. Abort!\n");
    exit(-1);
    }
if((SunOS_Linux=checkOS())==-1) exit(-1);

/*if(!(data=read_data(file,0,0,num_regions,0))) exit(-1);*/
/*START14*/
if(!(fp=fopen_sub(file,"r"))) exit(-1);
fgets(line,sizeof(line),fp);
for(firstlinelabels=0,strptr=line;*strptr;strptr++) if(isalpha(*strptr)) {firstlinelabels=1;break;} 
printf("firstlinelabels=%d\n",firstlinelabels); fflush(stdout);
fclose(fp);
if(!(data=read_data(file,0,0,num_regions,firstlinelabels))) exit(-1); 

if(!(ifh = read_ifh(region_files->files[0],(Interfile_header*)NULL))) exit(-1);
volreg = ifh->dim1*ifh->dim2*ifh->dim3;
if(!check_dimensions(num_region_files,region_files->files,volreg)) exit(-1);
if(!(reg=malloc(sizeof*reg*num_region_files))) {
    printf("Error: Unable to malloc reg\n");
    exit(-1);
    }
for(i=0;i<num_region_files;i++)
    if(!(reg[i]=extract_regions(region_files->files[i],0,0,(float*)NULL,0,SunOS_Linux,(char**)NULL))) exit(-1);
if(!(rbf=find_regions_by_file_cover(num_region_files,num_regions,reg,roi))) exit(-1);
if(!(temp_float=malloc(sizeof*temp_float*volreg))) {
    printf("Error: Unable to malloc temp_float\n");
    exit(-1);
    }
for(i=0;i<volreg;i++) temp_float[i]=0.;
for(max=-1.e20,min=1.e20,k=i=0;i<rbf->nreg;i++) {
    for(j=0;j<rbf->nvoxels_region[i];j++,k++) {
        temp_float[rbf->indices[k]] = (float)data->x[j][i];
        if(temp_float[rbf->indices[k]]>max) max = temp_float[rbf->indices[k]];
        else if(temp_float[rbf->indices[k]]<min) min = temp_float[rbf->indices[k]];
        }
    }

if(!writestack(out,temp_float,sizeof(float),(size_t)volreg,swapbytes)) exit(-1);
free_ifhregnames(ifh);
ifh->bigendian = !SunOS_Linux ? 1 : 0;
ifh->global_max = max;
ifh->global_min = min;
if(!write_ifh(out,ifh,0)) exit(-1);
printf("Output written to %s\n",out);
exit(0);
}
