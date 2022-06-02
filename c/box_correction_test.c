/**************
box_corection.c
**************/

/*$Revision:$*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>
#include <nrutil.h>

void transform_ATAm1_tc_test(char **glm_files,int num_glm_files,int ***tc,int num_tc,int **num_tc_to_sum)
{

int i,j,k;

double **ATAm1,**transform,denominator,**ATAm1xtransformT,**transformed_ATAm1;

LinearModel *glm;

ATAm1 = (double **)dmatrix(1,3,1,3);
ATAm1[1][1]=1.1; ATAm1[1][2]=3.2; ATAm1[1][3]=4.3;
ATAm1[2][1]=3.2; ATAm1[2][2]=1.5; ATAm1[2][3]=3.7;
ATAm1[3][1]=4.3; ATAm1[3][2]=3.7; ATAm1[3][3]=1.9;

printf("here transform_ATAm1_tc_test\n");

for(j=0;j<1;j++) {
    printf("here0\n");
    transform = (double **)dmatrix_0(1,num_tc,1,3);
    printf("here1\n");
    for(i=0;i<num_tc;i++) {
        if(num_tc_to_sum[i][j] == 1) {
            printf("here2a tc[%d][%d][0]=%d\n",i,j,tc[i][j][0]);
            transform[i+1][tc[i][j][0]] = 1;
            printf("here2b\n");
	    }
	else {
	    for(denominator=k=0;k<num_tc_to_sum[i][j];k++) denominator += 1/ATAm1[tc[i][j][k]][tc[i][j][k]];
	    for(k=0;k<num_tc_to_sum[i][j];k++) transform[i+1][tc[i][j][k]] = (1/ATAm1[tc[i][j][k]][tc[i][j][k]])/denominator; 
	    } 
         }
    ATAm1xtransformT = dmatrix_mult(ATAm1,transform,3,3,num_tc,3,(int)TRANSPOSE_SECOND);
    transformed_ATAm1 = dmatrix_mult(transform,ATAm1xtransformT,num_tc,3,3,num_tc,(int)TRANSPOSE_NONE);

    printf("transform\n");
    for(i=1;i<=num_tc;i++) {
        for(k=1;k<=3;k++) printf("%f ",transform[i][k]); 
	printf("\n");
        }
    printf("\n");

    printf("transformed_ATAm1\n");
    for(i=1;i<=num_tc;i++) {
        for(k=1;k<=num_tc;k++) printf("%f ",transformed_ATAm1[i][k]); 
	printf("\n");
        }
    printf("\n");

    free_dmatrix(transform,1,num_tc,1,3);
    free_dmatrix(ATAm1,1,num_tc,1,3);
    free_dmatrix(ATAm1xtransformT,1,3,1,num_tc);
    free_dmatrix(transformed_ATAm1,1,num_tc,1,num_tc);
    }
}
