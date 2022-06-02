/*********************************************************************
c
c subroutine: mat_read
c
c Purpose: Read matlab matrix.
c
c************************************************************************/

/*$Revision: 12.80 $*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mat.h>

void main(argc,argv)
 
int     argc;
char    *argv[];
 
{

double	*real,*imag,*mat;

int	i,j,k,nrow,ncol,status,type;

char *filnam,*matnam,*typestr,*string,*bytes;

MATFile *fp;
Matrix *mt;

if(argc < 2) {
    fprintf(stderr,"Usage: mat_read filnam");
    printf("Write matrix to stdout.\n");
    exit(-1);
    }

filnam = argv[1];

if((fp = matOpen(filnam,"r")) == NULL)
    exit(-1);

while((mt = (Matrix *)matGetNextMatrix(fp)) != NULL) {
    nrow = mxGetM(mt); 
    ncol = mxGetN(mt);
    matnam = mxGetName(mt);
    mat = mxGetPr(mt);
    type = mxIsNumeric(mt);
    if(type == 1) 
        typestr = "numeric";
    else
        typestr = "string";
    if((status = fprintf(stdout,"%s\n%s\n%d\n%d\n",matnam,typestr,nrow,ncol)) < 0) {
        fprintf(stderr,"Could not write header in mat_read.\n");
        exit(-1);
        }
    if(nrow*ncol > 0) {
        if(type == 1) {
            if((status=fwrite(mat,sizeof(double),nrow*ncol,stdout)) != nrow*ncol) {
                fprintf(stderr,"Could not write matrix in mat_read.\n");
                exit(-1);
                }
	    }
        else {
	    string = (char *)malloc(ncol+1);
	    bytes = (char *)malloc(nrow*ncol+1);
	    mxGetString(mt,bytes,nrow*ncol+1);
	    for(i=0;i<nrow;i++) {
	        for(j=0,k=0;j<ncol;j++,k+=nrow)
		    if(bytes[k] == ' ') 
		        string[j] = '_';
		    else
		        string[j] = bytes[k];
		string[ncol] = NULL;
		fprintf(stdout,"%s\n",string);
		}
	    free(string);
	    }
        }
    mxFreeMatrix(mt);
    }
if((status = fprintf(stdout,"null\nstring\n%d\n%d\n",nrow,ncol)) < 0) {
    fprintf(stderr,"Could not write header in mat_read.\n");
    exit(-1);
    }

status = matClose(fp);
mxFree(real);
mxFree(imag);

}
