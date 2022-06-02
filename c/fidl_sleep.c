/* Copyright 2/5/15 Washington University.  All Rights Reserved.
   fidl_sleep.c  $Revision: 1.15 $ */
#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "fidl.h"
#include "subs_util.h"
int main(int argc,char **argv)
{
char line[MAXNAME],write_back[MAXNAME],line2[MAXNAME],filename[MAXNAME],last_char,first_char,*strptr,evf[MAXNAME],*conc=NULL,
    *list=NULL,scratchf[31],timestr[23],*statestr[]={"Awake","","N1sleep","N2sleep","N3sleep"},*pwd,name[MAXNAME],*path=NULL,
    *pathdata=NULL,glmlist[1000],datalist[1000],*saveptr,*splitlistroot=NULL,*freesurfer=NULL,splitlist[1000],*ptr,*freesurfer1=NULL,
    *freesurfer2=NULL,*pathconc=NULL,*spreadsheet=NULL,*saveptr2=NULL,*saveptr3=NULL,*saveptr4=NULL,*saveptr5=NULL,
    *bold=NULL,*concloc=NULL,*motpath=NULL,*evpath=NULL,*concpath=NULL,
    *statestrshort[]={"W","N1","N2","N3","Wshort","N1short","N2short","N3short"};
size_t i1,j1,nlines=0,*Ri,R;
int i,j,cnt,*frame,state,sum,**frperstate,lcecho=0,lcmetrics=0,lcev=0,nhypnogram=0,*idx,cnt1,len,male=0,female=0,lcshort=0,state0,lcR;
double TR=2.08,time,dur=0.,Awake=0.,N1sleep=0.,N2sleep=0.,N3sleep=0.,skip=5.,age=0,sumage,sumage2,agemin,agemax,epoch=0.,
    sum1[4],sum2[4],td;
Files_Struct *hypnogram=NULL;
FILE *fp=NULL,*evp=NULL,*fp2=NULL,*fp3=NULL,*op;
if(argc<5) {
    fprintf(stderr,"  -hypnogram: Single column of numbers. 0:awake -2:N1sleep -3:N2sleep -4:N3sleep\n");
    fprintf(stderr,"              One file or many or listed in a conc. Separate metrics for each file.\n");
    fprintf(stderr,"  -TR:        Sampling rate in seconds.\n");
    fprintf(stderr,"  -echo       Print metrics to terminal.\n");
    fprintf(stderr,"  -metrics    Print metrics to file.\n");
    fprintf(stderr,"  -ev         Make event file. First five frames are skipped.\n");
    fprintf(stderr,"OPTIONS TO MAKE CONCS AND PUT THEM IN A LIST\n");
    fprintf(stderr,"  -list:   Name of list file to be created.\n");
    fprintf(stderr,"  -conc:   Text file. Single column. Path to each subject's data. No foreslash at the end.\n");
    fprintf(stderr,"  -bold:    Suffix for bold files. Ex1. xr3d_norm.4dfp.img Ex2. xr3d.4dfp.img\n");
    fprintf(stderr,"  -concloc: Where to put concs. Ex. /data/nil-bluearc/raichle/mcavoy/sleep/NOMODE1000/bold\n");
    fprintf(stderr,"  -motpath: Location of motion regressor files. Ex. /data/nil-bluearc/raichle/mcavoy/sleep/motion150720/RRdR2R2d\n");
    fprintf(stderr,"  -evpath: Location of event files. Ex. /data/nil-bluearc/raichle/mcavoy/sleep/NOMODE1000/ev");

    //START160923
    fprintf(stderr,"OPTIONS TO PUT CONCS IN A LIST\n");
    fprintf(stderr,"  -list:     Name of list file to be created.\n");
    fprintf(stderr,"  -conc:     Text file. Single column. Path to each subject's data. No foreslash at the end. Used to find t4\n");
    fprintf(stderr,"  -concpath: Location of concs. No foreslash at end.\n");
    fprintf(stderr,"  -motpath:  Location of motion regressor files. Ex. /data/nil-bluearc/raichle/mcavoy/sleep/motion150720/RRdR2R2d\n");
    fprintf(stderr,"  -evpath:   Location of event files. Ex. /data/nil-bluearc/raichle/mcavoy/sleep/NOMODE1000/ev");


    fprintf(stderr,"OPTIONS TO MAKE A GLM LIST\n");
    fprintf(stderr,"  -path:   Path to glms. No foreslash at end. GLMs must fulfill minimum time requirements for each state to be culled.\n");
    fprintf(stderr,"  -Awake   Time in seconds. Subjects with a minimum of this many seconds of Awake.\n");
    fprintf(stderr,"  -N1sleep Time in seconds. Subjects with a minimum of this many seconds of N1sleep. Output to terminal.\n");
    fprintf(stderr,"  -N2sleep Time in seconds. Subjects with a minimum of this many seconds of N2sleep. Output to terminal.\n");
    fprintf(stderr,"  -N3sleep Time in seconds. Subjects with a minimum of this many seconds of N3sleep. Output to terminal.\n");
    fprintf(stderr,"  -Awake -N1sleep -N2sleep -N3sleep are ANDed\n");
    fprintf(stderr,"OPTIONS TO MAKE DATALIST FOR FREESURFER SEGMENTATION\n");
    fprintf(stderr,"  -pathdata: Path to subject directories. No foreslash at end.\n");
    fprintf(stderr,"  -Awake     Time in seconds. Subjects with a minimum of this many seconds of Awake.\n");
    fprintf(stderr,"  -N1sleep   Time in seconds. Subjects with a minimum of this many seconds of N1sleep. Output to terminal.\n");
    fprintf(stderr,"  -N2sleep   Time in seconds. Subjects with a minimum of this many seconds of N2sleep. Output to terminal.\n");
    fprintf(stderr,"  -N3sleep   Time in seconds. Subjects with a minimum of this many seconds of N3sleep. Output to terminal.\n");
    fprintf(stderr,"  -Awake -N1sleep -N2sleep -N3sleep are ANDed\n");
    fprintf(stderr,"OPTIONS TO MAKE LIST FOR fidl_split\n");
    fprintf(stderr,"  -pathdata: Path to subject directories. No foreslash at end.\n");
    fprintf(stderr,"  -Awake     Time in seconds. Subjects with a minimum of this many seconds of Awake.\n");
    fprintf(stderr,"  -N1sleep   Time in seconds. Subjects with a minimum of this many seconds of N1sleep. Output to terminal.\n");
    fprintf(stderr,"  -N2sleep   Time in seconds. Subjects with a minimum of this many seconds of N2sleep. Output to terminal.\n");
    fprintf(stderr,"  -N3sleep   Time in seconds. Subjects with a minimum of this many seconds of N3sleep. Output to terminal.\n");
    fprintf(stderr,"  -Awake -N1sleep -N2sleep -N3sleep are ANDed\n");
    fprintf(stderr,"  -splitlistroot: Root for list name. eg wm2\n");
    fprintf(stderr,"  -freesurfer:    Include path. Name of FREESURFER SEGMENTATION file.\n");
    fprintf(stderr,"                  Insert ? in place of subject identifier. Surround with quotes.\n");
    fprintf(stderr,"                  eg \"/data/nil-bluearc/raichle/mcavoy/sleep/segment/?/nusmask/aparc+aseg_cerebralwm_ero2_mask_222.4dfp.img\"\n");
    fprintf(stderr,"OPTIONS TO MAKE LIST FOR fidl_motionreg. List includes concs, evs and t4s.\n");
    fprintf(stderr,"  -pathdata: Path to subject directories. No foreslash at end.\n");
    fprintf(stderr,"  -Awake:    Time in seconds. Subjects with a minimum of this many seconds of Awake.\n");
    fprintf(stderr,"  -N1sleep:  Time in seconds. Subjects with a minimum of this many seconds of N1sleep. Output to terminal.\n");
    fprintf(stderr,"  -N2sleep:  Time in seconds. Subjects with a minimum of this many seconds of N2sleep. Output to terminal.\n");
    fprintf(stderr,"  -N3sleep:  Time in seconds. Subjects with a minimum of this many seconds of N3sleep. Output to terminal.\n");
    fprintf(stderr,"  -Awake -N1sleep -N2sleep -N3sleep are ANDed\n");
    fprintf(stderr,"  -pathconc: Path to conc files. No foreslash at end.\n");
    fprintf(stderr,"OPTIONS TO CALCULATE MEAN AGE AND GENDER.\n");
    fprintf(stderr,"  -conc:        Text file. Single column. Path to each subject's hypnogram. Ex. hypnogram.conc\n");
    fprintf(stderr,"  -spreadsheet: Text file. Ex. subject_processing_track.txt\n");
    fprintf(stderr,"OPTIONS TO CREATE EVENT FILES WITH W,N1,N2,N3,Wshort,N1short,N2short,N3short.\n");
    fprintf(stderr,"  -hypnogram: Single column of numbers. 0:awake -2:N1sleep -3:N2sleep -4:N3sleep\n");
    fprintf(stderr,"              One file or many or listed in a conc. Separate metrics for each file.\n");
    fprintf(stderr,"  -short      Events include W,N1,N2,N3,Wshort,N1short,N2short,N3short.\n");
    fprintf(stderr,"  -epoch:     Minimum time in s not be considered short. Default is 0.\n");
    fprintf(stderr,"  -echo       Print metrics to terminal.\n");
    fprintf(stderr,"  -ev         Make event file. First five frames are skipped.  TR=2.08s\n");
    fprintf(stderr,"OPTIONS TO CREATE GLM LIST WITH W,N1,N2,N3,Wshort,N1short,N2short,N3short.\n");
    fprintf(stderr,"  -hypnogram: Single column of numbers. 0:awake -2:N1sleep -3:N2sleep -4:N3sleep\n");
    fprintf(stderr,"              One file or many or listed in a conc. Separate metrics for each file.\n");
    fprintf(stderr,"  -short      Events include W,N1,N2,N3,Wshort,N1short,N2short,N3short.\n");
    fprintf(stderr,"  -epoch:     Minimum time in s not be considered short. Default is 0.\n");
    fprintf(stderr,"  -path:      Path to glms. No foreslash at end.\n");
    fprintf(stderr,"              GLMs must fulfill minimum time requirements for each state to be culled.\n");
    fprintf(stderr,"  -Awake:     Time in seconds. Subjects with a minimum of this many seconds of Awake.\n");
    fprintf(stderr,"  -N1sleep:   Time in seconds. Subjects with a minimum of this many seconds of N1sleep. Output to terminal.\n");
    fprintf(stderr,"  -N2sleep:   Time in seconds. Subjects with a minimum of this many seconds of N2sleep. Output to terminal.\n");
    fprintf(stderr,"  -N3sleep:   Time in seconds. Subjects with a minimum of this many seconds of N3sleep. Output to terminal.\n");
    fprintf(stderr,"  -spreadsheet: Text file. Ex. /data/nil-bluearc/raichle/mcavoy/sleep/age_gender/subject_processing_track_TAB.txt\n");
    fprintf(stderr,"                If included, only righties are culled.\n");
    fprintf(stderr,"  -Awake -N1sleep -N2sleep -N3sleep are ANDed\n");
    exit(-1);
    }
for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-hypnogram") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nhypnogram;
        strptr = strrchr(argv[i+1],'.');
        if(!strcmp(strptr,".conc")) {
            if(!(hypnogram=read_conc(nhypnogram,&argv[i+1]))) exit(-1);
            }
        else {
            if(!(hypnogram=get_files(nhypnogram,&argv[i+1]))) exit(-1);
            }
        }
    if(!strcmp(argv[i],"-TR") && argc > i+1) TR=strtod(argv[++i],NULL);
    if(!strcmp(argv[i],"-echo")) lcecho=1;
    if(!strcmp(argv[i],"-metrics")) lcmetrics=1;
    if(!strcmp(argv[i],"-ev")) lcev=1;
    if(!strcmp(argv[i],"-conc") && argc > i+1) conc = argv[++i];
    if(!strcmp(argv[i],"-list") && argc > i+1) list = argv[++i];
    if(!strcmp(argv[i],"-path") && argc > i+1) path = argv[++i];
    if(!strcmp(argv[i],"-Awake") && argc > i+1) Awake=strtod(argv[++i],NULL);
    if(!strcmp(argv[i],"-N1sleep") && argc > i+1) N1sleep=strtod(argv[++i],NULL);
    if(!strcmp(argv[i],"-N2sleep") && argc > i+1) N2sleep=strtod(argv[++i],NULL);
    if(!strcmp(argv[i],"-N3sleep") && argc > i+1) N3sleep=strtod(argv[++i],NULL);
    if(!strcmp(argv[i],"-pathdata") && argc > i+1) pathdata = argv[++i];
    if(!strcmp(argv[i],"-splitlistroot") && argc > i+1) splitlistroot = argv[++i];
    if(!strcmp(argv[i],"-freesurfer") && argc > i+1) freesurfer = argv[++i];
    if(!strcmp(argv[i],"-pathconc") && argc > i+1) pathconc = argv[++i];
    if(!strcmp(argv[i],"-spreadsheet") && argc > i+1) spreadsheet = argv[++i];
    if(!strcmp(argv[i],"-short")) lcshort=1;
    if(!strcmp(argv[i],"-epoch") && argc > i+1) epoch=strtod(argv[++i],NULL);
    if(!strcmp(argv[i],"-bold") && argc > i+1) bold = argv[++i];
    if(!strcmp(argv[i],"-concloc") && argc > i+1) concloc = argv[++i];
    if(!strcmp(argv[i],"-motpath") && argc > i+1) motpath = argv[++i];
    if(!strcmp(argv[i],"-evpath") && argc > i+1) evpath = argv[++i];

    //START160923
    if(!strcmp(argv[i],"-concpath") && argc > i+1) concpath = argv[++i];

    }
