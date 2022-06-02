double erffc(double x)
{
	double gammp(double a, double x);
	double gammq(double a, double x);

	return x < 0.0 ? 1.0+gammp(0.5,x*x) : gammq(0.5,x*x);
}
/* (C) Copr. 1986-92 Numerical Recipes Software |oV'41.5.){2p49. */
