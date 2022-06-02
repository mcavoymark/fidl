/*********************************************************************
c
c subroutine: errorfc
c
c Purpose: Front-end for unix erfc routine.
c
c************************************************************************/

/*$Revision: 12.80 $*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <fidl.h>

/************************/
double _errorfc(argc,argv)
/************************/

int	argc;
char	*argv[];

{

double	*rv,*integral;

int	i;

int	length;

integral = (double *)argv[0];
rv     = (double *)argv[1];
length = (int)argv[2];

for(i=0;i<length;i++) {
    integral[i]   = erffc(rv[i]);
    }

}
