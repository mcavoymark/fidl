/* Copyright 5/18/05 Washington University.  All Rights Reserved.
   d2charvar.c  $Revision: 1.8 $*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "fidl.h"
char **d2charvar(size_t dim1,size_t *dim2){
    size_t i,length;
    char **array;
    if(!(array=(char **)malloc((size_t)(dim1*sizeof(char*))))) {
        printf("allocation failure 1 in d2char\n");
        return (char **)NULL;
        }
    for(length=i=0;i<dim1;i++) length += dim2[i];

    //if(!(array[0]=(char *)malloc((size_t)(length*sizeof(char))))) {
    //START151012
    if(!(array[0]=calloc(length,sizeof*array[0]))) {

        printf("allocation failure 2 in d2charvar\n");
        return (char **)NULL;
        }
    for(i=1;i<dim1;i++) array[i]= array[i-1] + dim2[i-1];
    return array;
    }
void free_d2charvar(char **array){
    free(array[0]);
    free(array);
    }
