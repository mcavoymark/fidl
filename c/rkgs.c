/***********************************************************************
rkgs.c -> /home/fafner/jmo/src/util/rkgs.f
************************************************************************
c
c Subroutine: RKGS
c
c Purpose: Solve a system of first order differential equations.
c
c Usage: call RKGS(prmt,y,dery,ndim,ihlsf,aux)
c
c Parameters:
c       prmt: Array of values specifying interval and accuracy.
c             prmt(1): Lower bound of interval (input).
c             prmt(2): Upper bound of interval (input).
c             prmt(3): Initial increment of independent variable (input)
c             prmt(4): Upper bound of error. (input)
c             prmt(5): Set nonzero in "outp" to terminate (not input).
c       y:    Vector of initial values. (input).
c       dery: Not used, formerly error weights for step size control.
c       ndim: Number of equations in system. (input)
c       ihlf: Number of bisections of initial increment (output).
c       fct:  User supplied function.
c       aux:  An auxiliary storage array of dimension (ndim,8)
c
c User supplied subroutines:
c       fct(x,y,dery): Compute next step (x) from last step (y) and
c                      increment (dery).
c
c References: IBM Scientific Subroutine Package.
c             Ralston/Wilf, "Mathematical Methods for Digital
c             Computers," Wiley, 1960, pp. 110-120.
c
c The code copied from the IBM SSP has been modified in
c the following ways:
c       1. The test for truncation error has been modified so that
c          the error is normalized to the current value of y.  This
c          improves performance when the true value of y(i) varies
c          from that of some y(j) by several orders of magnitude.
c          In this case, the original value would alter the step
c          size even if the truncation error in the smaller
c          variable was very large as a percentage of true value.
c       2. The last step size used is now returned in prmt(3).
c       3. The step size may be modified within the program
c          such that it becomes larger than the initial value.
c       4. The step size is no longer doubled when the truncation
c          error is less than 2% of the tolerance.  The factor by
c          which it is increased is now variable.  It starts at 2,
c          but is decreased if the step size was halved on the last
c          step.  This helps eliminate the situation where the
c          step size is halved and doubled on alternate steps.
c       5. The returned step size is limited to be less than or
c          equal to the length of the integration interval.
c       6. If the maximum number of bisections is exceeded, the
c          routine returns immediately.
c          and writes an error message to this effect.
c       7. The subroutine "outp" is no longer used.
c       8. The integer ideqn is passed to "fct".  It is intended
c          to be a selector for the function being integrated.
c
c Copied right 1988 by John Ollinger
c
c**************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>

void rkgs(double *prmt,double *y,int ndim,int ihlf,ideqn)
{

int i,j,istep,iend,itest,lchlf;

double a[4]={.5,.2928932,1.707107,1},b[4]={2,1,1,2},c[4]={.5,.2928932,1.707107,.5},
       **aux,x,xend,xlen,h,tolrnc,stpfct,temp,h1,r1,r2,delt;
       
a[3] = 1./6.;
GETMEM(aux,ndim,double *)
for(i=0;i<ndim;i++) GETMEM(aux[i],8,double)


x = prmt[0];
xend = prmt[1];
xlen = xend - x;
h = prmt[2];
/*
c       Normalize truncation error tolerance so that the value
c       passed corresponds to the fractional error.
*/
tolrnc = prmt[3]*ndim;
prmt[4] = 0;
stpfct = 2;

fct(x,y,dery,ideqn,ndim,prmt);

/*
c       Error test.
        if(h*(xend-x)) 38,37,2
2       continue
*/
temp = h*xlen;
if(temp < -.000001) {
/*38*/ihlf = 13;
    return;
    }
else if(temp > -.000001 && temp < .000001) {
/*37*/ihlf = 12;
    return;
    }
