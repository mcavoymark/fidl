/* Copyright 1/24/19 Washington University.  All Rights Reserved.
   getxyz.c  $Revision: 1.1 $*/
#include <string.h>
#include "getxyz.h"

#if 1
void getxyz(int n,double *col,double *row,double *slice,int cf_flip,int ydim,float *xyz,char *atlas){
    int i,j;
    if(cf_flip==12)
        for(j=i=0;i<n;i++)
            {xyz[j++]=(float)col[i]-1.;xyz[j++]=(float)(ydim-1-row[i]);xyz[j++]=(float)slice[i];}
    else if(cf_flip==412){
        if(!strcmp(atlas,"222MNI"))
            for(j=i=0;i<n;i++){xyz[j++]=(float)col[i]+2.;xyz[j++]=(float)(ydim-1-row[i]);xyz[j++]=(float)slice[i];}
        else
            for(j=i=0;i<n;i++){xyz[j++]=(float)col[i]+1.;xyz[j++]=(float)(ydim-1-row[i]);xyz[j++]=(float)slice[i];}
        }
    else if(cf_flip==52)
        for(j=i=0;i<n;i++)
            {xyz[j++]=(float)col[i]-1.;xyz[j++]=(float)row[i];xyz[j++]=(float)slice[i];}
    else
        for(j=i=0;i<n;i++)
            {xyz[j++]=(float)col[i]-1.;xyz[j++]=(float)row[i];xyz[j++]=(float)slice[i];}
    }
#endif
#if 0
//START190128
void getxyz(int n,double *col,double *row,double *slice,int cf_flip,int ydim,float *xyz,char *atlas){
    int i,j;
    if(cf_flip==12)

        #if 0
        for(j=i=0;i<n;i++)
            {xyz[j++]=(float)col[i]-1.;xyz[j++]=(float)(ydim-1-row[i]);xyz[j++]=(float)slice[i];}
        #endif
        //START190128
        if(strstr(atlas,"222"))
            for(j=i=0;i<n;i++){xyz[j++]=(float)col[i];xyz[j++]=(float)(ydim-1-row[i]);xyz[j++]=(float)slice[i];}
        else
            for(j=i=0;i<n;i++){xyz[j++]=(float)col[i]-1.;xyz[j++]=(float)(ydim-1-row[i]);xyz[j++]=(float)slice[i];}


    else if(cf_flip==412){

        //if(!strcmp(atlas,"222MNI"))
        //START190128
        if(strstr(atlas,"222"))

            for(j=i=0;i<n;i++){xyz[j++]=(float)col[i]+2.;xyz[j++]=(float)(ydim-1-row[i]);xyz[j++]=(float)slice[i];}
        else
            for(j=i=0;i<n;i++){xyz[j++]=(float)col[i]+1.;xyz[j++]=(float)(ydim-1-row[i]);xyz[j++]=(float)slice[i];}
        }
    else if(cf_flip==52)
        for(j=i=0;i<n;i++)
            {xyz[j++]=(float)col[i]-1.;xyz[j++]=(float)row[i];xyz[j++]=(float)slice[i];}
    else
        for(j=i=0;i<n;i++)
            {xyz[j++]=(float)col[i]-1.;xyz[j++]=(float)row[i];xyz[j++]=(float)slice[i];}
    }
#endif
