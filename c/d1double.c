/* Copyright 4/17/03 Washington University.  All Rights Reserved.
   d1double.c  $Revision: 1.2 $*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>

double *d1double(int dim)
{
    double *array;
    if(!(array=(double *)calloc((size_t)dim,sizeof(double)))) printf("Error: allocation failure in d1double\n");
    return array;
}
