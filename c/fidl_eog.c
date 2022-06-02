/* Copyright 3/02/12 Washington University.  All Rights Reserved.
   fidl_eog.c  $Revision: 1.2 $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>
static char rcsid[] = "$Header: /home/hannah/mcavoy/idl/clib/RCS/fidl_eog.c,v 1.2 2012/03/27 20:30:49 mcavoy Exp $";
main(int argc,char **argv)
{
char *strptr,*convolvef=NULL,*concf,root[MAXNAME],filename[MAXNAME],*rootptr,*timesoutf=NULL,**identifystrptr;
int i,j,k,kk,j1,j2,k1,nfiles=0,skiplines,posmaxi,negmaxi,TRlines,*lines,skipframes,*frames,linesmax,framessum,framesmax,blinklines,
    idx,nidentify,*identifyn,*number;
double skipsec=0.,samplingrateHz=0.,samplingratesec,threshuV=0.,posmax,negmax,boldTR=0.,**y,**z,w,**x,td,td1,tol=25.,**large,**small,
    **largec,**smallc,**largecd,**smallcd;
Files_Struct *files;
Data *data,*convolved;
FILE *fp;
print_version_number(rcsid,stdout);
if(argc<2) {
    fprintf(stderr,"  -files:          dats or a conc.\n");
    fprintf(stderr,"  -skipsec:        Ignore this many seconds of data at the beginning of every run. Default is zero.\n");
    fprintf(stderr,"  -samplingrateHz: Sampling rate in Hz.\n");
    fprintf(stderr,"  -threshuV:       Threshold in uV. Default is zero.\n");

    /*START120308*/
    fprintf(stderr,"  -convolve:       Convolve EOG with this function. Text file with a single column of numbers.\n");
    fprintf(stderr,"  -boldTR:         BOLD TR in sec. Convolved data is downsampled to this TR by averaging over the epoch.\n");

    /*START120320*/
    fprintf(stderr,"  -times_out:      Name of output file containing start and end times (s) of blinks.\n");
    fprintf(stderr,"  -tol:            Tolerance. Default is 25 uV.\n");
    fprintf(stderr,"                   Blink start and end points are both within the tolerance.\n");

    exit(-1);
    }
for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-files") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nfiles;
        strptr = strrchr(argv[i+1],'.');
        if(!strcmp(strptr,".conc")) {
            if(!(files=read_conc(argv[i+1]))) exit(-1);
            concf = argv[i+1];
            }
        else if(!strcmp(strptr,".dat")){
            if(!(files=get_files(nfiles,&argv[i+1]))) exit(-1);
            concf = files->files[0];
            }
        else {
            printf("Error: -files not conc or dat. Abort!\n");fflush(stdout);
            exit(-1);
            }
        }
    if(!strcmp(argv[i],"-skipsec") && argc > i+1)
        skipsec = atof(argv[++i]);
    if(!strcmp(argv[i],"-samplingrateHz") && argc > i+1)
        samplingrateHz = atof(argv[++i]);
    if(!strcmp(argv[i],"-threshuV") && argc > i+1)
        threshuV = atof(argv[++i]);
    if(!strcmp(argv[i],"-convolve") && argc > i+1)
        convolvef = argv[++i];
    if(!strcmp(argv[i],"-boldTR") && argc > i+1)
        boldTR = atof(argv[++i]);

    /*START120320*/
    if(!strcmp(argv[i],"-times_out") && argc > i+1)
        timesoutf = argv[++i];
    if(!strcmp(argv[i],"-tol") && argc > i+1)
        tol = atof(argv[++i]);

    }
if(!nfiles) {
    printf("fidlError: Need to specify a conc of dats with -files. Abort!\n");fflush(stdout);
    exit(-1);
    }
if(!samplingrateHz) {
    printf("fidlError: Need to specify the sampling rate in Hz with -samplingrateHz. Abort!\n");fflush(stdout); 
    exit(-1);
    }

strcpy(root,concf);
if(!(rootptr=get_tail_sans_ext(root))) exit(-1);
/*printf("root = %s  rootptr = %s\n",root,rootptr);fflush(stdout);*/

skiplines = (int)rint(skipsec*samplingrateHz);
samplingratesec=1./samplingrateHz;
printf("skipsec = %f  skiplines = %d  samplingrateHz = %f  samplingratesec = %f\n",skipsec,skiplines,samplingrateHz,samplingratesec);

/*printf("Any eye movement greater than %f uV will be reported.\n",threshuV);fflush(stdout);*/


