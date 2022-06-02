/* Copyright 5/22/03 Washington University.  All Rights Reserved.
   fidl_fcm.c  $Revision: 1.5 $ */

/* Pattern recognition with fuzzy objective function algorithms. James Bezdek 1981.*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>

static char rcsid[] = "$Header: /home/hannah/mcavoy/idl/clib/RCS/fidl_fcm.c,v 1.5 2006/01/09 18:27:47 mcavoy Exp $";

/*#define TOL 1e-20*/

#if 0
typedef struct {
    int nsubjects,npoints;
    char **subjects;
    double **x;
    } Data;
Data *read_data(char *datafile);
#endif

main(int argc,char **argv)
{
char *datafile=NULL,*outfile=NULL;
unsigned short seed[3];
int nclusters=0,i,j,k,**I,*Iflag;
double we=0,**d,**u,**uold,**v,*num_v,den_v,temp,tol=1.e-20;
Data *data;
FILE *fp;

print_version_number(rcsid,stdout);
if(argc < 9) {
    fprintf(stderr,"Fuzzy c-means fidl_fcm\n");        
    fprintf(stderr,"        -data: Each subjects' data is a row.\n");
    fprintf(stderr,"        -nclusters: Number of clusters.\n");
    fprintf(stderr,"        -weighting_exponent: [1,Inf). Typically 2. As the weighting exponent -> 1+, fuzzy c-means in\n");
    fprintf(stderr,"            theory to a 'generalized' hard c-means solution. The larger the weighting exponent, the\n");
    fprintf(stderr,"            'fuzzier' the membership assignments. Bezdek p.70\n");
    fprintf(stderr,"        -output: Output filename.\n");
    fprintf(stderr,"        -tol: Tolerance. Default 1e-20\n");
    exit(-1);
    }

for(i=1;i<argc;i++) { 
    if(!strcmp(argv[i],"-data") && argc > i+1)
        datafile = argv[++i];
    if(!strcmp(argv[i],"-nclusters") && argc > i+1)
        nclusters = atoi(argv[++i]);
    if(!strcmp(argv[i],"-weighting_exponent") && argc > i+1)
        we = atof(argv[++i]);
    if(!strcmp(argv[i],"-output") && argc > i+1)
        outfile = argv[++i];
    if(!strcmp(argv[i],"-tol") && argc > i+1)
        tol = atof(argv[++i]);
    }
if(!datafile) {
    printf("Error: You must specify a data file with -data. Abort!\n");
    exit(-1);
    }
if(!nclusters) {
    printf("Error: You must specify the number of clusters with -nclusters. Abort!\n");
    exit(-1);
    }
if(!we) {
    printf("Error: You must specify the weighting exponent with -weighting_exponent. Abort!\n");
    exit(-1);
    }
if(!outfile) {
    printf("Error: You must specify an ouput file with -output. Abort!\n");
    exit(-1);
    }


if(!(data = (Data *)read_data(datafile))) exit(-1);
#if 0
printf("data->nsubjects=%d data->npoints=%d\n");
for(i=0;i<data->nsubjects;i++) {
    printf("%s ",data->subjects[i]); for(j=0;j<data->npoints;j++) printf("%.2f ",data->x[i][j]); printf("\n"); 
    } 
#endif


if(nclusters >= data->nsubjects) {
    printf("nclusters=%d >= data->nsubjects=%d. nclusters must be < data->nsubejcts.\n",nclusters,data->nsubjects);
    exit(-1);
    }
if(!(d = d2double(nclusters,data->nsubjects))) exit(-1);
if(!(u = d2double(nclusters,data->nsubjects))) exit(-1);
if(!(uold = d2double(nclusters,data->nsubjects))) exit(-1);
if(!(v = d2double(nclusters,data->npoints))) exit(-1);
if(!(num_v = d1double(data->npoints))) exit(-1);
if(!(I = d2int(data->nsubjects,nclusters))) exit(-1);
if(!(Iflag = d1int(data->nsubjects))) exit(-1);

/*Very robust to initialization, but all equal does not work.*/
#if 0
seed[0] = 0; seed[1] = 0; seed[2] = 0;
/*seed[0] = 1; seed[1] = 0; seed[2] = 0;
seed[0] = 2; seed[1] = 1; seed[2] = 0;*/
/*seed[0] = 655333; seed[1] = 4; seed[2] = 1001;*/
for(i=0;i<nclusters;i++) {
    for(k=0;k<data->nsubjects;k++) {
        /*u[i][k] = (double)1./nclusters;*/
        /*u[i][k] = rand();*/
        u[i][k] = erand48(seed);
        }
    }
#endif

seed[0] = 0; seed[1] = 0; seed[2] = 0;
for(k=0;k<data->nsubjects;k++) {
    for(temp=i=0;i<nclusters-1;i++) temp += u[i][k] = erand48(seed);
    u[nclusters-1][k] = 1. - temp;
    }




#if 1
printf("u\n");
for(i=0;i<nclusters;i++) {
    for(k=0;k<data->nsubjects;k++) printf("%f ",u[i][k]); printf("\n");
    }
#endif

do {
    for(i=0;i<nclusters;i++) {
        for(k=0;k<data->nsubjects;k++) {
            uold[i][k] = u[i][k];
            }
        }
    #if 0
    printf("uold\n");
    for(i=0;i<nclusters;i++) {
        for(k=0;k<data->nsubjects;k++) printf("%f ",uold[i][k]); printf("\n");
        }
    #endif

    for(i=0;i<nclusters;i++) {
        for(j=0;j<data->npoints;j++) num_v[j] = 0; 
        for(den_v=k=0;k<data->nsubjects;k++) {
            temp = pow(u[i][k],we);
            for(j=0;j<data->npoints;j++) num_v[j] += temp*data->x[k][j];
            den_v += temp;
            }
        for(j=0;j<data->npoints;j++) v[i][j] = num_v[j] / den_v;
        }
    #if 0
    printf("v\n");
    for(i=0;i<nclusters;i++) {
        for(j=0;j<data->npoints;j++) printf("%f ",v[i][j]); printf("\n");
        }
    #endif

    for(i=0;i<nclusters;i++) {
        for(k=0;k<data->nsubjects;k++) {
            d[i][k] = 0;
            for(j=0;j<data->npoints;j++) {
                temp = data->x[k][j] - v[i][j]; 
                d[i][k] += temp * temp; 
                }
            }
        }
    #if 0
    printf("d\n");
    for(i=0;i<nclusters;i++) {
        for(k=0;k<data->nsubjects;k++) printf("%f ",d[i][k]); printf("\n");
        } 
    #endif

    for(k=0;k<data->nsubjects;k++) {
        Iflag[k] = 0; 
        for(i=0;i<nclusters;i++) {
            I[k][i] = 0;
            /*if(d[i][k] < (double)TOL) {*/
            if(d[i][k] < tol) {
                I[k][i] = 1;
                Iflag[k]++;
                }
            }
        }

    for(k=0;k<data->nsubjects;k++) {
        if(Iflag[k]) {
            for(i=0;i<nclusters;i++) {
                if(!I[k][i]) {
                    u[i][k] = 0;
                    }
                else {
                    u[i][k] = 1/Iflag[k];
                    } 
                } 
            }
        else {
            /*printf("here0\n");*/
            for(i=0;i<nclusters;i++) {
                for(temp=j=0;j<nclusters;j++) temp += pow(d[j][k],(double)(1./(1.-we)));
                u[i][k] = pow(d[i][k],(double)(1./(1.-we))) / temp;
                }   
            }
        }
    #if 0
    printf("u\n");
    for(i=0;i<nclusters;i++) {
        for(k=0;k<data->nsubjects;k++) printf("%f ",u[i][k]); printf("\n");
        }
    #endif

    for(i=0;i<nclusters;i++) {
        for(k=0;k<data->nsubjects;k++) {
            uold[i][k] -= u[i][k];
            }
        }
    for(temp=i=0;i<data->nsubjects;i++) {
        for(k=0;k<nclusters;k++) temp += uold[k][i]*uold[k][i];
        }
    printf("sqrt(temp)=%f\n",sqrt(temp));
    /*} while(sqrt(temp) > (double)TOL);*/
    } while(sqrt(temp) > tol);

if(!(fp = fopen_sub(outfile,"w"))) exit(-1); 
for(i=0;i<data->nsubjects;i++) {
    fprintf(fp,"%s ",data->subjects[i]); for(j=0;j<nclusters;j++) fprintf(fp,"%f ",u[j][i]); fprintf(fp,"\n");
    }
fclose(fp);

}

