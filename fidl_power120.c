/* Copyright 9/29/14 Washington University.  All Rights Reserved.
   fidl_power120.c  $Revision: 1.8 $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <math.h>
#include "fidl.h"
#include "subs_util.h"
int main(int argc,char **argv)
{
char *txt=NULL,*listf=NULL,line[10000],write_back[10000],*strptr,vc[MAXNAME],path[MAXNAME],fcparam[MAXNAME],filename[MAXNAME],
    line1[MAXNAME],write_back1[MAXNAME],*searchstr="set boldruns = (",last_char,first_char,*txtlpf=NULL,*concpath=NULL,
    frames[MAXNAME],*txtmot=NULL,*txtnus=NULL,*nuspath=NULL,timestr[23],*extpath=NULL,*segpath=NULL,*listptr,
    *segpathwm=NULL,*segpathcsf=NULL;
int SunOS_Linux,cnt,cnt1,len,run[100],*frame,fr,sum,wmero=4,csfero=1,i,j,N;
//size_t i,j,N;
FILE *fp,*fp1,*op;
if(argc<5) {
    fprintf(stderr,"THESE OPTIONS MAKE CONCS AND A LIST\n");
    fprintf(stderr,"  -txt:  Text file with three (or more) columns.\n");
    fprintf(stderr,"         First is the vc number.\n");
    fprintf(stderr,"         Second is the path to the data.\n");
    fprintf(stderr,"         Third is fc.params file.\n");
    fprintf(stderr,"  -list: Output. Name of list file.\n");
    fprintf(stderr,"  -concpath: Path to concs. No backslash at the end.\n");
    fprintf(stderr,"  -extpath:  Path to *.ext files. No backslash at the end.\n");
    fprintf(stderr,"  -segpathwm:  Path to white matter nusmasks. No backslash at the end.\n");
    fprintf(stderr,"  -segpathcsf: Path to ventricular nusmasks. No backslash at the end.\n");
    fprintf(stderr,"  -wmero:   Erosion level for white matter. Default is 4.\n");
    fprintf(stderr,"  -csfero:  Erosion level for csf. Default is 1.\n");
    fprintf(stderr,"THESE OPTIONS MAKE A LIST FOR EACH SET OF CONCS WITH DIFFERENT LENGTH RUNS\n");
    fprintf(stderr,"  -txtlpf: Text file with three columns.\n");
    fprintf(stderr,"           First is the vc number.\n");
    fprintf(stderr,"           Second is the path to the data.\n");
    fprintf(stderr,"           Third is fc.params file.\n");
    fprintf(stderr,"           Fourth is the number of frames.\n");
    fprintf(stderr,"           A separate list is made for each number of frames. -list is used as the root.\n");
    fprintf(stderr,"  -concpath: Path to concs. No backslash at the end.\n");
    fprintf(stderr,"             If not provided then PWD is used.\n");
    fprintf(stderr,"  -extpath:  Path to *.ext files. No backslash at the end.\n");
    fprintf(stderr,"  -list:    Output multiple lists. Name of list file. List name is appended with frame lengths.\n");
    fprintf(stderr,"  -segpath: Path to split nusmasks. No backslash at the end.\n");
    fprintf(stderr,"THESE OPTIONS MAKE MOTION CONCS AND A LIST\n");
    fprintf(stderr,"  -txtmot: Text file with three columns.\n");
    fprintf(stderr,"           First is the vc number.\n");
    fprintf(stderr,"           Second is the path to the data.\n");
    fprintf(stderr,"           Third is fc.params file.\n");
    fprintf(stderr,"  -list:   Output. Name of list file.\n");
    fprintf(stderr,"THESE OPTIONS MAKE A SHELL SCRIPT TO RESAMPLE THE NUSMASKS\n");
    fprintf(stderr,"  -txtnus:  Text file with three columns.\n");
    fprintf(stderr,"            First is the vc number.\n");
    fprintf(stderr,"            Second is the path to the data.\n");
    fprintf(stderr,"            Third is fc.params file.\n");
    fprintf(stderr,"            ONLY THE VC NUMBER IS USED.\n");
    fprintf(stderr,"  -nuspath: Path to nusmasks. No backslash at the end.\n");
    fprintf(stderr,"  -wmero:   Erosion level for white matter. Default is 4.\n");
    fprintf(stderr,"  -csfero:  Erosion level for csf. Default is 1.\n");
    exit(-1);
    }
for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-txt") && argc > i+1)
        txt=argv[++i];
    if(!strcmp(argv[i],"-extpath") && argc > i+1)
        extpath=argv[++i];
    if(!strcmp(argv[i],"-segpath") && argc > i+1)
        segpath=argv[++i];
    if(!strcmp(argv[i],"-txtlpf") && argc > i+1)
        txtlpf=argv[++i];
    if(!strcmp(argv[i],"-concpath") && argc > i+1)
        concpath=argv[++i];
    if(!strcmp(argv[i],"-txtmot") && argc > i+1)
        txtmot=argv[++i];
    if(!strcmp(argv[i],"-txtnus") && argc > i+1)
        txtnus=argv[++i];
    if(!strcmp(argv[i],"-nuspath") && argc > i+1)
        nuspath=argv[++i];
    if(!strcmp(argv[i],"-wmero") && argc > i+1)
        wmero=atoi(argv[++i]);
    if(!strcmp(argv[i],"-csfero") && argc > i+1)
        csfero=atoi(argv[++i]);
    if(!strcmp(argv[i],"-list") && argc > i+1)
        listf=argv[++i];
    if(!strcmp(argv[i],"-segpathwm") && argc > i+1)
        segpathwm=argv[++i];
    if(!strcmp(argv[i],"-segpathcsf") && argc > i+1)
        segpathcsf=argv[++i];
    }
if((SunOS_Linux=checkOS())==-1) exit(-1);
if(!concpath) {
    if(!(concpath=getenv("PWD"))) {
        printf("fidlError: Unable to get environment variable PWD\n");
        exit(-1);
        }
    }
if(!txt&&!txtlpf&&!txtmot&&!txtnus) {printf("fidlError: need to specify -txt or/and -txtlpf\n");exit(-1);fflush(stdout);}
if(txt) {
    if(!(fp=fopen_sub(txt,"r"))) exit(-1);
    if(!(op=fopen_sub(listf,"w"))) exit(-1);
    for(N=0;fgets(line,sizeof(line),fp);N++) {
        if((cnt=count_strings_new2(line,write_back,' ',' ',&last_char,&first_char,0))<3) {
            printf("fidlError: %s  line %lu has %d strings. Must have at least 3.\n",txt,(unsigned long)N+1,cnt);
            fflush(stdout);exit(-1);
            }
        strptr = grab_string_new(write_back,vc,&len);
        strptr = grab_string_new(strptr,path,&len);
        grab_string_new(strptr,fcparam,&len);
        if(!(fp1=fopen_sub(fcparam,"r"))) exit(-1);
        fgets(line1,sizeof(line1),fp1);
        if(!(strptr=strstr(line1,searchstr))) {
            printf("fidlError: %s  Needs to have: %s\n",line1,searchstr);fflush(stdout);exit(-1);
            }
        strptr += strlen(searchstr);
        if(!(cnt1=count_strings_new2(strptr,write_back1,')',')',&last_char,&first_char,0))) {
            printf("fidlError: cnt1=%d  Must be greater than zero.\n",cnt1);fflush(stdout);exit(-1);
            }
        strings_to_int(write_back1,run,cnt1);
        fclose(fp1);
        sprintf(filename,"%s.conc",vc);
        if(!(fp1=fopen_sub(filename,"w"))) exit(-1);
        for(i=0;i<cnt1;i++) fprintf(fp1,"%s/%s/bold%d/%s_b%d_xr3d.4dfp.img\n",path,vc,run[i],vc,run[i]);
        fclose(fp1);
        fprintf(op,"%s/%s\n%s/%s/atlas/%s_anat_ave_to_TRIO_KY_NDC_t4\n",concpath,filename,path,vc,vc);
        if(extpath) fprintf(op,"%s/%s_mot_motion.ext\n",extpath,vc);
        }
    fprintf(op,"\n");
    printf("N = %lu\n",(unsigned long)N);fflush(stdout);
    for(i=0;i<N;i++) fprintf(op,"/data/nil-bluearc/raichle/mcavoy/reg/split/global/glm_atlas_mask_222_LRreg.4dfp.img\n"); 
    if(segpathcsf) {
        fprintf(op,"\n");
        for(rewind(fp);fgets(line,sizeof(line),fp);) {
            cnt=count_strings_new2(line,write_back,' ',' ',&last_char,&first_char,0);
            strptr=grab_string_new(write_back,vc,&len);
            fprintf(op,"%s/%s_aparc+aseg_CSF_ero%d_mask_222_LRreg.4dfp.img\n",segpathcsf,vc,csfero);
            }
        }
    if(segpathwm) {
        fprintf(op,"\n");
        for(rewind(fp);fgets(line,sizeof(line),fp);) {
            cnt=count_strings_new2(line,write_back,' ',' ',&last_char,&first_char,0);
            strptr=grab_string_new(write_back,vc,&len);
            fprintf(op,"%s/%s_aparc+aseg_cerebralwm_ero%d_mask_222_LRreg.4dfp.img\n",segpathwm,vc,wmero);
            }
        }
    fclose(op);
    fclose(fp);
    printf("Output written to %s\n",listf);
    }
if(txtlpf) {
    if(!(listptr=get_tail_sans_ext(listf))) exit(-1);
    if(!(fp=fopen_sub(txtlpf,"r"))) exit(-1);
    for(N=0;fgets(line,sizeof(line),fp);N++) {
        if((cnt=count_strings_new2(line,write_back,' ',' ',&last_char,&first_char,0))!=4) {
            printf("here1 write_back=%s\n",write_back);fflush(stdout);
            printf("fidlError: %s  line %lu has %d strings. Must have 4.\n",txtlpf,(unsigned long)i+1,cnt);fflush(stdout);exit(-1);
            }
        }
    if(!(frame=malloc(sizeof*frame*N))) {
        printf("fidlError: Unable to malloc frame\n");
        exit(-1);
        }
    for(rewind(fp),i=0;fgets(line,sizeof(line),fp);i++) {
        cnt=count_strings_new2(line,write_back,' ',' ',&last_char,&first_char,0);
        strptr = grab_string_new(write_back,vc,&len);
        strptr = grab_string_new(strptr,path,&len);
        strptr = grab_string_new(strptr,fcparam,&len);
        grab_string_new(strptr,frames,&len);
        frame[i] = atoi(frames);
        }
    do {
        for(op=NULL,fr=0,rewind(fp),i=0;fgets(line,sizeof(line),fp);i++) {
            if(frame[i]) {
                if(!fr) {
                    sprintf(filename,"%s_%dframes.list",listptr,frame[i]);
                    if(!(op=fopen_sub(filename,"w"))) exit(-1);
                    fr = frame[i];
                    }
                if(frame[i]==fr) {
                    cnt=count_strings_new2(line,write_back,' ',' ',&last_char,&first_char,0);
                    strptr = grab_string_new(write_back,vc,&len);
                    strptr = grab_string_new(strptr,path,&len);
                    fprintf(op,"%s/%s.conc\n%s/%s/atlas/%s_anat_ave_to_TRIO_KY_NDC_t4\n",concpath,vc,path,vc,vc);
                    if(extpath) fprintf(op,"%s/%s_mot_motion.ext\n",extpath,vc);
                    fprintf(op,"/data/nil-bluearc/raichle/mcavoy/reg/split/global/glm_atlas_mask_222_LRreg.4dfp.img\n");
                    frame[i] = -1;
                    }
                }
            }
        if(segpath) {
            for(rewind(fp),i=0;fgets(line,sizeof(line),fp);i++) {
                if(frame[i]==-1) { 
                    cnt=count_strings_new2(line,write_back,' ',' ',&last_char,&first_char,0);
                    strptr = grab_string_new(write_back,vc,&len);
                    fprintf(op,"%s/%s_aparc+aseg_cerebralwm_ero4_mask_222_LRreg.4dfp.img\n",segpath,vc);
                    }
                }
            for(rewind(fp),i=0;fgets(line,sizeof(line),fp);i++) {
                if(frame[i]==-1) {
                    cnt=count_strings_new2(line,write_back,' ',' ',&last_char,&first_char,0);
                    strptr = grab_string_new(write_back,vc,&len);
                    fprintf(op,"%s/%s_aparc+aseg_CSF_ero1_mask_222_LRreg.4dfp.img\n",segpath,vc); 
                    frame[i] = 0;
                    }
                }
            }
        if(op) {
            fclose(op);
            printf("Output written to %s\n",filename);fflush(stdout);
            }
        for(sum=j=0;j<N;j++) sum+=frame[j];
        } while(sum);
    }
if(txtmot) {
    if(!(fp=fopen_sub(txtmot,"r"))) exit(-1);
    if(!(op=fopen_sub(listf,"w"))) exit(-1);
    for(;fgets(line,sizeof(line),fp);) {
        if((cnt=count_strings(line,write_back,' '))!=3) {
            printf("fidlError: %s  line %lu has %d strings. Must have 3.\n",txt,(unsigned long)i+1,cnt);fflush(stdout);exit(-1);
            }
        strptr = grab_string_new(write_back,vc,&len);
        strptr = grab_string_new(strptr,path,&len);
        grab_string_new(strptr,fcparam,&len);
        if(!(fp1=fopen_sub(fcparam,"r"))) exit(-1);
        fgets(line1,sizeof(line1),fp1);
        if(!(strptr=strstr(line1,searchstr))) {
            printf("fidlError: %s  Needs to have: %s\n",line1,searchstr);fflush(stdout);exit(-1);
            }
        strptr += strlen(searchstr);
        if(!(cnt1=count_strings_new2(strptr,write_back1,')',')',&last_char,&first_char,0))) {
            printf("fidlError: cnt1=%d  Must be greater than zero.\n",cnt1);fflush(stdout);exit(-1);
            }
        strings_to_int(write_back1,run,cnt1);
        fclose(fp1);
        sprintf(filename,"%s_mot.conc",vc);
        if(!(fp1=fopen_sub(filename,"w"))) exit(-1);
        for(i=0;i<cnt1;i++) fprintf(fp1,"%s/%s/movement/%s_b%d_xr3d.dat\n",path,vc,vc,run[i]);
        fclose(fp1);
        fprintf(op,"%s/%s\n",concpath,filename);
        }
    fclose(op);
    fclose(fp);
    printf("Output written to %s\n",listf);
    }
if(txtnus) {
    if(!(fp=fopen_sub(txtnus,"r"))) exit(-1);
    sprintf(filename,"resample_%s.csh",make_timestr2(timestr));
    if(!(op=fopen_sub(filename,"w"))) exit(-1);
    for(;fgets(line,sizeof(line),fp);) {
        if((cnt=count_strings(line,write_back,' '))!=4) {
            printf("fidlError: %s  line %lu has %d strings. Must have 4.\n",txt,(unsigned long)i+1,cnt);fflush(stdout);exit(-1);
            }
        strptr = grab_string_new(write_back,vc,&len);

        #if 0
        fprintf(op,"t4img_4dfp none %s/%s/nusmask/aparc+aseg_cerebralwm_ero4_mask_333 %s_aparc+aseg_cerebralwm_ero%d_mask_222 -O222 -n\n"
            ,nuspath,vc,vc,wmero);
        fprintf(op,"t4img_4dfp none %s/%s/nusmask/aparc+aseg_CSF_ero1_mask_333 %s_aparc+aseg_CSF_ero%d_mask_222 -O222 -n\n"
            ,nuspath,vc,vc,csfero);
        #endif
        //START160616
        fprintf(op,"t4img_4dfp none %s/%s/nusmask/aparc+aseg_cerebralwm_ero%d_mask_333 ",nuspath,vc,wmero);
        fprintf(op,"%s_aparc+aseg_cerebralwm_ero%d_mask_222 -O222 -n\n",vc,wmero);
        fprintf(op,"t4img_4dfp none %s/%s/nusmask/aparc+aseg_CSF_ero%d_mask_333 %s_aparc+aseg_CSF_ero%d_mask_222 -O222 -n\n",
            nuspath,vc,csfero,vc,csfero);

        }
    fclose(op);
    fclose(fp);
    printf("Output written to %s\n",filename);
    }
}
