/* Copyright 11/24/14 Washington University. All Rights Reserved.
   timesframesoffsets.c  $Revision: 1.5 $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#ifndef __sun__
    #include <stdint.h>
#endif
#include "fidl.h"
#include "read_data.h"
#include "subs_util.h"
typedef struct {
    char *filename;
    int NR,*frames,*frames_halfTR,*frames_quarterTR,*offsets_halfTR,*offsets_quarterTR;
    float *times,*fraction; 
    } Timesframesoffsets;
int timesframesoffsets(Timesframesoffsets *tfo)
{
    char line[10000],write_back[10000],last_char,first_char;
    int cnt,i;
    //size_t i;
    double TR,td,ftd;
    FILE *fp;
    Data *data;
    if(!(fp=fopen_sub(tfo->filename,"r"))) return 0;
    fgets(line,sizeof(line),fp);
    if((cnt=count_strings_new3(line,write_back,' ',' ',&last_char,&first_char,0,0))<2) {
        printf("fidlError: cnt=%d  Must be at least two.\n",cnt);fflush(stdout);return 0;
        }
    TR=strtod(write_back,NULL);
    /*printf("TR=%.20f\n",TR);*/

    fclose(fp);
    if(!(data=read_data(tfo->filename,0,1,1,0)))exit(-1);
    if(data->nsubjects!=tfo->NR) {
        printf("fidlError: data->nsubjects=%d tfo->NR=%d Must be equal.\n",data->nsubjects,tfo->NR);fflush(stdout);return 0;
        }
    for(i=0;i<data->nsubjects;i++) {
        tfo->times[i] = (float)data->x[i][0];
        td = data->x[i][0]/TR;
        tfo->frames[i] = (int)rint(td);
        ftd = floor(td);
        tfo->frames_halfTR[i]=tfo->frames_quarterTR[i]=(int)ftd;
        tfo->offsets_halfTR[i]=tfo->offsets_quarterTR[i]=0;
        tfo->fraction[i] = (float)(td - ftd);

        /*printf("tfo->frames[%d]=%d td=%.16f ftd=%.16f tfo->fraction[%d]=%.16f\n",i,tfo->frames[i],td,ftd,i,tfo->fraction[i]);
        fflush(stdout);*/

        if(tfo->fraction[i]<.25) {
            /* do nothing */
            }
        else if(tfo->fraction[i]<.75) {
            tfo->offsets_halfTR[i]=1;
            tfo->frames_halfTR[i]+=1;
            }
        else {
            tfo->frames_halfTR[i]+=1;
            }
        if(tfo->fraction[i]<.125) {
            /* do nothing */
            }
        else if(tfo->fraction[i]<.375) {
            tfo->offsets_quarterTR[i] = 3;
            tfo->frames_quarterTR[i]+=1;
            }
        else if(tfo->fraction[i]<.625) { 
            tfo->offsets_quarterTR[i] = 2;
            tfo->frames_quarterTR[i]+=1;
            }
        else if(tfo->fraction[i]<.875) {
            tfo->offsets_quarterTR[i] = 1;
            tfo->frames_quarterTR[i]+=1;
            }
        else {
            tfo->frames_quarterTR[i]+=1;
            }

        /*START141125*/
        if(tfo->fraction[i]==1.) tfo->fraction[i]=0.;
        /*printf("    tfo->fraction[%d]=%.16f\n",i,tfo->fraction[i]);fflush(stdout);*/

        }
    free_data(data);
    return 1;
}
int _timesframesoffsets(int argc,char **argv)
{
    Timesframesoffsets tfo;
    tfo.filename = (char*)argv[0];
    #ifdef __sun__
        tfo.NR = (int)argv[1];
    #else
        tfo.NR = (intptr_t)argv[1];
    #endif
    tfo.times = (float*)argv[2];
    tfo.fraction = (float*)argv[3];
    tfo.frames = (int*)argv[4];
    tfo.frames_halfTR = (int*)argv[5];
    tfo.frames_quarterTR = (int*)argv[6];
    tfo.offsets_halfTR = (int*)argv[7];
    tfo.offsets_quarterTR = (int*)argv[8];
    if(!timesframesoffsets(&tfo)) {printf("fidlError: Error in timesframesoffsets\n");fflush(stdout);return 0;}
    return 1;
}