#if 0
if(convolvef) {
    if(!(convolved=read_data(convolvef,0,0,1,0)))exit(-1);
    if(!(lines=malloc(sizeof*lines*files->nfiles))) {
        printf("fidlError: Unable to malloc lines\n");
        exit(-1);
        }
    if(!(frames=malloc(sizeof*frames*files->nfiles))) {
        printf("fidlError: Unable to malloc frames\n");
        exit(-1);
        }
    TRlines = (int)rint(samplingrateHz*boldTR);
    skipframes = (int)rint(skipsec/boldTR); 
    printf("TRlines = %d  skipframes = %d\n",TRlines,skipframes);
    for(framessum=framesmax=linesmax=i=0;i<files->nfiles;i++) {
        if(!(data=read_data(files->files[i],0,skiplines,1,0)))exit(-1);
        if((lines[i]=data->nsubjects)>linesmax) linesmax = lines[i];
        frames[i] = (lines[i]=data->nsubjects)/TRlines;
        framessum += (frames[i] = (lines[i]=data->nsubjects)/TRlines);
        if(frames[i]>framesmax) framesmax = frames[i];
        free_data(data);
        printf("lines[%d] = %d  frames[%d] = %d\n",i,lines[i],i,frames[i]);
        }
    framessum += skipframes*files->nfiles;
    printf("framessum = %d framesmax = %d\n",framessum,framesmax);
    if(!(x=d2double(files->nfiles,linesmax))) {
        printf("fidlError: Unable to malloc x\n");
        exit(-1);;
        }
    if(!(y=d2double(files->nfiles,linesmax))) {
        printf("fidlError: Unable to malloc y\n");
        exit(-1);;
        }
    if(!(z=d2double(files->nfiles,framesmax))) {
        printf("fidlError: Unable to malloc z\n");
        exit(-1);;
        }
    fflush(stdout);
    }
#endif
/*START120322*/
if(!(lines=malloc(sizeof*lines*files->nfiles))) {
    printf("fidlError: Unable to malloc lines\n");
    exit(-1);
    }
if(!(frames=malloc(sizeof*frames*files->nfiles))) {
    printf("fidlError: Unable to malloc frames\n");
    exit(-1);
    }
TRlines = (int)rint(samplingrateHz*boldTR);
skipframes = (int)rint(skipsec/boldTR);
for(framessum=framesmax=linesmax=i=0;i<files->nfiles;i++) {
    if(!(data=read_data(files->files[i],0,skiplines,1,0)))exit(-1);
    if((lines[i]=data->nsubjects)>linesmax) linesmax = lines[i];
    frames[i] = (lines[i]=data->nsubjects)/TRlines;
    framessum += (frames[i] = (lines[i]=data->nsubjects)/TRlines);
    if(frames[i]>framesmax) framesmax = frames[i];
    free_data(data);
    printf("lines[%d] = %d  frames[%d] = %d\n",i,lines[i],i,frames[i]);
    }
framessum += skipframes*files->nfiles;
printf("TRlines = %d skipframes = %d framessum = %d framesmax = %d\n",TRlines,skipframes,framessum,framesmax);fflush(stdout);
if(!(large=d2double(files->nfiles,linesmax))) {
    printf("fidlError: Unable to malloc large\n");
    exit(-1);;
    }
if(!(small=d2double(files->nfiles,linesmax))) {
    printf("fidlError: Unable to malloc small\n");
    exit(-1);;
    }



#if 0
if(convolvef) {
    if(!(convolved=read_data(convolvef,0,0,1,0)))exit(-1);
    if(!(x=d2double(files->nfiles,linesmax))) {
        printf("fidlError: Unable to malloc x\n");
        exit(-1);;
        }
    if(!(y=d2double(files->nfiles,linesmax))) {
        printf("fidlError: Unable to malloc y\n");
        exit(-1);;
        }
    if(!(z=d2double(files->nfiles,framesmax))) {
        printf("fidlError: Unable to malloc z\n");
        exit(-1);;
        }
    }
