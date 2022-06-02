/* Copyright 11/26/03 Washington University.  All Rights Reserved.
   so_erf.c  $Revision: 1.2 $

double erff(double x)
{
	double gammp(double a, double x);

	return x < 0.0 ? -gammp(0.5,x*x) : gammp(0.5,x*x);
}
/* (C) Copr. 1986-92 Numerical Recipes Software |oV'41.5.){2p49. */
