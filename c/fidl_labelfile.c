/* Copyright 9/27/10 Washington University.  All Rights Reserved.
   fidl_labelfile.c  $Revision: 1.4 $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>

static char rcsid[] = "$Header: /home/hannah/mcavoy/idl/clib/RCS/fidl_labelfile.c,v 1.4 2010/10/28 00:24:19 mcavoy Exp $";

main(int argc,char **argv)
{
char *file=NULL,*ifhfile=NULL,*out=NULL,*log=NULL,string[MAXNAME],*strptr,regname[MAXNAME];
int i,j,SunOS_Linux,atlas=222,*xi,*yi,*zi,*index,len,nvox,lregname,bigendian;
float *temp_float;
double *xd,*yd,*zd;
FILE *fp;
Data *data;
Interfile_header *ifh=NULL;
Atlas_Param *ap;

print_version_number(rcsid,stderr);
if(argc < 7) {
    fprintf(stderr,"    -file:  *.label file from freesurfer.\n");
    fprintf(stderr,"    -atlas: Either 111, 222 or 333. Default is 222. If you use -ifh, this field is ignored.\n");
    fprintf(stderr,"    -ifh:   Desired space, center, and mmppix are pulled from this file in lieu of -atlas.\n");
    fprintf(stderr,"    -out:   Name of output file.\n");
    fprintf(stderr,"    -log:   Name of log file.\n");
    exit(-1);
    }
for(i=1;i<argc;i++) { 
    if(!strcmp(argv[i],"-file") && argc > i+1)
        file = argv[++i];
    if(!strcmp(argv[i],"-atlas") && argc > i+1)
        atlas = atoi(argv[++i]);
    if(!strcmp(argv[i],"-ifh") && argc > i+1)
        ifhfile = argv[++i];
    if(!strcmp(argv[i],"-out") && argc > i+1)
        out = argv[++i];
    if(!strcmp(argv[i],"-log") && argc > i+1)
        log = argv[++i];
    }
if(!file) {
    printf("Error: No text file specified with -file option. Abort!\n");
    exit(-1);
    }
if((SunOS_Linux=checkOS())==-1) exit(-1);
if(!(data=read_data(file,1,2,4,0))) exit(-1); 
if(ifhfile) if(!(ifh=read_ifh(ifhfile,(Interfile_header*)NULL))) exit(-1);
if(!(ap=get_atlas_param(atlas,ifh))) exit(-1);
if(!(temp_float=malloc(sizeof*temp_float*ap->vol))) {
    printf("Error: Unable to malloc temp_float\n");
    exit(-1);
    }
for(i=0;i<ap->vol;i++) temp_float[i]=0.;
if(!(xi=malloc(sizeof*xi*data->nsubjects))) {
    printf("Error: Unable to malloc xi\n");
    exit(-1);
    }
if(!(yi=malloc(sizeof*yi*data->nsubjects))) {
    printf("Error: Unable to malloc yi\n");
    exit(-1);
    }
if(!(zi=malloc(sizeof*zi*data->nsubjects))) {
    printf("Error: Unable to malloc zi\n");
    exit(-1);
    }
if(!(xd=malloc(sizeof*xd*data->nsubjects))) {
    printf("Error: Unable to malloc xd\n");
    exit(-1);
    }
if(!(yd=malloc(sizeof*yd*data->nsubjects))) {
    printf("Error: Unable to malloc yd\n");
    exit(-1);
    }
if(!(zd=malloc(sizeof*zd*data->nsubjects))) {
    printf("Error: Unable to malloc zd\n");
    exit(-1);
    }
if(!(index=malloc(sizeof*index*data->nsubjects))) {
    printf("Error: Unable to malloc index\n");
    exit(-1);
    }
atlas_to_index(data->nsubjects,data->x,ap,xi,yi,zi,xd,yd,zd,index);

strcpy(string,file);
if(!(strptr=get_tail_sans_ext(string))) exit(-1);
len = strlen(strptr)+1;
if(!out) {
    if(!(out=malloc(sizeof*out*(len+10)))) {
        printf("Error: Unable to malloc out\n");
        exit(-1);
        }
    sprintf(out,"%s.4dfp.img",strptr);
    }

if(!log) {
    if(!(log=malloc(sizeof*log*(len+5)))) {
        printf("Error: Unable to malloc log\n");
        exit(-1);
        }
    sprintf(log,"%s.log",strptr);
    }

if(!(fp=fopen_sub(log,"w"))) exit(-1);
print_version_number(rcsid,fp);
fprintf(fp,"%s\n",file);
for(nvox=i=0;i<data->nsubjects;i++) {
    if(temp_float[index[i]]>0.) {
        for(j=0;j<i-1;j++) {
            if(index[i]==index[j]) {
                fprintf(fp,"%f %f %f has been mapped to the same index as %f %f %f\n",data->x[i][0],data->x[i][1],data->x[i][2],
                    data->x[j][0],data->x[j][1],data->x[j][2]);
                break;
                }
            }
        }
    else {
        temp_float[index[i]]=2.;
        nvox++;
        }
    }

bigendian = !SunOS_Linux?1:0;
if(ifh) {
    ifh->bigendian = bigendian;
    if(ifh->region_names) free_ifhregnames(ifh);
    }
else {
    if(!(ifh=init_ifh(4,ap->xdim,ap->ydim,ap->zdim,1,ap->voxel_size[0],ap->voxel_size[1],ap->voxel_size[2],bigendian))) 
        exit(-1);
    }
ifh->global_max = 2.; ifh->global_min = 0.;
ifh->nregions = 1;
sprintf(regname,"0 %s %d",strptr,nvox);
lregname = strlen(regname)+1;
if(!(ifh->region_names=d2charvar(ifh->nregions,&lregname))) exit(-1);
strcpy(ifh->region_names[0],regname);
if(!writestack(out,temp_float,sizeof(float),(size_t)ap->vol,0)) exit(-1);
if(!write_ifh(out,ifh,0)) exit(-1);
printf("Output written to %s\n",out);
fprintf(fp,"Output written to %s\n",out);
fprintf(stderr,"Log file written to %s\n",log);
fclose(fp);
exit(0);
}
