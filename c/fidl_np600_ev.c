/* Copyright 8/18/04 Washington University.  All Rights Reserved.
   fidl_np600_ev.c  $Revision: 1.14 $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>

static char rcsid[] = "$Header: /home/hannah/mcavoy/idl/clib/RCS/fidl_np600_ev.c,v 1.14 2008/12/12 20:46:43 mcavoy Exp $";

main(int argc,char **argv)
{
char *filename,concname[MAXNAME],string[MAXNAME],hdr[MAXNAME],*tmp="fidl_tmp.txt",write_back[MAXNAME];
int i,j,k,t,jj,startj,nruns=0,LC,SC,dLC,dSC,LO,SO,dLO,dSO,*present,nbold_files=0,et,et1,
    lcmodelclosed_openisbaseline=0,lcmodellongclosedandopen=0,lcaddstates=0,lcmodellongclosedonly=0,lcmodellongopenonly=0,
    lcexcludelongclosed=0,lcexcludelongopen=0,lcclosedrunsonly=0,lcseparate=0,cLO,cLC,lcskipfirsttrial=0,lcaddstatesseparate=0,
    lcaddBaseline=0,lc80strial=0,endj,lcout1s=0;
FILE *fp,*op;
Files_Struct *runs,*bold_files;

if(argc < 5) {
    fprintf(stderr,"    -filename:       Name of event file to be created.\n");
    fprintf(stderr,"    -bold_files:     \n");
    fprintf(stderr,"    -modelclosedonly Analysis includes long and short runs.\n");
    fprintf(stderr,"                     Only closed events are modeled.\n");
    fprintf(stderr,"    -modelclosed_openisbaseline\n");
    fprintf(stderr,"    -modellongclosedandopen\n");
    fprintf(stderr,"        -addstates         Add state events to -modellongclosedandopen\n");
    fprintf(stderr,"        -addstatesseparate Add state events to -modellongclosedandopen. Each state is a separate event type.\n");
    fprintf(stderr,"                           Also each occurence is a separate event type.\n");
    fprintf(stderr,"        -separate          Each occurrence is a separate event type to -modellongclosedandopen\n");
    fprintf(stderr,"        -skipfirsttrial    No event for the first trial of each run.\n");
    fprintf(stderr,"        -addBaseline       Each trial is accompanied by a Baseline.\n");
    fprintf(stderr,"        -80strial          Each trial is an 80s epoch spanning LC and LO (or vice versa).\n");
    fprintf(stderr,"        -out1s             Output: TR=1s. Each trial 1s.\n");
    fprintf(stderr,"    -modellongclosedonly.\n");
    fprintf(stderr,"    -modellongopenonly.\n");
    fprintf(stderr,"    -excludelongclosed  Make text file of long closed frames.\n");
    fprintf(stderr,"    -excludelongopen    Make text file of long open frames.\n");
    fprintf(stderr,"    -closedrunsonly     Only include runs beginning with LC\n");
    exit(-1);
    }
for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-filename") && argc > i+1)
        filename = argv[++i];
    if(!strcmp(argv[i],"-runs") && argc > i+1) {
        for(j=1;i+j < argc && strchr(argv[i+j],'-') != argv[i+j];j++) ++nruns;
        if(!(runs=get_files(nruns,&argv[i+1]))) exit(-1);
        i += nruns;
        }
    if(!strcmp(argv[i],"-bold_files") && argc > i+1) {
        for(j=1;i+j < argc && strchr(argv[i+j],'-') != argv[i+j];j++) ++nbold_files;
        if(!(bold_files=get_files(nbold_files,&argv[i+1]))) exit(-1);
        i += nbold_files;
        }
    if(!strcmp(argv[i],"-modelclosed_openisbaseline"))
        lcmodelclosed_openisbaseline = 1;
    if(!strcmp(argv[i],"-modellongclosedandopen"))
        lcmodellongclosedandopen = 1;
    if(!strcmp(argv[i],"-addstates"))
        lcaddstates = 1;
    if(!strcmp(argv[i],"-addstatesseparate"))
        lcaddstatesseparate = 1;
    if(!strcmp(argv[i],"-separate"))
        lcseparate = 1;
    if(!strcmp(argv[i],"-skipfirsttrial"))
        lcskipfirsttrial = 1;
    if(!strcmp(argv[i],"-modellongclosedonly"))
    if(!strcmp(argv[i],"-modellongclosedonly"))
        lcmodellongclosedonly = 1;
    if(!strcmp(argv[i],"-modellongopenonly"))
        lcmodellongopenonly = 1;
    if(!strcmp(argv[i],"-excludelongclosed"))
        lcexcludelongclosed = 1;
    if(!strcmp(argv[i],"-excludelongopen"))
        lcexcludelongopen = 1;
    if(!strcmp(argv[i],"-closedrunsonly"))
        lcclosedrunsonly = 1;
    if(!strcmp(argv[i],"-addBaseline"))
        lcaddBaseline = 1;
    if(!strcmp(argv[i],"-80strial"))
        lc80strial = 1;
    if(!strcmp(argv[i],"-out1s"))
        lcout1s = 1;
    }
printf("%s\n",filename);

if(lcexcludelongclosed) {
    if(!(fp = fopen_sub(filename,"w"))) exit(-1);
    for(t=1,i=0;i<nruns;i++) {
        if(!strcmp(runs->files[i],"LO")) {
            printf("    run %d %s long open\n",i+1,runs->files[i]);
            for(t+=16,j=0;j<4;j++,t+=32) fprintf(fp,"%d-%d ",t,t+15);
            fprintf(fp,"\n");
            t -= 16;
            }
        else if(!strcmp(runs->files[i],"LC")) {
            printf("    run %d %s long closed\n",i+1,runs->files[i]);
            for(j=0;j<4;j++,t+=32) fprintf(fp,"%d-%d ",t,t+15);
            fprintf(fp,"\n");
            }
        else if(!strcmp(runs->files[i],"SO")) {
            printf("    run %d %s short open\n",i+1,runs->files[i]);
            /*do nothing*/
            }
        else if(!strcmp(runs->files[i],"SC")) {
            printf("    run %d %s short closed\n",i+1,runs->files[i]);
            /*do nothing*/
            }
        else {
            printf("    Error: Unknown run type. Abort!\n");
            exit(-1);
            }
        }
    fclose(fp);
    }
