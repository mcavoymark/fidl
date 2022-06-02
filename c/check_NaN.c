/* Copyright 12/31/99 Washington University.  All Rights Reserved.
   check_NaN.c  $Revision: 1.4 $ */

#include <stdio.h>
#include <fidl.h>
#include <stdlib.h>
#include <string.h>
#include <nan.h>

char *getbold(char *line);

main(int argc,char *argv[])
{

char	**bold_files,**glm_files;

int	i,j,k,lenvol,num_bold_files=0,num_glm_files=0,start_b;

float   *temp_float;

Interfile_header *ifh;

LinearModel *glm;

FILE	*fp;



if(argc < 3) {
    fprintf(stderr,"Usage: check_NaN -bold_files file1 file2 ... \n");
    fprintf(stderr,"        -bold_files: List of bold files to be analyzed.\n");
    fprintf(stderr,"        -glm_files: List of bold files to be analyzed.\n");
    exit(-1);
    }

for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-bold_files") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_bold_files;
        GETMEM(bold_files,num_bold_files,char *)
        for(j=0;j<num_bold_files;j++) {
            GETMEM(bold_files[j],strlen(argv[++i])+1,char)
            strcpy(bold_files[j],argv[i]);
            }
        }
    if(!strcmp(argv[i],"-glm_files") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_glm_files;
        GETMEM(glm_files,num_glm_files,char *)
        for(j=0;j<num_glm_files;j++) {
            GETMEM(glm_files[j],strlen(argv[++i])+1,char)
            strcpy(glm_files[j],argv[i]);
            }
        }
    }
printf("BOLD files:\n");
for(i=0;i<num_bold_files;i++) {
    printf("    %s\n",bold_files[i]);
    ifh = read_ifh(bold_files[i]);
    lenvol = ifh->dim1*ifh->dim2*ifh->dim3;
    GETMEM(temp_float,lenvol,float)
    read_float(bold_files[i],temp_float,lenvol);
    for(j=0;j<lenvol;j++)
        if(NaN(temp_float[j])) {
	    printf("NaN\n");
            exit(-1);
            }
        /*else
	    printf("%f ",temp_float[j]);
    printf("\n");*/
    free_ifh(ifh,(int)FALSE);
    free(temp_float);
    }

printf("GLM files:\n");
for(i=0;i<num_glm_files;i++) {
    printf("    %s\n",glm_files[i]);
    glm = read_glm(glm_files[i],(int)TRUE);
    lenvol = glm->xdim*glm->ydim*glm->zdim;
    start_b = find_b(glm);
    GETMEM(temp_float,lenvol,float);
    if(!(fp = fopen(glm_files[i],"r"))) {
        fprintf(stderr,"Could not open %s in check_NaN for reading.\n",glm_files[i]);
        exit(-1);
        }
    /*if(fseek(fp,-lenvol*sizeof(float)*glm->Mcol,SEEK_END)) {*/
    if(fseek(fp,start_b,SEEK_SET)) {
        fprintf(stderr,"Error occured while seeking to %d in %s.\n",-lenvol*sizeof(float)*glm->Mcol,glm_files[i]);
        exit(-1);
        }
    for(j=0;j<glm->Mcol;j++) {
        printf("j = %d\n",j);
        if(fread(temp_float,sizeof(float),lenvol,fp) != lenvol) {
            fprintf(stderr,"Error reading parameter estimates from %s.\n",glm_files[i]);
            exit(-1);
            }
        for(k=0;k<lenvol;k++)
            if(NaN(temp_float[k])) {
	        printf("NaN\n");
                exit(-1);
                }
            /*else
	        printf("%f ",temp_float[k]);
        printf("\n");*/
        }
    free_glm(glm,(int)TRUE);
    free(temp_float);
    fclose(fp);
    }

}

/****************/
char *getbold(line)
/****************/
 
char    *line;
 
{
 
char    *ptr,*ptr1;
 
ptr = strstr(line,":");
ptr += 1;
ptr1 = strstr(line,"\n");
if(ptr1 != NULL)
    *ptr1 = 0;
while(ptr[0] == ' ')
    ++ptr;
return(ptr);
 
}