if(!(pwd=getenv("PWD"))) {
    printf("fidlError: Unable to get environment variable PWD\n");
    exit(-1);
    }

if(conc&&spreadsheet){
    if(!(fp=fopen_sub(conc,"r")))exit(-1);
    if(!(fp2=fopen_sub(spreadsheet,"r")))exit(-1);
    for(agemin=200.,agemax=sumage2=sumage=0.,R=j=0;fgets(line,sizeof(line),fp);){
        for(strptr=line,i=0;i<5;i++){saveptr=strtok_r(strptr,"/",&strptr);}
        rewind(fp2);fgets(line2,sizeof(line2),fp2);fgets(line2,sizeof(line2),fp2);
        for(cnt=0;fgets(line2,sizeof(line2),fp2);){
            for(strptr=line2,i=0;i<3;i++){saveptr2=strtok_r(strptr," 	",&strptr);}
            if(!(strcmp(saveptr,saveptr2))){
                saveptr3=strtok_r(strptr,"\t",&strptr);
                saveptr4=strtok_r(strptr,"\t",&strptr);
                strtok_r(strptr,"\t",&strptr);
                for(saveptr5=strptr;*strptr!='\t';strptr++);*strptr++=0;
                if(!(*saveptr5)){
                    if(!strcmp(saveptr4,"male"))male++;else if(!strcmp(saveptr4,"female"))female++;else{
                        printf("saveptr4=%s Neither male or female. Abort!\n",saveptr4);fflush(stdout);exit(-1);}
                    R++;
                    age=strtod(saveptr3,NULL);
                    if(age<agemin)agemin=age;
                    if(age>agemax)agemax=age;
                    sumage2+=age*age;
                    sumage+=age;
                    }
                cnt=1;break;
                }
            }
        if(cnt){printf("%d saveptr=%s\tsaveptr2=%s\tage=%g\t%s\thandedness=%s\n",++j,saveptr,saveptr2,age,saveptr4,saveptr5);
            fflush(stdout);}
        else{printf("saveptr=%s Not found in %s\n",saveptr,spreadsheet);fflush(stdout);exit(-1);}
        }
    fclose(fp);fclose(fp2);
    printf("%zd right handers age mean=%f standard deviation=%f min=%g max=%g male=%d female=%d \n",R,sumage/(double)R,
        sqrt((sumage2-sumage*sumage/(double)R)/(double)(R-1)),agemin,agemax,male,female);
    exit(0);
    }
