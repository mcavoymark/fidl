/* Copyright 3/23/07 Washington University.  All Rights Reserved.
   fidl_np645_simulation .c  $Revision: 1.5 $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>

static char rcsid[] = "$Header: /home/hannah/mcavoy/idl/clib/RCS/fidl_np645_simulation.c,v 1.5 2007/04/26 19:45:40 mcavoy Exp $";

double sub_rNC1NC2(double covC1C2,double covF1F2,double rNF1NC1,double varNF1,double sqrt_varNC2,double rNF2NC2,double varNF2,
    double sqrt_varNC1);

main(int argc,char **argv)
{
char *regnames=NULL,*out=NULL,*string_ptr,*fixation_cov=NULL,*closed_cov=NULL,rC1C2str[10],*pullout_file=NULL,*avg_rC1C2_file=NULL,
     *par_file="parameter.txt"; 
int i,j,k,l,m,ii,jj,kk,nrval,ncombo,npercent_varNO,nreg,npullout_percent_varNO=0,npullout_rval=0,npullout,inc_ii,inc_jj,flag,
    navg_rC1C2=0,num_regions=0,*roi;
double varF1=UNSAMPLED_VOXEL,varF2=UNSAMPLED_VOXEL,varC1=UNSAMPLED_VOXEL,varC2=UNSAMPLED_VOXEL,covF1F2=UNSAMPLED_VOXEL,
       covC1C2=UNSAMPLED_VOXEL,varNO1,varNO2,rNF1NC1,rNF2NC2,varNF1,varNF2,b2m4ac_NC1,sqrt_b2m4ac_NC1,mb_NC1,sqrt_varNC1_1,
       sqrt_varNC1_2,b2m4ac_NC2,sqrt_b2m4ac_NC2,mb_NC2,sqrt_varNC2_1,sqrt_varNC2_2,*rval,*percent_varNO,sqrt_varNC1,sqrt_varNC2,
       sol[4],*pullout_percent_varNO,*pullout_rval,rC1C2,td,avg_rC1C2; 
FILE *fp,*op,*avgp,*parp;
Data *dfixation_cov,*dclosed_cov;

if(argc < 13) {
    fprintf(stderr,"  Do one. \n");
    fprintf(stderr,"    -regnames: Identifier\n");
    fprintf(stderr,"    -varF1:   Variance region1 fixation.\n");
    fprintf(stderr,"    -varF2:   Variance region2 fixation.\n");
    fprintf(stderr,"    -varC1:   Variance region1 eyes closed.\n");
    fprintf(stderr,"    -varC2:   Variance region2 eyes closed.\n");
    fprintf(stderr,"    -covF1F2: Covariance region1 and region2 fixation.\n");
    fprintf(stderr,"    -covC1C2: Covariance region1 and region2 eyes closed.\n");
    fprintf(stderr,"    -out:     Output file.\n");
    fprintf(stderr,"  Do all. \n");
    fprintf(stderr,"    -fixation_cov:          Fixation covariance matrix. First column is region names.\n");
    fprintf(stderr,"    -closed_cov:            Closed covariance matrix. First column is region names.\n");
    fprintf(stderr,"    -pullout_percent_varNO: Print to a separate file.\n");
    fprintf(stderr,"    -pullout_rval:          Print to a separate file.\n");
    fprintf(stderr,"    -pullout_file:          Pullout output to be put in this file.\n");
    fprintf(stderr,"    -avg_rC1C2_file:        rC1C2 averaged over values of rval.\n");
    fprintf(stderr,"    -regions_of_interest:   First region is one.\n"); 
    exit(-1);
    }
for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-regnames") && argc > i+1)
        regnames = argv[++i];
    if(!strcmp(argv[i],"-varF1") && argc > i+1) {
        string_ptr = argv[i+1] + 1;
        if(*string_ptr == '.') string_ptr++;
        if(isdigit(*string_ptr)) varF1 = atof(argv[++i]);
        }
    if(!strcmp(argv[i],"-varF2") && argc > i+1) {
        string_ptr = argv[i+1] + 1;
        if(*string_ptr == '.') string_ptr++;
        if(isdigit(*string_ptr)) varF2 = atof(argv[++i]);
        }
    if(!strcmp(argv[i],"-varC1") && argc > i+1) {
        string_ptr = argv[i+1] + 1;
        if(*string_ptr == '.') string_ptr++;
        if(isdigit(*string_ptr)) varC1 = atof(argv[++i]);
        }
    if(!strcmp(argv[i],"-varC2") && argc > i+1) {
        string_ptr = argv[i+1] + 1;
        if(*string_ptr == '.') string_ptr++;
        if(isdigit(*string_ptr)) varC2 = atof(argv[++i]);
        }
    if(!strcmp(argv[i],"-covF1F2") && argc > i+1) {
        string_ptr = argv[i+1] + 1;
        if(*string_ptr == '.') string_ptr++;
        if(isdigit(*string_ptr)) covF1F2 = atof(argv[++i]);
        }
    if(!strcmp(argv[i],"-covC1C2") && argc > i+1) {
        string_ptr = argv[i+1] + 1;
        if(*string_ptr == '.') string_ptr++;
        if(isdigit(*string_ptr)) covC1C2 = atof(argv[++i]);
        }
    if(!strcmp(argv[i],"-out") && argc > i+1)
        out = argv[++i];
    if(!strcmp(argv[i],"-fixation_cov") && argc > i+1)
        fixation_cov = argv[++i];
    if(!strcmp(argv[i],"-closed_cov") && argc > i+1)
        closed_cov = argv[++i];
    if(!strcmp(argv[i],"-pullout_percent_varNO") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++npullout_percent_varNO;
        if(!(pullout_percent_varNO=malloc(sizeof*pullout_percent_varNO*npullout_percent_varNO))) {
            printf("Error: Unable to malloc pullout_percent_varNO\n");
            exit(-1);
            }
        for(j=0;j<npullout_percent_varNO;j++) pullout_percent_varNO[j] = atof(argv[++i]);
        }
    if(!strcmp(argv[i],"-pullout_rval") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++npullout_rval;
        if(!(pullout_rval=malloc(sizeof*pullout_rval*npullout_rval))) {
            printf("Error: Unable to malloc pullout_rval\n");
            exit(-1);
            }
        for(j=0;j<npullout_rval;j++) pullout_rval[j] = atof(argv[++i]);
        }
    if(!strcmp(argv[i],"-pullout_file") && argc > i+1)
        pullout_file = argv[++i];
    if(!strcmp(argv[i],"-avg_rC1C2_file") && argc > i+1)
        avg_rC1C2_file = argv[++i];
    if(!strcmp(argv[i],"-regions_of_interest") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_regions;
        if(!(roi=malloc(sizeof*roi*num_regions))) {
            printf("Error: Unable to malloc roi\n");
            exit(-1);
            }
        for(j=0;j<num_regions;j++) roi[j] = atoi(argv[++i]) - 1;
        }
    }
if(fixation_cov) {
    if(!closed_cov) {
        printf("Error: Need to specify -closed_cov\n");
        exit(-1);
        }
    if(!(dfixation_cov=read_data(fixation_cov))) exit(-1);
    if(!(dclosed_cov=read_data(closed_cov))) exit(-1);
    if((nreg=dfixation_cov->nsubjects)!=dclosed_cov->nsubjects) {
        printf("Error: Number of regions differ between files.\n");
        exit(-1);
        }
    for(i=0;i<nreg;i++) {
        if(strcmp(dclosed_cov->subjects[i],dfixation_cov->subjects[i])) {
            printf("Error: Regions differ between files.\n");
            exit(-1);
            }
        } 
    if(!(out=malloc(sizeof*out*MAXNAME))) {
        printf("Error: Unable to malloc out\n");
        exit(-1);
        }
    if((npullout=npullout_percent_varNO*npullout_rval)) {
        if(!pullout_file) {
            printf("Error: Need to specify -pullout_file\n");
            exit(-1);
            }
        if(!(op=fopen_sub(pullout_file,"w"))) exit(-1);
        fprintf(op,"**01**\tb2m4ac_NC1<0\n**02**\tb2m4ac_NC2<0\n\n");
        for(k=1,i=0;i<npullout_percent_varNO;i++) {
            for(j=0;j<npullout_rval;j++,k++) {
                fprintf(op,"rC1C2[%d]\tpercent_varNO=%f\trval=%f\n",k,pullout_percent_varNO[i],pullout_rval[j]);
                }
            }

        fprintf(op,"\nregion1\tregion2\tvarF1\tvarF2\tvarC1\tvarC2\tcovF1F2\tcovC1C2\t");
        for(i=0;i<npullout;i++) fprintf(op,"rC1C2[%d]\t",i+1);
        fprintf(op,"\n");

        /*fprintf(op,"\nregion1\tregion2\tvarF1\tvarF2\tvarC1\tvarC2\tcovF1F2\tcovC1C2\n");*/
        }
    if(avg_rC1C2_file) {
        #if 0
        if(!(avgp=fopen_sub(avg_rC1C2_file,"w"))) exit(-1);
        fprintf(avgp,"\nregion1\tregion2\tvarF1\tvarF2\tvarC1\tvarC2\tcovF1F2\tcovC1C2\n");
        #endif
        if(!(avgp=fopen_sub(avg_rC1C2_file,"w"))) exit(-1);
        if(!(parp=fopen_sub(par_file,"w"))) exit(-1);
        fprintf(parp,"\nregion1\tregion2\tvarF1\tvarF2\tvarC1\tvarC2\tcovF1F2\tcovC1C2\n");
        }
    if(num_regions) {
        nreg=num_regions;
        }
    else {
        if(!(roi=malloc(sizeof*roi*num_regions))) {
            printf("Error: Unable to malloc roi\n");
            exit(-1);
            }
        for(j=0;j<nreg;j++) roi[j]=j;
        }
    }
