/* Copyright 3/10/17 Washington University.  All Rights Reserved.
   fidl_chloe.c  $Revision: 1.3 $ */
#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fidl.h"
#include "subs_util.h"
int main(int argc,char **argv){
char *txt=NULL,*boldpath=NULL,*bold=NULL,*concpath=NULL,line[MAXNAME],*strptr,*saveptr=NULL,*saveptr2,write_back[MAXNAME],
    filename[MAXNAME],line2[MAXNAME],*list=NULL,scratchf[31],timestr[23],*txt_diagnosis=NULL, //*saveptr3
    *lists[]={"HighRiskPositive.list","HighRiskNegative.list","HighRiskUnknown.list","LowRiskPositive.list","LowRiskNegative.list",
        "LowRiskUnknown.list"},
    *diagnosis[]={"High Risk Positive","High Risk Negative","High Risk Unknown","Low Risk Positive","Low Risk Negative",
        "Low Risk Unknown"};
int i,j,ii,SunOS_Linux,useit,frames=130,count[]={0,0,0,0,0,0},nlist_demographics=0,argc_glmf=0;  //lcdemographics=0;
size_t i1,j1;
Interfile_header *ifh=NULL;
FILE *fp=NULL,*fp2,*op=NULL,*op1[]={NULL,NULL,NULL,NULL,NULL,NULL};
Files_Struct *glmf=NULL;
if(argc<3){
    printf("THESE OPTIONS MAKE CONCS AND A LIST THAT INCLUDES CONCS AND T4S\n");
    printf("  -txt:      Text file with four (or more) columns.\n");
    printf("             An example is /data/pruett/CPD/Users/mcavoy/subject_status_AVIlist_170301.txt\n");
    printf("  -boldpath: Where to find bold directories. Ex. /data/pruett/CPD/Users/mcavoy/NOMODE1000/bold\n");
    printf("  -bold:     Suffix for bold files. Ex. xr3d.4dfp.img\n");
    printf("  -concpath: Where to write conc. Ex. conc\n");
    printf("  -frames:   BOLDS no longer need to have exactly this many frames. Default is 130. Good to know which glms to check.\n");
    printf("  -list:     Name of list file to be created.\n");
    printf("THESE OPTIONS MAKE LISTS BY DIAGNOSIS\n");
    printf("  -txt:           Text file with four (or more) columns.\n");
    printf("                  An example is /data/pruett/CPD/Users/mcavoy/subject_status_AVIlist_170301.txt\n");
    printf("  -txt_diagnosis: Text file with three (or more) columns.\n");
    printf("                  An example is /data/pruett/CPD/Users/mcavoy/subject_status_AVIlist_170301_170320.txt\n");
    printf("                  col1: subject identifiers col2: subject identifiers col3: diagnosis\n");
    printf("                  Diagnoses: High Risk Postive, Negative, Unknown; Low Risk Positive, Negative, Unknown\n");
    printf("THESE OPTIONS READ A LIST AND REPORT SCAN DEMOGRAPHICS\n");
    printf("  -list_demographics:        Name of list file to be created.\n");
    exit(-1);
    }
for(i=1;i<argc;i++){
    if(!strcmp(argv[i],"-txt") && argc > i+1)txt=argv[++i];
    if(!strcmp(argv[i],"-boldpath") && argc > i+1)boldpath=argv[++i];
    if(!strcmp(argv[i],"-bold") && argc > i+1)bold=argv[++i];
    if(!strcmp(argv[i],"-concpath") && argc > i+1)concpath=argv[++i];
    if(!strcmp(argv[i],"-frames") && argc > i+1)frames=atoi(argv[++i]);
    if(!strcmp(argv[i],"-list") && argc > i+1)list=argv[++i];
    if(!strcmp(argv[i],"-txt_diagnosis") && argc > i+1)txt_diagnosis=argv[++i];
    if(!strcmp(argv[i],"-list_demographics") && argc > i+1){
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++)++nlist_demographics;
        if(!(glmf=read_files(nlist_demographics,&argv[(argc_glmf=i+1)])))exit(-1);
        i+=nlist_demographics;
        }
    }
