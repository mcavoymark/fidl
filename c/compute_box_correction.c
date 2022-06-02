/* Copyright 1/28/02 Washington University.  All Rights Reserved.
   compute_box_correction.c  $Revision: 1.5 $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <nrutil.h>
#include <fidl.h>

typedef struct Matrix_of_coeff_struct {
    char **epsilon_str;
    int nepsilon,**matrix_dim;
    double **matrix_of_coeff;
    }
Matrix_of_coeff;
Matrix_of_coeff *read_matrix_of_coeff(char *matrix_of_coeff_file);

static char rcsid[] = "$Header: /home/hannah/mcavoy/idl/clib/RCS/compute_box_correction.c,v 1.5 2003/04/23 21:46:07 mcavoy Exp $";

/************************/
main(int argc,char **argv)
/************************/
{
char *matrix_of_coeff_file,*covariance_file/*,*output_file*/;
int i,j,k,m,n,lenvol_cov,lc_check_matrix_of_coeff=(int)FALSE;
float *temp_float;
double **cov,**M,**covMT,**S,epsilon,*temp_double,box,sum,ss;
Matrix_of_coeff *moc;
Interfile_header *ifh;
FILE *fp;

print_version_number(rcsid);
if(argc < 3) {
    fprintf(stderr,"        -coefficients:          Text file that contains a contrast matrix for each main effect and interaction.\n");
    fprintf(stderr,"        -covariance:            4dfp file that contains the covariance matrix.\n");
    /*fprintf(stderr,"        -output:                Output filename that epsilons are written to.\n");*/
    fprintf(stderr,"        -check_matrix_of_coeff  Echos the coefficient matrix to the terminal.\n");
    exit(-1);
    }
for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-coefficients") && argc > i+1)
        matrix_of_coeff_file = argv[++i];
    if(!strcmp(argv[i],"-covariance") && argc > i+1)
        covariance_file = argv[++i];
    /*if(!strcmp(argv[i],"-output") && argc > i+1)
        output_file = argv[++i];*/
    if(!strcmp(argv[i],"-check_matrix_of_coeff"))
        lc_check_matrix_of_coeff = (int)TRUE;
    }
if(!(moc = (Matrix_of_coeff *)read_matrix_of_coeff(matrix_of_coeff_file))) exit(-1);
if(lc_check_matrix_of_coeff) {
    for(m=k=0;k<moc->nepsilon;k++) {
        printf("%s\n",moc->epsilon_str[k]);
        for(i=0;i<moc->matrix_dim[k][0];i++,m++) {
            for(j=0;j<moc->matrix_dim[k][1];j++) printf("%f ",moc->matrix_of_coeff[m][j]); 
            printf("\n");
            }
        }
    printf("\n");
    }

if(!(ifh = read_ifh(covariance_file))) exit(-1);

for(k=0;k<moc->nepsilon;k++) {
   if(ifh->dim1 != moc->matrix_dim[k][1]) {
        printf("%s ifh->dim1 = %d no. of coeff. = %d Discrepancy. Abort!\n",moc->epsilon_str[k],ifh->dim1,moc->matrix_dim[k][1]);
        exit(-1);
        }
    }

lenvol_cov = ifh->dim1*ifh->dim2;
cov = (double **)dmatrix(1,ifh->dim1,1,ifh->dim2);
if(ifh->number_format == (int)FLOAT_IF) {
    GETMEM(temp_float,lenvol_cov,float);
    if(!read_float(covariance_file,temp_float,lenvol_cov)) exit(-1);
    for(k=0,i=1;i<=ifh->dim1;i++) {
        for(j=1;j<=ifh->dim2;j++,k++) {
            cov[i][j] = (double)temp_float[k];
            }
        }
    }
else {
    GETMEM(temp_double,lenvol_cov,double);
    if(!read_double(covariance_file,temp_double,lenvol_cov)) exit(-1);
    for(k=0,i=1;i<=ifh->dim1;i++) {
        for(j=1;j<=ifh->dim2;j++,k++) {
            cov[i][j] = temp_double[k];
            }
        }
    }