else {
    if(!regnames) {
        printf("Error: Need to specify -regnames\n");
        exit(-1);
        }
    if(varF1==(double)UNSAMPLED_VOXEL) {
        printf("Error: Need to specify -varF1\n");
        exit(-1);
        }
    if(varF2==(double)UNSAMPLED_VOXEL) {
        printf("Error: Need to specify -varF2\n");
        exit(-1);
        }
    if(varC1==(double)UNSAMPLED_VOXEL) {
        printf("Error: Need to specify -varC1\n");
        exit(-1);
        }
    if(varC2==(double)UNSAMPLED_VOXEL) {
        printf("Error: Need to specify -varC2\n");
        exit(-1);
        }
    if(covF1F2==(double)UNSAMPLED_VOXEL) {
        printf("Error: Need to specify -covF1F2\n");
        exit(-1);
        }
    if(covC1C2==(double)UNSAMPLED_VOXEL) {
        printf("Error: Need to specify -covC1C2\n");
        exit(-1);
        }
    if(!out) {
        printf("Error: Need to specify -out\n");
        exit(-1);
        }
    nreg = 2;
    }
print_version_number(rcsid,stdout);

#if 0
npercent_varNO=101;
if(!(percent_varNO=malloc(sizeof*percent_varNO*npercent_varNO))) {
    printf("Error: Unable to malloc percent_varNO\n");
    exit(-1);
    }