else if(lcexcludelongopen) {
    if(!(fp = fopen_sub(filename,"w"))) exit(-1);
    for(t=1,i=0;i<nruns;i++) {
        if(!strcmp(runs->files[i],"LO")) {
            printf("    run %d %s long open\n",i+1,runs->files[i]);
            for(j=0;j<4;j++,t+=32) fprintf(fp,"%d-%d ",t,t+15);
            fprintf(fp,"\n");
            }
        else if(!strcmp(runs->files[i],"LC")) {
            printf("    run %d %s long closed\n",i+1,runs->files[i]);
            for(t+=16,j=0;j<4;j++,t+=32) fprintf(fp,"%d-%d ",t,t+15);
            fprintf(fp,"\n");
            t -= 16;
            }
        else if(!strcmp(runs->files[i],"SO")) {
            printf("    run %d %s short open\n",i+1,runs->files[i]);
            /*do nothing*/
            }
        else if(!strcmp(runs->files[i],"SC")) {
            printf("    run %d %s short closed\n",i+1,runs->files[i]);
            /*do nothing*/
            }
        else {
            printf("    Error: Unknown run type. Abort!\n");
            exit(-1);
            }
        }
    fclose(fp);
    }
else if(lcmodelclosed_openisbaseline) {
    if(!(fp = fopen_sub(filename,"w"))) exit(-1);
    LC=0; SC=1; dLC=40; dSC=10;
    fprintf(fp,"2.5 LC SC\n");
    for(t=i=0;i<nruns;i++) {
        if(!strcmp(runs->files[i],"LO")) {
            printf("    run %d %s long open\n",i+1,runs->files[i]);
            for(t+=40,j=0;j<4;j++,t+=80) fprintf(fp,"%8d %8d %8d\n",t,LC,dLC);         
            t -= 40;
            }
        else if(!strcmp(runs->files[i],"LC")) {
            printf("    run %d %s long closed\n",i+1,runs->files[i]);
            for(j=0;j<4;j++,t+=80) fprintf(fp,"%8d %8d %8d\n",t,LC,dLC);         
            }
        else if(!strcmp(runs->files[i],"SO")) {
            printf("    run %d %s short open\n",i+1,runs->files[i]);
            for(t+=10,j=0;j<16;j++,t+=20) fprintf(fp,"%8d %8d %8d\n",t,SC,dSC);         
            t -= 10;
            }
        else if(!strcmp(runs->files[i],"SC")) {
            printf("    run %d %s short closed\n",i+1,runs->files[i]);
            for(j=0;j<16;j++,t+=20) fprintf(fp,"%8d %8d %8d\n",t,SC,dSC);         
            }
        else {
            printf("    Error: Unknown run type. Abort!\n");
            exit(-1);
            }
        }
    fclose(fp);
    }