if(hypnogram&&lcshort){
    if(!(Ri=malloc(sizeof*Ri*hypnogram->nfiles))){printf("fidlError: Unable to malloc Ri\n");fflush(stdout);exit(-1);}
    if(spreadsheet){
        if(!(fp2=fopen_sub(spreadsheet,"r")))exit(-1);
        R=0;
        }
    else{
        R=hypnogram->nfiles;
        for(i1=0;i1<hypnogram->nfiles;i1++)Ri[i1]=i1;
        }
    if(!(frperstate=d2int(hypnogram->nfiles,8))){printf("fidlError: Unable to allocate frperstate\n");exit(-1);}
    for(agemin=200.,agemax=sumage2=sumage=0.,i1=0;i1<hypnogram->nfiles;i1++){
        if(!(fp=fopen_sub(hypnogram->files[i1],"r"))) exit(-1);
        for(nlines=0;fgets(line,sizeof(line),fp);nlines++) {
            if((cnt=count_strings_new3(line,write_back,' ',' ',&last_char,&first_char,0,0))!=1) {
                printf("fidlError: %s  line %lu has %d strings. Must have only 1.\n",hypnogram->files[i1],(unsigned long)nlines+1,
                    cnt);
                fflush(stdout);exit(-1);
                }
            }
        if(nlines!=1500) {printf("fidlError: %s  has %lu lines. Should be 1500.\n",hypnogram->files[i1],(unsigned long)nlines);
            fflush(stdout);exit(-1);}
        if(!(frame=malloc(sizeof*frame*(nlines+1)))) {printf("fidlError: Unable to malloc frame\n");fflush(stdout);exit(-1);}
        frame[nlines]=100;
        for(rewind(fp),j=0;fgets(line,sizeof(line),fp);j++) {
            frame[j]=(int)strtod(line,NULL);
            if(frame[j]!=0&&frame[j]!=-2&&frame[j]!=-3&&frame[j]!=-4) {
                printf("fidlError: %s  has an unexpected value of %d on line %lu. Must be 0,-2,-3 or -4.\n",hypnogram->files[i1],
                    frame[j],(unsigned long)j+1);fflush(stdout);exit(-1);
                }
            }
        fclose(fp);
        strcpy(line,hypnogram->files[i1]);
        for(strptr=line,i=0;i<5;i++)saveptr=strtok_r(strptr,"/",&strptr);
        lcR=1;
        if(spreadsheet){
            rewind(fp2);fgets(line2,sizeof(line2),fp2);fgets(line2,sizeof(line2),fp2);
            for(cnt=0;fgets(line2,sizeof(line2),fp2);){
                for(strptr=line2,i=0;i<3;i++){saveptr2=strtok_r(strptr," \t",&strptr);}
                if(!(strcmp(saveptr,saveptr2))){
                    saveptr3=strtok_r(strptr,"\t",&strptr);
                    saveptr4=strtok_r(strptr,"\t",&strptr);
                    strtok_r(strptr,"\t",&strptr);
                    for(saveptr5=strptr;*strptr!='\t';strptr++);*strptr++=0;
                    if((*saveptr5)){
                        lcR=0;
                        }
                    else{
                        if(!strcmp(saveptr4,"male"))male++;else if(!strcmp(saveptr4,"female"))female++;else{
                            printf("saveptr4=%s Neither male or female. Abort!\n",saveptr4);fflush(stdout);exit(-1);}
                        Ri[R++]=i1;
                        age=strtod(saveptr3,NULL);
                        if(age<agemin)agemin=age;
                        if(age>agemax)agemax=age;
                        sumage2+=age*age;
                        sumage+=age;
                        }
                    cnt=1;break;
                    }
                }
            }
        strcpy(write_back,hypnogram->files[i1]);
        if(!(strptr=get_tail_sans_ext(write_back))) exit(-1);
        if(lcecho)printf("%s\n",line);
        if(lcev){
            sprintf(evf,"%s_%s.fidl",line,strptr);
            if(!(evp=fopen_sub(evf,"w"))) exit(-1);
            fprintf(evp,"%.2f %s %s %s %s %s %s %s %s\n",TR,statestrshort[0],statestrshort[1],statestrshort[2],statestrshort[3],
                statestrshort[4],statestrshort[5],statestrshort[6],statestrshort[7]);
            }
        for(time=skip*TR,sum=0,state=frame[0],cnt=j1=1;j1<=nlines;j1++) {
            if(frame[j1]==state) cnt++;
            else {
                state0=!state?state:abs(state)-1;
                if((dur=(double)cnt*TR)<epoch)state0+=4; 
                if(lcev)fprintf(evp,"%.2f\t%d\t%.2f\n",time,state0,dur);
                if(lcecho)printf("%s\t%d\n",statestrshort[state0],cnt);
                if(state!=100)frperstate[i1][state0]+=cnt;
                time+=dur;
                sum+=cnt;
                state=frame[j1];cnt=1;
                }
            }
        if(sum!=(int)nlines) printf("fidlError: sum=%d nlines=%lu Should be equal.\n",sum,(unsigned long)nlines);
        if(lcecho) {
            printf("------------------\n");
            printf("%s %d %s %d %s %d %s %d %s %d %s %d %s %d %s %d\n",statestrshort[0],frperstate[i1][0],statestrshort[1],
                frperstate[i1][1],statestrshort[2],frperstate[i1][2],statestrshort[3],frperstate[i1][3],statestrshort[4],
                frperstate[i1][4],statestrshort[5],frperstate[i1][5],statestrshort[6],frperstate[i1][6],statestrshort[7],
                frperstate[i1][7]);
            printf("%s %.1fs %s %.1fs %s %.1fs %s %.1fs %s %.1fs %s %.1fs %s %.1fs %s %.1fs\n",statestrshort[0],frperstate[i1][0]*TR,
                statestrshort[1],frperstate[i1][1]*TR,statestrshort[2],frperstate[i1][2]*TR,statestrshort[3],frperstate[i1][3]*TR,
                statestrshort[4],frperstate[i1][4]*TR,statestrshort[5],frperstate[i1][5]*TR,statestrshort[6],frperstate[i1][6]*TR,
                statestrshort[7],frperstate[i1][7]*TR);
            printf("%s %.1fm %s %.1fm %s %.1fm %s %.1fm %s %.1fm %s %.1fm %s %.1fm %s %.1fm\n\n",statestrshort[0],
                frperstate[i1][0]*TR/60.,statestrshort[1],frperstate[i1][1]*TR/60.,statestrshort[2],frperstate[i1][2]*TR/60.,
                statestrshort[3],frperstate[i1][3]*TR/60.,statestrshort[4],frperstate[i1][4]*TR/60.,statestrshort[5],
                frperstate[i1][5]*TR/60.,statestrshort[6],frperstate[i1][6]*TR/60.,statestrshort[7],frperstate[i1][7]*TR/60.);
            }
        if(lcev){
            fclose(evp);
            if(!lcecho)printf("Output written to %s\n",evf);
            }
        free(frame);
        }
    if(Awake||N1sleep||N2sleep||N3sleep) {
        name[0]=0;
        if(Awake){
            sprintf(line,"Wake%g",Awake);
            strcat(name,line);
            }
        if(N1sleep){
            sprintf(line,"N1sleep%g",N1sleep);
            strcat(name,line);
            }
        if(N2sleep){
            sprintf(line,"N2sleep%g",N2sleep);
            strcat(name,line);
            }
        if(N3sleep){
            sprintf(line,"N3sleep%g",N3sleep);
            strcat(name,line);
            }
        if(path){
            sprintf(glmlist,"%s.list",name);
            if(!(fp=fopen_sub(glmlist,"w"))) exit(-1);
            }
        if(!(idx=malloc(sizeof*idx*R))){printf("fidlError: Unable to malloc idx\n");fflush(stdout);exit(-1);}
        for(i1=0;i1<R;i1++)idx[i1]=0;
        sum=0;
        if(Awake){sum++;for(i1=0;i1<R;i1++)if(frperstate[Ri[i1]][0]*TR>=Awake)idx[i1]++;}
        if(N1sleep){sum++;for(i1=0;i1<R;i1++)if(frperstate[Ri[i1]][1]*TR>=N1sleep)idx[i1]++;}
        if(N2sleep){sum++;for(i1=0;i1<R;i1++)if(frperstate[Ri[i1]][2]*TR>=N2sleep)idx[i1]++;}
        if(N3sleep){sum++;for(i1=0;i1<R;i1++)if(frperstate[Ri[i1]][3]*TR>=N3sleep)idx[i1]++;}

        //START161012
        for(i1=0;i1<4;i1++)sum1[i1]=sum2[i1]=0.; 

        for(cnt1=i1=0;i1<R;i1++) {
            if(idx[i1]==sum) {
                strcpy(line,hypnogram->files[Ri[i1]]);
                for(ptr=line,j=0;j<5;j++)strptr=strtok_r(ptr,"/",&ptr);
                printf("%s %s %6.1fs %s %6.1fs %s %6.1fs %s %6.1fs %s %6.1fs %s %6.1fs %s %6.1fs %s %6.1fs\n",strptr,statestrshort[0],
                    frperstate[Ri[i1]][0]*TR,statestrshort[1],frperstate[Ri[i1]][1]*TR,statestrshort[2],frperstate[Ri[i1]][2]*TR,
                    statestrshort[3],frperstate[Ri[i1]][3]*TR,statestrshort[4],frperstate[Ri[i1]][4]*TR,statestrshort[5],
                    frperstate[Ri[i1]][5]*TR,statestrshort[6],frperstate[Ri[i1]][6]*TR,statestrshort[7],frperstate[Ri[i1]][7]*TR);
                if(path)fprintf(fp,"%s/%s.glm\n",path,strptr);

                //START161012
                for(j1=0;j1<4;j1++){
                    sum1[j1]+=(td=frperstate[Ri[i1]][j1]*TR/60.);
                    sum2[j1]+=td*td; 
                    }

                cnt1++;
                }
            }
        printf("------------------\n%d subjects %s>=%.1fs %s>=%.1fs %s>=%.1fs %s>=%.1fs\n",cnt1,statestr[0],Awake,statestr[2],N1sleep,
            statestr[3],N2sleep,statestr[4],N3sleep);

        //printf("sum2=");for(i1=0;i1<4;i1++)printf("%f ",sum2[i1]);printf("\n");
        //START161012
        for(i1=0;i1<4;i1++)printf("%s mean %.1fm sd %.1fm\n",statestrshort[i1],sum1[i1]/(double)cnt1,
            sqrt((sum2[i1]-sum1[i1]*sum1[i1]/(double)cnt1)/(double)(cnt1-1)));

        if(spreadsheet){printf("spreadsheet %s\n    Only righties were selected.\n",spreadsheet);}
        if(path){
            fclose(fp);
            printf("Output written to %s\n",glmlist);fflush(stdout);
            }
        }
    fflush(stdout);exit(0);
    } 