for(percent_varNO[0]=0.00,i=1;i<npercent_varNO;i++) percent_varNO[i]=percent_varNO[i-1]+0.01;
#endif
#if 0
npercent_varNO=1;
if(!(percent_varNO=malloc(sizeof*percent_varNO*npercent_varNO))) {
    printf("Error: Unable to malloc percent_varNO\n");
    exit(-1);
    }
percent_varNO[0]=0.;
#endif
npercent_varNO=99;
if(!(percent_varNO=malloc(sizeof*percent_varNO*npercent_varNO))) {
    printf("Error: Unable to malloc percent_varNO\n");
    exit(-1);
    }
for(percent_varNO[0]=0.01,i=1;i<npercent_varNO;i++) percent_varNO[i]=percent_varNO[i-1]+0.01;


#if 0
nrval=101;
if(!(rval=malloc(sizeof*rval*nrval))) {
    printf("Error: Unable to malloc rval\n");
    exit(-1);
    }
for(rval[0]=0.,i=1;i<nrval;i++) rval[i]=rval[i-1]+0.01;
#endif
nrval=99;
if(!(rval=malloc(sizeof*rval*nrval))) {
    printf("Error: Unable to malloc rval\n");
    exit(-1);
    }
for(rval[0]=0.01,i=1;i<nrval;i++) rval[i]=rval[i-1]+0.01;
/*for(i=0;i<nrval;i++) printf("%f ",rval[i]); printf("\n");*/


