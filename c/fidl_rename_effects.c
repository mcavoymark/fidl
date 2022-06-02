/* Copyright 7/22/09 Washington University.  All Rights Reserved.
   fidl_rename_effects.c  $Revision: 1.11 $*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <math.h>
#include "fidl.h"
#include "subs_util.h"
#include "shouldiswap.h"
#include "write_glm.h"
int main(int argc,char **argv)
{
char **newnames,glm_tmp_file[MAXNAME],string[MAXNAME],*strptr,timestr[23],*log=NULL;
int i,j,k,argc_c=0,start_b,SunOS_Linux,num_glm_files=0,num_contrasts=0,swapbytes,*temp_int,neffect_labels=0,vol;
float *temp_float;
FILE *fp,*op,*flog;
LinearModel *glm;
Files_Struct *glm_files=NULL,*effect_labels=NULL;
TCnew *con;
if(argc < 7) {
    fprintf(stderr,"        -glm_files:     *.glm\n");
    fprintf(stderr,"        -contrasts:     Identify effects to be renamed. The first effect is 1.\n");
    fprintf(stderr,"        -effect_labels: Effects will have these names.\n");
    fprintf(stderr,"        -log:           Name of log file.\n");
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
    if(!strcmp(argv[i],"-effect_labels") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++neffect_labels;
        if(!(effect_labels=get_files(neffect_labels,&argv[i+1]))) exit(-1);
        i += neffect_labels;
        }
    if(!strcmp(argv[i],"-log") && argc > i+1)
        log = argv[++i];
    }
if((SunOS_Linux=checkOS())==-1) exit(-1);
if(log){if(!(flog=fopen_sub(log,"w")))exit(-1);} else flog=stdout;
if(!num_glm_files) {fprintf(flog,"fidlError: No -glm_files specified. Abort!\n");fflush(stdout);exit(-1);}
if(!num_contrasts) {fprintf(flog,"fidlError: No -contrasts specified. Abort!\n");fflush(flog);exit(-1);}
if(!neffect_labels) {fprintf(flog,"fidlError: No -effect_labels specified. Abort!\n");fflush(flog);exit(-1);}
if(!(temp_int=malloc(sizeof*temp_int*num_contrasts))) {fprintf(flog,"fidlError: Unable to malloc temp_int\n");fflush(flog);exit(-1);}
for(i=0;i<num_contrasts;i++) temp_int[i] = num_glm_files;
if(!(con=read_tc_string_TCnew(num_contrasts,temp_int,argc_c,argv,'+'))) exit(-1);
free(temp_int);

//sprintf(glm_tmp_file,".fidl_tmp%s.glm",make_timestr(timestr));
//START170330
sprintf(glm_tmp_file,"fidl%s.glm",make_timestr2(timestr));

fprintf(flog,"glm_tmp_file=%s\n",glm_tmp_file);
for(i=0;i<num_glm_files;i++) {
    if(!(glm=read_glm(glm_files->files[i],0,SunOS_Linux)))exit(-1);
    swapbytes=shouldiswap(SunOS_Linux,glm->ifh->bigendian);
    for(j=0;j<num_contrasts;j++) glm->ifh->glm_leffect_label[(int)con->tc[con->eachi[j]+i]-1] = effect_labels->strlen_files[j]; 
    if(!(newnames=d2charvar(glm->ifh->glm_all_eff,effect_labels->strlen_files))) exit(-1);
    for(k=j=0;j<glm->ifh->glm_all_eff;j++) {
        strptr = j==((int)con->tc[con->eachi[k]+i]-1) ? effect_labels->files[k++] : glm->ifh->glm_effect_label[j];
        strcpy(newnames[j],strptr);
        fprintf(flog,"newnames[%d]=%s %d\n",j,newnames[j],glm->ifh->glm_leffect_label[j]);
        }
    free_d2charvar(glm->ifh->glm_effect_label);
    if(!(glm->ifh->glm_effect_label=d2charvar(glm->ifh->glm_all_eff,effect_labels->strlen_files))) exit(-1);
    for(j=0;j<glm->ifh->glm_all_eff;j++) strcpy(glm->ifh->glm_effect_label[j],newnames[j]);
    vol = glm->ifh->glm_xdim*glm->ifh->glm_ydim*glm->ifh->glm_zdim;
    start_b = find_b(glm); /*This has to be before the write_glm call.*/
                           /*The text information has changed through the versions.*/
    if(!write_glm(glm_tmp_file,glm,(int)WRITE_GLM_THRU_FZSTAT,vol,swapbytes)) {
        fprintf(flog,"fidlError: writing WRITE_GLM_THRU_FZSTAT to %s\n",glm_tmp_file);
        exit(-1);
        }

    //if(!write_glm_grand_mean(glm_tmp_file,glm,vol,0,swapbytes))exit(-1);
    //START170330
    if(!write_glm_grand_mean(glm_tmp_file,glm,glm->nmaski,0,swapbytes))exit(-1);


    if(!(op=fopen(glm_tmp_file,"r+"))) {
        fprintf(flog,"fidlError: Could not open %s in fidl_rename_effects for writing.\n",glm_tmp_file);
        exit(-1);
        }
    if(fseek(op,0,SEEK_END)) {
        fprintf(flog,"fidlError: seeking to end of %s.\n",glm_tmp_file);
        exit(-1);
        }
    if(!(fp=fopen(glm_files->files[i],"r"))) {
        fprintf(flog,"fidlError: Could not open %s for reading.\n",glm_files->files[i]);
        exit(-1);
        }
    if(fseek(fp,start_b,SEEK_SET)) {
        fprintf(flog,"fidlError: seeking to %d in %s.\n",start_b,glm_files->files[i]);
        exit(-1);
        }

    #if 0
    if(!(temp_float=malloc(sizeof*temp_float*vol))) {
        fprintf(flog,"fidlError: Unable to malloc temp_float\n");
        exit(-1);
        }
    for(j=0;j<glm->ifh->glm_Mcol;j++) {
        if(fread(temp_float,sizeof(float),vol,fp)!=(size_t)vol) {
            fprintf(flog,"fidlError: reading parameter estimates from %s.\n",glm_files->files[i]);
            exit(-1);
            }
        if(fwrite(temp_float,sizeof(float),vol,op)!=(size_t)vol) {
            fprintf(flog,"fidlError: Could not write estimate j=%d to %s\n",j,glm_tmp_file);
            exit(-1);
            }
        }
    #endif
    //START173030
    if(!(temp_float=malloc(sizeof*temp_float*glm->nmaski))){
        fprintf(flog,"fidlError: Unable to malloc temp_float\n");
        exit(-1);
        }
    for(j=0;j<glm->ifh->glm_Mcol;j++){
        if(fread(temp_float,sizeof(float),glm->nmaski,fp)!=(size_t)glm->nmaski){
            fprintf(flog,"fidlError: reading parameter estimates from %s.\n",glm_files->files[i]);
            exit(-1);
            }
        if(fwrite(temp_float,sizeof(float),glm->nmaski,op)!=(size_t)glm->nmaski){
            fprintf(flog,"fidlError: Could not write estimate j=%d to %s\n",j,glm_tmp_file);
            exit(-1);
            }
        }

    fclose(fp);
    fclose(op);
    free(temp_float);
    free_d2charvar(newnames);
    free_glm(glm);
    sprintf(string,"mv -f %s %s",glm_tmp_file,glm_files->files[i]);
    if(system(string) == -1) {fprintf(flog,"fidlError: %s\n",string);fflush(stdout);exit(-1);}
    fprintf(flog,"    Output written to %s\n\n",glm_files->files[i]);
    }
}