if((SunOS_Linux=checkOS())==-1)exit(-1);
if(nlist_demographics){

    //START170523
    if(nlist_demographics==1){
        strcpy(line,argv[argc_glmf]);
        if(!(strptr=get_tail_sans_ext(line)))exit(-1); 
        sprintf(filename,"%s_3scans.list",strptr);
        if(!(fp=fopen_sub(filename,"w")))exit(-1);
        } 

    for(write_back[0]=i=i1=0;i1<glmf->nfiles;i1++){
        if(!(fp2=fopen_sub(glmf->files[i1],"r"))){
            count[3]++;
            }
        else{
            count[4]++;
            fclose(fp2);
            strptr=strcpy(line,glmf->files[i1]);
            saveptr=strtok_r(strptr,"V",&strptr);
            saveptr2=strtok_r(strptr,"_",&strptr);
            //printf("saveptr=%s  saveptr2=%s\n",saveptr,saveptr2);
            if(write_back[0]){
                if(!strcmp(saveptr,write_back)){
                    i++;
                    }
                else{
                    count[i]++;
                    i=0;
                    }
                }
            strcpy(write_back,saveptr);
            //printf("    i=%d\n",i); 
            }
        
        //START170523
        if(i==2){
            //printf("%s\n",saveptr);
            if(nlist_demographics==1){
                for(j1=i1-2,j=0;j<3;j++,j1++)fprintf(fp,"%s\n",glmf->files[j1]);
                }
            }

        }
    count[i]++;

    //START170523
    if(nlist_demographics==1){
        fclose(fp);
        printf("GLMs with 3 scans written to %s\n",filename);
        }


    for(i=0;i<nlist_demographics;i++)printf("%s\n",argv[argc_glmf+i]);
    printf("    %zd glms listed: %d exist and %d do not exist\n",glmf->nfiles,count[4],count[3]);
    printf("Number of subjects with 1 scan  = %d  %f%%\n",count[0],(double)count[0]/(double)(count[0]+count[1]+count[2])*100.);
    printf("Number of subjects with 2 scans = %d  %f%%\n",count[1],(double)count[1]/(double)(count[0]+count[1]+count[2])*100.);
    printf("Number of subjects with 3 scans = %d  %f%%\n",count[2],(double)count[2]/(double)(count[0]+count[1]+count[2])*100.);
    printf("Total number of subjects        = %d\n\n",count[0]+count[1]+count[2]);
    }
else if(!txt_diagnosis){



    if(!txt){printf("fidlError: need to specify -txt\n");exit(-1);}
    if(!(fp=fopen_sub(txt,"r")))exit(-1);
    sprintf(scratchf,"fidl%s.txt",make_timestr2(timestr));
    if(list)if(!(op=fopen_sub(list,"w")))exit(-1);
    for(fgets(line,sizeof(line),fp);fgets(line,sizeof(line),fp);){
        strptr=line;strtok_r(strptr,"\t",&strptr);strtok_r(strptr,"\t",&strptr);
        saveptr=strtok_r(strptr,"\t",&strptr);
        printf("saveptr= %s\t",saveptr);
        useit=0;
        if(strcmp(saveptr,"pruett lab not using")){
            saveptr2=strtok_r(strptr,"\t",&strptr);
            if(!strcmp(saveptr2,"locked in")){
                useit=1;
                }
            else if(!strcmp(saveptr2,"provisional")){

                //START170403
                //saveptr3=strtok_r(strptr,"\t",&strptr);

                if(!strcmp(saveptr,"PHI0189_214027_V24_4p0")){
                    useit=1;
                    }
                }
            printf("%s\t",saveptr2);
            }
        printf("useit=%d\n",useit);
        if(useit){
            sprintf(filename,"%s/%s.conc",concpath,saveptr);
            sprintf(write_back,"ls -1 %s/%s/bold*/*%s > %s\n",boldpath,saveptr,bold,filename);
            if((ii=system(write_back))==-1) {
                printf("fidlError: Unable to %s\n",write_back);
                exit(-1);
                }
            printf("write_back=%s  ii=%d\n",write_back,ii);
            if(ii!=512){
                if(!(fp2=fopen_sub(filename,"r")))exit(-1);
                for(;fgets(line2,sizeof(line2),fp2);){
                    strptr=line2;saveptr2=strtok_r(strptr," \t\n",&strptr);
                    printf("saveptr2=%s\n",saveptr2);
                    if(!strcmp(saveptr2,"ls: No match.")){
                        fclose(fp2);
                        sprintf(write_back,"rm %s\n",filename);
                        if(system(write_back)==-1) {
                            printf("fidlError: Unable to %s\n",write_back);
                            exit(-1);
                            }
                        }
                    else{
                        if(!(ifh=read_ifh(line2,ifh)))exit(-1);
                        if(ifh->orientation!=(int)TRANSVERSE)printf("    ifh->orientation=%d not in transverse plane\n",
                            ifh->orientation);
                        if(ifh->dim4!=frames) printf("    ifh->dim4=%d not equal to %d\n",ifh->dim4,frames);
                        free_ifh(ifh,1);
                        fprintf(fp2,"%s\n",saveptr2);
                        }
                    }
                fclose(fp2);
                printf("Conc written to %s\n",filename);
                if(list){
                    sprintf(write_back,"ls -1 %s/%s/atlas/%s_anat_ave_to_711-2N_t4 > %s\n",boldpath,saveptr,saveptr,scratchf);
                    if((ii=system(write_back))==-1) {
                        printf("fidlError: Unable to %s\n",write_back);
                        exit(-1);
                        }
                    printf("write_back=%s  ii=%d\n",write_back,ii);
                    if(ii!=512){
                        fprintf(op,"%s\n",filename);
                        if(!(fp2=fopen_sub(scratchf,"r"))) exit(-1);
                        for(;fgets(line2,sizeof(line2),fp2);)fprintf(op,"%s",line2);
                        fclose(fp2);
                        }
                    }
                }
            }
        }
    fclose(fp);
    if(list){
        fclose(op);
        printf("List written to %s\n",list);
        }
    sprintf(write_back,"rm -f %s\n",scratchf);
    if(system(write_back)==-1) {
        printf("fidlError: Unable to %s\n",write_back);
        exit(-1);
        }
    }
