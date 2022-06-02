/* Copyright 5/5/14 Washington University.  All Rights Reserved.
   fidl_lang_synRT.c  $Revision: 1.4 $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>
static char rcsid[] = "$Header: /home/hannah/mcavoy/idl/clib/RCS/fidl_lang_synRT.c,v 1.4 2014/07/02 18:34:53 mcavoy Exp $";
main(int argc,char **argv)
{
char *conc=NULL,*txt4=NULL,*txt8=NULL,string[MAXNAME],*strptr;
int i,j,k,i1,j1,i2,SunOS_Linux,**cond,txt=0,txt3=0,lcrr=0,rtc,*RTctrial,*CRtrial,cr;
double TR=0.,skipsec=0.,fix=0.,time,time1,time2,time3,**RT,*RTm,rt,rt2,rtm,rts,*RTtrial,rt1,rtw,rtn,buttons[3]={0.,0.,0.};
Files_Struct *bolds;
Dim_Param *dp;
Data *data4,*data8;
FILE *fp;
if(argc<3) {
    fprintf(stderr,"This is for synonym runs.\n");
    fprintf(stderr,"  -conc:    Conc file.\n");
    fprintf(stderr,"  -TR:      Sampling rate (s).\n");
    fprintf(stderr,"  -rr       Makes event file suitable for regional regressors.\n");
    fprintf(stderr,"  -txt4:    Name of eprime derived text file.\n");
    fprintf(stderr,"            Each run is 4 columns: TriggerScreen.RTTime\n");
    fprintf(stderr,"                                   Pause2.OnsetTime||GreenCross.OnsetTime\n");
    fprintf(stderr,"                                   WhiteCross.OnsetTime||WhiteCross1.OnsetTime\n");
    fprintf(stderr,"                                   Condition\n");
    fprintf(stderr,"  -txt8:    Name of eprime derived text file.\n");
    fprintf(stderr,"            Each run is 8 columns: Item3.OnsetTime\n");
    fprintf(stderr,"                                   Item3.RESP\n");
    fprintf(stderr,"                                   Item3.RTTime\n");
    fprintf(stderr,"                                   Response.RESP\n");
    fprintf(stderr,"                                   Response.RTTime\n");
    fprintf(stderr,"                                   Blank.RESP\n");
    fprintf(stderr,"                                   Blank.RTTime\n");
    fprintf(stderr,"                                   CR\n");

    /*START140619*/
    fprintf(stderr,"  -button1: Button for second item (ie first target).\n");
    fprintf(stderr,"  -button2: Button for third item (ie second target).\n");

    }
for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-conc") && argc > i+1)
        conc = argv[++i];
    if(!strcmp(argv[i],"-TR") && argc > i+1)
        TR = atof(argv[++i]);
    if(!strcmp(argv[i],"-rr"))
        lcrr=1;
    if(!strcmp(argv[i],"-txt4") && argc > i+1)
        txt4 = argv[++i];
    if(!strcmp(argv[i],"-txt8") && argc > i+1)
        txt8 = argv[++i];

    /*START140619*/
    if(!strcmp(argv[i],"-button1") && argc > i+1)
        buttons[1] = atof(argv[++i]);
    if(!strcmp(argv[i],"-button2") && argc > i+1)
        buttons[2] = atof(argv[++i]);

    }
if((SunOS_Linux=checkOS())==-1) exit(-1);
if(!conc) {printf("fidlError: need to specify -conc\n");fflush(stdout);exit(-1);}
if(TR<=0.) {printf("fidlError: TR=%f Need to specify -TR\n",TR);exit(-1);}
if(!txt4) {printf("fidlError: Need to specify -txt4\n");fflush(stdout);exit(-1);}
if(!txt8) {printf("fidlError: Need to specify -txt8\n");fflush(stdout);exit(-1);}
if(!(data4=read_data(txt4,0,0,0,0)))exit(-1);
if(!(data8=read_data(txt8,0,0,0,1)))exit(-1);
if(!(bolds=read_conc(1,&conc))) exit(-1);
if(!(dp=dim_param(bolds->nfiles,bolds->files,SunOS_Linux,0))) exit(-1);

/*START140619*/
if(buttons[1]==0.) {printf("fidlError: need to specify -button1\n");fflush(stdout);exit(-1);}
if(buttons[2]==0.) {printf("fidlError: need to specify -button2\n");fflush(stdout);exit(-1);}

/*for(i=0;i<data8->ncol;i++) printf("%s ",data8->colptr[i]); printf("\n");
for(i=0;i<data8->nsubjects;i++) {
    for(j=0;j<data8->npoints;j++) printf("%g ",data8->x[i][j]);
    printf("\n");
    }
for(i=0;i<data4->nsubjects;i++) {
    for(j=0;j<data4->npoints;j++) printf("%g ",data4->x[i][j]);
    printf("\n");
    }
printf("\n");*/

if(!(RTm=malloc(sizeof*RTm*3))) {
    printf("fidlError: Unable to malloc RTm\n");
    exit(-1);
    }
