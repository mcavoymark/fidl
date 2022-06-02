/* Copyright 12/31/99 Washington University.  All Rights Reserved.
   nrmlrv.c  $Revision: 12.85 $ */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#ifndef __sun__
    #include <stdint.h>
#endif
void nrmlrv();
void nrmlrv_doublestack(double *stack,int size,unsigned short *seed);
float _normal_vector(int argc,char **argv)
{
    float *rv,rv1,rv2;
    int i,length;
    unsigned short *seed;
    rv = (float *)argv[0];
    #ifdef __sun__
        length = (int)argv[1];
    #else
        length = (intptr_t)argv[1];
    #endif

seed = (unsigned short *)argv[2];


for(i=0;i<2*(length/2);i+=2) {
    nrmlrv(&rv1,&rv2,seed);
    rv[i]   = rv1;
    rv[i+1] = rv2;
    }

if(length != 2*(length/2)) {
    /* Odd number of elements in vector, get the last one. */
    nrmlrv(&rv1,&rv2,seed);
    rv[length-1]   = rv1;
    }

}


/*****************************/
void nrmlrv(gausse,gaussb,seed)
/*****************************/

float 	*gausse,
	*gaussb;
unsigned short *seed; /* 6 byte integer */

{

double  r,v1,v2,fac;

do {
    v1 = 2.*erand48(seed) - 1.;
    v2 = 2.*erand48(seed) - 1.;
    r = v1*v1 + v2*v2;
    } while(r >= 1.);
fac = sqrt(-2.*log(r)/r);
*gausse = v1*fac;
*gaussb = v2*fac;

}

/******************************************************************/
void nrmlrv_doublestack(double *stack,int size,unsigned short *seed)
/********************************************** 6 byte integer ****/
{
int i;

double r,v1,v2,fac;

for(i=0;i<size;i+=2) {
    do {
        v1 = 2.*erand48(seed) - 1.;
        v2 = 2.*erand48(seed) - 1.;
        r = v1*v1 + v2*v2;
        } while(r >= 1.);
    fac = sqrt(-2.*log(r)/r);
    stack[i] = v1*fac;
    if(i+1 < size) stack[i+1] = v2*fac;
    }
}
