/* Copyright 2/7/06 Washington University.  All Rights Reserved.
   fidl_histogram.c  $Revision: 1.9 $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>

static char rcsid[] = "$Header: /home/hannah/mcavoy/idl/clib/RCS/fidl_histogram.c,v 1.9 2010/05/05 16:58:43 mcavoy Exp $";

void makehistogram(int size,float *temp_float,float *range,int nbins,int *bin,int *inclusive);

main(int argc,char **argv)
{
char *mask_file=NULL,*out_file=NULL,write_back[MAXNAME],last_char,first_char;
int i,j,k,l,nfiles=0,nbins=0,*inclusive,SunOS_Linux,nstrings,vol,*bin,check[]={0,0,0,0},checkc=0,tbin,bootstrap=0,largestindex,
    binmax,binmaxi;
float *range,*temp_float,voxel,window,increment,start,end,tf,min,max,range1,*stack;
double *mode,*var,y1,y2,modedata,vardata,td,modelow,modeup,varlow,varup;
Files_Struct *files,*bins=NULL;
Interfile_header *ifh;
Mask_Struct *ms;
FILE *fp,*op;
unsigned short seed[3]={0,0,0};

if(argc < 5) {
    fprintf(stderr,"  -files:     4dfp files. Separate histogram for each file.\n");
    fprintf(stderr,"  Specify either -nbins (assumes uniform sampling) or -bins or -window, -increment, -start, -end.\n");
    fprintf(stderr,"  -nbins:     Assumes uniform sampling.\n");
    fprintf(stderr,"  -bins:      [or(lower bound,upper bound)or]\n");
    fprintf(stderr,"  -window:    Width of window.\n");
    fprintf(stderr,"  -increment: Increment window by this amount.\n");
    fprintf(stderr,"  -mask:      Histogram is taken over all voxels in the mask.\n");
    fprintf(stderr,"  -out:       Output file.\n");
    fprintf(stderr,"  -bootstrap: Number of bootstraps to compute.\n");
    exit(-1);
    }
for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-files") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nfiles;
        if(!(files=get_files(nfiles,&argv[i+1]))) exit(-1);
        i += nfiles;
        }
    if(!strcmp(argv[i],"-nbins") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        nbins = atoi(argv[++i]);
    if(!strcmp(argv[i],"-bins") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nbins;
        if(!(bins=get_files(nbins,&argv[i+1]))) exit(-1);
        i += nbins;
        }
    if(!strcmp(argv[i],"-window") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1]) {
        window = (float)atof(argv[++i]);
        check[0]=1;
        checkc++;
        }
    if(!strcmp(argv[i],"-increment") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1]) {
        increment = (float)atof(argv[++i]);
        check[1]=1;
        checkc++;
        }
    if(!strcmp(argv[i],"-start") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1]) {
        start = (float)atof(argv[++i]);
        check[2]=1;
        checkc++;
        }
    if(!strcmp(argv[i],"-end") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1]) {
        end = (float)atof(argv[++i]);
        check[3]=1;
        checkc++;
        }
    if(!strcmp(argv[i],"-mask") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        mask_file = argv[++i];
    if(!strcmp(argv[i],"-out") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        out_file = argv[++i];
    if(!strcmp(argv[i],"-bootstrap") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        bootstrap = atoi(argv[++i]);
    }
print_version_number(rcsid,stdout);
if((SunOS_Linux=checkOS())==-1) exit(-1);
if(!nfiles) {
    printf("Error: No -files specified\n");
    exit(-1);
    }
if(!nbins && !checkc) {
    printf("Error: Specify either -bins or -window, -increment, -start, -end.\n");
    exit(-1);
    }
if(!nbins && checkc<4) {
    if(!check[0]) {
        printf("Error: Need to specify -window.\n");
        exit(-1);
        }
    if(!check[1]) {
        printf("Error: Need to specify -increment.\n");
        exit(-1);
        }
    if(!check[2]) {
        printf("Error: Need to specify -start.\n");
        exit(-1);
        }
    if(!check[3]) {
        printf("Error: Need to specify -end.\n");
        exit(-1);
        }
    }
if(!out_file) {
    printf("Error: No -out file specified\n");
    exit(-1);
    }
if(!(ifh=read_ifh(files->files[0]))) return 0;
vol=ifh->dim1*ifh->dim2*ifh->dim3;
if(!check_dimensions(files->nfiles,files->files,vol)) exit(-1);
if(!(temp_float=malloc(sizeof*temp_float*vol))) {
    printf("Error: Unable to malloc temp_float in fidl_histogram\n");
    exit(-1);
    }
if(!(ms=get_mask_struct(mask_file,vol,(int*)NULL,SunOS_Linux,(LinearModel*)NULL))) exit(-1);

if(!nbins) {
    start = 0.;
    end = 10.;
    increment = 1.;
    window = 5.;
    for(tf=start+window;tf<=end;nbins++,tf+=increment); 
    printf("nbins=%d\n",nbins);
    exit(-1);
    }

if(!(range=malloc(sizeof*range*2*nbins))) {
    printf("Error: Unable to malloc range in fidl_histogram\n");
    exit(-1);
    }
if(!(inclusive=malloc(sizeof*inclusive*2*nbins))) {
    printf("Error: Unable to malloc inclusive in fidl_histogram\n");
    exit(-1);
    }
for(i=0;i<2*nbins;i++) inclusive[i]=0;
if(!(bin=malloc(sizeof*bin*nbins))) {
    printf("Error: Unable to malloc bin in fidl_histogram\n");
    exit(-1);
    }

if(bootstrap) {
    if(!(stack=malloc(sizeof*stack*ms->lenbrain))) {
        printf("Error: Unable to malloc stack in fidl_histogram\n");
        exit(-1);
        }
    if(!(mode=malloc(sizeof*mode*bootstrap))) {
        printf("Error: Unable to malloc mode in fidl_histogram\n");
        exit(-1);
        }
    if(!(var=malloc(sizeof*var*bootstrap))) {
        printf("Error: Unable to malloc var in fidl_histogram\n");
        exit(-1);
        }
    largestindex = (double)(ms->lenbrain-1);
    }

if(bins) {
    for(j=i=0;i<nbins;i++,j+=2) {
        if((nstrings=count_strings_new2(bins->files[i],write_back,')',']',&last_char,&first_char,1))!=2) {
            printf("Error: Bin %d must have 2 numbers\n");
            exit(-1);
            }
        if(first_char=='[') inclusive[j]=1;
        if(last_char==']') inclusive[j+1]=1;
        strings_to_float(write_back,&range[j],2);
        }
    printf("range="); for(i=0;i<2*nbins;i++) printf("%f ",range[i]); printf("\n");
    printf("inclusive="); for(i=0;i<2*nbins;i++) printf("%d ",inclusive[i]); printf("\n");
    }
else {
    for(j=i=0;i<nbins;i++,j+=2) inclusive[j]=1; 
    inclusive[2*nbins-1]=1;
    printf("inclusive="); for(i=0;i<2*nbins;i++) printf("%d ",inclusive[i]); printf("\n");
    }

if(!(fp=fopen_sub(out_file,"w"))) exit(-1); 
if(mask_file) fprintf(fp,"# MASK = %s\n",mask_file);
for(i=0;i<files->nfiles;i++) {
    if(!readstack(files->files[i],(float*)temp_float,sizeof(float),(size_t)vol,SunOS_Linux)) exit(-1);
    for(j=0;j<ms->lenbrain;j++) temp_float[j]=temp_float[ms->brnidx[j]]; 

    /*START31*/
    strcpy(write_back,files->files[i]); 
    if(!get_tail_sans_ext(write_back)) exit(-1);
    strcat(write_back,".dat");
    if(!(op=fopen_sub(write_back,"w"))) exit(-1); 
    for(j=0;j<ms->lenbrain;j++) fprintf(op,"%f\n",temp_float[j]);
    fclose(op);
    printf("All voxels within mask written to %s\n",write_back);

    if(!bins) {
        min_and_max_init(&min,&max);
        min_and_max_floatstack(temp_float,ms->lenbrain,&min,&max);
        printf("min=%f max=%f\n",min,max);
        range1=(max-min)/(float)nbins;
        for(j=k=0;k<nbins;k++,j+=2) {
            range[j]=min; 
            min+=range1;
            range[j+1]=min;
            }
        range[2*nbins-1]=max;
        }

    #if 0
    for(j=0;j<nbins;j++) bin[j]=0;
    for(j=0;j<ms->lenbrain;j++) {
        voxel = temp_float[j];
        for(l=k=0;k<nbins;k++,l+=2) {
            if(!inclusive[l] && !inclusive[l+1]) {
                if(voxel>range[l] && voxel<range[l+1]) bin[k]++;
                }
            else if(!inclusive[l] && inclusive[l+1]) {
                if(voxel>range[l] && voxel<=range[l+1]) bin[k]++;
                }
            else if(inclusive[l] && !inclusive[l+1]) {
                if(voxel>=range[l] && voxel<range[l+1]) bin[k]++;
                }
            else {
                if(voxel>=range[l] && voxel<=range[l+1]) bin[k]++;
                }
            }
        }
    #endif
    makehistogram(ms->lenbrain,temp_float,range,nbins,bin,inclusive);


    for(tbin=j=0;j<nbins;j++) tbin += bin[j];
    fprintf(fp,"# %s\n",files->files[i]);
    if(bins) {   
        for(j=0;j<nbins;j++) {
            printf("%s\t%d\t%f\n",bins->files[j],bin[j],(double)bin[j]/(double)tbin*100.);
            fprintf(fp,"%s\t%d\t%f\n",bins->files[j],bin[j],(double)bin[j]/(double)tbin*100.);
            }
        printf("\n");
        fprintf(fp,"\n");
        }
    else {
        range1/=2.;
        for(j=k=0;k<nbins;k++,j+=2) {
            printf("[%f,%f)\t%d\t%f\n",range[j],range[j+1],bin[k],(double)bin[k]/(double)tbin*100.);
            fprintf(fp,"%f\t%d\t%f\n",range[j]+range1,bin[k],(double)bin[k]/(double)tbin*100.);
            }
        printf("\n");
        fprintf(fp,"\n");
        }
    printf("%s total=%d\n",files->files[i],tbin);

    if(bootstrap) {
        for(binmax=j=0;j<nbins;j++) if(bin[j]>binmax) {binmax=bin[j];binmaxi=j;}
        modedata = bins ? atof(bins->files[binmaxi]) : range[binmaxi*2]+range1;
        for(y1=y2=0.,j=0;j<ms->lenbrain;j++) {td = (double)temp_float[j]; y2+=td*td; y1+=td;}
        vardata = (y2-y1*y1/(double)ms->lenbrain)/(double)(ms->lenbrain-1);
        for(k=0;k<bootstrap;k++) {
            for(j=0;j<ms->lenbrain;j++) stack[j] = temp_float[(int)rint(erand48(seed)*largestindex)];
            makehistogram(ms->lenbrain,stack,range,nbins,bin,inclusive);
            for(binmax=j=0;j<nbins;j++) if(bin[j]>binmax) {binmax=bin[j];binmaxi=j;}
            mode[k] = bins ? atof(bins->files[binmaxi]) : range[binmaxi*2]+range1;
            for(y1=y2=0.,j=0;j<ms->lenbrain;j++) {td = (double)stack[j]; y2+=td*td; y1+=td;}
            var[k] = (y2-y1*y1/(double)ms->lenbrain)/(double)(ms->lenbrain-1);
            }
        gsl_sort(mode,1,bootstrap); 
        /*printf("mode\n"); for(k=0;k<bootstrap;k++) printf("%d:%f ",k,mode[k]); printf("\n");*/
        modelow=mode[(int)rint(.025*bootstrap)];
        modeup=mode[(int)rint(.975*bootstrap)];
        gsl_sort(var,1,bootstrap); 
        varlow=var[(int)rint(.025*bootstrap)];
        varup=var[(int)rint(.975*bootstrap)];
        fprintf(fp,"bootstrap statistics  mode = %f  95%% confidence limits lower=%f upper=%f\n",modedata,modelow,modeup);
        fprintf(fp,"                      var  = %f  95%% confidence limits lower=%f upper=%f\n",vardata,varlow,varup);
        fprintf(fp,"                      sd   = %f  95%% confidence limits lower=%f upper=%f\n",sqrt(vardata),sqrt(varlow),
            sqrt(varup));
        printf("bootstrap statistics  mode = %f  95%% confidence limits lower=%f upper=%f\n",modedata,modelow,modeup);
        printf("                      var  = %f  95%% confidence limits lower=%f upper=%f\n",vardata,varlow,varup);
        printf("                      sd   = %f  95%% confidence limits lower=%f upper=%f\n",sqrt(vardata),sqrt(varlow),sqrt(varup));
        }


    }
fclose(fp);
printf("Output written to %s\n",out_file);
exit(0);
}
void makehistogram(int size,float *temp_float,float *range,int nbins,int *bin,int *inclusive)
{
    int j,k,l;
    float voxel;
    for(k=0;k<nbins;k++) bin[k]=0; 
    for(j=0;j<size;j++) {
        voxel = temp_float[j];
        for(l=k=0;k<nbins;k++,l+=2) {
            if(!inclusive[l] && !inclusive[l+1]) {
                if(voxel>range[l] && voxel<range[l+1]) bin[k]++;
                }
            else if(!inclusive[l] && inclusive[l+1]) {
                if(voxel>range[l] && voxel<=range[l+1]) bin[k]++;
                }
            else if(inclusive[l] && !inclusive[l+1]) {
                if(voxel>=range[l] && voxel<range[l+1]) bin[k]++;
                }
            else {
                if(voxel>=range[l] && voxel<=range[l+1]) bin[k]++;
                }
            }
        }
}
