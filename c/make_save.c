#include	<string.h>
#include	<stdio.h>

#define MAXNAME 256
#define TRUE 1
#define FALSE 0
 

#if 0
/*$Revision: 12.82 $*/
void main(argc,argv)
 
int     argc;
char    *argv[];
 
{
#endif
//START151111
int main(int argc,char **argv)
{

char	*stem,*path,filnam[MAXNAME];

#int	stat;

if(argc < 3) {
        printf("Usage: make_save path routine [file_name (no path)]\n");
        exit(-1);
        }
path = argv[1];
stem = argv[2];
if(argc == 3) 
    sprintf(filnam,"%s/%s.sav",path,stem);
else
    sprintf(filnam,"%s/%s.sav",path,argv[3]);
    
fprintf(stdout,"save,'%s',/ROUTINES,FILENAME='%s'\n",stem,filnam);

}
