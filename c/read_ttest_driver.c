/* Copyright 9/1/04 Washington University.  All Rights Reserved.
   read_ttest_driver.c  $Revision: 1.16 $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "fidl.h"
#include "read_ttest_driver.h"
#include "subs_util.h"
TtestDesign *read_ttest_driver(char *driver_file)
{
char line[MAXNAME],write_back[MAXNAME],junk[MAXNAME],*str_ptr,*ptr,lastchar;
int i,j,k,l,m,mm,nstrings,len,ii,tlnames=0;
size_t *len_type,*len_name,*len_files1,*len_files2,*len_cov;
FILE *fp;
TtestDesign *td;
if(!(td=malloc(sizeof*td))) {
    printf("Error: Unable to malloc td\n");
    return NULL;
    }
td->ntests = 0;
td->nnames = 0;
if(!(fp = fopen_sub(driver_file,"r"))) return NULL;
for(;fgets(line,sizeof(line),fp);) {
    if(!strncmp(line,"TTEST",5) || !strncmp(line,"NAMES",5)) td->ntests++;
    }
if(td->ntests) {
    ii = -1;
    }
else {
    printf("TTEST field not detected in %s, so we shall assume a single test.\n",driver_file);
    td->ntests = 1;
    ii = 0;
    }
if(!(len_type=malloc(sizeof*len_type*td->ntests))) {
    printf("Error: Unable to malloc len_type\n"); 
    return NULL;
    }
for(i=0;i<td->ntests;i++)len_type[i]=0;
if(!(len_name=malloc(sizeof*len_name*td->ntests))) {
    printf("Error: Unable to malloc len_name\n"); 
    return NULL;
    }
for(i=0;i<td->ntests;i++)len_name[i]=0;
if(!(td->nfiles1=malloc(sizeof*td->nfiles1*td->ntests))) {
    printf("Error: Unable to malloc td->nfiles1\n"); 
    return NULL;
    }
if(!(td->nfiles2=malloc(sizeof*td->nfiles2*td->ntests))) {
    printf("Error: Unable to malloc td->nfiles2\n"); 
    return NULL;
    }
if(!(td->ncov=malloc(sizeof*td->ncov*td->ntests))) { 
    printf("Error: Unable to malloc td->ncov\n"); 
    return NULL;
    }
for(i=0;i<td->ntests;i++) td->nfiles1[i]=td->nfiles2[i]=td->ncov[i]=0;
if(!(td->ncov_per_line=malloc(sizeof*td->ncov_per_line*td->ntests))) {
    printf("Error: Unable to malloc td->ncov_per_line\n"); 
    return NULL;
    }
for(j=k=l=0,i=ii,rewind(fp);fgets(line,sizeof(line),fp);) {
    if(!strncmp(line,"TTEST",5)) {
        i++;
        get_line(line,junk,":=",&len);
        if((nstrings = count_strings_new_nocomma(junk,write_back,' ',&lastchar)) != 2) {
            printf("TTEST := needs both the test type and test name listed. Abort!\n");
            return NULL;
            }
        str_ptr = grab_string_new(write_back,line,&len); 
        len_type[i] = len+1;
        grab_string_new(str_ptr,line,&len);
        len_name[i] = len+1;
        }
    else if(!strncmp(line,"NAMES",5)) {
        i++;
        get_line(line,junk,":=",&len);
        nstrings=count_strings_new_nocomma(junk,write_back,',',&lastchar);
        td->nnames += nstrings;
        for(str_ptr=write_back,m=0;m<nstrings;m++) {
            str_ptr=grab_string_new(str_ptr,line,&len);
            tlnames += len+1;
            }
        }
    else if((nstrings=count_strings_new_nocomma(line,write_back,',',&lastchar))) {  /*line could be blank*/
        if(i == -1) {
            printf("%s\n%s\n    TTEST specifier must come first. Abort!\n",driver_file,line);
            return (TtestDesign*)NULL;
            }
        parse_line(line,junk,":=",&len);

        //if(!junk) {
        //START170419
        if(!junk[0]){

            printf("%s\n%s\n    Missing := after FIRST, SECOND, COVARIATES, or INDVAR. Abort!\n",driver_file,line);
            return NULL;
            }
        if(!(nstrings=count_strings_new_nocomma(junk,write_back,',',&lastchar))) {  /*line could be "FIELD:=" or "FIELD :="*/
            printf("%s\n%s\n    Missing data. Abort!\n",driver_file,line);
            return NULL;
            }
        if(strstr(line,"FIRST")) {
            td->nfiles1[i]++;
            j++;
            }
        else if(strstr(line,"SECOND")) {
            td->nfiles2[i]++;
            k++;
            }

        /*else if(strstr(line,"COVARIATES")||strstr(line,"INDVAR")) {*/
        /*START140325*/
        else if(strstr(line,"COVARIATES")||strstr(line,"INDVAR")||strstr(line,"DF")) {

            td->ncov[i]++;
            l++;
            if(td->ncov[i] == 1) {
                td->ncov_per_line[i] = nstrings;
                }
            else if(td->ncov_per_line[i] != nstrings) {
                printf("%s\n%s\n    Need to have the same number of covariates on each line. Abort!\n",driver_file,line);
                return (TtestDesign*)NULL;
                }
            }
        else {
            printf("%s\n%s\n    Line formatted incorrectly. Abort!\n",driver_file,line);
            return (TtestDesign *)NULL;
            }
        }
    }
