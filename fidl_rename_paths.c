/* Copyright 7/22/09 Washington University.  All Rights Reserved.
   fidl_rename_paths.c  $Revision: 1.2 $*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>

main(int argc,char **argv)
{
char glm_tmp_file[MAXNAME],string[MAXNAME];
int i,j,k,SunOS_Linux,swapbytes,vol,start_b,num_glm_files=0,nconc=0,nev=0;
float *temp_float;
FILE *fp,*op;
LinearModel *glm;
Files_Struct *glm_files,*conc,*ev;
if(argc < 7) {
    fprintf(stderr,"        -glm_files: *.glm\n");
    fprintf(stderr,"        -conc:      These labels will replace the data file labels already present in the glm.\n");
    fprintf(stderr,"        -ev:        These labels will replace the event file labels already present in the glm.\n");
    exit(-1);
    }
for(i=1;i<argc;i++) { 
    if(!strcmp(argv[i],"-glm_files") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_glm_files;
        if(!(glm_files=get_files(num_glm_files,&argv[i+1]))) exit(-1);
        i += num_glm_files;
        }
    if(!strcmp(argv[i],"-conc") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nconc;
        if(!(conc=get_files(nconc,&argv[i+1]))) exit(-1);
        i += nconc;
        }
    if(!strcmp(argv[i],"-ev") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nev;
        if(!(ev=get_files(nev,&argv[i+1]))) exit(-1);
        i += nev;
        }
    }
if((SunOS_Linux=checkOS())==-1) exit(-1);
if(!num_glm_files) {
    printf("Error: No -glm_files specified. Abort!\n");
    exit(-1);
    }
if(!nconc&&!nev) {
    printf("Need to specify concs with -conc and/or event files with -ev\nnconc =%d nev = %d\n",nconc,nev);
    exit(-1);
    } 
if(nconc&&(nconc!=num_glm_files)) {
    printf("nonc = %d  nev = %d\n",nconc,num_glm_files);
    exit(-1);
    }
if(nev&&(nev!=num_glm_files)) {
    printf("nev = %d  num_glm_files = %d\n",nev,num_glm_files);
    exit(-1);
    }
sprintf(glm_tmp_file,".fidl_tmp%s.glm",make_timestr());
/*printf("glm_tmp_file=%s\n",glm_tmp_file); fflush(stdout);*/
for(i=0;i<num_glm_files;i++) {
    if(!(glm=read_glm(glm_files->files[i],0,SunOS_Linux))) exit(-1);
    swapbytes=shouldiswap(SunOS_Linux,glm->ifh->bigendian);
    if(nconc) {
        if(glm->ifh->file_name)free(glm->ifh->file_name);
        if(!(glm->ifh->file_name=malloc(sizeof*glm->ifh->file_name*(conc->strlen_files[i]+1)))) {
            printf("Error: Unable to malloc ifh->file_name\n");
            exit(-1); 
            }
        strcpy(glm->ifh->file_name,conc->files[i]);
        }
    if(nev) {
        if(glm->ifh->file_name)free(glm->ifh->glm_event_file);
        if(!(glm->ifh->glm_event_file=malloc(sizeof*glm->ifh->glm_event_file*(ev->strlen_files[i]+1)))) {
            printf("Error: Unable to malloc glm->ifh->glm_event_file\n");
            exit(-1); 
            }
        strcpy(glm->ifh->glm_event_file,ev->files[i]);
        }
    vol = glm->ifh->glm_xdim*glm->ifh->glm_ydim*glm->ifh->glm_zdim;
    start_b = find_b(glm); /*This has to be before the write_glm call.*/
                           /*The text information has changed through the versions.*/
    if(!write_glm(glm_tmp_file,glm,(int)WRITE_GLM_THRU_FZSTAT,vol,0,swapbytes,glm->ifh->bigendian)) {
        printf("Error writing WRITE_GLM_THRU_FZSTAT to %s\n",glm_tmp_file);
        exit(-1);
        }
    if(!write_glm(glm_tmp_file,glm,(int)WRITE_GLM_GRAND_MEAN,vol,0,swapbytes,glm->ifh->bigendian)) {
        fprintf(stderr,"Error writing WRITE_GLM_GRAND_MEAN to %s\n",glm_tmp_file);
        exit(-1);
        }
    if(!(op=fopen(glm_tmp_file,"r+"))) {
        printf("Could not open %s in fidl_rename_effects for writing.\n",glm_tmp_file);
        exit(-1);
        }
    if(fseek(op,0,SEEK_END)) {
        printf("Error seeking to end of %s.\n",glm_tmp_file);
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
    free_glm(glm,0);
    sprintf(string,"mv -f %s %s",glm_tmp_file,glm_files->files[i]);
    if(system(string) == -1) {
        printf("Error: %s\n",string);
        exit(-1);
        }
    printf("Output written to %s\n",glm_files->files[i]);fflush(stdout);
    }
}
