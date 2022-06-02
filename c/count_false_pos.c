#include <stdlib.h>
#include <stdio.h>
#include  <fidl.h>

main(int argc,char **argv)
{
char *filename;
int i,count=0,number,size;
FILE *fp;

if (argc < 5) {
    fprintf(stderr,"Usage: count_false_pos -size 5000 -filename name_of_file\n\n");
    exit(-1);
    }
for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-filename") && argc > i+1) {
        GETMEM(filename,strlen(argv[i+1])+1,char)
        strcpy(filename,argv[++i]);
        }
    if(!strcmp(argv[i],"-size") && argc > i+1)
        size = atoi(argv[++i]);
    }

if(!(fp = fopen(filename,"r"))) {
    printf("Unable to open %s\n",filename);
    exit(-1);
    }
for(count=i=0;i<size;i++) {
    fscanf(fp,"%d",&number);
    count += number;
    }
fclose(fp);
fprintf(stdout,"%s\n",filename);
fprintf(stdout,"number of false positives = %d  number of runs = %d\n",count,size);
fprintf(stdout,"p value = %g\n",(float)count/size);
}
