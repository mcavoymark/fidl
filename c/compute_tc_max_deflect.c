/* Copyright 6/28/02 Washington University.  All Rights Reserved.
   compute_tc_max_deflect.c  $Revision: 1.5 $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>


main(int argc,char **argv)
{
char	**files,filename[(int)MAXNAME],*str_ptr,string[(int)MAXNAME];

int	i,j,k,m,num_files=0,lenvol,vol,index,nframes=0,*frames;

float	*temp_float,*maxdef,*maxdeffr;

double  max,temp_double,sum;

Interfile_header *ifh;

if(argc < 3) {
    fprintf(stderr,"Usage: tc_max_deflect -file timecourse1.4dfp.img timecourse2.4dfp.img\n");
    fprintf(stderr,"        -file:      4dfp timecourse(s).\n");
    fprintf(stderr,"        -frames:    Restrict analysis to these frames. First frame is 1.\n");
    fprintf(stderr,"                    By default, all frames in the timecourse are analyzed.\n");
    exit(-1);
    }

for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-file") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_files;
        GETMEM(files,num_files,char *)
        for(j=0;j<num_files;j++) {
            GETMEM(files[j],strlen(argv[i+1+j])+1,char)
            strcpy(files[j],argv[i+1+j]);
            }
        i += num_files;
        }
    if(!strcmp(argv[i],"-frames") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nframes;
        GETMEM(frames,nframes,int);
        for(j=0;j<nframes;j++) frames[j] = atoi(argv[++i]) - 1;
        }
    }
    
for(i=0;i<num_files;i++) {
    if(!(ifh = read_ifh(files[i]))) exit(-1);
    lenvol = ifh->dim1*ifh->dim2*ifh->dim3*ifh->dim4;
    vol = ifh->dim1*ifh->dim2*ifh->dim3;
    GETMEM(temp_float,lenvol,float);
    GETMEM(maxdef,vol,float);
    GETMEM(maxdeffr,vol,float);
    if(!read_float(files[i],temp_float,lenvol)) exit(-1);

    if(!nframes) {
        nframes = ifh->dim4;
        GETMEM(frames,nframes,int);
        for(j=0;j<nframes;j++) frames[j] = j;
        }

    for(j=0;j<vol;j++) {


        /*for(max=-1,sum=k=0;k<ifh->dim4;k++) {
            temp_double = fabs((double)temp_float[j+vol*k]);
            sum += temp_double;
            if(temp_double > max) {
                max = temp_double; 
                index = k;
                }
            }*/

        for(max=-1,sum=m=k=0;k<ifh->dim4;k++) {
            if(k == frames[m]) {
                temp_double = fabs((double)temp_float[j+vol*k]);
                sum += temp_double;
                if(temp_double > max) {
                    max = temp_double;
                    index = k;
                    }
                m++;
                }
            }


        if(sum < .001) {
            maxdef[j] = (float)UNSAMPLED_VOXEL;
            maxdeffr[j] = (float)UNSAMPLED_VOXEL;
            }
        else {
            maxdef[j] = temp_float[j+vol*index];
            maxdeffr[j] = (float)(index + 1);
            }
        }

    ifh->dim4 = 1;

    strcpy(string,files[i]);
    *strstr(string,".4dfp.img") = 0;
    if(str_ptr=strrchr(string,'/')) {
        str_ptr++;
        }
    else {
        str_ptr = string;
        }

    sprintf(filename,"%s_maxdef.4dfp.img",str_ptr);
    if(!write_float(filename,maxdef,vol)) exit(-1);
    min_and_max(maxdef,vol,&ifh->global_min,&ifh->global_max);
    if(!write_ifh(filename,ifh,(int)FALSE)) exit(-1);
    printf("Maximum deflection written to %s\n",filename);

    sprintf(filename,"%s_maxdeffr.4dfp.img",str_ptr);
    if(!write_float(filename,maxdeffr,vol)) exit(-1);
    min_and_max(maxdeffr,vol,&ifh->global_min,&ifh->global_max);
    if(!write_ifh(filename,ifh,(int)FALSE)) exit(-1);
    printf("Frame of maximum deflection written to %s\n",filename);

    free_ifh(ifh,(int)FALSE);
    free(temp_float);
    free(maxdef);
    free(maxdeffr);
    }
}