if(conc&&list) {
    sprintf(scratchf,"fidl%s.txt",make_timestr2(timestr));
    if(!(fp=fopen_sub(conc,"r"))) exit(-1);
    if(!(op=fopen_sub(list,"w"))) exit(-1);
    for(;fgets(line,sizeof(line),fp);) {
        strcpy(line2,line);
        cnt=count_strings_new3(line2,write_back,'/',' ',&last_char,&first_char,0,' ');
        for(strptr=write_back,j=0;j<cnt;j++) strptr=grab_string_new(strptr,name,&cnt1);

        #if 0
        sprintf(filename,"%s/%s.conc",concloc,name);
        fprintf(op,"%s\n",filename);
        grab_string_new(line,line2,&len);
        sprintf(write_back,"ls -1 %s/bold*/*%s > %s\n",line2,bold,filename);
        if(system(write_back)==-1) {
            printf("fidlError0: Unable to %s\n",write_back);
            exit(-1);
            }
        printf("Conc file written to %s\n",filename);
        #endif
        //START160923
        grab_string_new(line,line2,&len);
        if(concloc){
            sprintf(filename,"%s/%s.conc",concloc,name);
            fprintf(op,"%s\n",filename);
            grab_string_new(line,line2,&len);
            sprintf(write_back,"ls -1 %s/bold*/*%s > %s\n",line2,bold,filename);
            if(system(write_back)==-1) {
                printf("fidlError0: Unable to %s\n",write_back);
                exit(-1);
                }
            printf("Conc file written to %s\n",filename);
            }
        else if(concpath){
            fprintf(op,"%s/%s.conc\n",concpath,name);
            }

        sprintf(write_back,"ls -1 %s/atlas/*_anat_ave_to_SLEEP_mpr_t4 > %s\n",line2,scratchf);
        if(system(write_back)==-1) {
            printf("fidlError0: Unable to %s\n",write_back);
            exit(-1);
            }
        if(!(fp2=fopen_sub(scratchf,"r"))) exit(-1);
        for(;fgets(line,sizeof(line),fp2);) fprintf(op,"%s",line);
        fclose(fp2);
        fprintf(op,"%s/%s_motion.ext\n%s/%s_AASM_hypnogram.fidl\n",motpath,name,evpath,name);
        }
    fclose(op);
    printf("List file written to %s\n",list);
    fclose(fp);
    sprintf(write_back,"rm %s",scratchf);
    if(system(write_back)==-1) {
        printf("fidlError: Unable to %s\n",write_back);
        exit(-1);
        }
    }