else { 
    /*Start of a Runge-Kutta step. Preparations for Runge-Kutta method.*/
    for(i=0;i<ndim;i++) {
        aux[i][0] = y[i];
        aux[i][1] = dery[i];
        aux[i][2] = 0;
        aux[i][5] = 0;
        }
    h = 2*h;
    ihlf = -1;
    istep = 0;
    iend = 0;

    while(1) {
	/*Start of Runge-Kutta step.*/
/*4*/   temp = (x+h-xend)*h;
        if(temp > .000001) {
            h1 = .5*h; 
	    h = xend - x;
            iend = 1;
            }
        else if(temp > -.000001 && temp < .000001)
	    iend = 1;

        /*Record initial values of this step.*/
/*9*/   for(itest=0;;istep++) {
            /*Start of innermost Runge-Kutta "loop".*/

            for(j=0;j<4;j++) {
                for(i=0;i<ndim;i++) {
                    r1 = h*dery[i];
                    r2 = a[j]*(r1-b[j]*aux[i][5]);
                    y[i] += r2;
                    r2 *= 3;
                    aux[i][5] += r2 - c[j]*r1;
                    }
                if(!j || j==2) x += .5*h;
                if(j != 3) fct(x,y,dery,ideqn,ndim,prmt);
		}

            if(itest < .000001) {
                /*Cannot test accuracy if itest equals zero.*/
                for(i=0;i<ndim;i++) aux[i][3] = y[i];
	        itest = 1;
	        istep = 2*istep - 2;
/*18*/          ihlf++;
                x += -h;
	        h *= .5;
                for(i=0;i<ndim;i++) {
	            y[i] = aux[i][0];
	            dery[i] = aux[i][1];
	            aux[i][5] = aux[i][2];
	            }
		/*go to 9 -> for(j=0;j<4;j++)*/
	        }
	    else {
	        /*Test accuracy since itest equals one.*/
/*20*/          imod = istep/2;
	        temp = istep-2*imod;
	        if(temp < -.000001 || temp > .000001) {
/*21*/              fct(x,y,dery,ideqn,ndim,prmt);
		    for(i=0;i<ndim;i++) {
		        aux[i][4] = y[i];
		        aux[i][6] = dery[i];
		        }
		    /*go to 9 -> for(j=0;j<4;j++)*/
		    }
	        else {
	            /*Compute test value of delt.*/ 
/*23*/              for(delt=i=0;i<ndim;i++) 
		        if(y[i] < -.000001 || y[i] > .000001) delt += fabs((aux[i][3]-y[i])/y[i]); 
		    if(delt-tolrnc > .000001) {
		        /* Error is too great.  Bisect interval if maximum number
		           of bisections not reached.  Otherwise, continue with
		           old step size. */
/*25*/                  if(ihlf-10 < -.000001) {
/*26*/                      for(i=0;i<ndim;i++) aux[i][3] = aux[i][4];
		            istep = 2*istep - 4;
			    x -= h;
			    iend = 0;
			    h1 = h;
			    lchlf = 1;
/*18*/      		    ihlf++;
            		    x += -h;
            		    h *= .5;
                            for(i=0;i<ndim;i++) {
                	        y[i] = aux[i][0];
                	        dery[i] = aux[i][1];
                	        aux[i][5] = aux[i][2];
                	        }
			    /*go to 9 -> for(j=0;j<4;j++)*/
			    }
		        else {
/*36*/                      ihlf = 13;
			    fprintf(stderr,"Max number of bisections exceeded first time: t = %f\n",x);
			    exit(-1);
			    }
		        }
		    else {
		        /*Result values are good.*/
/*28*/                  fct(x,y,dery,ideqn,ndim,prmt);
		        for(i=0;i<ndim;i++) {
			    aux[i][0] = y[i];
			    aux[i][1] = dery[i];
			    aux[i][2] = aux[i][5];
			    }
		        if(iend > .000001) {
			    if(h1 > xlen) h1 = xlen; /*Limit step size to length of interval.*/
			    prmt[2] = h1;
			    printf("here: go to 39 -> return\n");
/*39*/                      return;
			    }
		        else
			    h1 = h;
		        ihlf--;
		        istep /= 2;
	 	        h *= 2;
		        imod = istep/2;
		        temp = istep-2*imod;
		        if(temp > -.000001 && temp < .000001) {
/*34*/                      sumdelt += delt;
			    nsum++;
			    if(nsum >= 3) {
			        if(sumdelt < .06*tolrnc) {
				    /*Error averages less than 2% of tolerance per step, double the step size.*/
				    if(lchlf == 1) 
				        /*Step size just halved, decrease factor.*/
				        stpfct = 1. + .5*(stpfct - 1.); 
				    ihlf--;
				    istep /= 2;
				    h *= 2;
			            }
			        nsum = 0;
			        sumdelt = 0.;
			        }
			    /*go to 4*/ break;
			    }
			else {
			    /*go to 4*/ break;
			    }
			} /*else*/
		    } /*else*/
		} /*else*/
	    } /*for(itest=0;;istep++)*/
        } /*while(1)*/
    } /*if,else*/
}
