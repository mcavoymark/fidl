/* Copyright 6/14/06 Washington University.  All Rights Reserved.
   fidl_rename_contrasts.c  $Revision: 1.5 $*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>

main(int argc,char **argv)
{
char **newnames,*glm_tmp_file="tmp.glm",string[MAXNAME],*strptr;

int i,j,k,argc_c,vol,start_b,SunOS_Linux,num_glm_files=0,num_contrasts=0,nc_names=0,swapbytes,*temp_int;

float *temp_float;

FILE *fp,*op;
LinearModel *glm;
Files_Struct *glm_files,*c_names;
TCnew *con;

if(argc < 7) {
    fprintf(stderr,"        -glm_files: *.glm files\n\n");
    fprintf(stderr,"        -contrasts: Contrasts to be renamed. The first contrast is 1.\n");
    fprintf(stderr,"        -c_names:  Contrasts will have these names. One for each contrast.\n");
    exit(-1);
    }
for(i=1;i<argc;i++) { 
    if(!strcmp(argv[i],"-glm_files") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_glm_files;
        if(!(glm_files=get_files(num_glm_files,&argv[i+1]))) exit(-1);
        i += num_glm_files;
        }
    if(!strcmp(argv[i],"-contrasts") && argc > i+1) {
        for(j=1;i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_contrasts;
        argc_c = i+1;
        i += num_contrasts;
        }
    if(!strcmp(argv[i],"-c_names") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nc_names;
        if(!(c_names=get_files(nc_names,&argv[i+1]))) exit(-1);
        i += nc_names;
        }
    }
if((SunOS_Linux=checkOS())==-1) exit(-1);
if(!num_glm_files) {
    printf("Error: No -glm_files specified. Abort!\n");
    exit(-1);
    }
if(!num_contrasts) {
    printf("Error: No -contrasts specified. Abort!\n");
    exit(-1);
    }
if(!nc_names) {
    printf("Error: No -c_names specified. Avort!\n");
    exit(-1);
    }

if(!(temp_int=malloc(sizeof*temp_int*num_contrasts))) {
    printf("Error: Unable to malloc temp_int\n");
    exit(-1);
    }
for(i=0;i<num_contrasts;i++) temp_int[i] = num_glm_files;
if(!(con=read_tc_string_TCnew(num_contrasts,temp_int,argc_c,argv,'+'))) exit(-1);
free(temp_int);


for(i=0;i<num_glm_files;i++) {
    if(!(glm=read_glm(glm_files->files[i],(int)FALSE,SunOS_Linux))) exit(-1);
    swapbytes=shouldiswap(SunOS_Linux,glm->ifh->bigendian);

    for(j=0;j<num_contrasts;j++) glm->lcontrast_labels[(int)con->tc[con->eachi[j]+i]-1] = c_names->strlen_files[j]; 

    /*printf("glm->lcontrast_labels="); for(j=0;j<glm->nc;j++) printf("%d ",glm->lcontrast_labels[j]); printf("\n");*/


    #if 0
    for(max_length=j=0;j<glm->ifh->glm_nc;j++) if(glm->lcontrast_labels[j]>max_length) max_length = glm->lcontrast_labels[j];
    if(!(temp_int=malloc(sizeof*temp_int*glm->ifh->glm_nc))) {
        printf("Error: Unable to malloc temp_int\n");
        exit(-1);
        }
    for(j=0;j<glm->ifh->glm_nc;j++) temp_int[j] = max_length;
    if(!(newnames=d2charvar(glm->ifh->glm_nc,temp_int))) exit(-1);
    free(temp_int);
    for(j=0;j<glm->ifh->glm_nc;j++) strcpy(newnames[j],glm->contrast_labels[j]);
    for(j=0;j<num_contrasts;j++) strcpy(newnames[(int)con->tc[con->eachi[j]+i]-1],c_names->files[j]);
    for(j=0;j<num_contrasts;j++) printf("Information: Old label = %s  New label = %s\n",
        glm->contrast_labels[(int)con->tc[con->eachi[j]+i]-1],c_names->files[j]);
    free(glm->contrast_labels[0]);
    free(glm->contrast_labels);
    if(!(glm->contrast_labels=malloc(sizeof*glm->contrast_labels*glm->ifh->glm_nc))) {
        printf("Error: Unable to allocate glm->contrast_labels\n");
        exit(-1);
        }
    for(j=0;j<glm->ifh->glm_nc;j++) glm->contrast_labels[j] = newnames[j];
    #endif

    if(!(newnames=d2charvar(glm->ifh->glm_nc,glm->lcontrast_labels))) exit(-1);
    for(k=j=0;j<glm->ifh->glm_nc;j++) {
        strptr = j==((int)con->tc[con->eachi[k]+i]-1) ? c_names->files[k++] : glm->contrast_labels[j];
        strcpy(newnames[j],strptr);
        printf("newnames[%d]=%s\n",j,newnames[j]);
        }
    free_d2charvar(glm->contrast_labels);
    /*glm->contrast_labels = newnames;*/

    if(!(glm->contrast_labels=d2charvar(glm->ifh->glm_nc,glm->lcontrast_labels))) exit(-1);
    for(j=0;j<glm->ifh->glm_nc;j++) strcpy(glm->contrast_labels[j],newnames[j]);


    vol = glm->ifh->glm_xdim*glm->ifh->glm_ydim*glm->ifh->glm_zdim;
    start_b = find_b(glm); /*This has to be before the write_glm call.*/
                           /*The text information has changed through the versions.*/

    if(!write_glm(glm_tmp_file,glm,(int)WRITE_GLM_THRU_FZSTAT,vol,0,swapbytes)) {
        printf("Error writing WRITE_GLM_THRU_FZSTAT to %s\n",glm_tmp_file);
        exit(-1);
        }
    if(!write_glm(glm_tmp_file,glm,(int)WRITE_GLM_GRAND_MEAN,vol,0,swapbytes)) {
        fprintf(stderr,"Error writing WRITE_GLM_GRAND_MEAN to %s\n",glm_tmp_file);
        exit(-1);
        }
    if(!(op=fopen(glm_tmp_file,"r+"))) {
        printf("Could not open %s in fix_grand_mean for writing.\n",glm_tmp_file);
        exit(-1);
        }
    if(fseek(op,0,SEEK_END)) {
        printf("Error seeking to en of %s.\n",glm_tmp_file);
        exit(-1);
        }
    if(!(fp=fopen(glm_files->files[i],"r"))) {
        printf("Could not open %s for reading.\n",glm_files[i]);
        exit(-1);
        }
    if(fseek(fp,start_b,SEEK_SET)) {
        printf("Error seeking to %d in %s.\n",start_b,glm_files[i]);
        exit(-1);
        }
    if(!(temp_float=malloc(sizeof*temp_float*vol))) {
        printf("Error: Unable to malloc temp_float\n");
        exit(-1);
        }
    for(j=0;j<glm->ifh->glm_Mcol;j++) {
        if(fread(temp_float,sizeof(float),vol,fp) != vol) {
            printf("Error reading parameter estimates from %s.\n",glm_files[i]);
            exit(-1);
            }
        if(fwrite(temp_float,sizeof(float),vol,op) != vol) {
            fprintf(stderr,"Could not write estimate j=%d to %s\n",j,glm_tmp_file);
            exit(-1);
            }
        }
    fclose(fp);
    fclose(op);
    free(temp_float);
    free_d2charvar(newnames);
    free_glm(glm,(int)FALSE);

    sprintf(string,"mv -f %s %s",glm_tmp_file,glm_files->files[i]);
    if(system(string) == -1) {
        printf("Error: %s\n",string);
        exit(-1);
        }
    printf("    Output written to %s\n\n",glm_files->files[i]);
    }
}
