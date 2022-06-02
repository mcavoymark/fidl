/*#include <math.h>
#define NRANSI
#include "nrutil.h"*/

/*#include <stdio.h>
#include <stdlib.h>
#include <string.h>*/
#include <math.h>

/*#include <fidl.h>*/
/*START150323*/
#include "fidl.h"

#define NRANSI
#include "nrutil.h"


/*float pythag(float a, float b)*/
double pythag(double a, double b)
{
	/*float absa,absb;*/
	double absa,absb;
	absa=fabs(a);
	absb=fabs(b);

	/*if (absa > absb) return absa*sqrt(1.0+SQR(absb/absa));
	else return (absb == 0.0 ? 0.0 : absb*sqrt(1.0+SQR(absa/absb)));*/
	if (absa > absb) return absa*sqrt(1.0+DSQR(absb/absa));
	else return (absb == 0.0 ? 0.0 : absb*sqrt(1.0+DSQR(absa/absb)));
}
#undef NRANSI
/* (C) Copr. 1986-92 Numerical Recipes Software |oV'41.5.){2p49. */
