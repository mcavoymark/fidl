/* Copyright 11/19/18 Washington University.  All Rights Reserved.
   fidl_Events82.c  $Revision: 1.1 $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "read_data.h"
#include "d2double.h"
#include "fidl.h"
#include "subs_util.h"

int main(int argc,char **argv){
char *eventfile=NULL,string[MAXNAME],*strptr;
int i,j,l,m,*i0,*eliminate;
double TR=2.,current=0; 
Data *data;
FILE *fp;
if(argc<3){
    printf("-eventfile: Event file to be edited. Output is <event file root>_edit.fidl\n");
    exit(-1);
    }
for(i=1;i<argc;i++){
    if(!strcmp(argv[i],"-eventfile") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])eventfile=argv[++i];
    }

if(!eventfile){printf("fidlError: Need to specify -eventfile\n");exit(-1);}

if(!(data=read_data(eventfile,0,1,3,0)))exit(-1);

printf("data->nsubjects=%d data->npoints=%d\n",data->nsubjects,data->npoints);fflush(stdout);

for(i=0;i<data->nsubjects;i++){
    for(j=0;j<data->npoints;j++)printf("%g\t",data->x[i][j]);
    printf("\n");
    }

if(!(i0=malloc(sizeof*i0*data->nsubjects))){printf("fidlError: Unable to malloc i0\n");exit(-1);}
if(!(eliminate=malloc(sizeof*eliminate*data->nsubjects))){printf("fidlError: Unable to malloc eliminate\n");exit(-1);}

for(m=j=0;j<3;j++){
    for(l=0;l<data->nsubjects;l++)if((int)data->x[l][1]==j){current=data->x[l][0];break;}
    i0[m++]=l;
    for(i=++l;i<data->nsubjects;i++){
        if((int)data->x[i][1]==j){
            if(fabs(data->x[i][0]-current)>TR){
                current=data->x[i][0];
                i0[m++]=i;
                }
            }
        }
    }
printf("\nNEW m=%d\n---\n",m);
for(i=0;i<m;i++){
    for(j=0;j<data->npoints;j++)printf("%g\t",data->x[i0[i]][j]);
    printf("\n");
    }

for(j=0;j<m;j++){
    if((int)data->x[i0[j]][1]==1){
        for(i=0;i<m;i++){
            if((int)data->x[i0[i]][1]==2){
                if(fabs(data->x[i0[i]][0]-data->x[i0[j]][0])<TR){
                    eliminate[i0[j]]=1;
                    break;
                    }
                }
            }
        }
    }

printf("\nNEW2 m=%d\n---\n",m);
for(i=0;i<m;i++){
    //if(!eliminate[i0[i]])for(j=0;j<data->npoints;j++)printf("%g\t",data->x[i0[i]][j]);
    if(!eliminate[i0[i]])printf("%.3f %g %g",data->x[i0[i]][0],data->x[i0[i]][1],data->x[i0[i]][2]);
    printf("\n");
    }

strcpy(string,eventfile);
if(!(strptr=get_tail_sans_ext(string)))exit(-1);
strcat(strptr,"_edit.fidl");

if(!(fp=fopen_sub(strptr,"w")))exit(-1);
fprintf(fp,"2.0 Movie Fine Coarse\n");
for(i=0;i<m;i++)if(!eliminate[i0[i]])fprintf(fp,"%.3f %g %g\n",data->x[i0[i]][0],data->x[i0[i]][1],data->x[i0[i]][2]);
fclose(fp);
}
