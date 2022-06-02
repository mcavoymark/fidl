/* Copyright 12/20/01 Washington University.  All Rights Reserved.
   compute_mag_norm.c  $Revision: 1.6 $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>

typedef struct Normalization_struct {
    char **filenames;
    int count_names,*nnames,*nweights;
    float ***weights;
    }
Normalization;

Normalization *read_normalization(char *normalization_file);

static char rcsid[] = "$Header: /home/hannah/mcavoy/idl/clib/RCS/compute_mag_norm.c,v 1.6 2003/04/23 21:54:03 mcavoy Exp $";

/************************/
main(int argc,char **argv)
/************************/
{
char *normalization_file,filename[MAXNAME];
int i,j,k,m,n,lenvol,*unsampled_mask;
float *temp_float;
double denominator;
Normalization *norm;
Memory_Map **mm;
Interfile_header *ifh;

print_version_number(rcsid);
if(argc < 3) {
    fprintf(stderr,"        -normalization:         Text file that contains the normalization parameters.\n");
    exit(-1);
    }
for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-normalization") && argc > i+1)
        normalization_file = argv[++i];
    }

if(!(norm = (Normalization *)read_normalization(normalization_file))) exit(-1);
/*BEGIN CHECK*/
/*for(m=i=0;i<norm->count_names;i++) {
    for(j=0;j<norm->nnames[i];j++,m++) {
        printf("%s ",norm->filenames[m]);
        for(k=0;k<norm->nweights[i];k++) printf("%g ",norm->weights[i][j][k]);
        printf("\n");
        }
    printf("\n");
    }*/
/*END CHECK*/

for(m=i=0;i<norm->count_names;i++) {

    for(j=0;j<norm->nnames[i];j++,m++) {
        ifh = read_ifh(norm->filenames[m]);
        if(!j) {
            lenvol = ifh->dim1*ifh->dim2*ifh->dim3;
            }
        else if(ifh->dim1*ifh->dim2*ifh->dim3 != lenvol) {
            printf("Dimensions of images are not equal.\n");
            exit(-1);
            }
        free_ifh(ifh,(int)FALSE);
        }

    GETMEM(mm,norm->nnames[i],Memory_Map *);
    GETMEM_0(unsampled_mask,lenvol,int);
    GETMEM(temp_float,lenvol,float);

    for(m-=norm->nnames[i],j=0;j<norm->nnames[i];j++,m++) {
        if(!(mm[j] = map_disk(norm->filenames[m],lenvol,0))) exit(-1);
        for(k=0;k<lenvol;k++) { /*unsampled voxels are marked with a 1*/
            if(mm[j]->ptr[k] == (float)UNSAMPLED_VOXEL) unsampled_mask[k] = 1; 
            }
        }

    for(m-=norm->nnames[i],j=0;j<norm->nnames[i];j++,m++) {
        for(k=0;k<lenvol;k++) {
            for(denominator=n=0;n<norm->nnames[i];n++) denominator += (double)(norm->weights[i][j][n]*mm[n]->ptr[k]*mm[n]->ptr[k]);
            temp_float[k] = (float)(mm[j]->ptr[k] / sqrt(denominator));
            }
        for(k=0;k<lenvol;k++) if(unsampled_mask[k]) temp_float[k] = (float)UNSAMPLED_VOXEL;
        strcpy(filename,norm->filenames[m]);
        *strstr(filename,".4dfp.img") = 0;
        strcat(filename,"_magnorm.4dfp.img");
        if(!write_float(filename,temp_float,lenvol)) exit(-1);
        printf("Output written to %s\n",filename);
        ifh = read_ifh(norm->filenames[m]);
        min_and_max(temp_float,lenvol,&ifh->global_min,&ifh->global_max);
        if(!write_ifh(filename,ifh,(int)FALSE)) exit(-1);
        free_ifh(ifh,(int)FALSE);
        }
    for(j=0;j<norm->nnames[i];j++) unmap_disk(mm[j]);

    free(unsampled_mask);
    free(temp_float);
    free(mm);
    }
}

