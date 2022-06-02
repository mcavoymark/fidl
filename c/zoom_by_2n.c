/* Copyright 12/31/99 Washington University.  All Rights Reserved.
   zoom_by_2n.c  $Revision: 1.9 $ */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#ifndef __sun__
    #include <stdint.h>
#endif
#include "fidl.h"
int _zoom_by_2n(int argc,char **argv){
    void zoom_by_2n(float *image_in,float *image_out,int xdim,int ydim,int zoom_factor,int zoom_type);
    float *image_in,*image_out;
    int xdim,ydim,zoom_factor,zoom_type;
    image_in = (float *)argv[0];
    image_out = (float *)argv[1];
    #ifdef __sun__
        xdim = (int)argv[2];
        ydim = (int)argv[3];
        zoom_factor = (int)argv[4];
        zoom_type = (int)argv[5];
    #else
        xdim = (intptr_t)argv[2];
        ydim = (intptr_t)argv[3];
        zoom_factor = (intptr_t)argv[4];
        zoom_type = (intptr_t)argv[5];
    #endif
    zoom_by_2n(image_in,image_out,xdim,ydim,zoom_factor,zoom_type);

    /*START150929*/
    return 1;

    }



#if 0
double
bilinterp(X1,X2,Y,N1,N2,xx1,xx2)
double *X1,*X2,**Y,xx1,xx2;
int N1,N2;              /* length of X1 and X2 */
{
   int i,j,k;
   double y1,y2,y3,y4,t,u;

   for (i=0; i<N1-1; i++)
      if (xx1<X1[i+1] || i==N1-2) {
         j=i;
         break;
      }
   for (i=0; i<N2-1; i++)
      if (xx2<X2[i+1] || i==N2-2) {
         k=i;
         break;
      }
   y1=Y[j][k];
   y2=Y[j+1][k];
   y3=Y[j+1][k+1];
   y4=Y[j][k+1];
   t=(xx1-X1[j])/(X1[j+1]-X1[j]);
   u=(xx2-X2[k])/(X2[k+1]-X2[k]);
   return (1-t)*(1-u)*y1 + t*(1-u)*y2 + t*u*y3 + (1-t)*u*y4;
}
#endif