#endif
/*START120322*/
if(convolvef) {
    if(!(convolved=read_data(convolvef,0,0,1,0)))exit(-1);
    if(!(largec=d2double(files->nfiles,linesmax))) {
        printf("fidlError: Unable to malloc largec\n");
        exit(-1);;
        }
    if(!(smallc=d2double(files->nfiles,linesmax))) {
        printf("fidlError: Unable to malloc smallc\n");
        exit(-1);;
        }
    if(convolvef&&(boldTR>samplingratesec)) { /*downsample*/
        if(!(largecd=d2double(files->nfiles,framesmax))) {
            printf("fidlError: Unable to malloc largecd\n");
            exit(-1);;
            }
        if(!(smallcd=d2double(files->nfiles,framesmax))) {
            printf("fidlError: Unable to malloc smallcd\n");
            exit(-1);;
            }
        }

    /*START120323*/
    if(!(identifystrptr=malloc(sizeof*identifystrptr*files->nfiles))) {
        printf("fidlError: Unable to malloc identifystrptr\n");
        exit(-1);
        }
    if(!(identifyn=malloc(sizeof*identifyn*files->nfiles))) {
        printf("fidlError: Unable to malloc identifyn\n");
        exit(-1);
        }
    for(i=0;i<files->nfiles;i++) identifyn[i]=0;
    if(!(number=malloc(sizeof*number*files->nfiles))) {
        printf("fidlError: Unable to malloc number\n");
        exit(-1);
        }

    }

#if 1
/*BEGIN*/

blinklines = 0.25/samplingratesec;
if(timesoutf) if(!(fp=fopen_sub(timesoutf,"w"))) exit(-1);
for(i=0;i<files->nfiles;i++) {
    printf("run %d %s\n",i+1,files->identify[i]);
    printf("%s\n",files->files[i]);
    if(!(data=read_data(files->files[i],0,skiplines,1,0)))exit(-1);
    for(negmax=posmax=0.,j=0;j<data->nsubjects;j++) {
        if(data->x[j][0]>posmax) {posmax = data->x[j][0];posmaxi = skiplines+1+j;} 
        if(data->x[j][0]<negmax) {negmax = data->x[j][0];negmaxi = skiplines+1+j;}  
        } 
    printf("    posmax = %f\tline = %d\n",posmax,posmaxi);
    printf("    negmax = %f\tline = %d\n",negmax,negmaxi);
    if(timesoutf) fprintf(fp,"%s\n",files->files[i]);
    for(j=0;j<data->nsubjects;j++) small[i][j]=data->x[j][0]; 
    for(small[i][0]=data->x[0][0],j=1;j<data->nsubjects-1;j++) {
        td = fabs(data->x[j][0]);
        if(td>=threshuV) {
            if((td>=fabs(data->x[j-1][0]))&&(td>=fabs(data->x[j+1][0]))) {
                j1 = (j-blinklines)<0 ? 0:j-blinklines;
                j2 = (j+blinklines)>(data->nsubjects-1) ? data->nsubjects-1:j+blinklines;
                for(td1=td,idx=j,k=j1;k<=j2;k++) if(fabs(data->x[k][0])>=td1) {td1=fabs(data->x[k][0]);idx=k;}
                if(idx<j) j1 = (idx-blinklines)<0 ? 0:idx-blinklines;
                else if(idx>j) {j2 = (idx+blinklines)>(data->nsubjects-1) ? data->nsubjects-1:idx+blinklines;j=idx;}
                if(j1) while(fabs(data->x[j1][0])>tol) if(!(j1--))break;

                /*while(fabs(data->x[j2][0])>tol) j2++;*/
                /*START120326*/
                for(;j2<(data->nsubjects-1)&&fabs(data->x[j2][0])>tol;) j2++;

                printf("line = %d\t%f s\t%f\n",skiplines+1+j,(skiplines+j)*samplingratesec,data->x[j][0]);
                printf("\tline = %d\t%f s\t%f\n",skiplines+1+j1,(skiplines+j1)*samplingratesec,data->x[j1][0]);
                printf("\tline = %d\t%f s\t%f\n",skiplines+1+j2,(skiplines+j2)*samplingratesec,data->x[j2][0]);
                if(timesoutf) fprintf(fp,"%f\t%f\n",(skiplines+j1)*samplingratesec,(skiplines+j2)*samplingratesec);
                for(k=j1;k<=j2;k++) {large[i][k] = data->x[k][0];small[i][k] = 0.;}
                j=j2+1;
                }
            }
        }
    if(convolvef) {
        for(j1=j=0;j<data->nsubjects;j++) {
            if(j>=convolved->nsubjects) j1++;
            if((j-j1)>=convolved->nsubjects) {
                printf("fidlError: j=%d j1=%d j-j1=%d convolved->nsubjects=%d\n",j,j1,j-j1,convolved->nsubjects);fflush(stdout);
                exit(-1);
                }
            for(w=0.,kk=0,k=j;k>=j1;k--,kk++) {
                largec[i][j] += fabs(large[i][k])*convolved->x[kk][0]; /*Ramot2011NI*/
                w += convolved->x[kk][0];
                }
            if(w) largec[i][j]/=w;
            }
        for(j1=j=0;j<data->nsubjects;j++) {
            if(j>=convolved->nsubjects) j1++;
            if((j-j1)>=convolved->nsubjects) {
                printf("fidlError: j=%d j1=%d j-j1=%d convolved->nsubjects=%d\n",j,j1,j-j1,convolved->nsubjects);fflush(stdout);
                exit(-1);
                }
            for(w=0.,kk=0,k=j;k>=j1;k--,kk++) {
                smallc[i][j] += fabs(small[i][k])*convolved->x[kk][0]; /*Ramot2011NI*/
                w += convolved->x[kk][0];
                }
            if(w) smallc[i][j]/=w;
            }
        }



    free_data(data);
    }