/*printf("temp_double\n");
for(k=0,i=1;i<=ifh->dim1;i++) {
    for(j=1;j<=ifh->dim2;j++,k++) printf("%.3f ",temp_double[k]);
    printf("\n");
    }
printf("\n");*/
printf("cov\n");
for(k=0,i=1;i<=ifh->dim1;i++) {
    for(j=1;j<=ifh->dim2;j++,k++) printf("%f ",cov[i][j]);
    printf("\n");
    }
printf("\n\n");


box = (double)calculate_epsilon_whole_matrix_box(cov,ifh->dim1);
printf("Box's epsilon = %f\n\n",box);
/*If the numbers don't match eg box=6, then the prototype is being missed.*/



/*if(!(fp = fopen(output_file,"w"))) {
    printf("Error opening %s in compute_box_correction.\n",output_file);
    exit(-1);
    }*/

for(m=k=0;k<moc->nepsilon;k++) {
    M = (double **)dmatrix(1,moc->matrix_dim[k][0],1,moc->matrix_dim[k][1]);
    for(i=1;i<=moc->matrix_dim[k][0];i++,m++) {
        for(j=1;j<=moc->matrix_dim[k][1];j++) {
            M[i][j] = moc->matrix_of_coeff[m][j-1];
            }
        }
    covMT = (double **)dmatrix_mult(cov,M,ifh->dim1,ifh->dim2,moc->matrix_dim[k][0],moc->matrix_dim[k][1],(int)TRANSPOSE_SECOND);
    S = (double **)dmatrix_mult(M,covMT,moc->matrix_dim[k][0],moc->matrix_dim[k][1],ifh->dim1,moc->matrix_dim[k][0],
        (int)TRANSPOSE_NONE);

    for(i=1;i<=moc->matrix_dim[k][0];i++) {
        for(j=1;j<=moc->matrix_dim[k][0];j++) printf("%8.5f ",S[i][j]);
        printf("\n");
        }

    epsilon = (double)calculate_epsilon_whole_matrix(S,moc->matrix_dim[k][0]);
    printf("%s = %f\n",moc->epsilon_str[k],epsilon);

    sum = ss = 0;
    for(i=1;i<=moc->matrix_dim[k][0];i++) {
        sum += S[i][i];
        ss += S[i][i]*S[i][i];
        }
    epsilon = (sum*sum)/(moc->matrix_dim[k][0]*ss);
    printf("TRACE %s = %f\n\n",moc->epsilon_str[k],epsilon);

    free_dmatrix(M,1,moc->matrix_dim[k][0],1,moc->matrix_dim[k][1]);
    free_dmatrix(covMT,1,ifh->dim1,1,moc->matrix_dim[k][0]);
    free_dmatrix(S,1,moc->matrix_dim[k][0],1,moc->matrix_dim[k][0]);
    }
}