if(!(RTtrial=malloc(sizeof*RTtrial*3))) {
    printf("fidlError: Unable to malloc RTtrial\n");
    exit(-1);
    }
for(i=0;i<3;i++) RTtrial[i]=0.;
if(!(RTctrial=malloc(sizeof*RTctrial*3))) {
    printf("fidlError: Unable to malloc RTctrial\n");
    exit(-1);
    }
for(i=0;i<3;i++) RTctrial[i]=0;
if(!(RT=d2double(3,10))) exit(-1);
for(i=0;i<3;i++)for(j=0;j<10;j++)RT[i][j]=(double)UNSAMPLED_VOXEL;


/*START140619*/
if(!(CRtrial=malloc(sizeof*CRtrial*3))) {
    printf("fidlError: Unable to malloc CRtrial\n");
    exit(-1);
    }
for(i=0;i<3;i++) CRtrial[i]=0.;



#if 0
for(time=0.,i2=i1=i=0;i<dp->nfiles;time+=dp->tdim[i++]*TR,i1+=4,i2+=9) {
    time2=time+dp->tdim[i]*TR;
    for(j1=j=0;j<data4->nsubjects;j++) {
        if((time1=time+(data4->x[j][i1+1]-data4->x[j][i1])/1000.)<time2) {
            for(rt=0.,rtc=k=0;k<4;k++,j1++) {
                /*printf("data8->x[%d][%d]=%g data8->x[%d][%d]=%g data8->x[%d][%d]=%g\n",j1,i2+5,data8->x[j1][i2+5],j1,i2+3,
                    data8->x[j1][i2+3],j1,i2+1,data8->x[j1][i2+1]);*/
                if(data8->x[j1][i2+5]!=(double)UNSAMPLED_VOXEL) {
                    /*printf("data8->x[%d][%d]=%f - data8->x[%d][%d]=%f = %f\n",j1,i2+6,data8->x[j1][i2+6],j1,i2,data8->x[j1][i2],
                        data8->x[j1][i2+6]-data8->x[j1][i2]);fflush(stdout);*/
                    rt += data8->x[j1][i2+6]-data8->x[j1][i2];
                    rtc++;
                    }
                else if(data8->x[j1][i2+3]!=(double)UNSAMPLED_VOXEL) {
                    /*printf("data8->x[%d][%d]=%f - data8->x[%d][%d]=%f = %f\n",j1,i2+4,data8->x[j1][i2+4],j1,i2,data8->x[j1][i2],
                        data8->x[j1][i2+4]-data8->x[j1][i2]);fflush(stdout);*/
                    rt += data8->x[j1][i2+4]-data8->x[j1][i2];
                    rtc++;
                    } 
                else if(data8->x[j1][i2+1]!=(double)UNSAMPLED_VOXEL) {
                    /*printf("data8->x[%d][%d]=%f - data8->x[%d][%d]=%f = %f\n",j1,i2+2,data8->x[j1][i2+2],j1,i2,data8->x[j1][i2],
                        data8->x[j1][i2+2]-data8->x[j1][i2]);fflush(stdout);*/
                    rt += data8->x[j1][i2+2]-data8->x[j1][i2];
                    rtc++;
                    } 
                } 
            /*printf("        rt=%f rtc=%d\n",rt,rtc);fflush(stdout);*/
            if(rtc) {
                RT[(int)data4->x[j][i1+3]][j] = rt/(double)rtc;
                RTtrial[(int)data4->x[j][i1+3]] += rt;
                RTctrial[(int)data4->x[j][i1+3]] += rtc;
                }
            }
        }
    }