else {
    if(!nhypnogram){printf("fidlError: Need to provide -hypnogram\n");fflush(stdout);exit(-1);}
    if(!(frperstate=d2int(hypnogram->nfiles,5))) {
        printf("fidlError: Unable to allocate frperstate\n");
        exit(-1);
        }
    for(i1=0;i1<hypnogram->nfiles;i1++) {
        if(!(fp=fopen_sub(hypnogram->files[i1],"r"))) exit(-1);
        for(nlines=0;fgets(line,sizeof(line),fp);nlines++) {
            if((cnt=count_strings_new3(line,write_back,' ',' ',&last_char,&first_char,0,0))!=1) {
                printf("fidlError: %s  line %lu has %d strings. Must have only 1.\n",hypnogram->files[i1],(unsigned long)nlines+1,
                    cnt);
                fflush(stdout);exit(-1);
                }
            }
        if(nlines!=1500) {printf("fidlError: %s  has %lu lines. Should be 1500.\n",hypnogram->files[i1],(unsigned long)nlines);
            fflush(stdout);exit(-1);}
        if(!(frame=malloc(sizeof*frame*(nlines+1)))) {printf("fidlError: Unable to malloc frame\n");fflush(stdout);exit(-1);}
        frame[nlines]=100;
        for(rewind(fp),j=0;fgets(line,sizeof(line),fp);j++) {
            frame[j]=(int)strtod(line,NULL);
            if(frame[j]!=0&&frame[j]!=-2&&frame[j]!=-3&&frame[j]!=-4) {
                printf("fidlError: %s  has an unexpected value of %d on line %lu. Must be 0,-2,-3 or -4.\n",hypnogram->files[i1],
                    frame[j],(unsigned long)j+1);fflush(stdout);exit(-1); 
                }
            }
        fclose(fp);
        strcpy(line,hypnogram->files[i1]);
        if((cnt=count_strings_new3(line,write_back,'/',' ',&last_char,&first_char,0,' '))!=7) {
            printf("fidlError: %s  has %d strings. Must have 7.\n",hypnogram->files[i1],cnt);fflush(stdout);exit(-1);
            }
        for(strptr=write_back,j=0;j<5;j++) strptr=grab_string_new(strptr,line,&cnt);
        strcpy(write_back,hypnogram->files[i1]);
        if(!(strptr=get_tail_sans_ext(write_back))) exit(-1);
        if(lcmetrics) {
            sprintf(filename,"%s_%s_metrics.txt",line,strptr);
            if(!(fp=fopen_sub(filename,"w"))) exit(-1);
            fprintf(fp,"%s\n",line);
            }
        if(lcecho)printf("%s\n",line);
        if(lcev) {
            sprintf(evf,"%s_%s.fidl",line,strptr);
            if(!(evp=fopen_sub(evf,"w"))) exit(-1);
            fprintf(evp,"%.2f %s %s %s %s\n",TR,statestr[0],statestr[2],statestr[3],statestr[4]);
            }
        for(time=skip*TR,sum=0,state=frame[0],cnt=j1=1;j1<=nlines;j1++) {
            if(frame[j1]==state) cnt++;
            else {
                if(lcmetrics)fprintf(fp,"%s\t%d\n",statestr[abs(state)],cnt);
                if(lcecho)printf("%s\t%d\n",statestr[abs(state)],cnt);
                if(lcev)fprintf(evp,"%.2f\t%d\t%.2f\n",time,!state?state:abs(state)-1,dur=((double)cnt*TR));
                time+=dur;
                if(state!=100)frperstate[i1][abs(state)]+=cnt;
                sum+=cnt;
                state=frame[j1];cnt=1;
                }
            }
        if(sum!=(int)nlines) printf("fidlError: sum=%d nlines=%lu Should be equal.\n",sum,(unsigned long)nlines);
        if(lcmetrics) {
            fprintf(fp,"------------------\n%s %d %s %d %s %d %s %d\n%s %.1fs %s %.1fs %s %.1fs %s %.1fs\n",statestr[0],
                frperstate[i1][0],statestr[2],frperstate[i1][2],statestr[3],frperstate[i1][3],statestr[4],frperstate[i1][4],
                statestr[0],frperstate[i1][0]*TR,statestr[2],frperstate[i1][2]*TR,statestr[3],frperstate[i1][3]*TR,statestr[4],
                frperstate[i1][4]*TR);
            fclose(fp);
            if(!lcecho)printf("Output written to %s\n",filename);
            }
        if(lcecho) {
            printf("------------------\n");
            printf("%s %d %s %d %s %d %s %d\n",statestr[0],frperstate[i1][0],statestr[2],frperstate[i1][2],statestr[3],
                frperstate[i1][3],statestr[4],frperstate[i1][4]);
            printf("%s %.1fs %s %.1fs %s %.1fs %s %.1fs\n",statestr[0],frperstate[i1][0]*TR,statestr[2],frperstate[i1][2]*TR,
                statestr[3],frperstate[i1][3]*TR,statestr[4],frperstate[i1][4]*TR);
            printf("%s %.1fm %s %.1fm %s %.1fm %s %.1fm\n\n",statestr[0],frperstate[i1][0]*TR/60.,statestr[2],frperstate[i1][2]*TR/60.,
                statestr[3],frperstate[i1][3]*TR/60.,statestr[4],frperstate[i1][4]*TR/60.);
            }
        if(lcev) {
            fclose(evp);
            if(!lcecho)printf("Output written to %s\n",evf);
            }
        free(frame);
        }
    if(Awake||N1sleep||N2sleep||N3sleep) {
        name[0]=0;
        if(Awake){
            sprintf(line,"Awake%g",Awake);
            strcat(name,line);
            }
        if(N1sleep){
            sprintf(line,"N1sleep%g",N1sleep);
            strcat(name,line);
            }
        if(N2sleep){
            sprintf(line,"N2sleep%g",N2sleep);
            strcat(name,line);
            }
        if(N3sleep){
            sprintf(line,"N3sleep%g",N3sleep);
            strcat(name,line);
            }
        if(path){
            sprintf(glmlist,"%s.list",name);
            if(!(fp=fopen_sub(glmlist,"w"))) exit(-1);
            }
        if(pathdata){
            if(!splitlistroot) {
                sprintf(datalist,"%s.%s",name,!pathconc?"txt":"list");
                if(!(fp2=fopen_sub(datalist,"w"))) exit(-1);
                }
            if(splitlistroot) {
                sprintf(splitlist,"%s_%s.list",splitlistroot,name);
                if(!(fp3=fopen_sub(splitlist,"w"))) exit(-1);
                freesurfer1=strtok_r(freesurfer,"?",&saveptr);
                freesurfer2=strtok_r(NULL,"?",&saveptr);
                }
            }
        if(!(idx=malloc(sizeof*idx*hypnogram->nfiles))) {printf("fidlError: Unable to malloc idx\n");fflush(stdout);exit(-1);}
        for(i1=0;i1<hypnogram->nfiles;i1++)idx[i1]=0;
        sum=0;
        if(Awake) {sum++;for(i1=0;i1<hypnogram->nfiles;i1++)if(frperstate[i1][0]*TR>=Awake)idx[i1]++;}
        if(N1sleep) {sum++;for(i1=0;i1<hypnogram->nfiles;i1++)if(frperstate[i1][2]*TR>=N1sleep)idx[i1]++;}
        if(N2sleep) {sum++;for(i1=0;i1<hypnogram->nfiles;i1++)if(frperstate[i1][3]*TR>=N2sleep)idx[i1]++;}
        if(N3sleep) {sum++;for(i1=0;i1<hypnogram->nfiles;i1++)if(frperstate[i1][4]*TR>=N3sleep)idx[i1]++;}
        if(pathconc)sprintf(scratchf,"fidl%s.txt",make_timestr2(timestr));
        for(cnt1=i1=0;i1<hypnogram->nfiles;i1++) {
            if(idx[i1]==sum) {
                strcpy(line,hypnogram->files[i1]);
                for(ptr=line,j=0;j<5;j++)strptr=strtok_r(ptr,"/",&ptr);
                printf("%s\t%s %.1fs %s %.1fs %s %.1fs %s %.1fs\n",strptr,statestr[0],frperstate[i1][0]*TR,statestr[2],
                    frperstate[i1][2]*TR,statestr[3],frperstate[i1][3]*TR,statestr[4],frperstate[i1][4]*TR);
                if(path)fprintf(fp,"%s/%s.glm\n",path,strptr);
                if(fp2){
                    if(!pathconc)fprintf(fp2,"%s\t%s\n",pathdata,strptr);else{ 
                        fprintf(fp2,"%s/%s.conc\n",pathconc,strptr);
                        sprintf(write_back,"ls -1 %s/%s/atlas/*_anat_ave_to_SLEEP_mpr_t4 > %s\n",pathdata,strptr,scratchf);
                        printf("%s\n",write_back);
                        if(system(write_back)==-1) {
                            printf("fidlError0: Unable to %s\n",write_back);
                            exit(-1);
                            }
                        if(!(op=fopen_sub(scratchf,"r"))) exit(-1);
                        for(;fgets(write_back,sizeof(write_back),op);) fprintf(fp2,"%s",write_back);
                        fclose(op);
                        fprintf(fp2,"%s/%s_AASM_hypnogram.fidl\n",pathconc,strptr);
                        }
                    }

                if(fp3)fprintf(fp3,"%s%s%s\t%s\n",freesurfer1,strptr,freesurfer2,strptr);
                cnt1++;
                }
            }


        printf("------------------\n%d subjects %s>=%.1fs %s>=%.1fs %s>=%.1fs %s>=%.1fs\n",cnt1,statestr[0],Awake,statestr[2],N1sleep,
            statestr[3],N2sleep,statestr[4],N3sleep);
        if(path){
            fclose(fp);
            printf("Output written to %s\n",glmlist);fflush(stdout);
            }
        if(fp2){
            fclose(fp2);
            printf("Output written to %s\n",datalist);fflush(stdout);
            }
        if(fp3){
            fclose(fp3);
            printf("Output written to %s\n",splitlist);fflush(stdout);
            }

        }
    fflush(stdout);
    }

}