if(timesoutf) fclose(fp);


/*START120322*/
sprintf(filename,"%s_large.dat",rootptr);
if(!(fp=fopen_sub(filename,"w"))) exit(-1);
for(i=0;i<linesmax;i++) {
    for(j=0;j<files->nfiles;j++) fprintf(fp,"%f\t",large[j][i]);
    fprintf(fp,"\n");
    }
fclose(fp);
printf("Output written to %s\n",filename);fflush(stdout);
sprintf(filename,"%s_small.dat",rootptr);
if(!(fp=fopen_sub(filename,"w"))) exit(-1);
for(i=0;i<linesmax;i++) {
    for(j=0;j<files->nfiles;j++) fprintf(fp,"%f\t",small[j][i]);
    fprintf(fp,"\n");
    }
fclose(fp);
printf("Output written to %s\n",filename);fflush(stdout);


/*END*/
#endif

#if 0
if(convolvef) {
    sprintf(filename,"%s_%dHz_raw.dat",rootptr,(int)samplingrateHz);
    if(!(fp=fopen_sub(filename,"w"))) exit(-1);
    for(i=0;i<linesmax;i++) {
        for(j=0;j<files->nfiles;j++) fprintf(fp,"%f\t",x[j][i]);
        fprintf(fp,"\n");
        }
    fclose(fp);
    printf("Output written to %s\n",filename);fflush(stdout);
    
    sprintf(filename,"%s_%dHz.dat",rootptr,(int)samplingrateHz);
    if(!(fp=fopen_sub(filename,"w"))) exit(-1);
    for(i=0;i<linesmax;i++) {
        for(j=0;j<files->nfiles;j++) fprintf(fp,"%f\t",y[j][i]);
        fprintf(fp,"\n");
        }
    fclose(fp);
    printf("Output written to %s\n",filename);fflush(stdout);
    
    if(convolvef&&(boldTR>samplingratesec)) { /*downsample*/
        for(i=0;i<files->nfiles;i++) {
            for(k1=j=0;j<frames[i];j++) {
                for(k=0;k<TRlines;k++,k1++) z[i][j] += y[i][k1];
                z[i][j] /= TRlines;
                }
            }
        }
    sprintf(filename,"%s.dat",rootptr);
    if(!(fp=fopen_sub(filename,"w"))) exit(-1);
    for(i=0;i<framesmax;i++) {
        for(j=0;j<files->nfiles;j++) fprintf(fp,"%f\t",z[j][i]);
        fprintf(fp,"\n");
        }
    fclose(fp);
    printf("Output written to %s\n",filename);fflush(stdout);
    
    sprintf(filename,"%s_fidl.dat",rootptr);
    if(!(fp=fopen_sub(filename,"w"))) exit(-1);
    for(i=0;i<files->nfiles;i++) fprintf(fp,"eog%d\t",i+1); fprintf(fp,"\n");
    for(i=0;i<files->nfiles;i++) {
        for(j=0;j<frames[i];j++) {
            for(k=0;k<files->nfiles;k++) fprintf(fp,"%f\t",i==k?z[i][j]:0.);
            fprintf(fp,"\n");
            }
        }
    fclose(fp);
    printf("Output written to %s\n",filename);fflush(stdout);
    }