else{ 

    #if 0
    op=fopen_sub("HighRiskPositive.list","w")))exit(-1);
    op1=fopen_sub("HighRiskNegative.list","w")))exit(-1);
    op2=fopen_sub("HighRiskUnknown.list","w")))exit(-1);
    op3=fopen_sub("LowRiskPositive.list","w")))exit(-1);
    op4=fopen_sub("LowRiskNegative.list","w")))exit(-1);
    op5=fopen_sub("LowRiskUnknown.list","w")))exit(-1);
    #endif
    #if 0
    op=fopen_sub(lists[0],"w")))exit(-1);
    op1=fopen_sub(lists[1],"w")))exit(-1);
    op2=fopen_sub(lists[2],"w")))exit(-1);
    op3=fopen_sub(lists[3],"w")))exit(-1);
    op4=fopen_sub(lists[4],"w")))exit(-1);
    op5=fopen_sub(lists[5],"w")))exit(-1);
    #endif
    for(i=0;i<6;i++)if(!(op1[i]=fopen_sub(lists[i],"w")))exit(-1);


    if(!(fp=fopen_sub(txt_diagnosis,"r")))exit(-1);

    //fgets(line,sizeof(line),fp);
    //for(hipos=hineg=hiun=lowpos=lowneg=lowun=0;fgets(line,sizeof(line),fp);){
    for(fgets(line,sizeof(line),fp);fgets(line,sizeof(line),fp);){

        strptr=line;
        saveptr=strtok_r(strptr,"\t",&strptr);
        //printf("saveptr=%s\n",saveptr);
        strtok_r(strptr,"\t",&strptr); 
        saveptr2=strtok_r(strptr,"\t\n\r",&strptr); 

        #if 0
        if(!strcmp("High Risk Positive",saveptr2)){fprintf(op,"s\n",saveptr);hipos++;}
        else if(!strcmp("High Risk Negative",saveptr2)){fprintf(op1,"s\n",saveptr);hineg++;}
        else if(!strcmp("High Risk Unknown",saveptr2)){fprintf(op2,"s\n",saveptr);hiun++;}
        else if(!strcmp("Low Risk Positive",saveptr2)){fprintf(op3,"s\n",saveptr);lowpos++;}
        else if(!strcmp("Low Risk Negative",saveptr2)){fprintf(op4,"s\n",saveptr);lowneg++;}
        else if(!strcmp("Low Risk Unknown",saveptr2)){fprintf(op5,"s\n",saveptr);lowun++;}
        #endif
        #if 0
        if(!strcmp("High Risk Positive",saveptr2)){fprintf(op,"s\n",saveptr);count[0]++;}
        else if(!strcmp("High Risk Negative",saveptr2)){fprintf(op1,"s\n",saveptr);count[1]++;}
        else if(!strcmp("High Risk Unknown",saveptr2)){fprintf(op2,"s\n",saveptr);count[2]++;}
        else if(!strcmp("Low Risk Positive",saveptr2)){fprintf(op3,"s\n",saveptr);count[3]++;}
        else if(!strcmp("Low Risk Negative",saveptr2)){fprintf(op4,"s\n",saveptr);count[4]++;}
        else if(!strcmp("Low Risk Unknown",saveptr2)){fprintf(op5,"s\n",saveptr);count[5]++;}
        #endif
        //for(i=0;i<6;i++)if(!strcmp(diagnosis[i],saveptr2)){fprintf(op1[i],"%s.glm\n",saveptr);count[i]++;break;}
        for(i=0;i<6;i++){
            printf("diagnosis[%d]=%sEND saveptr2=%sEND\n",i,diagnosis[i],saveptr2);
            if(!strcmp(diagnosis[i],saveptr2)){fprintf(op1[i],"%s.glm\n",saveptr);count[i]++;break;}
            }

        }

    //fclose(op);fclose(op1);fclose(op2);fclose(op3);fclose(op4);fclose(op5);fclose(fp);
    fclose(fp);
    for(i=0;i<6;i++){fclose(op1[i]);printf("List written to %s %d\n",lists[i],count[i]);}

    }
    
}
