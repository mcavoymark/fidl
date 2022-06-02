/* Copyright 8/29/05 Washington University.  All Rights Reserved.
   files_struct.c  $Revision: 1.16 $*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fidl.h"
#include "subs_util.h"

Files_Struct *read_files(int nfiles,char **argv){
    char *strptr;
    Files_Struct *files;
    if(!nfiles){printf("fidlError: Need to provide *.list, *.conc, *.img, *.nii or *.nii.gz\n");return NULL;}

    #if 0
    if(!(strptr=strrchr(argv[0],'.'))){
        if(!(strptr=strrchr(argv[0],'_'))){printf("fidlError: read_files Unknown file type.\n");return NULL;}
        }
    if(!strcmp(strptr,".conc")||!strcmp(strptr,".list")){
        if(!(files=read_conc(nfiles,&argv[0])))return NULL;
        }
    else if(!strcmp(strptr,".img")||!strcmp(strptr,".nii")||!strcmp(strptr-4,".nii.gz")||!strcmp(strptr,"_t4")||!strcmp(strptr,".txt")){
        if(!(files=get_files(nfiles,&argv[0])))return NULL;
        }
    else{
        printf("fidlError: read_files strptr=%s  not *.conc, *.list, *.img, *.nii, *.nii.gz or *_t4. Abort!\n",strptr);return NULL;
        }
    #endif
    //START210513
    strptr=strrchr(argv[0],'.');
    if(strptr) if(!strcmp(strptr,".conc")||!strcmp(strptr,".list")){
        if(!(files=read_conc(nfiles,&argv[0])))return NULL;
        return files;
        }
    if(!(files=get_files(nfiles,&argv[0])))return NULL;
 

    return files;
    }


Files_Struct *get_files(int num_region_files,char **argv)
{
    int j;
    Files_Struct *fi;
    if(!(fi=malloc(sizeof*fi))) {
        printf("Error: Unable to malloc fi in get_files\n");
        return NULL;
        }
    fi->nfiles = num_region_files;
    if(!(fi->strlen_files=malloc(sizeof*fi->strlen_files*num_region_files))) {
        printf("Error: Unable to malloc fi->strlen_files in get_files\n");
        return NULL;
        }
    for(fi->max_length=0,j=0;j<num_region_files;j++) {
        fi->strlen_files[j] = strlen(argv[j])+1;
        if(fi->strlen_files[j] > fi->max_length) fi->max_length = fi->strlen_files[j];
        }
    if(!(fi->files=d2charvar(num_region_files,fi->strlen_files))) return NULL;
    for(j=0;j<num_region_files;j++) strcpy(fi->files[j],argv[j]);

    //START210526
    if(!(fi->nfileseach=malloc(sizeof*fi->nfileseach))) {
        printf("fidlError: Unable to malloc fi->nfileseach in get_files\n");
        return NULL;
        }
    fi->nfileseach[0]=num_region_files;

    return fi;
}

#if 0
Files_Struct *init_FS(size_t nfiles)
{
    size_t i;
    Files_Struct *fi;
    if(!(fi=malloc(sizeof*fi))) {
        printf("Error: Unable to malloc fi in init_FS\n");
        return NULL;
        }
    fi->nfiles = nfiles; 
    if(!(fi->strlen_files=malloc(sizeof*fi->strlen_files*fi->nfiles))) {
        printf("Error: Unable to malloc fi->strlen_files in init_FS\n");
        return NULL;
        }
    if(!(fi->strlen_identify=malloc(sizeof*fi->strlen_identify*fi->nfiles))) {
        printf("Error: Unable to malloc fi->strlen_identify in init_FS\n");
        return NULL;
        }
    for(i=0;i<fi->nfiles;i++) fi->strlen_identify[i]=0;
    fi->nstrings=fi->max_length=0;
    return fi;
}
#endif
//START210427
Files_Struct *init_FS(size_t tnfiles,int nfiles){
    size_t i;
    Files_Struct *fi;
    if(!(fi=malloc(sizeof*fi))) {
        printf("fidlError: Unable to malloc fi in init_FS\n");
        return NULL;
        }
    fi->nfiles=tnfiles; 
    if(!(fi->strlen_files=malloc(sizeof*fi->strlen_files*fi->nfiles))) {
        printf("fidlError: Unable to malloc fi->strlen_files in init_FS\n");
        return NULL;
        }
    if(!(fi->strlen_identify=malloc(sizeof*fi->strlen_identify*fi->nfiles))) {
        printf("fidlError: Unable to malloc fi->strlen_identify in init_FS\n");
        return NULL;
        }
    for(i=0;i<fi->nfiles;i++) fi->strlen_identify[i]=0;
    fi->nstrings=fi->max_length=0;
    if(!(fi->nfileseach=malloc(sizeof*fi->nfileseach*nfiles))) {
        printf("fidlError: Unable to malloc fi->nfileseach in init_FS\n");
        return NULL;
        }
    return fi;
    }

Files_Struct *read_conc(int nfile,char **file)
{
    char line[MAXNAME],junk[MAXNAME],write_back[MAXNAME],*strptr,dummy;
    int len,nstrings;
    size_t i,nfiles,tnfiles;
    FILE *fp;
    Files_Struct *fi;
    for(tnfiles=i=0;i<(size_t)nfile;i++) {
        if(!(fp=fopen_sub(file[i],"r"))) return NULL;
        for(nfiles=0;fgets(line,sizeof(line),fp);) {
            if(strstr(line,"number_of_files")||strstr(line,"paradigm_format")) {
                /*do nothing*/
                }
            else {
                get_line_wwosearchstr(line,junk,"file:",&len);
                if((nstrings=count_strings_new(junk,write_back,' ',&dummy))>2) nstrings=2; else if(!nstrings) continue;
                nfiles++;
                }
            }
        fclose(fp);
        if(!nfiles) {printf("fidlError: %s is empty\n",file[i]);fflush(stdout);continue;}
        tnfiles+=nfiles;
        }
    if(!tnfiles) return NULL;

    //if(!(fi=init_FS(tnfiles))) return NULL;
    //START210427
    if(!(fi=init_FS(tnfiles,nfiles))) return NULL;

    for(nfiles=i=0;i<(size_t)nfile;i++) {
        if(!(fp=fopen_sub(file[i],"r"))) return NULL;

        //for(;fgets(line,sizeof(line),fp);) {
        //START210427
        for(fi->nfileseach[i]=0;fgets(line,sizeof(line),fp);) {

            if(strstr(line,"number_of_files")||strstr(line,"paradigm_format")) {
                /*do nothing*/
                }
            else {
                get_line_wwosearchstr(line,junk,"file:",&len);
                if((nstrings=count_strings_new(junk,write_back,' ',&dummy))>2) nstrings=2; else if(!nstrings) continue;
                fi->nstrings += nstrings;
                strptr=grab_string_new(write_back,junk,&len);
                fi->strlen_files[nfiles] = len+1;
                if(fi->strlen_files[nfiles]>fi->max_length) fi->max_length = fi->strlen_files[nfiles];
                if(nstrings==2) {
                    grab_string_new(strptr,junk,&len);
                    fi->strlen_identify[nfiles] = len+1;
                    }
                nfiles++;

                //START210427
                fi->nfileseach[i]++; 

                }
            }
        fclose(fp);
        }
    if(fi->nstrings==fi->nfiles) {
        fi->identify = (char**)NULL;
        }
    else if(fi->nstrings==(2*fi->nfiles)) {
        fi->nstrings=2;
        if(!(fi->identify=d2charvar(fi->nfiles,fi->strlen_identify))) return NULL;
        }
    else {
        printf("fidlError: Some files are missing identifiers. Abort!\n");
        return NULL;
        }
    if(!(fi->files=d2charvar(fi->nfiles,fi->strlen_files))) return NULL;
    for(nfiles=i=0;i<(size_t)nfile;i++) {
        if(!(fp=fopen_sub(file[i],"r"))) return NULL;
        for(;fgets(line,sizeof(line),fp);) {
            if(strstr(line,"number_of_files")||strstr(line,"paradigm_format")) {
                /*do nothing*/
                }
            else {
                get_line_wwosearchstr(line,junk,"file:",&len);
                if((nstrings=count_strings_new(junk,write_back,' ',&dummy))>2) nstrings=2; else if(!nstrings) continue;
                strptr=grab_string_new(write_back,junk,&len);
                strcpy(fi->files[nfiles],junk);
                if(nstrings==2) {
                    grab_string_new(strptr,junk,&len);
                    strcpy(fi->identify[nfiles],junk);
                    }
                nfiles++;
                }
            }
        fclose(fp);
        }
    return fi;
}
void free_files_struct(Files_Struct *fi)
{
    free_d2charvar(fi->files);
    if(fi->identify) free_d2charvar(fi->identify);
    free(fi->strlen_identify);
    free(fi->strlen_files);

    //START210427
    free(fi->nfileseach); 

    free(fi);
}