#if 0
Data *read_data(char *datafile)
{
    char line[MAXNAME],write_back[MAXNAME],*str_ptr;
    int i,nstrings,*lengths;
    FILE *fp;
    Data *data;

    GETMEM(data,1,Data);

    if(!(fp = fopen_sub(datafile,"r"))) return (Data *)NULL; 
    for(data->nsubjects=data->npoints=0,i=1;fgets(line,sizeof(line),fp);i++) {
        if((nstrings=count_strings(line,write_back,' '))) {
            if(!data->nsubjects) {
                data->npoints = nstrings - 1;
                }
            else if(--nstrings != data->npoints) {
                printf("Each line must have the same number of data points. Line %d has %d points. Should have %d. Abort!\n",
                    i,nstrings,data->npoints);
                return (Data *)NULL;
                }
            data->nsubjects++;
            }
        }
    if(!data->nsubjects) {
        printf("Error: %s appears to be empty. Abort!\n",datafile);
        return (Data *)NULL;
        }

    if(!(data->x = d2double(data->nsubjects,data->npoints))) exit(-1);
    if(!(lengths = d1int(data->nsubjects))) exit(-1);
    for(i=0,rewind(fp);fgets(line,sizeof(line),fp);) {
        if((nstrings=count_strings(line,write_back,' '))) {
            str_ptr = grab_string(write_back,line);
            lengths[i] = strlen(line) + 1;
            strings_to_double(str_ptr,data->x[i++],data->npoints);
            }
        }
    if(!(data->subjects = d2charvar(data->nsubjects,lengths))) exit(-1);
    free(lengths);
    for(i=0,rewind(fp);fgets(line,sizeof(line),fp);) {
        if((nstrings=count_strings(line,write_back,' '))) {
            grab_string(write_back,line);
            strcpy(data->subjects[i++],line);
            }
        }
    return (Data *)data;
}
#endif
