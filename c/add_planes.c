/* Copyright 3/28/02 Washington University.  All Rights Reserved.
   add_planes.c  $Revision: 1.5 $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <fidl.h>

static char rcsid[] = "$Header: /home/hannah/mcavoy/idl/clib/RCS/add_planes.c,v 1.5 2003/04/23 21:56:25 mcavoy Exp $";

/************************/
main(int argc,char **argv)
/************************/
{
char **input_files,**output_names,filename[MAXNAME],*str_ptr;

int i,j,lenvol,area,num_input_files=0,num_output_names=0;

float *temp_float;

Interfile_header *ifh;

print_version_number(rcsid);
if (argc < 3) {
    fprintf(stderr,"    A plane of zeros is entered before the first slice and a second plane after the last slice.\n");
    fprintf(stderr,"        -input_files:  List of files.\n");
    fprintf(stderr,"        -output_names: Stacks are written to these names.\n");
    fprintf(stderr,"                       If no names are provided, then the default is <filename>_add_planes.4dfp.img\n");
    exit(-1);
    }

for(i=1;i<argc;i++) { 
    if(!strcmp(argv[i],"-input_files") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_input_files;
        GETMEM(input_files,num_input_files,char *);
        for(j=0;j<num_input_files;j++) {
            GETMEM(input_files[j],strlen(argv[i+1+j])+1,char);
            strcpy(input_files[j],argv[i+1+j]);
            }
        i += num_input_files;
        }
    if(!strcmp(argv[i],"-output_names") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_output_names;
        GETMEM(output_names,num_output_names,char *);
        for(j=0;j<num_output_names;j++) {
            GETMEM(output_names[j],strlen(argv[i+1+j])+1,char);
            strcpy(output_names[j],argv[i+1+j]);
            }
        i += num_output_names;
        }
    }
if(!num_input_files) {
    fprintf(stdout,"Error: No input files. Abort!\n");
    exit(-1);
    }
if(num_output_names) {
    if(num_output_names != num_input_files) {
        printf("Error: Need to provide the same number of output_names as input_files. Abort!\n");
        exit(-1);
        }
    }

for(i=0;i<num_input_files;i++) {
    if(!(ifh = read_ifh(input_files[i]))) exit(-1);
    area = ifh->dim1*ifh->dim2;
    lenvol = ifh->dim1*ifh->dim2*ifh->dim3;
    GETMEM_0(temp_float,lenvol+2*area,float);
    if(!read_float(input_files[i],&temp_float[area],lenvol)) exit(-1);

    if(num_output_names) {
        strcpy(filename,output_names[i]);
        }
    else {
        *strstr(input_files[i],".4dfp.img") = 0;
        if(str_ptr=strrchr(input_files[i],'/')) {
            str_ptr++;
            }
        else {
            str_ptr = input_files[i];
            }
        sprintf(filename,"%s_add_planes.4dfp.img",str_ptr);
        }

    if(!write_float(filename,temp_float,lenvol+2*area)) exit(-1);
    fprintf(stdout,"Output written to %s\n",filename);
    min_and_max(temp_float,lenvol,&ifh->global_min,&ifh->global_max);
    ifh->dim3 = ifh->dim3 + 2;
    if(!write_ifh(filename,ifh,(int)FALSE)) exit(-1);

    free(temp_float);
    free_ifh(ifh,(int)FALSE);
    }

} /*END*/
