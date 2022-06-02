/* Copyright 5/2/06 Washington University.  All Rights Reserved.
   x2_to_z.c  $Revision: 1.4 $ */
#include <stdio.h>
#include <math.h>
#include <gsl/gsl_cdf.h>
#include "constants.h"
#include "x2_to_z.h"

void x2_to_z(double *x,double *z,int vol,double *df){
    int i;
    double td;
    for(i=0;i<vol;i++) {

        //if(x[i]==(double)UNSAMPLED_VOXEL || x[i]<0. || df[i]<=0.) {
        //START171211
        if(x[i]==(double)UNSAMPLED_VOXEL || x[i]<0. || df[i]<=0. || !isfinite(x[i])) {

            z[i] = (double)UNSAMPLED_VOXEL;
            }

        //START171212
        else if(x[i]==0.) {
            z[i]=0;
            }

        else {
            td = gsl_cdf_chisq_Q(x[i],df[i]);
            z[i] = gsl_cdf_ugaussian_Qinv(td/2.);
            }
        }
    }


//171215         
#ifndef M_LN2
    #define M_LN2 0.693147180559945309417
#endif
#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif
void x2_to_z_asym(double *x,double *z,int vol,double *df){
    int i,j;
    double log2pi,z0sq,dfdiv2,xdiv2,product,logp,td;
    log2pi = (double)M_LN2 + log((double)M_PI);
    for(j=0;j<vol;j++){
        if(x[j]==(double)UNSAMPLED_VOXEL||x[j]<0.||df[j]<=0.||!isfinite(x[j])||(x[j]==0.&&df[j]==1)){
            z[j]=(double)UNSAMPLED_VOXEL;
            }
        else if(x[j]==0.){
            z[j]=0.;
            }
        else if(df[j]==1){
            dfdiv2=df[j]/2.;

            //for(product=1.,i=1;i<=20;i++)product*=-2.*(dfdiv2-i)/x[j];
            //START171219
            //for(logp=0.,product=1.,i=1;i<=20;i++)logp+=(product*=-2.*(dfdiv2-(double)i)/x[j]);
            #if 0
            for(logp=0.,product=1.,i=1;i<=20;i++){
                logp+=(product*=-2.*(dfdiv2-(double)i)/x[j]);
                //logp+=(product*=(-2.*(dfdiv2-(double)i)/x[j]));
                //printf("-2.*(dfdiv2-(double)i)/x[j]=%f product=%f logp=%f\n",-2.*(dfdiv2-(double)i)/x[j],product,logp);
                }
            #endif
            //START180105
            for(logp=0.,product=1.,i=1;i<=20;i++)logp+=(product*=2.*(dfdiv2-(double)i)/x[j]);

            //printf("here0 logp=%f\n",logp);fflush(stdout);

            xdiv2=x[j]/2.;

            //logp=(dfdiv2-1.)*log(xdiv2)-xdiv2-lgamma(dfdiv2)+log(1.+product); 
            //START171219
            logp=(dfdiv2-1.)*log(xdiv2)-xdiv2-lgamma(dfdiv2)+log(1.+logp); 

            if(!isfinite(logp)){
                z[j]=(double)UNSAMPLED_VOXEL;
                }

            else if(logp < -14.05) {
                z0sq = -2.*logp - log2pi;
                z[j] = sqrt(z0sq);
                for(i=0;i<3;i++) {
                    z[j] = sqrt(z0sq - 2*log(z[j]) + 2*log(1-pow(z[j],-2.)+3*pow(z[j],-4.)));
                    }
                }
            else {
                td=gsl_cdf_chisq_Q(x[j],df[j]);
                z[j]=gsl_cdf_ugaussian_Qinv(td/2.);
                }
            }
        else{
            td=gsl_cdf_chisq_Q(x[j],df[j]);
            z[j]=gsl_cdf_ugaussian_Qinv(td/2.);
            }
        }
    }