if(avg_rC1C2_file) {
    for(k=0;k<nreg;k++) fprintf(avgp,"%s\t",dfixation_cov->subjects[roi[k]]);
    fprintf(avgp,"\n");
    } 


/*for(m=k=0;k<nreg-1;k++) {*/
for(k=0;k<nreg;k++) {
/*for(k=0;k<1;k++) {*/
    if(fixation_cov) {
        varF1 = dfixation_cov->x[roi[k]][roi[k]];
        varC1 = dclosed_cov->x[roi[k]][roi[k]];
        }
    /*for(l=k+1;l<nreg;l++) {*/
    for(l=0;l<nreg;l++) {
    /*for(l=0;l<1;l++) {*/
        if(fixation_cov) {
            varF2 = dfixation_cov->x[roi[l]][roi[l]];
            varC2 = dclosed_cov->x[roi[l]][roi[l]];
            covF1F2 = dfixation_cov->x[roi[k]][roi[l]];
            covC1C2 = dclosed_cov->x[roi[k]][roi[l]];
            sprintf(out,"%s_%s.txt",dfixation_cov->subjects[roi[k]],dclosed_cov->subjects[roi[l]]);
            }
        if(!(fp=fopen_sub(out,"w"))) exit(-1);
        fprintf(fp,"%s %s\n",fixation_cov?dfixation_cov->subjects[roi[k]]:regnames,fixation_cov?dclosed_cov->subjects[roi[l]]:"");
        fprintf(fp,"varF1=%f varF2=%f varC1=%f varC2=%f covF1F2=%f covC1C2=%f\n\n",varF1,varF2,varC1,varC2,covF1F2,covC1C2);
        if(pullout_file) {
            fprintf(op,"%s\t%s\t",dfixation_cov->subjects[roi[k]],dclosed_cov->subjects[roi[l]]);

            fprintf(op,"%.4f\t%.4f\t%.4f\t%.4f\t%.4f\t%.4f\t",varF1,varF2,varC1,varC2,covF1F2,covC1C2);
            /*fprintf(op,"%.4f\t%.4f\t%.4f\t%.4f\t%.4f\t%.4f\n",varF1,varF2,varC1,varC2,covF1F2,covC1C2);*/
            }
        if(avg_rC1C2_file) {
            #if 0
            fprintf(avgp,"%s\t%s\t",dfixation_cov->subjects[roi[k]],dclosed_cov->subjects[roi[l]]);
            fprintf(avgp,"%.4f\t%.4f\t%.4f\t%.4f\t%.4f\t%.4f\n",varF1,varF2,varC1,varC2,covF1F2,covC1C2);
            #endif
            fprintf(parp,"%s\t%s\t%.4f\t%.4f\t%.4f\t%.4f\t%.4f\t%.4f\n",dfixation_cov->subjects[roi[k]],
                dclosed_cov->subjects[roi[l]],varF1,varF2,varC1,varC2,covF1F2,covC1C2);
            }

        /*printf("%s %s\n",fixation_cov?dfixation_cov->subjects[roi[k]]:regnames,fixation_cov?dclosed_cov->subjects[roi[l]]:"");
        printf("varF1=%f varF2=%f varC1=%f varC2=%f covF1F2=%f covC1C2=%f\n",varF1,varF2,varC1,varC2,covF1F2,covC1C2);*/
        
        /*for(kk=jj=j=0;j<npercent_varNO;j++) {*/
        for(avg_rC1C2=0.,navg_rC1C2=kk=jj=j=0;j<npercent_varNO;j++) {

            varNO1 = percent_varNO[j]*varF1;
            varNO2 = percent_varNO[j]*varF2;

            /*for(avg_rC1C2=0.,navg_rC1C2=ii=i=0;i<nrval;i++) {*/
            for(ii=i=0;i<nrval;i++) {

                varNF1 = varF1 - varNO1; 
                varNF2 = varF2 - varNO2; 
                rNF1NC1=rNF2NC2=rval[i];
                /*printf("varNF1=%f varNF2=%f rval[%d]=%f\n",varNF1,varNF2,i,rval[i]);*/

                sprintf(rC1C2str,"******");
                flag=0;
                if((b2m4ac_NC1=rNF1NC1*rNF1NC1*varNF1+varC1-varF1)<0.) {
                    printf("Error: b2m4ac_NC1=%f <0  Must be positive. percent_varNO[%d]=%f rval[%d]=%f\n",
                        b2m4ac_NC1,j,percent_varNO[j],i,rval[i]);
                    fprintf(fp,"percent_varNO=%f rval=%f **01**",percent_varNO[j],rval[i]); 
                    sprintf(rC1C2str,"**01**");
                    flag=1;
                    }
                else {
                    sqrt_b2m4ac_NC1 = sqrt(b2m4ac_NC1);
                    mb_NC1 = -rNF1NC1*sqrt(varNF1);
                    sqrt_varNC1_1 = mb_NC1 + sqrt_b2m4ac_NC1;
                    sqrt_varNC1_2 = mb_NC1 - sqrt_b2m4ac_NC1;
                     
                    /*printf("    sqrt_varNC1_1=%f sqrt_varNC1_2=%f\n",sqrt_varNC1_1,sqrt_varNC1_2);*/

                    if((b2m4ac_NC2=rNF2NC2*rNF2NC2*varNF2+varC2-varF2)<0.) {
                        printf("Error: b2m4ac_NC2=%f <0  Must be positive.\n",b2m4ac_NC2);
                        fprintf(fp,"percent_varNO=%f rval=%f **02**",percent_varNO[j],rval[i]); 
                        sprintf(rC1C2str,"**02**");
                        flag=1;
                        }
                    else {
                        sqrt_b2m4ac_NC2 = sqrt(b2m4ac_NC2);
                        mb_NC2 = -rNF2NC2*sqrt(varNF2);
                        sqrt_varNC2_1 = mb_NC2 + sqrt_b2m4ac_NC2;
                        sqrt_varNC2_2 = mb_NC2 - sqrt_b2m4ac_NC2;
                        /*printf("    sqrt_varNC2_1=%f sqrt_varNC2_2=%f\n",sqrt_varNC2_1,sqrt_varNC2_2);*/
        


                        #if 0
                        ncombo=0;
                        rC1C2=sol[0]=sol[1]=sol[2]=sol[3]=0.;
                        if((sqrt_varNC1_1*sqrt_varNC2_1)>0.) {
                            sqrt_varNC1=fabs(sqrt_varNC1_1);
                            sqrt_varNC2=fabs(sqrt_varNC2_1);
                            td=sub_rNC1NC2(covC1C2,covF1F2,rNF1NC1,varNF1,sqrt_varNC2,rNF2NC2,varNF2,sqrt_varNC1);
                            printf("    sqrt_varNC1_1 sqrt_varNC2_1 %f\n",td);
                            if(td>=0.0&&td<=1.0) {
                                sol[0]=td;
                                ncombo++;
                                rC1C2+=td;
                                }
                            }
                        if((sqrt_varNC1_1*sqrt_varNC2_2)>0.) {
                            sqrt_varNC1=fabs(sqrt_varNC1_1);
                            sqrt_varNC2=fabs(sqrt_varNC2_2);
                            td=sub_rNC1NC2(covC1C2,covF1F2,rNF1NC1,varNF1,sqrt_varNC2,rNF2NC2,varNF2,sqrt_varNC1);
                            printf("    sqrt_varNC1_1 sqrt_varNC2_2 %f\n",td);
                            if(td>=0.0&&td<=1.0) {
                                sol[1]=td;
                                ncombo++;
                                rC1C2+=td;
                                }
                            }
                        if((sqrt_varNC1_2*sqrt_varNC2_1)>0.) {
                            sqrt_varNC1=fabs(sqrt_varNC1_2);
                            sqrt_varNC2=fabs(sqrt_varNC2_1);
                            td=sub_rNC1NC2(covC1C2,covF1F2,rNF1NC1,varNF1,sqrt_varNC2,rNF2NC2,varNF2,sqrt_varNC1);
                            printf("    sqrt_varNC1_2 sqrt_varNC2_1 %f\n",td);
                            if(td>=0.0&&td<=1.0) {
                                sol[2]=td;
                                ncombo++;
                                rC1C2+=td;
                                }
                            }
                        if((sqrt_varNC1_2*sqrt_varNC2_2)>0.) {
                            sqrt_varNC1=fabs(sqrt_varNC1_2);
                            sqrt_varNC2=fabs(sqrt_varNC2_2);
                            td=sub_rNC1NC2(covC1C2,covF1F2,rNF1NC1,varNF1,sqrt_varNC2,rNF2NC2,varNF2,sqrt_varNC1);
                            printf("    sqrt_varNC1_2 sqrt_varNC2_2 %f\n",td);
                            if(td>=0.0&&td<=1.0) {
                                sol[3]=td;
                                ncombo++;
                                rC1C2+=td;
                                }
                            }
                        rC1C2/=(double)ncombo;
                        if(rC1C2>=rval[i]) {
                            fprintf(fp,"percent_varNO=%f rval=%f rNC1NC2=%f\n",percent_varNO[j],rval[i],rC1C2);
                            avg_rC1C2+=rC1C2;
                            navg_rC1C2++;
                            }
                        #endif

                        ncombo=0;
                        rC1C2=0.;
                        if(sqrt_varNC1_1>0.&&sqrt_varNC2_1>0.) {
                            sqrt_varNC1=sqrt_varNC1_1;
                            sqrt_varNC2=sqrt_varNC2_1;
                            ncombo++;
                            }
                        if(sqrt_varNC1_1>0.&&sqrt_varNC2_2>0.) {
                            sqrt_varNC1=sqrt_varNC1_1;
                            sqrt_varNC2=sqrt_varNC2_2;
                            ncombo++;
                            }
                        if(sqrt_varNC1_2>0.&&sqrt_varNC2_1>0.) {
                            sqrt_varNC1=sqrt_varNC1_2;
                            sqrt_varNC2=sqrt_varNC2_1;
                            ncombo++;
                            }
                        if(sqrt_varNC1_2>0.&&sqrt_varNC2_2>0.) {
                            sqrt_varNC1=sqrt_varNC1_2;
                            sqrt_varNC2=sqrt_varNC2_2;
                            ncombo++;
                            }
                        if(ncombo>1) {
                            printf("Error: ncombo=%d  Should be 1.\n",ncombo);
                            exit(-1);
                            }
                        td=sub_rNC1NC2(covC1C2,covF1F2,rNF1NC1,varNF1,sqrt_varNC2,rNF2NC2,varNF2,sqrt_varNC1);
                        /*printf("    sqrt_varNC1_1 sqrt_varNC2_1 %f\n",td);*/

                        /*if(td>=0.0&&td<=1.0001) rC1C2=td;*/
                        if(td>=0.0) {
                            if((rC1C2=td)>1.0) rC1C2=1.0;
                            }

                        if(rC1C2>=rval[i]) {
                        /*if(fabs(rC1C2-rval[i])>=0.0001) {*/
                            fprintf(fp,"percent_varNO=%f rval=%f rNC1NC2=%f\n",percent_varNO[j],rval[i],rC1C2);
                            avg_rC1C2+=rC1C2;
                            navg_rC1C2++;
                            /*fprintf(fp,"    avg_rC1C2=%f navg_rC1C2=%d\n",avg_rC1C2,navg_rC1C2);*/
                            }
                        else {
                            /*fprintf(fp,"%f\n",fabs(rC1C2-rval[i]));*/
                            fprintf(fp,"percent_varNO=%f rval=%.24f **03**td=%.24f rC1C2=%.24f\n",percent_varNO[j],rval[i],td,rC1C2);
                            }
                        sprintf(rC1C2str,"%.4f",rC1C2);
                        }
                    }
                inc_jj=inc_ii=0; 
                if(pullout_file) {
                    if(fabs(percent_varNO[j]-pullout_percent_varNO[jj])<.001) inc_jj=1;
                    if(fabs(rval[i]-pullout_rval[ii])<.001) inc_ii=1;

                    #if 0
                    printf("j=%d i=%d jj=%d ii=%d inc_jj=%d inc_ii=%d  ",j,i,jj,ii,inc_jj,inc_ii);
                    printf("percent_varNO[%d]=%f rval[%d]=%f\n",j,percent_varNO[j],i,rval[i]);
                    printf("    pullout_percent_varNO[%d]=%f pullout_rval[%d]=%f\n",jj,pullout_percent_varNO[jj],ii,pullout_rval[ii]);
                    printf("    rval[%d]-pullout_rval[%d]=%f",i,ii,rval[i]-pullout_rval[ii]);
                    #endif

                    if(inc_jj&&inc_ii) {
                        fprintf(op,"%s\t",rC1C2str);
                        #if 0
                        fprintf(op,"\trC1C2[%d]=\t%s\n",kk++,rC1C2str);
                        if(!flag) {
                            fprintf(op,"\t\tr\t %.4f\t%.4f\t%.4f\t%.4f\n",sol[0],sol[1],sol[2],sol[3]);
                            fprintf(op,"\t\troots\t %.4f\t%.4f\t%.4f\t%.4f\t\n",sqrt_varNC1_1,sqrt_varNC1_2,sqrt_varNC2_1,
                                sqrt_varNC2_2);
                            }
                        #endif
                        }
                    }
                if(inc_ii && ii<(npullout_rval-1)) ii++;
                } /*for(ii=i=0;i<nrval;i++)*/

            #if 0
            avg_rC1C2/=(double)navg_rC1C2;
            if(avg_rC1C2_file) fprintf(avgp,"\tpercent_varNO=%f %.4f\n",percent_varNO[j],avg_rC1C2);
            #endif

            fprintf(fp,"\n");
            if(inc_jj && jj<(npullout_percent_varNO-1)) jj++;
            } /*for(jj=j=0;j<npercent_varNO;j++)*/

        /*printf("avg_rC1C2=%f navg_rC1C2=%d\n",avg_rC1C2,navg_rC1C2);*/
        avg_rC1C2/=(double)navg_rC1C2;
        /*if(avg_rC1C2_file) fprintf(avgp,"\t%.4f\n",avg_rC1C2);*/
        if(avg_rC1C2_file) fprintf(avgp,"%.4f\t",avg_rC1C2);

        fclose(fp);
        printf("Output written to %s\n",out);
        if(pullout_file) fprintf(op,"\n");
        }
    if(avg_rC1C2_file) fprintf(avgp,"\n");
    }
