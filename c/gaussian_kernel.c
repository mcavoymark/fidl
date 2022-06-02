/****************
gaussian_kernel.c
****************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>

enum{Bold_run = 64*64*16*128,
     How_big = LEN_ATLAS};


main(int argc,char **argv)
{
void write_float(char *filename,float *x,int size);
void mean_and_var(double *image,int size,double *mean,double *var);
int mean_and_var_of_smoothed_images(double *image,int size,double *mean,double *var);

char *filename,outfile[MAXNAME],fwhmstr[10],appendstr[MAXNAME];
int i,j,nseed=0,nfile=0;
float *image,fwhm=0;
double *temp_double,*gauss,sumsq;

if (argc < 5) {
    fprintf(stderr,"Usage: gaussian_kernel -filename kernel -gauss_smoth 2\n\n");
    exit(-1);
    }

for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-filename") && argc > i+1) {
        GETMEM(filename,strlen(argv[i+1])+1,char)
        strcpy(filename,argv[++i]);
        }
    if(!strcmp(argv[i],"-gauss_smoth") && argc > i+1) {
        fwhm = atof(argv[++i]);
        sprintf(fwhmstr,"_fwhm%.1f",fwhm);
        }
    }

GETMEM(image,How_big,float)
GETMEM_0(temp_double,How_big,double)
/*temp_double[How_big/2] = 1;*/
temp_double[135199] = 1;

if(strchr(filename,'.')) *(strchr(filename,'.')) = 0;
sprintf(outfile,"%s.4dfp.img",filename);
write_float(outfile,image,How_big);

if(fwhm > 0.) {
    printf("fwhm = %g\n",fwhm);
    gauss = gauss_smoth(temp_double,128,128,75,fwhm,fwhm);

    for(j=0;j<How_big;j++) image[j] = gauss[j];
    if(strchr(filename,'.')) *(strchr(filename,'.')) = 0;
    sprintf(outfile,"%s%s.4dfp.img",filename,fwhmstr);
    write_float(outfile,image,How_big);

    for(sumsq=j=0;j<How_big;j++) sumsq += gauss[j]*gauss[j];
    printf("sumsq = %g\n",sumsq);
    free(gauss);
    }
}

void write_float(char *filename,float *x,int size)
{
    FILE *fp;

    if(!(fp = fopen(filename,"w"))) {
        fprintf(stderr,"Could not open %s in white_noise.\n",filename);
        exit(-1);
        }
    if((fwrite(x,sizeof(float),size,fp)) != size) {
        fprintf(stderr,"Could not write image in %s in white_noise.\n",filename);
        exit(-1);
        }
    fclose(fp);
}

void mean_and_var(double *image,int size,double *mean,double *var)
{ 
    int i;
    double avg=0,avg2=0;

    for(i=0;i<size;i++) {
        avg += image[i];
        avg2 += image[i]*image[i];
        }
    *mean = avg/size;
    *var = (avg2-avg*avg/size)/(size-1);
}

int mean_and_var_of_smoothed_images(double *image,int size,double *mean,double *var)
{ 
    int i,j,k,m,count=0;
    double avg=0,avg2=0;

    for(m=i=0;i<75;i++)
        for(j=0;j<128;j++)
            for(k=0;k<128;k++,m++)
                if(!(i%4) && !(j%4) && !(k%4)) {
                    ++count;
                    avg += image[m];
                    avg2 += image[m]*image[m];
                    }
    *mean = avg/count;
    *var = (avg2-avg*avg/count)/(count-1);
    return count; 
}
