/* Copyright 1/10/19 Washington University.  All Rights Reserved.
   d2int.c  $Revision: 1.1 $*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "d2int.h"
int **d2int(int dim1,int dim2)
{
    int i,**array;
    if(!(array=(int **)malloc((size_t)(dim1*sizeof(int*))))) {
        printf("allocation failure 1 in d2int\n");
        return (int **)NULL;
        }
    if(!(array[0]=(int *)calloc((size_t)(dim1*dim2),sizeof(int)))) {
        printf("allocation failure 2 in d2int\n");
        return (int **)NULL;
        }
    for(i=1;i<dim1;i++) array[i]=array[i-1]+dim2;
    return array;
}
void free_d2int(int **array)
{
    free(array[0]);
    free(array);
}