/***************************************************************/
Matrix_of_coeff *read_matrix_of_coeff(char *matrix_of_coeff_file)
/***************************************************************/
{
char line[MAXNAME],write_back[MAXNAME],*str_ptr;
int i,j,k,lc_unnormalized=(int)FALSE,nrow,ncol,nstrings; 
double magnitude;
FILE *fp;
Matrix_of_coeff *moc;

GETMEM(moc,1,Matrix_of_coeff);
if(!(fp = fopen(matrix_of_coeff_file,"r"))) {
    printf("Error opening %s in read_matrix_of_coeff.\n",matrix_of_coeff_file);
    return (Matrix_of_coeff *)NULL;
    }
for(moc->nepsilon=0;fgets(line,sizeof(line),fp);) {
    if(strstr(line,"UNNORMALIZED")) {
        lc_unnormalized = (int)TRUE;
        }
    else if(strstr(line,":=")) {
        moc->nepsilon++;
        }
    }
/*printf("moc->nepsilon=%d\n",moc->nepsilon);*/
GETMEM(moc->epsilon_str,moc->nepsilon,char *);

if(!(moc->matrix_dim = (int **)d2int(moc->nepsilon,2))) exit(-1);



for(rewind(fp),k=0;fgets(line,sizeof(line),fp);) {
    if(strstr(line,"UNNORMALIZED")) {
        /*do nothing*/ 
        }
    else if(strstr(line,":=")) {
        grab_string(line,write_back);
        GETMEM(moc->epsilon_str[k],strlen(write_back)+1,char);
        strcpy(moc->epsilon_str[k],write_back);
        str_ptr = getstr(line);
        nstrings=count_strings(str_ptr,write_back,' ');
        if(nstrings != 2) {
            printf("k = %d  nstrings = %d  Should equal 2. Abort!\n",k,nstrings);
            exit(-1);
            }
        /*printf("str_ptr = %s\n",str_ptr);*/
        strings_to_int(str_ptr,moc->matrix_dim[k++],2);
        /*printf("k=%d moc->matrix_dim=%d %d\n",k-1,moc->matrix_dim[k-1][0],moc->matrix_dim[k-1][1]);*/
        }
    }
/*printf("\n");
for(k=0;k<moc->nepsilon;k++) printf("k=%d moc->matrix_dim=%d %d\n",k,moc->matrix_dim[k][0],moc->matrix_dim[k][1]); 
printf("\n");*/
if(k != moc->nepsilon) {
    printf("k = %d  moc->nepsilon = %d  Discrepancy! Abort!\n",k,moc->nepsilon);
    exit(-1);
    }
/*for(k=0;k<moc->nepsilon;k++) printf("k=%d moc->matrix_dim=%d %d\n",k,moc->matrix_dim[k][0],moc->matrix_dim[k][1]);*/
for(nrow=ncol=k=0;k<moc->nepsilon;k++) {
    /*printf("k=%d moc->matrix_dim = %d %d\n",k,moc->matrix_dim[k][0],moc->matrix_dim[k][1]);*/
    nrow += moc->matrix_dim[k][0];
    if(!ncol) {
        ncol =  moc->matrix_dim[k][1];
        }
    else if(ncol != moc->matrix_dim[k][1]) {
        printf("ncol = %d  moc->matrix_dim[%d][1] = %d  Discrepancy! Abort!\n",ncol,k,moc->matrix_dim[k][1]);
        exit(-1);
        }
    }

if(!(moc->matrix_of_coeff = (double **)d2double(nrow,ncol))) exit(-1);

for(rewind(fp),k=0;fgets(line,sizeof(line),fp);) {
    if(strstr(line,"UNNORMALIZED")) {
        /*do nothing*/ 
        }
    else if(strstr(line,":=")) {
        /*do nothing*/
        }
    else if(nstrings=count_strings(line,write_back,' ')) {  /*line could be blank*/
        if(nstrings != ncol) {
            printf("ncol = %d  nstrings = %d  Discrepancy! Abort!\n",ncol,nstrings);
            exit(-1);
            }
        /*printf("write_back=%s\n",write_back);*/
        strings_to_double(write_back,moc->matrix_of_coeff[k++],ncol);
        }
    }
if(k != nrow) {
    printf("nrow = %d  k = %d  Discrepancy! Abort!\n",nrow,k);
    exit(-1);
    }

for(k=0;k<nrow;k++) {
    for(magnitude=j=0;j<ncol;j++) magnitude += moc->matrix_of_coeff[k][j];
    if(fabs(magnitude) > 0.001) {
        printf("Row %d is not a valid contrast. The coefficients do no sum to zero. Abort!\n",k+1);
        exit(-1);
        }
    }
for(k=0;k<nrow-1;k++) {
    for(i=k+1;i<nrow;i++) {
        for(magnitude=j=0;j<ncol;j++) magnitude += moc->matrix_of_coeff[k][j]*moc->matrix_of_coeff[i][j];
        if(fabs(magnitude) > 0.001) {
            printf("Row %d and %d are not orthogonal. Abort!\n",k+1,i+1);
            exit(-1);
            }
        }
    }

if(lc_unnormalized == (int)TRUE) {
    for(k=0;k<nrow;k++) {
        for(magnitude=j=0;j<ncol;j++) magnitude += moc->matrix_of_coeff[k][j]*moc->matrix_of_coeff[k][j];
        magnitude = sqrt(magnitude);
        for(j=0;j<ncol;j++) moc->matrix_of_coeff[k][j] /= magnitude;
        }
    }
/*fclose(fp);*/
/*for(k=0;k<nrow;k++) {
    for(j=0;j<ncol;j++) printf("%.0f ",moc->matrix_of_coeff[k][j]);
    printf("\n");
    }*/
return (Matrix_of_coeff *)moc;
}
