/* Copyright 12/31/99 Washington University.  All Rights Reserved.
   boynton_model.c  $Revision: 12.41 $ */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "fidl.h"
float _boynton_model(int argc,char **argv)
{
    float   t,duration,delta,tau;
    double   td,hrfd;
    Hrfparam param;
    t = *((float *)argv[0]);
    duration = *((float *)argv[1]);
    delta = *((float *)argv[2]);
    tau = *((float *)argv[3]);
    td = (double)t;
    param.duration = (double)duration;
    param.delta = (double)delta;
    param.tau = (double)tau;
    param.A = (double)HRF_A;
    param.C = (double)HRF_C;
    boynton_model(1,&td,&param,1,&hrfd);
    return (float)hrfd;
}
void boynton_model(int nt,double *t,Hrfparam *param,int lc_impulse,double *hrf)
{
    int i;
    double tp,b,x,p1,p2,duration=param->duration,delta=param->delta,tau=param->tau,A=param->A,C=param->C;

    /*printf("boynton_model duration=%f delta=%f tau=%f A=%f C=%f\n",duration,delta,tau,A,C);*/


    if(duration<.5 && lc_impulse) { /*Impulse response.*/
        for(i=0;i<nt;i++) {
            if((tp=t[i]-delta)>0.) {
                hrf[i] = (tp*tp/(tau*tau))*exp(-tp/tau);
                }
            else {
                hrf[i] = 0.;
                }
            }
        }
    else {
        for(i=0;i<nt;i++) {
            if((tp=t[i]-delta)>duration) {
                b = duration;
                }
            else {
                b = tp>0. ? tp : 0.;
                }
            x = (tp - b)/tau;
            p1 = x*x + 2.*x + 2.;
            p2 = (tp*tp/(tau*tau)) + 2.*tp/tau + 2.;
            hrf[i] = A*(exp(-(tp-b)/tau)*p1 - exp(-tp/tau)*p2)+C;
            }
        }
    /*printf("hrf=");for(i=0;i<nt;i++)printf("%f ",hrf[i]);printf("\n");*/
}


#if 0
float _boynton_model(int argc,char **argv)
{
float   t,duration,delta,tau;
double   td,durationd,deltad,taud,hrfd;

t = *((float *)argv[0]);
duration = *((float *)argv[1]);
delta = *((float *)argv[2]);
tau = *((float *)argv[3]);

td = (double)t;
durationd = (double)duration;
deltad = (double)delta;
taud = (double)tau;

boynton_model(1,&td,durationd,deltad,taud,1,&hrfd);
return (float)hrfd;
}
void boynton_model(int nt,double *t,double duration,double delta,double tau,double A,double C,int lc_impulse,double *hrf)
{
int i;
double tp,b,x,p1,p2;
if(duration<.5 && lc_impulse) { /*Impulse response.*/
    for(i=0;i<nt;i++) {
        if((tp=t[i]-delta)>0.) {
            hrf[i] = (tp*tp/(tau*tau))*exp(-tp/tau);
            }
        else {
            hrf[i] = 0.;
            }
        }    
    }
else {
    for(i=0;i<nt;i++) {
        if((tp=t[i]-delta)>duration) {
            b = duration;
            }
        else {
            b = tp>0. ? tp : 0.;
            }
        x = (tp - b)/tau;
        p1 = x*x + 2.*x + 2.;
        p2 = (tp*tp/(tau*tau)) + 2.*tp/tau + 2.;
        /*hrf[i] = 1.93*tau*(exp(-(tp-b)/tau)*p1 - exp(-tp/tau)*p2);*/
        hrf[i] = A*(exp(-(tp-b)/tau)*p1 - exp(-tp/tau)*p2)+C;
        }
    }
}
#endif

#if 0
float _boynton_model(int argc,char **argv)
{
float   t,duration,delta,tau,hrf;

t = *((float *)argv[0]);
duration = *((float *)argv[1]);
delta = *((float *)argv[2]);
tau = *((float *)argv[3]);

hrf = boynton_model(t,duration,delta,tau,(int)TRUE);
/*printf("t=%f duration=%f delta=%f tau=%f hrf=%f\n",t,duration,delta,tau,hrf);*/
return hrf;
}

float boynton_model(float t,float duration,float delta,float tau,int lc_impulse)
{
float	tp,b,x,hrf,p1,p2;

tp = t - delta;
/*if(duration < .001) {*/
if(duration < .5 && lc_impulse == (int)TRUE) {
    /* Impulse response. */
    if(tp > 0) 
        hrf = (tp*tp/(tau*tau))*exp(-tp/tau);
    else
        hrf = 0.;
    }
else {
    if(tp > duration) {
        b = duration;
        }
    else {
        if(tp > 0)
            b = tp;
        else
            b = 0;
        }
    x = (tp - b)/tau;
    p1 = x*x + 2.*x + 2.;
    p2 = (tp*tp/(tau*tau)) + 2.*tp/tau + 2.;
    hrf = 1.93*tau*(exp(-(tp-b)/tau)*p1 - exp(-tp/tau)*p2);
    }
#endif