if(pullout_file) {
    fflush(op);
    fclose(op);
    printf("Output written to %s\n",pullout_file);
    }
if(avg_rC1C2_file) {
    fflush(avgp);
    fclose(avgp);
    printf("Output written to %s\n",avg_rC1C2_file);
    fflush(parp);
    fclose(parp);
    printf("Output written to %s\n",par_file);
    }
}
    
double sub_rNC1NC2(double covC1C2,double covF1F2,double rNF1NC1,double varNF1,double sqrt_varNC2,double rNF2NC2,double varNF2,
    double sqrt_varNC1)
{
    #if 0
    printf("sqrt_varNC1=%f sqrt_varNC2=%f\n",sqrt_varNC1,sqrt_varNC2);
    printf("numerator=%f\n",covC1C2-covF1F2-rNF1NC1*sqrt(varNF1)*sqrt_varNC2-rNF2NC2*sqrt(varNF2)*sqrt_varNC1);
    printf("denominator=%f\n",sqrt_varNC1*sqrt_varNC2);
    printf("covC1C2=%f covF1F2=%f rNF1NC1=%f sqrt(varNF1)=%f sqrt_varNC2=%f rNF2NC2=%f sqrt(varNF2)=%f sqrt_varNC1=%f\n",
        covC1C2,covF1F2,rNF1NC1,sqrt(varNF1),sqrt_varNC2,rNF2NC2,sqrt(varNF2),sqrt_varNC1);
    #endif
    return (covC1C2-covF1F2-rNF1NC1*sqrt(varNF1)*sqrt_varNC2-rNF2NC2*sqrt(varNF2)*sqrt_varNC1)/(sqrt_varNC1*sqrt_varNC2);
    /*return (covC1C2-covF1F2-.1*sqrt(varNF1)*sqrt_varNC2-.1*sqrt(varNF2)*sqrt_varNC1)/(sqrt_varNC1*sqrt_varNC2);*/
}
