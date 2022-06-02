/* Copyright 12/15/05 Washington University.  All Rights Reserved.
   fidl_lminusrdivlplusr.c  $Revision: 1.3 $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>

static char rcsid[] = "$Header: /home/hannah/mcavoy/idl/clib/RCS/fidl_lminusrdivlplusr.c,v 1.3 2007/04/10 21:54:45 mcavoy Exp $";

main(int argc,char **argv)
{
char *out,filename[MAXNAME],string[MAXNAME];
int i,j,nl=0,nr=0,nindroots=0,SunOS_Linux,vol,*nimage,*index,nindex,swapbytes;
float *tfl,*tfr,*temp_float;
double *ratio,*avgratio,den;
Files_Struct *lfiles,*rfiles,*indroots;
Interfile_header *ifh;

print_version_number(rcsid,stderr);

if(argc < 7) {
    fprintf(stderr,"    -l:        Lefties.\n");
    fprintf(stderr,"    -r:        Righties.\n");
    fprintf(stderr,"    -out:      Average of ratios.\n");
    fprintf(stderr,"    -indroots: Individual ratios.\n");
    exit(-1);
    }
for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-l") && argc > i+1) {
        for(j=1;i+j < argc && strchr(argv[i+j],'-') != argv[i+j];j++) ++nl;
        if(!(lfiles=get_files(nl,&argv[i+1]))) exit(-1);
        i += nl;
        }
    if(!strcmp(argv[i],"-r") && argc > i+1) {
        for(j=1;i+j < argc && strchr(argv[i+j],'-') != argv[i+j];j++) ++nr;
        if(!(rfiles=get_files(nr,&argv[i+1]))) exit(-1);
        i += nr;
        }
    if(!strcmp(argv[i],"-out") && argc > i+1)
        out = argv[++i];
    if(!strcmp(argv[i],"-indroots") && argc > i+1) {
        for(j=1;i+j < argc && strchr(argv[i+j],'-') != argv[i+j];j++) ++nindroots;
        if(!(indroots=get_files(nindroots,&argv[i+1]))) exit(-1);
        i += nindroots;
        }
    }
if((SunOS_Linux=checkOS())==-1) exit(-1);
if(nl!=nr) {
    printf("nl=%d nr=%d They must be equal. Abort!\n",nl,nr);
    exit(-1);
    }
if(nindroots) {
    if(nindroots != nl) {
        printf("nl=%d nindroots=%d They must be equal. Abort!\n",nl,nindroots);
        exit(-1);
        }
    }

if(!(ifh=read_ifh(lfiles->files[0]))) exit(-1);
vol = ifh->dim1*ifh->dim2*ifh->dim3;
if(!check_dimensions(nl,lfiles->files,vol)) exit(-1);
if(!check_dimensions(nl,rfiles->files,vol)) exit(-1);
swapbytes=shouldiswap(SunOS_Linux,ifh->bigendian);

if(!(tfl=malloc(sizeof*tfl*vol))) {
    printf("Error: Unable to malloc tfl\n");
    exit(-1);
    }
if(!(tfr=malloc(sizeof*tfr*vol))) {
    printf("Error: Unable to malloc tfr\n");
    exit(-1);
    }
if(!(temp_float=malloc(sizeof*temp_float*vol))) {
    printf("Error: Unable to malloc temp_float\n");
    exit(-1);
    }
if(!(ratio=malloc(sizeof*ratio*vol))) {
    printf("Error: Unable to malloc ratio\n");
    exit(-1);
    }
if(!(avgratio=malloc(sizeof*avgratio*vol))) {
    printf("Error: Unable to malloc avgratio\n");
    exit(-1);
    }
for(i=0;i<vol;i++) avgratio[i] = 0.;
if(!(nimage=malloc(sizeof*nimage*vol))) {
    printf("Error: Unable to malloc nimage\n");
    exit(-1);
    }
for(i=0;i<vol;i++) nimage[i] = 0;
if(!(index=malloc(sizeof*index*vol))) {
    printf("Error: Unable to malloc index\n");
    exit(-1);
    }
for(i=0;i<nl;i++) {
    if(!readstack(lfiles->files[i],(float*)tfl,sizeof(float),(size_t)vol,SunOS_Linux)) exit(-1);
    if(!readstack(rfiles->files[i],(float*)tfr,sizeof(float),(size_t)vol,SunOS_Linux)) exit(-1);
    for(nindex=j=0;j<vol;j++) {
        if(tfl[j]!=(float)UNSAMPLED_VOXEL && tfr[j]!=(float)UNSAMPLED_VOXEL) { 
            if(fabs(den=((double)tfl[j]+(double)tfr[j]))>.00000001) {
                avgratio[j] += ratio[nindex] = ((double)tfl[j]-(double)tfr[j])/den;
                nimage[j]++;
                index[nindex++] = j;
                }
            }
        }
    if(nindroots) {
        for(j=0;j<vol;j++) temp_float[j] = 0.;
        for(j=0;j<nindex;j++) temp_float[index[j]] = (float)ratio[j];
        sprintf(filename,indroots->files[i]);
        if(!strstr(filename,".4dfp.img")) strcat(filename,".4dfp.img");
        if(!writestack(filename,temp_float,sizeof(float),(size_t)vol,swapbytes)) exit(-1); 
        if(ifh->file_name) free(ifh->file_name);
        j = strlen(lfiles->files[i]) + strlen(rfiles->files[i]) + 2;
        if(!(ifh->file_name=malloc(sizeof*ifh->file_name*j))) {
            printf("Error: Unable to malloc ifh->file_name\n");
            exit(-1);
            }    
        sprintf(ifh->file_name,"%s %s\n",lfiles->files[i],rfiles->files[i]);
        min_and_max_init(&ifh->global_min,&ifh->global_max);
        min_and_max_doublestack(ratio,nindex,&ifh->global_min,&ifh->global_max);
        if(!write_ifh(filename,ifh,(int)FALSE)) exit(-1);
        printf("Output written to %s\n",filename);
        } 
    }
for(nindex=i=0;i<vol;i++) {
    temp_float[i] = 0.;
    if(nimage[i]) {
        ratio[nindex] = avgratio[i]/(double)nimage[i];
        temp_float[i] = (float)ratio[nindex];
        index[nindex++] = i;
        }
    }
sprintf(filename,out);
if(!strstr(filename,".4dfp.img")) strcat(filename,".4dfp.img");
if(!writestack(filename,temp_float,sizeof(float),(size_t)vol,swapbytes)) exit(-1);
if(ifh->file_name) free(ifh->file_name);
sprintf(string,"average of %d ratios",nl);
if(!(ifh->file_name=malloc(sizeof*ifh->file_name*(strlen(string)+1)))) {
    printf("Error: Unable to malloc ifh->file_name\n");
    exit(-1);
    }
min_and_max_init(&ifh->global_min,&ifh->global_max);
min_and_max_doublestack(ratio,nindex,&ifh->global_min,&ifh->global_max);
if(!write_ifh(filename,ifh,(int)FALSE)) exit(-1);
printf("Output written to %s\n",filename);
}
