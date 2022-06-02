/* Copyright 5/24/13 Washington University.  All Rights Reserved.
   alloc.c  $Revision: 1.2 $*/
#include <stdio.h>
#include <stdlib.h>

#if 0
double*** newarrayd(int icount, int jcount, int kcount, int type_size)
{
    int i,j;

    printf("here-1\n");fflush(stdout);

    #if 1
    double*** iret = (double***)malloc(icount*sizeof(double***)+icount*jcount*sizeof(double**)+icount*jcount*kcount*type_size);
    double** jret = (double**)(iret+icount);
    double* kret = (double*)(jret+icount*jcount);

    printf("here0\n");fflush(stdout);

    for(i=0;i<icount;i++)
        iret[i] = &jret[i*jcount];

    printf("here1\n");fflush(stdout);

    for(i=0;i<icount;i++)
        for(j=0;j<jcount;i++)
            jret[i*jcount+j] = &kret[i*jcount*kcount*type_size+j*kcount*type_size];

    printf("here2\n");fflush(stdout);

    #endif


    return iret;
}


void*** newarray(int icount, int jcount, int kcount, int type_size)
{
    int i,j;

    printf("here-1\n");fflush(stdout);

    #if 0
    void*** iret = (void***)malloc(icount*sizeof(void***)+icount*jcount*sizeof(void**)+icount*jcount*kcount*type_size);
    void** jret = (void**)(iret+icount);
    char* kret = (char*)(jret+icount*jcount);

    printf("here0\n");fflush(stdout);

    for(i=0;i<icount;i++)
        iret[i] = &jret[i*jcount];

    printf("here1\n");fflush(stdout);

    for(i=0;i<icount;i++)
        for(j=0;j<jcount;i++)
            jret[i*jcount+j] = &kret[i*jcount*kcount*type_size+j*kcount*type_size];

    printf("here2\n");fflush(stdout);

    #endif

    void*** iret = (void***)malloc(icount*sizeof(void**)+icount*jcount*sizeof(void*)+icount*jcount*kcount*type_size);
    void** jret = (void*)(iret+icount);
    char* kret = (char*)(jret+icount*jcount);

    printf("here0\n");fflush(stdout);

    for(i=0;i<icount;i++)
        iret[i] = &jret[i*jcount];

    printf("here1\n");fflush(stdout);

    for(i=0;i<icount;i++)
        for(j=0;j<jcount;i++)
            jret[i*jcount+j] = &kret[i*jcount*kcount*type_size+j*kcount*type_size];

    printf("here2\n");fflush(stdout);



    return iret;
}
#endif
#if 0
void ***d3(size_t d1,size_t d2,size_t d3,size_t size)
{
    size_t i,j;
    void ***array;
    if(!(array=malloc(sizeof(void*)*d1+sizeof(void**)*d1*d2+size*d1*d2*d3))) {
        printf("fidlError: Unable to malloc array in d3\n");
        return NULL;
        }
    for(i=0;i<d1;i++) {
        array[i] = (void**)array+d1+i*d2;
        for(j=0;j<d2;++j) array[i][j] = array+d1+d1*d2+i*d2*d3*size+j*d3*size;
        }
}
#endif
#if 0
void ***d3(size_t d1,size_t d2,size_t d3,size_t size)
{
    size_t i,j;
    void ***array;
    if(!(array=malloc(sizeof(void*)*d1+sizeof(void**)*d1*d2+size*d1*d2*d3))) {
        printf("fidlError: Unable to malloc array in d3\n");
        return NULL;
        }
    for(i=0;i<d1;i++) {
        array[i] = (void**)(array+d1*size)+i*d2*size;
        for(j=0;j<d2;++j) array[i][j] = (void*)(array+d1*size+d1*d2*size)+i*d2*d3*size+j*d3*size;
        }
}
#endif


double ***d3d(size_t d1,size_t d2,size_t d3)
{
    size_t i,j;
    double ***array;
    if(!(array=malloc(sizeof(double*)*d1+sizeof(double**)*d1*d2+sizeof(double)*d1*d2*d3))) {
        printf("fidlError: Unable to malloc array in d3d\n");
        return NULL;
        }
    for(i=0;i<d1;i++) {
        array[i] = (double**)(array+d1)+i*d2;
        for(j=0;j<d2;++j) array[i][j] = (double*)(array+d1+d1*d2)+i*d2*d3+j*d3;
        }
}


#if 0
double **d2d(size_t d1,size_t d2)
{
    size_t i,j;
    double **array;
    if(!(array=malloc(sizeof(double*)*d1+sizeof(double)*d1*d2))) {
        printf("fidlError: Unable to malloc array in d2f\n");
        return NULL;
        }
    for(i=0;i<d1;i++) array[i] = (double*)(array+d1)+i*d2;
    return array;
}
float **d2f(size_t d1,size_t d2)
{
    size_t i;
    float **array;
    if(!(array=malloc(sizeof(float*)*d1+sizeof(float)*d1*d2))) {
        printf("fidlError: Unable to malloc array in d2f\n");
        return NULL;
        }
    for(i=0;i<d1;i++) array[i] = (float*)(array+d1)+i*d2;
    return array;
}
int **d2iv(size_t d1,int *d2)
{
    size_t i,len=0;
    int **array;
    for(i=0;i<d1;i++) len+=(size_t)d2[i];
    if(!(array=malloc(sizeof(int*)*d1+sizeof(int)*len))) {
        printf("fidlError: Unable to malloc array in d2iv\n");
        return NULL;
        }
    for(i=1;i<d1;i++) array[i] = (int*)(array+d1)+d2[i-1];
    return array;
}
#endif
