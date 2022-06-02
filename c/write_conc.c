/* Copyright 11/25/15 Washington University.  All Rights Reserved.
   write_conc.c  $Revision: 1.2 $*/
#include <stdio.h>
#include <stdlib.h>
#include "fidl.h"
#include "subs_util.h"
int write_conc(char *root,int nfiles,char **identify,int rootonly){
    char concname[MAXNAME],filename[MAXNAME];
    int i;
    FILE *fp;
    if(nfiles==1) {
        sprintf(filename,"%s.4dfp.img",root);
        printf("File written to %s\n",filename);
        }
    else {
        sprintf(concname,"%s.conc",root);
        if(!(fp=fopen_sub(concname,"w"))) return 0;
        for(i=0;i<nfiles;i++) {
            if(!rootonly) sprintf(filename,"%s_b%d.4dfp.img",root,i+1); else sprintf(filename,"%s.4dfp.img",root);
            fprintf(fp,"%s %s\n",filename,!identify?"":identify[i]);
            }
        fclose(fp);
        printf("Concatenated file written to %s\n",concname);
        }
    return 1;
    }
