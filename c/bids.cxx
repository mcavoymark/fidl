/* Copyright 1/22/19 Washington University.  All Rights Reserved.
   bids.cxx  $Revision: 1.1 $*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "bids.h"
#include "constants.h"
#include "subs_util.h"
#include "strutil.h"

int bids::bids0(char* file0){ 
    char line[MAXNAME],junk[MAXNAME],last_char,first_char,*strptr;
    int i,len,cnt=0,cnt0;
    FILE *fp;    
    if(!(fp=fopen_sub((file=file0),"r")))return 0;
    for(cnt0=0;fgets(line,sizeof(line),fp)&&cnt0<2;){
        strptr=get_line_wnull(line,junk,":",&len);
        if(!len) continue;
        if(!strcmp(strptr,"\"RepetitionTime\"")){
            TR=strtod(junk,NULL);
            cnt0++;
            }
        else if(!strcmp(strptr,"\"SliceTiming\"")){
            long pos = ftell(fp); 
            for(ntimes=0;fgets(line,sizeof(line),fp);){
                if(!(cnt=count_strings_new3(line,junk,' ',' ',&last_char,&first_char,0,0))){
                    printf("fidlError: Empty field in SliceTiming\n");fflush(stdout);return 0;
                    }
                ntimes++;
                if(cnt==2)break;
                }
            times=new double[ntimes]; 
            if(fseek(fp,pos,(int)SEEK_SET))return 0;
            for(i=0;i<ntimes;i++){
                fgets(line,sizeof(line),fp);
                count_strings_new3(line,junk,' ',' ',&last_char,&first_char,0,0);
                times[i]=strtod(junk,NULL);
                } 
            cnt0++;
            }
        }
    if(cnt!=2){
        printf("fidlError: Either \"RepetitionTime\" or \"SliceTiming\" not found\n");
        return 0;
        }
    for(i=0;i<ntimes;i++){

        #if 0
        //printf("times[%d]=%f ",i,times[i]);
        times[i]/=TR;
        //printf("TR=%f times[%d]/TR=%f\n",TR,i,times[i]);
        if(times[i]<0.||times[i]>=1.){
            printf("fidlError: times[%d]=%f Must be between 0 and 1.\n",i,times[i]);fflush(stdout);return 0;
            }
        #endif
        //START190122
        //printf("times[%d]=%f ",i,times[i]);
        times[i]=.5-times[i]/TR; 
        //printf("TR=%f .5-times[%d]/TR=%f\n",TR,i,times[i]);
        if(fabs(times[i])>0.5){
            printf("fidlError: times[%d]=%f Must be between -0.5 and 0.5\n",i,times[i]);
            fflush(stdout);return 0;
            }


        }
    fclose(fp);
    return 1;
    }

int bids::bidsSliceTimingtxt(char *file){
    FILE *fp;    
    if(!(fp=fopen_sub(file,"w")))return 0;
    for(int i=0;i<ntimes;i++)fprintf(fp,"%f\n",times[i]);
    fclose(fp);
    return 1;
    }
