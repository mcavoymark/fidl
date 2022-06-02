/*************************************************************

Program: mgh_to_4dfp

Purpose: Convert MGH data to 4dfp file.

Date: November 27, 1995

By: John Ollinger

***************************************************************/

/*$Revision: 12.80 $*/

#include	<stdio.h>
#include	<math.h>
#include	<stdlib.h>

#define MAXNAME 120

main(int argc,char *argv[])

{

char	*stem,*ext,*format,filnam[MAXNAME],outnam[MAXNAME],*outstem;

int	i,num_planes,num_frames,xydim,len,pln,stat,lenimg,lenvol,frm,
	offset,dskptr,pad;

short	*simg;

float	*fimg,*image;

FILE	*fpr,*fpw;

for(i=0;i<argc;i++) {
    if(!strcmp(argv[i],"-help")) {
	printf("\nYou didn't really expect this  to work, did you?\n\n");
	exit(-1);
	}
    }

if(argc < 8) {
    fprintf(stderr,"Usage: mgh_to_4dfp stem extension -short/-float number_of_planes number_of_frames XY_dimension pad stem_of_output_file_name [-help]\n");
    printf("\nThis program builds file names, one for each plane of data, by\nconcatenating the stem, the plane number, and the extension, i.e., as\nstem??extension.  The output files outstem.4dfp.img and outstem.4dfp.hdr will\nbe created.\n");
    printf("\nExample: The files AVG7_S4run43_001.bshort through AVG7_S4run43_016.bshort\nwould be processed using the command\nmgh_to_4dfp AVG7_S4run43_0 .bshort -short 17 110 64 5 AVG7_S4run43\n");
    printf("pad is the number of images at the front of the file to skip.\n");
    exit(-1);
    }

stem = argv[1];
ext  = argv[2];
format  = argv[3];
num_planes = atoi(argv[4]);
num_frames = atoi(argv[5]);
xydim = atoi(argv[6]);
pad = atoi(argv[7]);
outstem = argv[8];

lenimg = xydim*xydim;
lenvol = lenimg*num_planes;
len = lenimg*num_frames;
if(!strcmp(format,"-short"))
    simg = (short *)malloc(len*sizeof(short));
fimg = (float *)malloc(len*sizeof(float));

sprintf(outnam,"%s.4dfp.img",outstem);
if((fpw = fopen(outnam,"w")) == NULL) {
    fprintf(stderr,"Could not open %s.\n",filnam);
    exit(-1);
    }

for(pln=0;pln<num_planes;pln++) {

    sprintf(filnam,"%s%02i%s",stem,pln,ext);
    printf("Reading %s\n",filnam);

    if((fpr = fopen(filnam,"r")) == NULL) {
        fprintf(stderr,"Could not open %s.\n",filnam);
        exit(-1);
        }

    if(!strcmp(format,"-short")) {
	if((stat=fread(simg,sizeof(short),len,fpr)) != len) {
            fprintf(stderr,"Could not read from %s in mgh_to_4dfp.\n",filnam);
            exit(NULL);
            }
	for(i=0;i<len;i++)
	    fimg[i] = (float)simg[i];
	}
    else {
	if((stat=fread(fimg,sizeof(float),len,fpr)) != len) {
            fprintf(stderr,"Could not read from %s in mgh_to_4dfp.\n",filnam);
            exit(NULL);
            }
	}
    fclose(fpr);

    for(frm=0,offset=0;frm<num_frames;frm++,offset+=lenimg) {
        if(frm > pad-1) {
	    dskptr = sizeof(float)*(lenimg*pln + (frm - pad)*lenvol);
	    if((stat=fseek(fpw,(long)dskptr,SEEK_SET)) != NULL) {
    	        fprintf(stderr,"Did not seek correctly in %s.\n",outnam);
	        return(NULL);
	        }
            if((stat=fwrite(&fimg[offset],sizeof(float),lenimg,fpw)) != lenimg) {
                fprintf(stderr,"Could not write to %s in write_norm.\n",filnam);
                exit(-1);
                }
	    }
        }
    }

fclose(fpw);

printf("Images written to %s.\n",outnam);
sprintf(outnam,"%s.4dfp.img.rec",outstem);
if((fpw = fopen(outnam,"w")) == NULL) {
    fprintf(stderr,"Could not open %s\n",filnam);
    exit(-1);
    }
fprintf(fpw,"xdim:\t\t\t%i\n",xydim);
fprintf(fpw,"ydim:\t\t\t%i\n",xydim);
fprintf(fpw,"zdim:\t\t\t%i\n",num_planes);
fprintf(fpw,"vdim:\t\t\t%i\n",num_frames-pad);
fprintf(fpw,"xpix:\t\t\t1\n");
fprintf(fpw,"ypix:\t\t\t1\n");
fprintf(fpw,"zpix:\t\t\t1\n");
if(!strcmp(format,"-short"))
    fprintf(fpw,"bitsperpix:\t\t\t16\n");
else
    fprintf(fpw,"bitsperpix:\t\t\t32\n");

}