/***********************************************************************************************/
void zoom_by_2n(float *image_in,float *image_out,int xdim,int ydim,int zoom_factor,int zoom_type)
/***********************************************************************************************/
{
float	del,yd1,yd2,xd1,xd2,dxy;

int	xin,xin_off,xout,xout_off,yin,yout,i,j,xdim_out,ydim_out,nrep;

del = 1./(2*zoom_factor);
dxy = 1./zoom_factor;
xdim_out = zoom_factor*xdim;
ydim_out = zoom_factor*ydim;
nrep = zoom_factor/2;

switch(zoom_type) {

case BILINEAR: {

    #if 0
    /* First and last rows. */
    xin_off = (ydim-1)*xdim;
    xout_off = (ydim_out-nrep)*xdim_out;
    image_out[0] = image_in[0];
    image_out[xout_off] = image_in[xin_off];
    for(yout=0,xout=0;yout<nrep;yout++) {
        for(i=0;i<nrep;i++,xout++) {
            image_out[xout] = image_in[0];
            image_out[xout+xout_off] = image_in[xin_off-1];
            }
        for(xin=1;xin<xdim-1;xin++) {
            xd1 = 1-del;
            xd2 = del;
            for(i=0;i<zoom_factor;i++,xout++) {
                image_out[xout] = xd1*image_in[xin-1] + xd2*image_in[xin];
                image_out[xout+xout_off] = xd1*image_in[xin-1+xin_off] + xd2*image_in[xin+xin_off];
                xd1 -= dxy;
                xd2 += dxy;
                }
            }
        for(i=0;i<nrep;i++,xout++) {
            image_out[xout] = image_in[xdim-1];
            image_out[xout+xout_off] = image_in[xout_off+xdim-1];
            }
        }
    image_out[xout] = image_in[xin];
    image_out[xout+xout_off] = image_in[xin+xin_off];

    /* Middle rows. */
    for(yin=1,xin_off=0,xout_off=nrep*xdim_out,yout=0;yin<ydim-1;yin++,xin_off+=xdim) {
        yd1 = 1-del;
        yd2 = del;
        for(j=0;j<zoom_factor;j++,yout++,xout_off+=xdim_out) {
            for(xout=0,i=0;i<nrep;i++,xout++)
                image_out[xout+xout_off] = yd1*image_in[xin_off] 
                                         + yd2*image_in[xin_off+xdim];
            for(xin=1;xin<xdim;xin++) {
                xd1 = 1-del;
                xd2 = del;
                for(i=0;i<zoom_factor;i++,xout++) {
                    image_out[xout+xout_off] = xd1*yd1*image_in[xin-1+xin_off] 
                                             + xd2*yd1*image_in[xin+xin_off]
                                             + xd1*yd2*image_in[xin-1+xin_off+xdim]
                                             + xd2*yd2*image_in[xin+xin_off+xdim];
                    xd1 -= dxy;
                    xd2 += dxy;
                    }
                }
            for(i=0;i<nrep;i++,xout++)
                image_out[xout+xout_off] = yd1*image_in[xin+xin_off] 
                                         + yd2*image_in[xin+xin_off+xdim];
            yd1 -= dxy;
            yd2 += dxy;
            }
        }
    break;
    }
    #endif

    /* First and last rows. */
    xin_off = (ydim-1)*xdim;
    xout_off = (ydim_out-nrep)*xdim_out;
    for(yout=0,xout=0;yout<nrep;yout++) {
        for(i=0;i<nrep;i++,xout++) {
            image_out[xout] = image_in[0];
            image_out[xout+xout_off] = image_in[xin_off];
            }
        for(xin=1;xin<xdim;xin++) {
            xd1 = 1-del;
            xd2 = del;
            for(i=0;i<zoom_factor;i++,xout++) {
                image_out[xout] = xd1*image_in[xin-1] + xd2*image_in[xin];
                image_out[xout+xout_off] = xd1*image_in[xin-1+xin_off] + xd2*image_in[xin+xin_off];
                xd1 -= dxy;
                xd2 += dxy;
                }
            }
        for(i=0;i<nrep;i++,xout++) {
            image_out[xout] = image_in[xdim-1];
            image_out[xout+xout_off] = image_in[xin_off+xdim-1];
            }
        }

    /* Middle rows. */
    for(yin=1,xin_off=0,xout_off=nrep*xdim_out,yout=0;yin<ydim;yin++,xin_off+=xdim) {
        yd1 = 1-del;
        yd2 = del;
        for(j=0;j<zoom_factor;j++,yout++,xout_off+=xdim_out) {
            for(xout=0,i=0;i<nrep;i++,xout++) {
                image_out[xout+xout_off] = yd1*image_in[xin_off] + yd2*image_in[xin_off+xdim];
                }
            for(xin=1;xin<xdim;xin++) {
                xd1 = 1-del;
                xd2 = del;
                for(i=0;i<zoom_factor;i++,xout++) {
                    image_out[xout+xout_off] = xd1*yd1*image_in[xin-1+xin_off]
                                             + xd2*yd1*image_in[xin+xin_off]
                                             + xd1*yd2*image_in[xin-1+xin_off+xdim]
                                             + xd2*yd2*image_in[xin+xin_off+xdim];
                    xd1 -= dxy;
                    xd2 += dxy;
                    }
                }
            for(i=0;i<nrep;i++,xout++)
                image_out[xout+xout_off] = yd1*image_in[xin+xin_off-1]
                                         + yd2*image_in[xin+xin_off+xdim-1];
            yd1 -= dxy;
            yd2 += dxy;
            }
        }
    break;
    }















#if 0
double
bilinterp(X1,X2,Y,N1,N2,xx1,xx2)
double *X1,*X2,**Y,xx1,xx2;
int N1,N2;              /* length of X1 and X2 */
{
   int i,j,k;
   double y1,y2,y3,y4,t,u;

   for (i=0; i<N1-1; i++)
      if (xx1<X1[i+1] || i==N1-2) {
         j=i;
         break;
      }
   for (i=0; i<N2-1; i++)
      if (xx2<X2[i+1] || i==N2-2) {
         k=i;
         break;
      }
   y1=Y[j][k];
   y2=Y[j+1][k];
   y3=Y[j+1][k+1];
   y4=Y[j][k+1];
   t=(xx1-X1[j])/(X1[j+1]-X1[j]);
   u=(xx2-X2[k])/(X2[k+1]-X2[k]);
   return (1-t)*(1-u)*y1 + t*(1-u)*y2 + t*u*y3 + (1-t)*u*y4;
}
#endif














case NEAREST_NEIGHBOR: {
    for(yin=0,xin_off=0,xout_off=0,yout=0;yin<ydim-1;yin++,xin_off+=xdim) {
        for(j=0;j<zoom_factor;j++,yout++,xout_off+=xdim_out) {
            for(xin=0,xout=0;xin<xdim;xin++) {
                for(i=0;i<zoom_factor;i++,xout++) {
                    image_out[xout+xout_off] = image_in[xin+xin_off];
                    }
                }
            }
        }
    break;
    }
default: {}
 }


}
 
