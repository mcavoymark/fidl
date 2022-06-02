/*********************************************************

Function: matrix_mult

Purpose: Multiply and NxK matrix by a KxM matrix, i.e.
        Result = AB where A is NxK and B is KxM when Transpose=0, and
        Result = transpose(A)B where A is NxK and B is KxM when Transpose=1

By: John Ollinger

Date: 8/27/98

Convention: Matrices are assumed to follow the numerical recipes
            conventions for indices, i.e., starting at one.

**********************************************************/

/*$Revision: 12.80 $*/

#include <math.h>
#include <fidl.h>
#include <nrutil.h>

/**********************************************************************/
float **matrix_mult(float **A,float **B,int NrowA,int NcolA,int NrowB,int NcolB,int Transpose)
/**********************************************************************/

{

int	i,j,k;

float	**C;


switch(Transpose) {
    case TRANSPOSE_NONE:
        /* Don't transpose the first matrix in product. */
        C = matrix(1,NrowA,1,NcolB);
        for(i=1;i<=NrowA;i++) {
            for(j=1;j<=NcolB;j++) {
                C[i][j] = 0.;
                for(k=1;k<=NcolA;k++)
                    C[i][j] += A[i][k]*B[k][j];
                }
            }
        break;
    case TRANSPOSE_FIRST:
        C = matrix(1,NcolA,1,NcolB);
        for(i=1;i<=NcolA;i++) {
            for(j=1;j<=NcolB;j++) {
                C[i][j] = 0.;
                for(k=1;k<=NrowA;k++)
                    C[i][j] += A[k][i]*B[k][j];
                }
            }
        break;
    case TRANSPOSE_SECOND:
        C = matrix(1,NrowA,1,NrowB);
        for(i=1;i<=NrowA;i++) {
            for(j=1;j<=NrowB;j++) {
                C[i][j] = 0.;
                for(k=1;k<=NcolB;k++)
                    C[i][j] += A[i][k]*B[j][k];
                }
            }
        break;
    default: 
        fprintf(stderr,"Invalid transpose parameter in matrix_mult.\n");
    }

return(C);

}