#endif
/*START120322*/
if(convolvef) {

    sprintf(filename,"%s_largeconv.dat",rootptr);
    if(!(fp=fopen_sub(filename,"w"))) exit(-1);
    for(i=0;i<linesmax;i++) {
        for(j=0;j<files->nfiles;j++) fprintf(fp,"%f\t",largec[j][i]);
        fprintf(fp,"\n");
        }
    fclose(fp);
    printf("Output written to %s\n",filename);fflush(stdout);

    sprintf(filename,"%s_smallconv.dat",rootptr);
    if(!(fp=fopen_sub(filename,"w"))) exit(-1);
    for(i=0;i<linesmax;i++) {
        for(j=0;j<files->nfiles;j++) fprintf(fp,"%f\t",smallc[j][i]);
        fprintf(fp,"\n");
        }
    fclose(fp);
    printf("Output written to %s\n",filename);fflush(stdout);
   
    if(convolvef&&(boldTR>samplingratesec)) { /*downsample*/
        for(i=0;i<files->nfiles;i++) {
            for(k1=j=0;j<frames[i];j++) {
                for(k=0;k<TRlines;k++,k1++) largecd[i][j] += largec[i][k1];
                largecd[i][j] /= TRlines;
                }
            }
        for(i=0;i<files->nfiles;i++) {
            for(k1=j=0;j<frames[i];j++) {
                for(k=0;k<TRlines;k++,k1++) smallcd[i][j] += smallc[i][k1];
                smallcd[i][j] /= TRlines;
                }
            }
        sprintf(filename,"%s_largeconvdown.dat",rootptr);
        if(!(fp=fopen_sub(filename,"w"))) exit(-1);
        for(i=0;i<framesmax;i++) {
            for(j=0;j<files->nfiles;j++) fprintf(fp,"%f\t",largecd[j][i]);
            fprintf(fp,"\n");
            }
        fclose(fp);
        printf("Output written to %s\n",filename);fflush(stdout);
        sprintf(filename,"%s_smallconvdown.dat",rootptr);
        if(!(fp=fopen_sub(filename,"w"))) exit(-1);
        for(i=0;i<framesmax;i++) {
            for(j=0;j<files->nfiles;j++) fprintf(fp,"%f\t",smallcd[j][i]);
            fprintf(fp,"\n");
            }
        fclose(fp);
        printf("Output written to %s\n",filename);fflush(stdout);
        }
    else {
        largecd=largec;smallcd=smallc;
        }

    #if 0
    sprintf(filename,"%s_fidl.dat",rootptr);
    if(!(fp=fopen_sub(filename,"w"))) exit(-1);
    for(i=0;i<files->nfiles;i++) fprintf(fp,"large%d\t",i+1);
    for(i=0;i<files->nfiles;i++) fprintf(fp,"small%d\t",i+1); fprintf(fp,"\n");
    for(i=0;i<files->nfiles;i++) {
        for(j=0;j<frames[i];j++) {
            for(k=0;k<files->nfiles;k++) fprintf(fp,"%f\t",i==k?largecd[i][j]:0.);
            for(k=0;k<files->nfiles;k++) fprintf(fp,"%f\t",i==k?smallcd[i][j]:0.);
            fprintf(fp,"\n");
            }
        }
    fclose(fp);
    printf("Output written to %s\n",filename);fflush(stdout);
    #endif
    /*START120323*/
    sprintf(filename,"%s_fidl.dat",rootptr);
    if(!(fp=fopen_sub(filename,"w"))) exit(-1);

    if(!files->identify) {
        for(i=0;i<files->nfiles;i++) fprintf(fp,"large%d\t",i+1);
        for(i=0;i<files->nfiles;i++) fprintf(fp,"small%d\t",i+1); fprintf(fp,"\n");
        }
    else {
        identifystrptr[0] = files->identify[0];
        identifyn[0] = 1;
        nidentify = 1;
        number[0] = 1;
        for(i=1;i<files->nfiles;i++) {
            for(j=0;j<nidentify;j++) {
                if(!strcmp(files->identify[i],identifystrptr[j])) {
                    number[i] = ++identifyn[j];
                    break;
                    }
                }
            if(j==nidentify) {
                identifystrptr[nidentify] = files->identify[i];
                number[i] = identifyn[nidentify] = 1;
                nidentify++;
                }
            }
        for(i=0;i<files->nfiles;i++) fprintf(fp,"large_%s%d\t",files->identify[i],number[i]);
        for(i=0;i<files->nfiles;i++) fprintf(fp,"small_%s%d\t",files->identify[i],number[i]);fprintf(fp,"\n");
        }
    for(i=0;i<files->nfiles;i++) {
        for(j=0;j<frames[i];j++) {
            for(k=0;k<files->nfiles;k++) fprintf(fp,"%f\t",i==k?largecd[i][j]:0.);
            for(k=0;k<files->nfiles;k++) fprintf(fp,"%f\t",i==k?smallcd[i][j]:0.);
            fprintf(fp,"\n");
            }
        }
    fclose(fp);
    printf("Output written to %s\n",filename);fflush(stdout);
    }
}
