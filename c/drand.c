/*********************************************************************
c
c subroutine: drand
c
c Purpose: Front-end for unix drand48 routine.
c
c************************************************************************/

/*$Revision: 12.80 $*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/**********************/
double _drand(argc,argv)
/**********************/

int	argc;
char	*argv[];

{

double	*rv;

int	i;

unsigned short	*seed,length;

rv     = (double *)argv[0];
length = (int)argv[1];
seed   = (unsigned short *)argv[2];

for(i=0;i<length;i++) {
    rv[i]   = erand48(seed);
    }

}
