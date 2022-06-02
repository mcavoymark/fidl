/******************
scale_white_noise.c
******************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>

enum{Lenvol = 64*64*16,
     Bold_run = Lenvol*128,
     Skip_frames = 4};


main(int argc,char **argv)
{
void write_float(char *filename,float *x,int size);
void read_float(char *filename,float *x,int size);
void rescale_white_noise_to_fran_null(int size,float *white_noise_image,float white_noise_max,float white_noise_min,
                                      float fran_null_max,float fran_null_min);


char *fran_null_file,*white_noise_file,outfile[MAXNAME];
int i,j;
float *image,max,min,fran_null_max,fran_null_min,white_noise_max,white_noise_min,mean;


for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-fran_null") && argc > i+1) {
        GETMEM(fran_null_file,strlen(argv[i+j+1])+1,char)
        strcpy(fran_null_file,argv[i+j+1]);
        }
    if(!strcmp(argv[i],"-white_noise") && argc > i+1) {
        GETMEM(white_noise_file,strlen(argv[i+j+1])+1,char)
        strcpy(white_noise_file,argv[i+j+1]);
        }
    }

GETMEM(image,Bold_run,float)

#if 0
read_float(fran_null_file,image,Bold_run);
for(mean=0,min=1.e20,max=-1.e20,j=Skip_frames*Lenvol;j<Bold_run;j++) {
    if(image[j] > max) 
        max = image[j];
    else if(image[j] < min)
	min = image[j];
    mean += image[j];
    }
fran_null_max = max;
fran_null_min = min;
mean /= Bold_run;
fprintf(stdout,"%s\n\tmax = %f\tmin = %f\tmean = %f\n",fran_null_file,fran_null_max,fran_null_min,mean);
#endif

read_float(white_noise_file,image,Bold_run);
#if 0
for(mean=0,min=1.e20,max=-1.e20,j=Skip_frames*Lenvol;j<Bold_run;j++) {
    if(image[j] > max)
        max = image[j];
    else if(image[j] < min)
        min = image[j];
    mean += image[j];
    }
white_noise_max = max;
white_noise_min = min;
mean /= Bold_run;
fprintf(stdout,"%s\n\tmax = %f\tmin = %f\tmean = %f\n",white_noise_file,white_noise_max,white_noise_min,mean);

rescale_white_noise_to_fran_null(Bold_run,image,white_noise_max,white_noise_min,fran_null_max,fran_null_min);
/* This doesn't work because the 1/f effect gets incorporated into the rescaling.*/
for(mean=0,min=1.e20,max=-1.e20,j=0;j<Bold_run;j++) {
    if(image[j] > max)
        max = image[j];
    else if(image[j] < min)
        min = image[j];
    mean += image[j];
    }
fprintf(stdout,"%s : max = %f   min = %f\n",white_noise_file,max,min);
#endif

/*for(j=0;j<Bold_run;j++) image[j] += 1000;
  This doesn't work because when the mean is subtracted off before computing the power spectrum anyway.*/


if(strchr(white_noise_file,'.')) *(strchr(white_noise_file,'.')) = 0;
sprintf(outfile,"%s_scaled.4dfp.img",white_noise_file);
write_float(outfile,image,Bold_run);
fprintf(stdout,"Output written to: %s\n",outfile);

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

void read_float(char *filename,float *x,int size)
{
    FILE *fp;

    if(!(fp = fopen(filename,"r"))) {
        fprintf(stderr,"Could not open %s in white_noise.\n",filename);
        exit(-1);
        }
    if((fread(x,sizeof(float),size,fp)) != size) {
        fprintf(stderr,"Could not read image in %s in white_noise.\n",filename);
        exit(-1);
        }
    fclose(fp);
}

void rescale_white_noise_to_fran_null(int size,float *white_noise_image,float white_noise_max,float white_noise_min,
				      float fran_null_max,float fran_null_min)
{
    int i;
    float m,b;

    m = (fran_null_max-fran_null_min)/(white_noise_max-white_noise_min);
    b = fran_null_min - m*white_noise_min;
    for(i=0;i<size;i++) white_noise_image[i] = white_noise_image[i]*m + b; /*This is the only way to do this calculation.*/
} 