else if(lcmodellongclosedandopen) {
    if(nruns != nbold_files) {
        printf("Error: nbold_files=%d nruns=%d Should be equal. Abort!\n",nbold_files,nruns);
        exit(-1);
        }
    if(!(present=malloc(sizeof*present*nruns))) {
        printf("Error: Unable to malloc present\n");
        exit(-1);
        }

    dLC = 40;

    /*START41*/
    if(lc80strial) dLC=80;
    if(lcout1s) dLC=1;

    if(lcseparate) {
        if(!(fp = fopen_sub(tmp,"w+"))) exit(-1);
        }
    else if(lcaddstates) {
        if(!(fp = fopen_sub(filename,"w"))) exit(-1);
        fprintf(fp,"2.5 LC LO sLC sL0\n");
        }
    else if(lcaddstatesseparate) {
        if(!(fp = fopen_sub(tmp,"w+"))) exit(-1);
        }
    else if(lcaddBaseline) {
        if(!(fp = fopen_sub(filename,"w"))) exit(-1);
        fprintf(fp,"2.5 LC LO Baseline\n");
        }
    else {
        if(!(fp = fopen_sub(filename,"w"))) exit(-1);

        /*fprintf(fp,"2.5 LC LO\n");*/
        /*START41*/
        fprintf(fp,"%s LC LO\n",!lcout1s?"2.5":"1");

        }
    for(et1=et=cLO=cLC=k=t=i=0;i<nruns;i++) {
        jj = !strcmp(runs->files[i],"LO") ? 1 : 0;
        if(!strcmp(runs->files[i],"LO") || !strcmp(runs->files[i],"LC")) {
            printf("    run %d %s long %s",i+1,runs->files[i],!strcmp(runs->files[i],"LO")?"open":"closed");
            if(!strcmp(runs->files[i],"LO")) {
                if(lcclosedrunsonly) {
                    printf(" DO NOTHING\n");
                    continue;
                    }
                }
            printf("\n");
            startj=0;
            endj = !lc80strial ? 8 : 4;
            if(lcskipfirsttrial) {

                #if 0
                startj=1;t+=40;
                if(lc80strial) dLC=80;
                #endif
                /*START41*/
                startj=1;
                if(!lcout1s) t+=40;
                
                }
            if(lc80strial) {
                jj = !strcmp(runs->files[i],"LO") ? 0 : 1;
                for(j=startj;j<endj;j++,t+=dLC) fprintf(fp,"%8d %8d %8d\n",t,jj,dLC);
                t+=40;
                }
            else if(lcseparate) {
                for(j=startj;j<endj;j++,t+=dLC,et++) {
                    strcat(hdr,!((j+jj)%2)?"LC":"LO");
                    sprintf(string,"%d ",et);
                    strcat(hdr,string);
                    fprintf(fp,"%8d %8d %8d\n",t,et,dLC);
                    }
                }
            else if(lcaddstates) {
                for(j=startj;j<endj;j++,t+=dLC) {
                    fprintf(fp,"%8d %8d %8d\n",t,(j+jj)%2+2,dLC);
                    fprintf(fp,"%8d %8d %8d\n",t,(j+jj)%2,dLC);
                    }
                }
            else if(lcaddstatesseparate) {
                for(j=startj;j<endj;j++,t+=dLC,et++) {
                    sprintf(string,"constant%d %s%d ",et+1,!((j+jj)%2)?"LC":"LO",et+1);
                    strcat(hdr,string);
                    fprintf(fp,"%8d %8d %8d\n",t,et1++,dLC);
                    fprintf(fp,"%8d %8d %8d\n",t,et1++,dLC);
                    }
                }
            else if(lcaddBaseline) {
                for(j=startj;j<endj;j++,t+=dLC) {
                    fprintf(fp,"%8d %8d %8d\n",t,(j+jj)%2,dLC);
                    fprintf(fp,"%8d 2 %8d\n",t,dLC);
                    }
                }
            else {
                for(j=startj;j<endj;j++,t+=dLC) {
                    fprintf(fp,"%8d %8d %8d\n",t,(j+jj)%2,dLC);
                    }
                }
            present[k++] = i;
            }









        else if(!strcmp(runs->files[i],"SO")) {
            printf("    run %d %s short open DO NOTHING\n",i+1,runs->files[i]);
            }
        else if(!strcmp(runs->files[i],"SC")) {
            printf("    run %d %s short closed DO NOTHING\n",i+1,runs->files[i]);
            }
        else {
            /*printf("    Error: Unknown run type. Abort!\n");
            exit(-1);*/
            printf("    run %d %s DO NOTHING\n",i+1,runs->files[i]);
            }
        }
    if(lcseparate||lcaddstatesseparate) {
        if(!(op = fopen_sub(filename,"w"))) exit(-1);
        if(lcseparate) fprintf(op,"2.5 %s\n",hdr);
        /*else if(lcaddstatesseparate) fprintf(op,"2.5 LC LO %s\n",hdr);*/
        else if(lcaddstatesseparate) fprintf(op,"2.5 %s\n",hdr);
        for(rewind(fp);fgets(string,sizeof(string),fp);) {
            count_strings(string,write_back,' '); 
            fprintf(op,"%s\n",write_back);
            }
        fclose(op);
        fclose(fp);
        if(remove(tmp)) {
            printf("Error: Unable to remove %s\n",tmp);
            exit(-1);
            }
        }
    else {
        fclose(fp);
        }
    *strrchr(filename,'.') = 0;
    sprintf(concname,"%s.conc",filename);
    if(!(op = fopen_sub(concname,"w"))) exit(-1);
    fprintf(op,"    number_of_files:%d\n",k);
    for(i=0;i<k;i++) fprintf(op,"               file:%s\n",bold_files->files[present[i]]);
    fclose(op);
    printf("Concatenated file written to %s\n",concname);
    }
















