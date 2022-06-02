/* Copyright 10/23/12 Washington University.  All Rights Reserved.
   machine_epsilon.c  $Revision: 1.2 $ */
#include <stdio.h>
/*#include <fidl.h>*/

/*FLT_EPSILON=1.19209E-07 DBL_EPSILON=2.22045E-16
  Only use these routines if the above precomputed values don't exist.*/

float machine_epsilon(int print)
{
    float machEps = 1.;
    if(print) printf( "current Epsilon, 1 + current Epsilon\n" );
    do {
        if(print) printf( "%G\t%.20f\n", machEps, (1.0f + machEps) );
        machEps /= 2.0;
        } while ((float)(1.0 + (machEps/2.0)) != 1.0);
    if(print) printf("Calculated machine epsilon: %G\n", machEps );
    return machEps;
}
double machine_epsilon_double(int print)
{
    double machEps = 1.;
    if(print) printf( "current Epsilon, 1 + current Epsilon\n" );
    do {
        if(print) printf( "%G\t%.20f\n", machEps, (1.0f + machEps) );
        machEps /= 2.0;
        } while ((double)(1.0 + (machEps/2.0)) != 1.0);
    if(print) printf("junk2 Calculated Machine epsilon: %G\n", machEps );
    return machEps;
}
