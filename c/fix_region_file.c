/* Copyright 7/26/01 Washington University.  All Rights Reserved.
   fix_region_file.c  $Revision: 1.14 $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>
main(int argc,char **argv)
{
int	i,j,num_files=0,num_new_filenames=0,vol,SunOS_Linux,swapbytes,nregname=0;
float	*temp_float,min,max;
Interfile_header *ifh;
Files_Struct *files,*new_filenames=NULL;


#if 0
if(argc < 4) {
    fprintf(stderr,"Corrects FIDL region files made from analyze that have the first region defined with a value of 1.\n");
    fprintf(stderr,"Usage: fix_region_file -files $FILES -new_filenames $NEW_FILENAMES\n");
    fprintf(stderr,"        -files:         List of region files to be fixed.\n");
    fprintf(stderr,"        -new_filenames: Filename of corrected region file.\n");
    fprintf(stderr,"                        If the same name is given, then it will be overwritten.\n");
    exit(-1);
    }
#endif
/*START121031*/
if(argc < 4) {
    fprintf(stderr,"Corrects files that have the first region defined with a value of 1.\n");
    fprintf(stderr,"    -files:         Region files to be fixed.\n");
    fprintf(stderr,"    -new_filenames: Optional.\n");
    fprintf(stderr,"    -regname:       Optional. Region names.\n");
    exit(-1);
    }

for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-files") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_files;
        if(!(files=get_files(num_files,&argv[i+1]))) exit(-1);
        i += num_files;
        }
    if(!strcmp(argv[i],"-new_filenames") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_new_filenames;
        if(!(new_filenames=get_files(num_new_filenames,&argv[i+1]))) exit(-1);
        i += num_new_filenames;
        }

    /*START121031*/
    /*KEEP*/
    #if 0
    if(!strcmp(argv[i],"-regname") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nregname;
        if(!(regname=get_files(nregname,&argv[i+1]))) exit(-1);
        i += nregname;
        }
    #endif


    }
if((SunOS_Linux=checkOS())==-1) exit(-1);

#if 0
for(i=0;i<num_files;i++) {
    if(!(ifh = read_ifh(files->files[i]))) exit(-1);
    vol = ifh->dim1*ifh->dim2*ifh->dim3;
    if(!(temp_float=malloc(sizeof*temp_float*vol))) exit(-1);
    if(!readstack(files->files[i],(float*)temp_float,sizeof*temp_float,(size_t)vol,SunOS_Linux)) exit(-1);
    min_and_max(temp_float,vol,&min,&max);
    if(max <= ifh->nregions || !ifh->nregions) {
        for(j=0;j<vol;j++) if(temp_float[j] >= 1) ++temp_float[j];
        if(!writestack(new_filenames->files[i],temp_float,sizeof*temp_float,(size_t)vol,SunOS_Linux)) exit(-1);
        printf("Output written to %s\n",new_filenames->files[i]);
        if(!ifh->nregions) assign_region_names(ifh,temp_float,vol,max,(double*)NULL,(double*)NULL,(float*)NULL);
        free(temp_float);
        if(!write_ifh(new_filenames->files[i],ifh,(int)FALSE)) exit(-1);
        free_ifh(ifh,(int)FALSE);
        }
    else {
        printf("Message: %s is ok.\n",files->files[i]);
        }
    }
#endif
for(i=0;i<num_files;i++) {
    if(!(ifh = read_ifh(files->files[i],(Interfile_header*)NULL))) exit(-1);
    vol = ifh->dim1*ifh->dim2*ifh->dim3;
    if(!(temp_float=malloc(sizeof*temp_float*vol))) exit(-1);
    if(!readstack(files->files[i],(float*)temp_float,sizeof*temp_float,(size_t)vol,SunOS_Linux)) exit(-1);
    min_and_max(temp_float,vol,&min,&max);
    if(max <= ifh->nregions || !ifh->nregions) {
        for(j=0;j<vol;j++) if(temp_float[j] >= 1) ++temp_float[j];
        swapbytes = shouldiswap(SunOS_Linux,ifh->bigendian);
        if(!writestack(new_filenames->files[i],temp_float,sizeof*temp_float,(size_t)vol,swapbytes)) exit(-1);
        if(!ifh->nregions) assign_region_names(ifh,temp_float,vol,max,(double*)NULL,(double*)NULL,(float*)NULL);
        if(!write_ifh(new_filenames->files[i],ifh,(int)FALSE)) exit(-1);
        printf("Output written to %s\n",new_filenames->files[i]);
        }
    else {
        printf("Message: %s is ok.\n",files->files[i]);
        }
    free(temp_float);
    free_ifh(ifh,(int)FALSE);
    }
}