else if(lcmodellongclosedonly) {
    LC=0; dLC=40;
    if(!(fp = fopen_sub(filename,"w"))) exit(-1);
    fprintf(fp,"2.5 LC\n");
    for(t=i=0;i<nruns;i++) {
        if(!strcmp(runs->files[i],"LO")) {
            printf("    run %d %s long open\n",i+1,runs->files[i]);
            for(t+=40,j=0;j<4;j++,t+=80) fprintf(fp,"%8d %8d %8d\n",t,LC,dLC);
            t -= 40;
            }
        else if(!strcmp(runs->files[i],"LC")) {
            printf("    run %d %s long closed\n",i+1,runs->files[i]);
            for(j=0;j<4;j++,t+=80) fprintf(fp,"%8d %8d %8d\n",t,LC,dLC);
            }
        else if(!strcmp(runs->files[i],"SO")) {
            printf("    run %d %s short open DO NOTHING\n",i+1,runs->files[i]);
            }
        else if(!strcmp(runs->files[i],"SC")) {
            printf("    run %d %s short closed DO NOTHING\n",i+1,runs->files[i]);
            }
        else {
            printf("    Error: Unknown run type. Abort!\n");
            exit(-1);
            }
        }
    fclose(fp);
    }
else if(lcmodellongopenonly) {
    LO=0; dLO=40;
    if(!(fp = fopen_sub(filename,"w"))) exit(-1);
    fprintf(fp,"2.5 LO\n");
    for(t=i=0;i<nruns;i++) {
        if(!strcmp(runs->files[i],"LO")) {
            printf("    run %d %s long open\n",i+1,runs->files[i]);
            for(j=0;j<4;j++,t+=80) fprintf(fp,"%8d %8d %8d\n",t,LO,dLO);
            }
        else if(!strcmp(runs->files[i],"LC")) {
            printf("    run %d %s long closed\n",i+1,runs->files[i]);
            for(t+=40,j=0;j<4;j++,t+=80) fprintf(fp,"%8d %8d %8d\n",t,LO,dLO);
            t -= 40;
            }
        else if(!strcmp(runs->files[i],"SO")) {
            printf("    run %d %s short open DO NOTHING\n",i+1,runs->files[i]);
            }
        else if(!strcmp(runs->files[i],"SC")) {
            printf("    run %d %s short closed DO NOTHING\n",i+1,runs->files[i]);
            }
        else {
            printf("    Error: Unknown run type. Abort!\n");
            exit(-1);
            }
        }
    fclose(fp);
    }
else { /*modelopen_closedisbaseline*/
    LO=0; SO=1; dLO=40; dSO=10;
    if(!(fp = fopen_sub(filename,"w"))) exit(-1);
    fprintf(fp,"2.5 LO SO\n");
    for(t=i=0;i<nruns;i++) {
        if(!strcmp(runs->files[i],"LO")) {
            printf("    run %d %s long open\n",i+1,runs->files[i]);
            for(j=0;j<4;j++,t+=80) fprintf(fp,"%8d %8d %8d\n",t,LO,dLO);
            }
        else if(!strcmp(runs->files[i],"LC")) {
            printf("    run %d %s long closed\n",i+1,runs->files[i]);
            for(t+=40,j=0;j<4;j++,t+=80) fprintf(fp,"%8d %8d %8d\n",t,LO,dLO);
            t -= 40;
            }
        else if(!strcmp(runs->files[i],"SO")) {
            printf("    run %d %s short open\n",i+1,runs->files[i]);
            for(j=0;j<16;j++,t+=20) fprintf(fp,"%8d %8d %8d\n",t,SO,dSO);
            }
        else if(!strcmp(runs->files[i],"SC")) {
            printf("    run %d %s short closed\n",i+1,runs->files[i]);
            for(t+=10,j=0;j<16;j++,t+=20) fprintf(fp,"%8d %8d %8d\n",t,SO,dSO);
            t -= 10;
            }
        else {
            printf("    Error: Unknown run type. Abort!\n");
            exit(-1);
            }
        }
    fclose(fp);
    }
/*fclose(fp);*/
printf("\n");
}
