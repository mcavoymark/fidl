/* Copyright 4/22/15 Washington University.  All Rights Reserved.
   read_frames_file.c  $Revision: 1.5 $*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fidl.h" 
#include "read_frames_file.h" 
#include "subs_util.h" 
FS *read_frames_file(char *filename,int lccond_and_frames,int starti,int readthismany)
{                                                                    /*0 = read all*/

    //char line[MAXNAME],write_back[MAXNAME],string[MAXNAME],*strptr,*write_back_ptr,last_char,first_char; /*dummy*/
    //START151125
    char line[10000],write_back[10000],string[10000],*strptr,*write_back_ptr,last_char,first_char; /*dummy*/

    int nstrings,i,j=0,k,l,m;
    FS *fs;
    FILE *fp;
    if(!(fp=fopen_sub(filename,"r"))) return NULL;
    if(!(fs=malloc(sizeof*fs))) {
        printf("fidlError: Unable to malloc fs\n");
        return NULL;
        }
    fs->nfactornames = fs->nfactors = 0; 
    fs->classi = fs->subjecti = fs->runi = fs->conditioni = -1;
    if(lccond_and_frames>1) {
        if(!fgets(line,sizeof(line),fp)) {
            printf("fidlError: %s is empty.\n",filename);fflush(stdout);
            return NULL;
            }

        /*if(!(fs->nfactornames=count_strings_new(line,write_back,' ',&dummy))) {*/
        /*START150423*/
        //if(!(fs->nfactornames=count_strings_new3(line,write_back,' ',' ',&last_char,&first_char,0,0))) { 
        //START151202
        if(!(fs->nfactornames=count_strings_new3(line,write_back,' ',' ',&last_char,&first_char,0,' '))) { 

            printf("fidlError: Header is missing. %s\n",filename);fflush(stdout);
            return NULL;
            }
        for(strptr=write_back,i=j=0;j<fs->nfactornames;j++) {
            strptr = grab_string_new(strptr,line,&k);
            i += k+1;
            }
        if(!(fs->factornames=malloc((size_t)(fs->nfactornames*sizeof(char*))))) {
            printf("fidlError: Unable to allocate fs->factornames\n");
            return NULL;
            }
        if(!(fs->factornames[0]=malloc((size_t)(i*sizeof(char))))) {
            printf("fidlError: Unable to allocate fs->factornames[0]\n");
            return NULL;
            }
        for(strptr=fs->factornames[0],write_back_ptr=write_back,j=0;j<fs->nfactornames;j++) {
            write_back_ptr = grab_string_new(write_back_ptr,line,&m);
            strcpy(strptr,line);
            fs->factornames[j] = strptr;
            strptr+=m+1;
            if(!strcmp("class",fs->factornames[j])) fs->classi=j;
            else if(!strcmp("subject",fs->factornames[j])) fs->subjecti=j;
            else if(!strcmp("run",fs->factornames[j])) fs->runi=j;
            else if(!strcmp("condition",fs->factornames[j])) fs->conditioni=j;
            }
        lccond_and_frames = fs->nfactors = fs->nfactornames-1;
        /*for(j=0;j<fs->nfactor;j++)printf("%s ",fs->factornames[j]);printf("\n");fflush(stdout);*/
        }
    for(fs->ntc=fs->nlines=0;fgets(line,sizeof(line),fp);) {

        //if((nstrings=count_strings_new3(line,write_back,' ',' ',&last_char,&first_char,0,0))) {
        //START151202
        if((nstrings=count_strings_new3(line,write_back,' ',' ',&last_char,&first_char,0,' '))) {

            ++fs->nlines;
            if((i=nstrings-lccond_and_frames-starti)<readthismany) {
                printf("fidlError: Line %d has %d points after starti=%d. Needs at least readthismany=%d following starti\n",
                    j,i,starti,readthismany);
                }
            else if(i>readthismany && readthismany) {
                i = readthismany;
                }   
            fs->ntc += i;
            }
        }
    /*printf("fs->nlines=%d fs->ntc=%d\n",fs->nlines,fs->ntc);fflush(stdout)*/
    if(!(fs->frames_per_line=malloc(sizeof*fs->frames_per_line*fs->nlines))) {
        printf("fidlError: Unable to malloc fs->frames_per_line\n");
        return NULL;
        }
    if(!(fs->num_frames_to_sum=malloc(sizeof*fs->num_frames_to_sum*fs->ntc))) {
        printf("fidlError: Unable to malloc fs->num_frames_to_sum\n");
        return NULL;
        }
    if(lccond_and_frames>1) {
        if(!(fs->factors=malloc(sizeof*fs->factors*fs->nlines))) {
            printf("fidlError: Unable to malloc fs->factors\n");
            return NULL;
            }
        if(!(fs->factors[0]=malloc(sizeof*fs->factors[0]*fs->nlines*fs->nfactors))) {
            printf("fidlError: Unable to malloc fs->factors[0]\n");
            return NULL;
            }
        for(i=1;i<fs->nlines;i++) fs->factors[i]=fs->factors[i-1]+fs->nfactors;
        }

    /*START121022*/
    if(!(fs->frames_cond=malloc(sizeof*fs->frames_cond*fs->nlines))) {
        printf("fidlError: Unable to malloc fs->frames_cond\n");
        return NULL;
        }

