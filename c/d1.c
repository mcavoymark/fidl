/* Copyright 6/25/04 Washington University.  All Rights Reserved.
   d1.c  $Revision: 1.4 $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>

void *d1(size_t size,size_t nitems)
{
    /*int i;*/

    unsigned char *array=NULL;
    /*printf("nitems=%d size=%d\n",nitems,size);*/
    if(!(array=calloc(nitems,size))) printf("Error: allocation failure in d1\n");

    /*if(!(array=malloc(nitems*size))) printf("Error: allocation failure in d1\n");*/
    /*for(i=0;i<nitems-1;i++) array[i] = 0;*/

    /*printf("HERE\n");*/
    return array;
}
