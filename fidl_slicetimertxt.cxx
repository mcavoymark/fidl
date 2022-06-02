/* Copyright 1/22/19 Washington University.  All Rights Reserved.
   fidl_slicetimertxt.cxx  $Revision: 1.1 $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fidl.h"
#include "bids.h"

int main(int argc,char **argv)
{
char *json=NULL,string[MAXNAME],*strptr;
int i;
bids b0;

if(argc<2){
    printf("    -json: BIDS json file\n");
    printf("Output is written as <json root>_SliceTiming.txt to the same directory as json file.\n");
    exit(-1);
    }
for(i=1;i<argc;i++){
    if(!strcmp(argv[i],"-json") && argc > i+1){
        json = argv[++i];
        }
    }
if(!json){
    printf("fidlError: Need to specify -json\n");
    exit(-1);
    }

#if 0
if(!b0.bids0(json))exit(-1);
strcpy(string,json);
if(!(strptr=get_tail_sans_ext(string)))exit(-1);
strcat(string,"_SliceTiming.txt");
if(!b0.bidsSliceTimingtxt(string))exit(-1);
printf("%s SliceTiming written to %s\n",json,string);fflush(stdout);
#endif
//START190205
strcpy(string,json);
strptr=strrchr(string,'.');
if(strcmp(strptr,".json")){*strptr=0;strcat(string,".json");}
if(!b0.bids0(string))exit(-1);
strptr=strrchr(string,'.');
*strptr=0;strcat(string,"_SliceTiming.txt");
if(!b0.bidsSliceTimingtxt(string))exit(-1);
printf("%s\n",string);fflush(stdout);

exit(0);
}
