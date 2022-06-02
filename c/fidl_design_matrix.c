/* Copyright 9/23/03 Washington University.  All Rights Reserved.
   fidl_design_matrix.c  $Revision: 1.2 $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>

main(int argc,char **argv)
{
int i,j,k,l,ntrials=48,nruns=6,skip=4,nframes=150,*jitter,*jitter_cue;

int condition[48]={0,1,2,3,
                   1,2,0,3,
                   2,1,3,0,
                   3,0,2,1,
                   1,3,2,0,
                   2,0,3,1,
                   0,2,1,3,
                   2,3,1,0,
                   3,2,0,1,
                   1,0,3,2,
                   3,0,1,2,
                   2,1,0,3};
float TR=2.5;
double temp_double;
unsigned short seed[]={0,0,0};
FILE *fp;


/*uniform .2 .62.*/
if(!(jitter=d1int(ntrials))) exit(-1);
for(i=0;i<ntrials;i++) {
    temp_double = erand48(seed);
    if(temp_double < .49) {
        jitter[i] = 13;
        }
    else if(temp_double < .76) {
        jitter[i] = 14;
        }
    else {
        jitter[i] = 15;
        }
    }
for(j=k=l=i=0;i<ntrials;i++) {
    if(jitter[i] == 13) {
        j++;
        }
    else if(jitter[i] == 14) {
        k++; 
        }
    else if(jitter[i] == 15) {
        l++;
        }
    else {
        printf("jitter=%d  Not an allowed value. Abort!\n",jitter[i]);
        exit(-1);
        }
    }
printf("jitter 13=%d 14=%d 15=%d\n",j,k,l);

/*uniform .35 .67*/
if(!(jitter_cue=d1int(ntrials))) exit(-1);
for(i=0;i<ntrials;i++) {
    temp_double = erand48(seed);
    if(temp_double < .51) {
        jitter_cue[i] = 4;
        }
    else if(temp_double < .75) {
        jitter_cue[i] = 5;
        }
    else {
        jitter_cue[i] = 6;
        }
    }
for(j=k=l=i=0;i<ntrials;i++) {
    if(jitter_cue[i] == 4) {
        j++;
        }
    else if(jitter_cue[i] == 5) {
        k++;
        }
    else if(jitter_cue[i] == 6) {
        l++;
        }
    else {
        printf("jitter_cue=%d  Not an allowed value. Abort!\n",jitter_cue[i]);
        exit(-1);
        }
    }
printf("jitter_cue 4=%d 5=%d 6=%d\n",j,k,l);




if(!(fp = fopen_sub("singlecue_jitterboth_exp.fidl","w"))) exit(-1); 
fprintf(fp,"2.5 words letters patterns control cue\n");
for(k=i=0;i<nruns;i++,l+=skip) {
    l = i*nframes + skip;
    for(j=skip;j<nframes;) {
        fprintf(fp,"%-8.1f 4  12.5\n",TR*(float)l); 
        /*l += 5;*/
        l += jitter_cue[k];
        if(l > (i+1)*150) break;
        fprintf(fp,"%-8.1f %d  22.5\n",TR*(float)l,condition[k]); 
        /*l += jitter[k++];*/
        /*if(k == 8*(i+1)) break;*/
        l += jitter[k];
        if((l > (i+1)*150) || (k == 8*(i+1))) {
            break; 
            }
        else {
            k++;
            }
        }
    }
fclose(fp);
printf("k=%d\n",k);
}