/*#if 0*/

    rewind(fp);
    if(lccond_and_frames>1) fgets(line,sizeof(line),fp);
    for(fs->nframes=i=k=0;fgets(line,sizeof(line),fp);) {

        //printf("here40 line=%sEND\n",line);

        //if((nstrings=count_strings_new3(line,write_back,' ',' ',&last_char,&first_char,0,0))) {
        //START151202
        if((nstrings=count_strings_new3(line,write_back,' ',' ',&last_char,&first_char,0,' '))){

            //printf("here41 nstrings=%dEND\n",nstrings);
            //printf("here41 line=%sEND\n",line);
            //printf("here41 write_back=%sEND\n",write_back);

            /*START150423*/
            #if 0
            l = nstrings-lccond_and_frames-starti;
            if(l>readthismany && readthismany) l = readthismany;
            #endif


            strptr = write_back;
            if(lccond_and_frames>1) {
                for(j=0;j<lccond_and_frames;j++) strings_to_int(strptr,fs->factors[i],fs->nfactors);
                nstrings -= fs->nfactors;
                }


            /*fs->frames_per_line[i] = l;*/
            /*START150423*/
            fs->frames_per_line[i] = (((l=nstrings-lccond_and_frames-starti)>readthismany)&&readthismany)?readthismany:l;


            //printf("here50 strptr=%sEND\n",strptr);

            if(lccond_and_frames) {

                /*strptr = grab_string(strptr,line);*/
                /*START150423*/
                strptr = grab_string_new(strptr,line,&l);

                //printf("here51 line=%sEND\n",line);


                /*fs->frames_cond[i] = atoi(line);*/
                /*START150423*/
                fs->frames_cond[i]=(int)strtol(line,NULL,10);

                nstrings--;
                }
            i++;


            /*printf("nstrings=%d lccond_and_frames=%d starti=%d\n",nstrings,lccond_and_frames,starti);*/
            for(m=j=0;j<nstrings;j++) {

                //printf("strptr=%sEND\n",strptr);

                //strptr = grab_string(strptr,line);
                /*START150423*/
                strptr = grab_string_new(strptr,line,&l);

                if(j>=starti) {

                    //printf("line=%sEND\n",line);

                    //fs->num_frames_to_sum[k] = count_strings_new(line,string,'+',&dummy);
                    //fs->num_frames_to_sum[k] = count_strings_new(line,string,'+',&last_char);
                    /*START150423*/
                    //fs->num_frames_to_sum[k] = count_strings_new3(line,string,'+','+',&last_char,&first_char,0,0);
                    //START151202
                    fs->num_frames_to_sum[k] = count_strings_new3(line,string,'+','+',&last_char,&first_char,0,' ');

                    //printf("fs->num_frames_to_sum[%d]=%d fs->ntc=%d\n",k,fs->num_frames_to_sum[k],fs->ntc);

                    fs->nframes += fs->num_frames_to_sum[k++];
                    if(++m==readthismany) break;
                    }
                }
            }
        }
    //printf("fs->nlines=%d fs->nframes=%d\n",fs->nlines,fs->nframes);fflush(stdout);
#if 1

    /*START121022*/
    #if 0
    if(!(fs->frames_cond=malloc(sizeof*fs->frames_cond*fs->nlines))) {
        printf("Error: Unable to malloc fs->frames_cond\n");
        return NULL;
        }
    #endif

    /*printf("here100 fs->nframes=%d\n",fs->nframes);fflush(stdout);*/


    if(!(fs->frames=malloc(sizeof*fs->frames*fs->nframes))) {
        printf("fidlError: Unable to malloc fs->frames\n");
        return NULL;
        }
    rewind(fp);
    if(lccond_and_frames>1) fgets(line,sizeof(line),fp);
    for(k=0;fgets(line,sizeof(line),fp);) {

        //if((nstrings=count_strings_new3(line,write_back,' ',' ',&last_char,&first_char,0,0))) {
        //START151202
        if((nstrings=count_strings_new3(line,write_back,' ',' ',&last_char,&first_char,0,' '))) {

            strptr = write_back;

            if(lccond_and_frames) {

                /*strptr = grab_string(strptr,line);*/
                /*START150423*/
                strptr = grab_string_new(strptr,line,&l);

                /*START121022*/
                /*fs->frames_cond[l++] = atoi(line);*/

                nstrings--;
                }
            for(m=j=0;j<nstrings;j++) {

                /*strptr = grab_string(strptr,line);*/
                /*START150423*/
                strptr = grab_string_new(strptr,line,&l);

                if(j>=starti) {

                    /*i = count_strings_new(line,string,'+',&dummy);*/
                    /*START150423*/
                    //i = count_strings_new3(line,string,'+','+',&last_char,&first_char,0,0);
                    //START151202
                    i = count_strings_new3(line,string,'+','+',&last_char,&first_char,0,' ');

                    strings_to_int(string,&fs->frames[k],i);
                    k += i;
                    if(++m==readthismany) break;
                    }
                }
            }
        }
    for(i=0;i<fs->nframes;i++) fs->frames[i]--;

#endif

    fclose(fp);
    return fs;
}
