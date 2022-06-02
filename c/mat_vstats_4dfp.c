/* mat_vtats.c - mex routine to calculate parameters for image volume   */
/* matlab format  vstats = mat_vstats(img, DIM )                        */
/* Where        img is vector of image values in x, y, z order          */
/*              DIM is number of voxels for each dimension x, y, z      */
/* and          vstats is 8 value 3d parameter list for image           */
/*                                                                      */
/* Matthew Brett 8/98 implementing some of Worsley 1996 HBM 4:58        */
/* Bug fixes 27/5/99 -> Version 2                                       */

/******************************************************************************
mat_vstats_4dfp.c
Broken out of mex and SPM formats to run as a stand alone application on 4dfp's
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>

main(int argc,char **argv)
{
        double  *vstat;      /* matrices from / to matlab */

        int vstatlen = 8; /* no of counter variables returned */

char*** make_3d_arr(int x,int y,int z);

char ***ii;	/* temporary buffer for calculation */
char *filename;

int lenvol,i,k,x,y,z,m,n;

float *img;

Interfile_header *ifh;

/* image parameter counter variables */
int P = 0;
int Ex = 0;
int Ey = 0;
int Ez = 0;
int Fxy = 0;
int Fxz = 0;
int Fyz = 0;
int C = 0;


if (argc < 3) {
    fprintf(stderr,"Usage: mat_vstats_4dfp -region_file filename\n");
    fprintf(stderr,"        -region_file:    The voxels for the ROI should be greater than zero, with all other voxels\n");
    fprintf(stderr,"                         zero. Currently, the program is set up to calculate resel parameters for only\n");
    fprintf(stderr,"                         a single region.\n");
    exit(-1);
    }
for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-region_file") && argc > i+1) {
        GETMEM(filename,strlen(argv[i+1])+1,char)
        strcpy(filename,argv[++i]);
	}
    }
if((ifh=read_ifh(filename)) == NULL) {
    fprintf(stdout,"Error: Could not open %s\n",filename);
    /*This must be stdout for fidl error traps.*/
    exit(-1);
    }
lenvol = ifh->dim1*ifh->dim2*ifh->dim3;
GETMEM(img,lenvol,float)
*strrchr(filename,'.') = 0;
strcat(filename,".img");
read_float(filename,img,lenvol);

/* temporary buffer for calculating, padded with zero planes */
ii = make_3d_arr(ifh->dim1+1,ifh->dim2+1,ifh->dim3+1);

/* initialise buffer with binarised image matrix */
k = 0;
for(z=0;z<ifh->dim3;z++)
    for(y=0;y<ifh->dim2;y++)
        for(x=0;x<ifh->dim1;x++)
            ii[x][y][z] = (char)(img[k++]>0);
                                                                          
/* calculation of image parameters */
        
for (z = 0; z< ifh->dim3; z++) {
  for (y = 0; y< ifh->dim2; y++){
    for (x = 0; x< ifh->dim1; x++){
      if (ii[x][y][z]) {
        P++;
        if (ii[x+1][y][z]) {     /* x edge */
          Ex++;
          if (ii[x][y+1][z] && ii[x+1][y+1][z]) { /* xy face */
            Ey++;
            Fxy = Fxy + 1;
            if (ii[x][y][z+1] && ii[x+1][y][z+1] && ii[x][y+1][z+1] && ii[x+1][y+1][z+1]){
              /* Cube */
              Ez++;
              Fxz++;
              Fyz++;
              C++;
            } else {    /* not cube is xy face */
              if (ii[x][y][z+1]) {   /* z edge */
                Ez++;
                if (ii[x +1][y][z+1]) { /* xz face */
                  Fxz++;
                }
                if (ii[x][y+1][z+1]) { /* yz face */
                  Fyz++;
                }
              } /* if z edge */
            } /*  else  cube */
          } else { /* not xy face is x edge */
            if (ii[x][y +1][z]) { /* y edge */
              Ey++;
              if (ii[x][y][z+1] && ii[x][y+1][z+1]) {  /* yz face */
                Ez++;
                Fyz++;
                if (ii[x+1][y][z+1]) {  /* xz face */
                  Fxz++;
                }
              } else {   /* not yz face is y edge */
                if (ii[x][y][z+1]) {   /* z edge */
                  Ez++;
                  if (ii[x +1][y][z+1]) { /* xz face */
                    Fxz++;
                  }
                } /* z edge */
              } /*  else  yz face */
            } else { /* not y edge][ is x edge */
              if (ii[x][y][z+1]) {  /* z edge */
                Ez++; 
                if (ii[x+1][y][z+1]) {  /* xz face */
                  Fxz++;
                }
              } /* z edge */
            } /*  else  y edge */
          }  /*  else xy face */
        } else { /* not x edge */
          if (ii[x][y+1][z]) { /* y edge */
            Ey++;
            if (ii[x][y][z+1] && ii[x][y+1][z+1]) {  /* is yz face */
              Ez++;
              Fyz++;
            } else {  /* not yz face, is y edge */
              if (ii[x][y][z+1]) {  /* z edge */
                Ez++;
              }
            } /*  else  yz face */
          } else {  /* not y edge */
            if (ii[x][y][z+1]) {  /* z edge */
              Ez++;
            }        
          } /*  else  y edge */
        } /*  else  x edge */
      } /* positive voxel */
    } /* for x */
  } /* for y */
} /* for z */

fprintf(stdout,"P = %d\nEx = %d\nEy = %d\nEz = %d\nFxy = %d\nFxz = %d\nFyz = %d\nC = %d\n",P,Ex,Ey,Ez,Fxy,Fxz,Fyz,C);
}

char*** make_3d_arr(int x,int y,int z)
/* allocates memory and array pointers for 3d array */
{
    char*** arr;

    int i,j;

     
    GETMEM_0(arr,x,char**)
    GETMEM_0(arr[0],x*y,char*)
    GETMEM_0(arr[0][0],x*y*z,char)

    for(i=0;i<x;i++) {
        arr[i] = arr[0] + (y * i);
        arr[i][0] = arr[0][0] + (y * z * i);
        for(j=0;j<y;j++)
            arr[i][j] = arr[i][0] + (z * j);
        }

    return arr;
}

#if 0
void free_3d_arr(char*** arr)
/* frees memory from 3d array */
{
        mxFree(arr[0][0]);
        mxFree(arr[0]);
        mxFree(arr);
}
#endif
