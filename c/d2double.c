/* Copyright 4/17/03 Washington University.  All Rights Reserved.
   d2double.c  $Revision: 1.6 $*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#include "fidl.h"
//START160908
#include "d2double.h"

double **d2double(int dim1,int dim2) {
    int i;
    double **array;
    if(!(array=malloc((size_t)(dim1*sizeof(double*))))) {
        printf("allocation failure 1 in d2double\n");
        return NULL;
        }
    if(!(array[0]=calloc((size_t)(dim1*dim2),sizeof(double)))) {
        printf("allocation failure 2 in d2double\n");
        return NULL;
        }
    for(i=1;i<dim1;i++) array[i]=array[i-1]+dim2;
    return array;
    }
void free_d2double(double **array) {
    free(array[0]);
    free(array);
    }