if(!(td->type=d2charvar(td->ntests,len_type))) return NULL; 
if(!(td->name=d2charvar(td->ntests,len_name))) return NULL; 
free(len_type);
free(len_name);
if(!(len_files1=malloc(sizeof*len_files1*j))) {
    printf("Error: Unable to malloc len_files1\n");
    return NULL;
    }
if(!(len_files2=malloc(sizeof*len_files2*k))) { 
    printf("Error: Unable to malloc len_files2\n");
    return NULL;
    }
if(!(len_cov=malloc(sizeof*len_cov*l))) {
    printf("Error: Unable to malloc len_cov\n");
    return NULL;
    }
if(!(td->namesptr=malloc(sizeof*td->namesptr*td->nnames))) {
    printf("Error: Unable to malloc td->namesptr\n");
    return NULL;
    }
if(!(td->lnames=malloc(sizeof*td->lnames*td->nnames))) {
    printf("Error: Unable to malloc td->lnames\n");
    return NULL;
    }
printf("tlnames=%d td->nnames=%d\n",tlnames,td->nnames);
if(!(td->names=malloc(sizeof*td->names*tlnames))) {
    printf("Error: Unable to malloc td->names\n");
    return NULL;
    }
for(mm=j=k=l=0,i=-1,rewind(fp),ptr=td->names;fgets(line,sizeof(line),fp);) {
    if(!strncmp(line,"TTEST",5)) {
        i++;
        get_line(line,junk,":=",&len);
        if((nstrings = count_strings_new_nocomma(junk,write_back,' ',&lastchar)) != 2) {
            printf("TTEST := needs both the test type and test name listed. Abort!\n");
            return NULL;
            }
        str_ptr = grab_string_new(write_back,line,&len); 
        strcpy(td->type[i],line);
        grab_string_new(str_ptr,line,&len);
        strcpy(td->name[i],line);
        }
    else if(!strncmp(line,"NAMES",5)) {
        get_line(line,junk,":=",&len);
        nstrings=count_strings_new_nocomma(junk,write_back,',',&lastchar);
        for(str_ptr=write_back,m=0;m<nstrings;m++) {
            str_ptr=grab_string_new(str_ptr,line,&td->lnames[mm]);
            strcpy(ptr,line);
            td->namesptr[mm]=ptr;
            ptr += ++td->lnames[mm++];
            }
        }
    else if((nstrings=count_strings_new_nocomma(line,write_back,',',&lastchar))) {  /*line could be blank*/
        parse_line(line,junk,":=",&len);
        if(strstr(line,"FIRST")) {
            grab_string_new(junk,write_back,&len); /*grab_string is needed to prevent an extra space at the end*/
            len_files1[j++] = len+1;
            }
        else if(strstr(line,"SECOND")) {
            grab_string_new(junk,write_back,&len); /*grab_string is needed to prevent an extra space at the end*/
            len_files2[k++] = len+1;
            }

        /*else if(strstr(line,"COVARIATES")||strstr(line,"INDVAR")) {*/
        /*START140325*/
        else if(strstr(line,"COVARIATES")||strstr(line,"INDVAR")||strstr(line,"DF")) {

            len_cov[l++] = len+1;
            }

        }
    }
if(!(td->files1=d2charvar(j,len_files1))) return (TtestDesign*)NULL; 
if(!(td->files2=d2charvar(k,len_files2))) return (TtestDesign*)NULL; 
if(!(td->cov=d2charvar(l,len_cov))) return (TtestDesign*)NULL; 
free(len_files1);
free(len_files2);
free(len_cov);
td->total_nfiles = j + k + l;
for(i=j=k=0,rewind(fp);fgets(line,sizeof(line),fp);) {
    if(!strncmp(line,"TTEST",5)) {
        /*do nothing*/
        }
    else {
        parse_line(line,junk,":=",&len);
        if(strstr(line,"FIRST")) {
            grab_string_new(junk,write_back,&len); /*grab_string is needed to prevent an extra space at the end*/
            strcpy(td->files1[i++],write_back);
            }
        else if(strstr(line,"SECOND")) {
            grab_string_new(junk,write_back,&len); /*grab_string is needed to prevent an extra space at the end*/
            strcpy(td->files2[j++],write_back);
            }

        /*else if(strstr(line,"COVARIATES")||strstr(line,"INDVAR")) {*/
        /*START140325*/
        else if(strstr(line,"COVARIATES")||strstr(line,"INDVAR")||strstr(line,"DF")) {

            strcpy(td->cov[k++],junk);
            }
        }
    }
fclose(fp);
return td;
}
