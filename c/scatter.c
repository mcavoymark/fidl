/*********************************************************************
c
c subroutine: scatter
c
c Purpose: Construct image from x,y,z coordinates and values. 
	   (Same as scatter function in array processing lingo.)
c
c************************************************************************/

/*$Revision: 12.80 $*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/**********************/
double _scatter(argc,argv)
/**********************/

int	argc;
char	*argv[];

{

float	*image,*value;

int	i,*x,*y,*z,xdim,ydim,zdim,npts,len;

image = (float *)argv[0];
x     = (int *)argv[1];
y     = (int *)argv[2];
z     = (int *)argv[3];
value = (float *)argv[4];
npts  = (int)argv[5];
xdim  = (int)argv[6];
ydim  = (int)argv[7];
zdim  = (int)argv[8];
/*printf("%d %d %d %d %d %d %d %d %d\n",image,x,y,z,value,npts,xdim,ydim,zdim);*/

for(i=0;i<npts;i++) {
    image[x[i]+xdim*(y[i]+ydim*z[i])] = value[i];
    }

}