#endif
/*START140619*/
for(time=0.,i2=i1=i=0;i<dp->nfiles;time+=dp->tdim[i++]*TR,i1+=4,i2+=9) {
    time2=time+dp->tdim[i]*TR;
    for(j1=j=0;j<data4->nsubjects;j++) {
        if((time1=time+(data4->x[j][i1+1]-data4->x[j][i1])/1000.)<time2) {
            for(rt=0.,cr=rtc=k=0;k<4;k++,j1++) {
                /*printf("data8->x[%d][%d]=%g data8->x[%d][%d]=%g data8->x[%d][%d]=%g\n",j1,i2+5,data8->x[j1][i2+5],j1,i2+3,
                    data8->x[j1][i2+3],j1,i2+1,data8->x[j1][i2+1]);*/
                if(data8->x[j1][i2+5]!=(double)UNSAMPLED_VOXEL) {
                    /*printf("data8->x[%d][%d]=%f - data8->x[%d][%d]=%f = %f\n",j1,i2+6,data8->x[j1][i2+6],j1,i2,data8->x[j1][i2],
                       data8->x[j1][i2+6]-data8->x[j1][i2]);fflush(stdout);*/
                    rt += data8->x[j1][i2+6]-data8->x[j1][i2];
                    rtc++;

                    if(buttons[(int)data8->x[j1][i2+7]]==data8->x[j1][i2+5]) cr++;

                    }
                else if(data8->x[j1][i2+3]!=(double)UNSAMPLED_VOXEL) {
                    /*printf("data8->x[%d][%d]=%f - data8->x[%d][%d]=%f = %f\n",j1,i2+4,data8->x[j1][i2+4],j1,i2,data8->x[j1][i2],
                       data8->x[j1][i2+4]-data8->x[j1][i2]);fflush(stdout);*/
                    rt += data8->x[j1][i2+4]-data8->x[j1][i2];
                    rtc++;

                    if(buttons[(int)data8->x[j1][i2+7]]==data8->x[j1][i2+3]) cr++;

                    }
                else if(data8->x[j1][i2+1]!=(double)UNSAMPLED_VOXEL) {
                    /*printf("data8->x[%d][%d]=%f - data8->x[%d][%d]=%f = %f\n",j1,i2+2,data8->x[j1][i2+2],j1,i2,data8->x[j1][i2],
                       data8->x[j1][i2+2]-data8->x[j1][i2]);fflush(stdout);*/
                    rt += data8->x[j1][i2+2]-data8->x[j1][i2];
                    rtc++;

                    if(buttons[(int)data8->x[j1][i2+7]]==data8->x[j1][i2+1]) cr++;

                    }
                }
            /*printf("        rt=%f rtc=%d\n",rt,rtc);fflush(stdout);*/
            if(rtc) {
                RT[(int)data4->x[j][i1+3]][j] = rt/(double)rtc;
                RTtrial[(int)data4->x[j][i1+3]] += rt;
                RTctrial[(int)data4->x[j][i1+3]] += rtc;

                CRtrial[(int)data4->x[j][i1+3]] += cr;

                }
            }
        }
    }




/*printf("\nBlock values\nabstract\tsensory\tnumber\n");
for(j=0;j<10;j++) {
    for(i=0;i<3;i++) printf("%f\t",RT[i][j]);
    printf("\n");
    }*/
strcpy(string,conc);
if(!(strptr=get_tail_sans_ext(string))) exit(-1);
for(i=0;i<3;i++) {
    for(rt=0.,rtc=j=0;j<10;j++) {
        if(RT[i][j]!=(double)UNSAMPLED_VOXEL) {
            rt += RT[i][j];
            rtc++;
            }
        }
    RTm[i] = rt/(double)rtc;
    }
for(rt=rt2=0.,i=0;i<3;i++) {
    rt += RTm[i]; 
    rt2 += RTm[i]*RTm[i];
    }
rtm = rt/3.;
rts = sqrt((rt2 - rt*rt/3.)/(3.-1.));
printf("Block values\tabstract sensory number\n");
printf("%s\t%f\t%f\t%f\n",strptr,RTm[0],RTm[1],RTm[2]);
printf("%s\t%f\t%f\t%f\tzscore\n",strptr,(RTm[0]-rtm)/rts,(RTm[1]-rtm)/rts,(RTm[2]-rtm)/rts);

for(rt=rt2=0.,rtc=i=0;i<3;i++) {
    rt1 = RTtrial[i]/RTctrial[i];
    rt += rt1;
    rt2 += rt1*rt1;
    }
rtm = rt/3.;
rts = sqrt((rt2 - rt*rt/3.)/(3.-1.));
printf("\nTrial RT values\tabstract sensory number\n");
printf("number of trials\t%d\t%d\t%d\n",RTctrial[0],RTctrial[1],RTctrial[2]);
printf("%s\t",strptr); for(i=0;i<3;i++) printf("%f\t",RTtrial[i]/(double)RTctrial[i]); printf("\n"); 
printf("%s\t",strptr); for(i=0;i<3;i++) printf("%f\t",(RTtrial[i]/(double)RTctrial[i]-rtm)/rts); printf("zscore\n"); 

rtw = (RTtrial[0]+RTtrial[1])/(double)(RTctrial[0]+RTctrial[1]);
rtn = RTtrial[2]/(double)RTctrial[2];
rtm = (rt=(rtw+rtn))/2.;
rt2 = rtw*rtw + rtn*rtn; 
rts = sqrt((rt2 - rt*rt/2.)/(2.-1.));
printf("\nTrial RT values\tword number\n");
printf("number of trials\t%d\t%d\n",RTctrial[0]+RTctrial[1],RTctrial[2]);
printf("%s\t%f\t%f\n",strptr,rtw,rtn);
printf("%s\t%f\t%f\tzscore\n",strptr,(rtw-rtm)/rts,(rtn-rtm)/rts);

/*START140619*/
printf("\nTrial Acc values\tabstract sensory number\n");
printf("number of trials\t%d\t%d\t%d\n",RTctrial[0],RTctrial[1],RTctrial[2]);
printf("%s\t",strptr); for(i=0;i<3;i++) printf("%f\t",(double)CRtrial[i]/(double)RTctrial[i]); printf("\n\n");

fflush(stdout);
}
