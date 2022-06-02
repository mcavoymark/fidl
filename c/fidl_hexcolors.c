/* Copyright 9/6/05 Washington University.  All Rights Reserved.
   fidl_hexcolors.c  $Revision: 1.2 $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>

static char rcsid[] = "$Header: /home/hannah/mcavoy/idl/clib/RCS/fidl_hexcolors.c,v 1.2 2005/09/08 21:14:37 mcavoy Exp $";
enum{Ncolors=101};

main(int argc,char **argv)
{
char *file,filename[MAXNAME],string[MAXNAME],*strptr;
int i,j,k;
Dat *dat;
FILE *fp;

if (argc < 3) {
    fprintf(stderr,"Usage: fidl_hexcolors -file /home/hannah/mcavoy/idl/linecolors.dat\n");
    fprintf(stderr,"    -file: red, green, blue color file with names.\n");
    fprintf(stderr,"           See linecolors.dat in the idl directory.\n");
    exit(-1);
    }
for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-file") && argc > i+1)
        file = argv[++i];
    }
print_version_number(rcsid,stdout);
if(!(dat=readdat(file))) exit(-1);
for(strptr=dat->name,k=i=0;i<dat->nlines;strptr+=dat->len[i++]) {
    sprintf(filename,"square_%s_filled.m.pm",strptr);
    if(!(fp = fopen_sub(filename,"w"))) exit(-1);
    fprintf(fp,"/* XPM */\n");
    fprintf(fp,"static char * junk_m_pm[] = {\n");
    fprintf(fp,"/* width height ncolors cpp [x_hot y_hot] */\n");
    fprintf(fp,"\"16 16 1 1 0 0\",\n");
    fprintf(fp,"/* colors */\n");
    fprintf(fp,"\"       s iconColor7    m white c #");
    for(j=0;j<3;j++,k++) {
        fprintf(fp,"%02x",(int)dat->data[k]);
        printf("int=%d hex=%02x  ",(int)dat->data[k],(int)dat->data[k]);
        }
    printf("\n");
    fprintf(fp,"\",\n");
    fprintf(fp,"/* pixels */\n");
    fprintf(fp,"\"                \",\n");
    fprintf(fp,"\"                \",\n");
    fprintf(fp,"\"                \",\n");
    fprintf(fp,"\"                \",\n");
    fprintf(fp,"\"                \",\n");
    fprintf(fp,"\"                \",\n");
    fprintf(fp,"\"                \",\n");
    fprintf(fp,"\"                \",\n");
    fprintf(fp,"\"                \",\n");
    fprintf(fp,"\"                \",\n");
    fprintf(fp,"\"                \",\n");
    fprintf(fp,"\"                \",\n");
    fprintf(fp,"\"                \",\n");
    fprintf(fp,"\"                \",\n");
    fprintf(fp,"\"                \",\n");
    fprintf(fp,"\"                \"};\n");
    fclose(fp);
    printf("Ouput written to %s\n",filename);
    }
}
