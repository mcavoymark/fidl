/* Copyright 12/12/14 Washington University.  All Rights Reserved.
   fidl_IBIS.c  $Revision: 1.4 $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fidl.h>
#include <gsl/gsl_sort.h>
int main(int argc,char **argv)
{
char *dir=NULL,*txt=NULL,*list=NULL,line[MAXNAME],write_back[MAXNAME],filename[MAXNAME],line2[MAXNAME],*strptr,timestr[23],
    scratchf[31],scratchf2[33],write_back2[MAXNAME],*pwd,*movedir=NULL,*glmlist=NULL;
int ntags=0,ncol,*coli,len,frames=130;
size_t i,j;
Files_Struct *tags=NULL;
FILE *fp,*fp2,*fp3,*op=NULL,*op2;
Interfile_header *ifh=NULL;
if(argc<7) {
    fprintf(stderr,"  -dir:  Location of bold. Include backslash at the end.\n");

    /*START150109*/
    fprintf(stderr,"         OR Location of glms if using -glmlist.\n");

    fprintf(stderr,"  -txt:  Spreadsheet saved as txt that identifies subject folders. Any subject without a # is used.\n");
    fprintf(stderr,"  -tags: Labels of interest on first row of spreadsheet. Ex. -tags 6mo 12mo 24mo\n");

    fprintf(stderr,"These options make concs and a list for glm computation.\n");
    fprintf(stderr,"  -frames: BOLDS must have exactly this many frames. Default is 130.\n");
    fprintf(stderr,"  -list: Name of list file to be output.\n");
    fprintf(stderr,"  -movedir:  Location of ext head motion files. Include backslash at the end.\n");

    /*START150109*/
    fprintf(stderr,"This option makes a glm list.\n");
    fprintf(stderr,"  -glmlist: Name of list file to be output.\n");

    }
for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-dir") && argc > i+1) dir = argv[++i];
    if(!strcmp(argv[i],"-txt") && argc > i+1) txt = argv[++i];
    if(!strcmp(argv[i],"-tags") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++ntags;
        if(!(tags=get_files(ntags,&argv[i+1]))) exit(-1);
        i += ntags;
        }

    /*START150107*/
    if(!strcmp(argv[i],"-frames") && argc > i+1) frames = atoi(argv[++i]);

    if(!strcmp(argv[i],"-list") && argc > i+1) list = argv[++i];
    if(!strcmp(argv[i],"-movedir") && argc > i+1) movedir = argv[++i];

    /*START150109*/
    if(!strcmp(argv[i],"-glmlist") && argc > i+1) glmlist = argv[++i];

    }
if(list) printf("frames =%d\n",frames);fflush(stdout);
if(!(coli=malloc(sizeof*coli*tags->nfiles))) {
    printf("fidlError: Unable to malloc coli\n");
    exit(-1);
    }
if(!(pwd=getenv("PWD"))) {
    printf("fidlError: Unable to get environment variable PWD\n");
    exit(-1);
    }
if(!(fp=fopen_sub(txt,"r"))) exit(-1);
do{fgets(line,sizeof(line),fp);}while(line[0]=='#');
if((ncol = count_tab(line,write_back,0)) < 1) {
    printf("List the column headers on the first line of %s.\n",txt);
    exit(-1);
    }
for(i=0;i<tags->nfiles;i++) {
    for(strptr=write_back,j=0;j<ncol;j++) { 
        if(!strcmp(strptr,tags->files[i])) {
            coli[i]=j;
            fflush(stdout);
            break;
            }
        strptr += strlen(strptr)+1;
        }
    }
gsl_sort_int(coli,1,tags->nfiles);
if(list) {
    sprintf(scratchf,"fidl%s.txt",make_timestr2(timestr));
    sprintf(scratchf2,"fidl%s_2.txt",timestr);
    if(!(op=fopen_sub(list,"w"))) exit(-1);
    }

/*START150109*/
if(glmlist) {
    if(!(op=fopen_sub(glmlist,"w"))) exit(-1);
    }

for(;fgets(line,sizeof(line),fp);) {
    if(line[0]=='#') continue;
    if((count_tab(line,write_back,0))) {
        for(strptr=write_back,j=i=0;i<ncol;i++) {
            if(i==coli[j]) {
                if(list) {
                    sprintf(line,"ls -1 %s%s > %s\n",dir,strptr,scratchf);
                    if(system(line)==-1) {
                        printf("fidlError0: Unable to %s\n",line);
                        exit(-1);
                        }
                    if(!(fp2=fopen_sub(scratchf,"r"))) exit(-1);
                    sprintf(filename,"%s.conc",strptr);
                    for(op2=NULL;fgets(line,sizeof(line),fp2);) {
                        if(strstr(line,"bold")) {
                            grab_string_new(line,write_back2,&len);
                            sprintf(line2,"ls -1 %s%s/%s/*xr3d_norm.4dfp.img > %s",dir,strptr,write_back2,scratchf2);
                            if(system(line2)==-1) {
                                printf("fidlError: Unable to %s\n",line2);
                                exit(-1);
                                }
                            if(!(fp3=fopen_sub(scratchf2,"r"))) exit(-1);
                            fgets(line2,sizeof(line2),fp3);
                            grab_string_new(line2,write_back2,&len);
                            if(!(ifh=read_ifh(write_back2,ifh))) exit(-1);
                            if(ifh->orientation!=(int)TRANSVERSE||ifh->dim4!=frames) {
                                if(ifh->orientation!=(int)TRANSVERSE) 
                                    printf("    ifh->orientation=%d not in transverse plane\n",ifh->orientation);
                                if(ifh->dim4!=frames) printf("    ifh->dim4=%d not equal to %d\n",ifh->dim4,frames);
                                }
                            else {
                                if(!op2) if(!(op2=fopen_sub(filename,"w"))) exit(-1);
                                fprintf(op2,"%s",line2);
                                }
                            free_ifh(ifh,1);
                            fclose(fp3);
                            }
                        }
                    fclose(fp2);
                    if(op2) {
                        fclose(op2);
                        printf("Output written to %s\n",filename);fflush(stdout);
                        sprintf(line,"ls -1 %s%s/atlas/*anat_ave_to_711-2N_t4 > %s",dir,strptr,scratchf2);
                        if(system(line)==-1) {
                            printf("fidlError: Unable to %s\n",line);
                            exit(-1);
                            }
                        if(!(fp2=fopen_sub(scratchf2,"r"))) exit(-1);
                        fgets(line,sizeof(line),fp2);
                        fclose(fp2);
                        fprintf(op,"%s/%s\n%s",pwd,filename,line);
                        if(movedir) fprintf(op,"%s%s_motion.ext\n",movedir,strptr);
                        }
                    }

                /*START150109*/
                if(glmlist) {
                    if(!i) { 
                        if(strcmp(strptr,"Control")) break;
                        }
                    else {
                        *(strptr+strlen(strptr))=0;
                        fprintf(op,"%s%s.glm\n",dir,strptr);
                        }

                    }

                if(++j==tags->nfiles) break;
                }
            strptr += strlen(strptr)+1;
            }
        }
    }
fclose(fp);
if(list) {
    fclose(op);
    printf("Output written to %s\n",list);
    sprintf(line,"rm %s %s",scratchf,scratchf2);
    if(system(line)==-1) {
        printf("fidlError: Unable to %s\n",line);
        exit(-1);
        }
    }

/*START150109*/
if(glmlist) {
    fclose(op);
    printf("Output written to %s\n",glmlist);
    }


fflush(stdout);
}
