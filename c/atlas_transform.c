/* Copyright 6/21/01 Washington University.  All Rights Reserved.
   atlas_transform.c  $Revision: 1.18 $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "fidl.h"
main(int argc,char **argv)
{
char *xform_file=NULL,*directory=NULL,filename[MAXNAME],*conc_name=NULL,string[MAXNAME],*strptr,atlas[7]="",atlas_in[7]="";
int i,j,k,n,lenvol,xdim,ydim,zdim,tdim,A_or_B_or_U,num_bold_files=0,SunOS_Linux,swapbytes;
float *input_float,*temp_float,*t4=NULL,dxdy,dz;
double *temp_double,*stat,*center_in=NULL;
FILE *fp,*op;
Interfile_header *ifh;
Atlas_Param *ap,*ap_in;
Files_Struct *bold_files;
Memory_Map *mm;
Dim_Param *dp;
if (argc < 5) {
    fprintf(stderr,"Usage: atlas_transform -bold_files stack.4dfp.img -xform_file motionst19_anat_ave_to_711-2B_t4\n");
    fprintf(stderr,"        -bold_files: 4dfp stacks to be transformed to atlas space.\n");
    fprintf(stderr,"        -xform_file: 2A or 2B t4 file defining the transform to atlas space.\n");
    fprintf(stderr,"        -atlas:      Either 111, 222 or 333.\n");
    fprintf(stderr,"        -conc_name:  Conc file will be created with this name.\n");
    fprintf(stderr,"        -directory:  Specify directory for output files. Include backslash at end.\n\n");
    exit(-1);
    }
for(i=1;i<argc;i++) { 
    if(!strcmp(argv[i],"-bold_files") && argc > i+1) {
        for(j=1;i+j < argc && strchr(argv[i+j],'-') != argv[i+j];j++) ++num_bold_files;
        if(!(bold_files=get_files(num_bold_files,&argv[i+1]))) exit(-1);
        i += num_bold_files;
        }
    if(!strcmp(argv[i],"-xform_file") && argc > i+1)
        xform_file = argv[++i];
    if(!strcmp(argv[i],"-conc_name") && argc > i+1)
        conc_name = argv[++i];
    if(!strcmp(argv[i],"-atlas") && argc > i+1)
        strcpy(atlas,argv[++i]);
    if(!strcmp(argv[i],"-directory") && argc > i+1)
        directory = argv[++i];
    }
if(!xform_file) { 
    printf("fidlMessage: No -xform_file, assuming the identity matrix.\n");fflush(stdout);
    }
else {
    if(!(t4=malloc(sizeof*t4*(size_t)T4SIZE))) {
        printf("Error: Unable to malloc t4\n");
        return 0;
        }
    if(!read_xform(xform_file,t4)) exit(-1);
    if((A_or_B_or_U=twoA_or_twoB(xform_file)) == 2) {
        printf("Error: Illegally named t4 file.\n");
        exit(-1);
        }
    }
if((SunOS_Linux=checkOS())==-1) return 0;
if(!(ap=get_atlas_param(atlas,(Interfile_header*)NULL))) exit(-1);
if(!(dp=dim_param(num_bold_files,bold_files->files,SunOS_Linux,0,(int*)NULL,(int*)NULL))) exit(-1);
if(get_atlas(dp->vol,atlas_in)) {
    if(!(ap_in=get_atlas_param(atlas_in,(Interfile_header*)NULL))) exit(-1);
    center_in = ap_in->center;
    }

if(!(temp_float=malloc(sizeof*temp_float*ap->vol))) {
    printf("Error: Unable to malloc temp_float\n"); 
    exit(-1);
    }
if(!(stat=malloc(sizeof*stat*ap->vol))) {
    printf("Error: Unable to malloc stat\n");
    exit(-1);
    }
if(!(temp_double=malloc(sizeof*temp_double*dp->vol))) {
    printf("Error: Unable to malloc temp_double\n");
    exit(-1);
    }
if(conc_name) {
    if(!(op=fopen_sub(conc_name,"w"))) exit(-1);
    fprintf(op,"number_of_files:%d\n",bold_files->nfiles);
    }
for(n=0;n<num_bold_files;n++) {
    swapbytes = shouldiswap(SunOS_Linux,dp->bigendian[n]);
    strcpy(string,bold_files->files[n]);
    if(!(strptr=get_tail_sans_ext(string))) exit(-1);
    sprintf(filename,"%s%s%s.4dfp.img",directory,strptr,ap->str);
    if(!(fp = fopen_sub(filename,"w"))) exit(-1);
    if(!(mm = map_disk(bold_files->files[n],dp->tdim[n]*dp->vol,0,sizeof(float)))) exit(-1);
    for(k=i=0;i<dp->tdim[n];i++) {
        if(!swapbytes) {
            for(j=0;j<dp->vol;j++,k++) temp_double[j] = (double)mm->ptr[k];
            }
        else {
            for(j=0;j<dp->vol;j++,k++) temp_float[j] = mm->ptr[k];
            swap_bytes((unsigned char *)temp_float,sizeof(float),(size_t)dp->vol);
            for(j=0;j<dp->vol;j++) temp_double[j] = (double)temp_float[j];
            }
        if(!t4_atlas(temp_double,stat,t4,dp->xdim,dp->ydim,dp->zdim,dp->dxdy,dp->dz,A_or_B_or_U,dp->orientation,ap,center_in)) 
            exit(-1);
        for(j=0;j<ap->vol;j++) temp_float[j] = (float)stat[j];
        if(!(fwrite_sub(temp_float,sizeof(float),(size_t)ap->vol,fp,SunOS_Linux))) {
            printf("Error writing to %s. Are you out of memory? Abort!\n",filename);
            exit(-1);
            }
        }
    if(!unmap_disk(mm)) exit(-1);
    fclose(fp);
    if(!(ifh = read_ifh(bold_files->files[n],(Interfile_header*)NULL))) exit(-1);
    ifh->voxel_size_1 = ap->voxel_size[0];
    ifh->voxel_size_2 = ap->voxel_size[1];
    ifh->voxel_size_3 = ap->voxel_size[2];
    ifh->dim1 = ap->xdim;
    ifh->dim2 = ap->ydim;
    ifh->dim3 = ap->zdim;
    for(i=0;i<3;i++) {
        ifh->center[i] = ap->center[i];
        ifh->mmppix[i] = ap->mmppix[i];
        }
    ifh->bigendian = !SunOS_Linux?1:0;
    if(!write_ifh(filename,ifh,(int)FALSE)) exit(-1);
    printf("Output written to %s\n",filename);
    if(conc_name) fprintf(op,"file:%s\n",filename);
    }
if(conc_name) {
    fclose(op); 
    printf("Concatenated file written to %s\n",conc_name);
    }
}