/*********************************************************/
Normalization *read_normalization(char *normalization_file)
/*********************************************************/
{
char line[MAXNAME],write_back[MAXNAME],*str_ptr,*flag1="No filename listed.",*flag2="No weights listed.";
int i,j,k,m,nstrings,count_weights=0,flag,max_nnames=0,max_nweights=0,*nnames_for_weights;
FILE *fp;
Normalization *norm;

GETMEM(norm,1,Normalization);
norm->count_names = 0;
if(!(fp = fopen(normalization_file,"r"))) {
    printf("Error opening %s in read_normalization.\n",normalization_file);
    return (Normalization *)NULL;
    }
for(;fgets(line,sizeof(line),fp);) {
    if(strstr(line,"NAMES")) {
        norm->count_names++;
        }
    else if(strstr(line,"WEIGHTS")) {
        count_weights++;
        }
    }
if(!norm->count_names) {
    printf("NAMES field not detected in %s. Abort!\n",normalization_file);
    return (Normalization *)NULL;
    }
if(!count_weights) {
    printf("WEIGHTS field not detected in %s. Abort!\n",normalization_file);
    return (Normalization *)NULL;
    }
if(count_weights == 1) {
    /*do nothing*/
    }
else if(count_weights != norm->count_names) {
    printf("%d WEIGHTS and %d NAMES. Must have the same number of each. Abort!\n",count_weights,norm->count_names); 
    return (Normalization *)NULL;
    }
GETMEM_0(norm->nnames,norm->count_names,int);
GETMEM_0(nnames_for_weights,norm->count_names,int);
GETMEM_0(norm->nweights,norm->count_names,int);
/*flag : 0=low 1=NAMES 2=WEIGHTS*/
for(i=j=-1,flag=0,rewind(fp);fgets(line,sizeof(line),fp);) {
    if(strstr(line,"NAMES")) {
        flag = 1;
        ++i;
        }
    else if(strstr(line,"WEIGHTS")) {
        flag = 2;
        ++j;
        }
    else if(nstrings=count_strings(line,write_back,' ')) {  /*line could be blank*/
        if(!flag) {
            printf("Missing NAMES or WEIGHTS specifier. Abort!\n");
            return (Normalization *)NULL;
            }
        if(!(nstrings=count_strings(getstr(line),write_back,' '))) {  /*line could be "FIELD:=" or "FIELD :="*/
            printf("%s\n%s    %s Abort!\n",normalization_file,line,flag==1?flag1:flag2);
            return (Normalization *)NULL;
            }
        flag==1 ? norm->nnames[i]++ : nnames_for_weights[j]++;
        if(flag == 2) {
            if(!norm->nweights[j]) {
                norm->nweights[j] = nstrings;
                }
            else if(norm->nweights[j] != nstrings) {
                printf("Not all names have the same number of weights. Abort!\n");
                return (Normalization *)NULL;
                }
            }
        }   
    }        
for(i=0;i<count_weights;i++) { 
    if(norm->nnames[i] != nnames_for_weights[i]) {
        printf("For set %d, NAMES has %d records while WEIGHTS has %d records. Abort!\n",i,norm->nnames[i],nnames_for_weights[i]);
        return (Normalization *)NULL;
        }
    }
free(nnames_for_weights);
for(i=++j;i<norm->count_names;i++) norm->nweights[i] = norm->nweights[0];
for(j=i=0;i<norm->count_names;i++) {
    j += norm->nnames[i];
    if(norm->nnames[i] > max_nnames) max_nnames = norm->nnames[i];
    if(norm->nweights[i] > max_nweights) max_nweights = norm->nweights[i];
    }
if(!(norm->weights = (float ***)d3float(norm->count_names,max_nnames,max_nweights))) exit(-1);
GETMEM(norm->filenames,j,char *);
/*flag : 0=low 1=NAMES 2=WEIGHTS*/
for(i=-1,j=0,rewind(fp);fgets(line,sizeof(line),fp);) {
    if(strstr(line,"NAMES")) {
        flag = 1;
        }
    else if(strstr(line,"WEIGHTS")) {
        flag = 2;
        ++i;
        k=0;
        }
    else if(nstrings=count_strings(line,write_back,' ')) {  /*line could be blank*/
        str_ptr = getstr(write_back);
        if(flag == 1) {
            /*GETMEM(norm->filenames[j],strlen(str_ptr)+1,char);
            strcpy(norm->filenames[j++],str_ptr);*/

            grab_string(str_ptr,line); /*grab_string is needed to prevent an extra space at the end*/
            GETMEM(norm->filenames[j],strlen(line)+1,char);
            strcpy(norm->filenames[j++],line);
            }
        else {
            strings_to_float(str_ptr,norm->weights[i][k++],norm->nweights[i]);
            }
        }
    }
fclose(fp);
return (Normalization *)norm;
}
