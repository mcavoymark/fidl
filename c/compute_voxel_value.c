/* Copyright 7/18/02 Washington University.  All Rights Reserved.
   compute_voxel_value.c  $Revision: 1.9 $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "fidl.h"
main(int argc,char **argv)
{
char filename[MAXNAME],*mode="a",atlas[7];
int i,j,lenvol,num_mcomp_files=0,num_af3d_files=0,num_output_names=0,SunOS_Linux;
float threshold=0,*temp_float;
Interfile_header *ifh;
Af3d **af3d;
Files_Struct *mcomp_files,*af3d_files,*output_names;
Atlas_Param *ap;
FILE *fp;

if(argc < 9) {
    fprintf(stderr,"Usage: compute_voxel_value\n");
    fprintf(stderr,"        -image_files:         4dfp images.\n");
    fprintf(stderr,"        -point_files:         One for each image. Can be an af3d file or a simple list of points.\n");
    fprintf(stderr,"        -output_names:        Names of output text files. One for each mcomp file.\n");
    fprintf(stderr,"                              If none are included, the af3d file is appended with '.txt'.\n");
    fprintf(stderr,"        -threshold:           Atlas coordinates whose value are less than this threshold are ignored.\n");
    fprintf(stderr,"                              The default is 0.\n");
    fprintf(stderr,"        -append or -overwrite Append or overwrite output_name. Append is the default.\n");
    exit(-1);
    }
for(i=1;i<argc;i++) { 
    if(!strcmp(argv[i],"-image_files") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_mcomp_files;
        if(!(mcomp_files=get_files(num_mcomp_files,&argv[i+1]))) exit(-1);
        i += num_mcomp_files;
        }
    if(!strcmp(argv[i],"-point_files") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_af3d_files;
        if(!(af3d_files=get_files(num_af3d_files,&argv[i+1]))) exit(-1);
        i += num_af3d_files;
        }
    if(!strcmp(argv[i],"-output_names") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_output_names;
        if(!(output_names=get_files(num_output_names,&argv[i+1]))) exit(-1);
        i += num_output_names;
        }
    if(!strcmp(argv[i],"-threshold") && argc > i+1) {
        threshold = atof(argv[++i]);
        }
    if(!strcmp(argv[i],"-overwrite")) {
        if(!(mode=malloc(sizeof*mode*2))) {
            printf("Error: Unable to malloc mode\n");
            exit(-1);
            }
        strcpy(mode,"w");
        }
    }
if(num_mcomp_files != num_af3d_files) {
    printf("num_mcomp_files = %d  num_af3d_files = %d  Should be equal. Abort!\n");
    exit(-1);
    }
if((SunOS_Linux=checkOS())==-1) exit(-1);
if(!(af3d=malloc(sizeof*af3d*num_mcomp_files))) {
    printf("Error: Unable to malloc af3d\n");
    exit(-1);
    }
for(i=0;i<num_mcomp_files;i++) {
    if(!(ifh=read_ifh(mcomp_files->files[i],(Interfile_header*)NULL))) exit(-1);
    lenvol = ifh->dim1*ifh->dim2*ifh->dim3;
    if(!(temp_float=malloc(sizeof*temp_float*lenvol))) {
        printf("Error: Unable to malloc temp_float\n");
        exit(-1);
        }
    get_atlas(lenvol,atlas);
    if(!(ap=get_atlas_param(atlas,(Interfile_header*)NULL))) exit(-1);
    if(!readstack(mcomp_files->files[i],(float*)temp_float,sizeof(float),(size_t)lenvol,SunOS_Linux,ifh->bigendian)) exit(-1);
    if(!(af3d[i]=read_af3d(af3d_files->files[i],ap,threshold,"af3d"))) exit(-1); 
    if(!num_output_names) {
        sprintf(filename,"%s.txt",af3d_files->files[i]);
        }
    else {
        strcpy(filename,output_names->files[i]);
        }
    if(!(fp=fopen_sub(filename,mode))) exit(-1);
    fprintf(fp,"image file : %s\npoints file : %s\n",mcomp_files->files[i],af3d_files->files[i]);                   
    printf("image file : %s\npoints file : %s\n",mcomp_files->files[i],af3d_files->files[i]);                   
    fprintf(fp,"threshold applied to points file = %f\n\n",threshold);
    printf("threshold applied to points file = %f\n\n",threshold);

    for(j=0;j<af3d[i]->nindex;j++) {
        /*fprintf(fp,"%6.1f %6.1f %6.1f    %6.1f\n",af3d[i]->coordinates[j][0],af3d[i]->coordinates[j][1],
            af3d[i]->coordinates[j][2],temp_float[af3d[i]->index[j]]);
        fprintf(stdout,"%6.1f %6.1f %6.1f    %6.1f\n",af3d[i]->coordinates[j][0],af3d[i]->coordinates[j][1],
            af3d[i]->coordinates[j][2],temp_float[af3d[i]->index[j]]);*/
        fprintf(fp,"%6.1f %6.1f %6.1f    %g\n",af3d[i]->coordinates[j][0],af3d[i]->coordinates[j][1],
            af3d[i]->coordinates[j][2],temp_float[af3d[i]->index[j]]);
        fprintf(stdout,"%6.1f %6.1f %6.1f    %g\n",af3d[i]->coordinates[j][0],af3d[i]->coordinates[j][1],
            af3d[i]->coordinates[j][2],temp_float[af3d[i]->index[j]]);
        }
    fprintf(fp,"\n");
    fclose(fp);
    free_ifh(ifh,(int)FALSE);
    free(temp_float);
    }
}
