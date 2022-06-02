/* Copyright 8/29/05 Washington University.  All Rights Reserved.
   maxminimgconc.c  $Revision: 1.7 $*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>
short _maxminimgconc(int argc,char **argv)
{
char *filename;
float *max,*min;
filename = (char*)argv[0];
max = (float*)argv[1];
min = (float*)argv[2];
if(!(maxminimgconc(filename,max,min))) return 0; 
return 1;    
}
int maxminimgconc(char *filename,float *max,float *min)
{
    char *strptr;
    int i,size,SunOS_Linux;
    Files_Struct *fi;
    Dim_Param *dp;
    Memory_Map *mm;
    if((SunOS_Linux=checkOS())==-1) return 0;
    strptr = strrchr(filename,'.');
    if(!strcmp(strptr,".conc")) {

        /*if(!(fi=read_conc(filename))) return 0;*/
        /*START140716*/
        if(!(fi=read_conc(1,&filename))) return 0;

        }
    else if(!strcmp(strptr,".img")){
        if(!(fi=get_files(1,&filename))) exit(-1);
        }
    else {
        return 0;
        }
    if(!(dp=dim_param(fi->nfiles,fi->files,SunOS_Linux,0))) return 0;
    min_and_max_init(min,max);
    for(i=0;i<fi->nfiles;i++) {
        size = dp->vol*dp->tdim[i];
        if(!(mm=map_disk(fi->files[i],size,0,sizeof(float)))) return 0;
        min_and_max_floatstack(mm->ptr,size,min,max);
        if(!unmap_disk(mm)) return 0;
        }
    free_dim_param(dp);
    free_files_struct(fi);
    return 1;
}    
