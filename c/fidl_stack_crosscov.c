/* Copyright 1/27/10 Washington University. All Rights Reserved.
   fidl_stack_crosscov.c  $Revision: 1.2 $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>
#include <gsl/gsl_eigen.h>

static char rcsid[] = "$Header: /home/hannah/mcavoy/idl/clib/RCS/fidl_stack_crosscov.c,v 1.2 2010/02/10 23:12:17 mcavoy Exp $";

main(int argc,char **argv)
{
char *strptr,*scratchdir=NULL,rootname[MAXNAME],string[MAXNAME];
int i,j,k,l,m,n,o,ii,i1,j1,k1,k2,kk,idx,l1,l2,m1,m2,n1,nconcs=0,nruns=0,nroots=0,nregions=0,argc_runs,*vol,lenvol,SunOS_Linux,
    nconcpersub=0,*concpersub,concpersubmax,regmax,dim4,maxlik_unbias=1,nfiles,*ti,lc_names_only=0,*dim1; 
float *temp_float;
double *temp_double,*corr;
Files_Struct *concs,*roots,*regions,**concss;
Interfile_header *ifh;
FILE *fp;
if(argc < 5) {
    fprintf(stderr,"This program averages and assembles autocorrelation files computed by autocorr_guts.\n");
    fprintf(stderr,"    -concs:       Conc files for precomputed autocorrelation files.\n");
    fprintf(stderr,"    -roots:       Output roots.\n");
    fprintf(stderr,"    -regions:     Region names.\n");
    fprintf(stderr,"    -scratchdir:  Name of scratch directory to remove.\n");
    fprintf(stderr,"    -concpersub:  Number of concs for each subject. One number per subject.\n");
    fprintf(stderr,"    -crosscorr    Compute cross correlation.\n");
    exit(-1);
    }
for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-concs") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nconcs;
        if(!(concs=get_files(nconcs,&argv[i+1]))) exit(-1);
        i += nconcs;
        }
    if(!strcmp(argv[i],"-roots") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nroots;
        if(!(roots=get_files(nroots,&argv[i+1]))) exit(-1);
        i += nroots;
        }
    if(!strcmp(argv[i],"-regions") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nregions;
        if(!(regions=get_files(nregions,&argv[i+1]))) exit(-1);
        i += nregions;
        }
    if(!strcmp(argv[i],"-scratchdir") && argc > i+1)
        scratchdir = argv[++i];
    if(!strcmp(argv[i],"-concpersub") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nconcpersub;
        if(!(concpersub=malloc(sizeof*concpersub*nconcpersub))) {
            printf("Error: Unable to malloc concpersub\n");
            exit(-1);
            }
        for(concpersubmax=j=0;j<nconcpersub;j++) if((concpersub[j]=atoi(argv[++i]))>concpersubmax) concpersubmax=concpersub[j];
        }
    if(!strcmp(argv[i],"-crosscorr"))
        maxlik_unbias=-1;
    }
print_version_number(rcsid,lc_names_only?stderr:stdout);
if((SunOS_Linux=checkOS())==-1) exit(-1);
if(!nconcs) {
    printf("Error: Need to specify -concs\n");
    exit(-1);
    }
if(!nroots) {
    printf("Error: Need to specify -roots\n");
    exit(-1);
    }
if(!nconcpersub) {
    nconcpersub = nconcs;
    if(!(concpersub=malloc(sizeof*concpersub*nconcpersub))) {
        printf("Error: Unable to malloc concpersub\n");
        exit(-1);
        }
    for(i=0;i<nconcpersub;i++) concpersub[i]=1;
    }
if(lc_names_only) {
    if(!scratchdir) if(!(scratchdir=make_scratchdir())) exit(-1);
    for(i=0;i<nconcpersub;i++) {
        sprintf(rootname,"%s%s_%s",scratchdir?scratchdir:"",roots->files[i],maxlik_unbias==-1?"crosscorr":"crosscov");
        sprintf(string,"%s.conc",rootname);
        }
    printf("Concatenated file written to %s\n",string);
    exit(0);
    }
if(!nregions) {
    printf("Voxel level analysis.\n");
    }
if(!(concss=malloc(sizeof*concss*nconcs))) {
    printf("Error: Unable to allocate concss\n");
    exit(-1);
    }
for(i=0;i<nconcs;i++) if(!(concss[i]=read_conc(concs->files[i]))) exit(-1); 

for(nfiles=concss[0]->nfiles,i1=i=0;i<nconcpersub;i++) {
    for(j=0;j<concpersub[i];j++,i1++) {
        if(nfiles!=concss[i1]->nfiles) {
            printf("Error: nfiles=%d concss[%d]->nfiles=%d Must be equal.\n",nfiles,i1,concss[i1]->nfiles);
            exit(-1);
            }
        }
    }
if(!(vol=malloc(sizeof*vol*nconcs*nfiles))) {
    printf("Error: Unable to malloc vol\n");
    exit(-1);
    }
if(!(dim1=malloc(sizeof*dim1*nconcs*nfiles))) {
    printf("Error: Unable to malloc dim1\n");
    exit(-1);
    }
if(!(ti=malloc(sizeof*ti*nregions))) {
    printf("Error: Unable to malloc ti\n");
    exit(-1);
    }
for(dim4=-1,nfiles=concss[0]->nfiles,regmax=k1=i1=i=0;i<nconcpersub;i++) {
    for(j=0;j<nregions;j++) ti[j]=0;
    for(j1=j=0;j<concpersub[i];j++,i1++,j1+=l2) {
        for(k=0;k<nfiles;k++,k1++) {
            if(!(ifh=read_ifh(concss[i1]->files[k]))) exit(-1);
            if(dim4==-1) {
                dim4 = ifh->dim4; 
                }
            else if(dim4!=ifh->dim4) {
                printf("Error: dim4=%d ifh->dim4=%d Must be equal.\n",dim4,ifh->dim4);
                exit(-1);
                }
            dim1[k1]=ifh->dim1;
            if((vol[k1]=ifh->dim1*ifh->dim2*ifh->dim3)!=ifh->nregions) {
                printf("Error: vol=%d ifh->nregions=%d Must be equal.\n",vol[k1],ifh->nregions);
                exit(-1);
                }
            if(regmax<ifh->nregions) regmax=ifh->nregions;
            for(l1=j1,l=0;l<(l2=ifh->nregions);l++,l1++) {
                strcpy(string,ifh->region_names[l]); /*gets decimated*/
                if(!(strptr=strtok(string," "))) {
                    printf("Error: No token found - spot 1. Abort!\n");
                    exit(-1);
                    }
                if(!(strptr = strtok((char*)NULL," "))) {
                    printf("Error: No token found - spot 2. Abort!\n");
                    exit(-1);
                    }
                if(!strcmp(strptr,regions->files[l1])) ti[l1]++;
                }
            free_ifh(ifh,0);
            }
        }
    for(j=0;j<nregions;j++) {
        if(ti[j]!=nfiles) {
            printf("Error: ti[%d]=%d nfiles=%d Must be equal.\n",j,ti[j],nfiles);
            exit(-1);
            }
        }
    }
free(ti);
printf("regmax=%d dim4=%d\n",regmax,dim4); /*regmax=10 dim4 = 106*/
fflush(stdout);
if(!(temp_float=malloc(sizeof*temp_float*dim4*regmax))) {
    printf("Error: Unable to malloc temp_float\n");
    exit(-1);
    }
if(!(temp_double=malloc(sizeof*temp_double*dim4*nregions))) {
    printf("Error: Unable to malloc temp_double\n");
    exit(-1);
    }
if(!(corr=malloc(sizeof*corr*dim4))) {
    printf("Error: Unable to malloc corr\n");
    exit(-1);
    }

if(!(ifh=init_ifh(4,dim4-maxlik_unbias,1,1,nregions*nregions,1,1,1,!SunOS_Linux?1:0))) exit(-1);
ifh->number_format = (int)DOUBLE_IF;
ifh->dof_condition = (float)(dim4-maxlik_unbias); 
ifh->nregions = nregions;
ifh->region_names = regions->files;
for(i1=i=0;i<nconcpersub;i1+=concpersub[i++]) {
    sprintf(rootname,"%s%s_%s",scratchdir?scratchdir:"",roots->files[i],maxlik_unbias==-1?"crosscorr":"crosscov");
    if(!write_conc(rootname,nfiles,concss[i1]->identify)) exit(-1);
    for(j=0;j<nfiles;j++) {
        for(k1=i1,k2=i1*nfiles,kk=k=0;k<concpersub[j];k++,k1++,k2+=nfiles) {
            strptr = concss[k1]->files[j];
            printf("strptr=%s\n",strptr);
            lenvol = vol[k2+j]*dim4;
            if(!readstack(strptr,(float*)temp_float,sizeof(float),(size_t)lenvol,SunOS_Linux)) exit(-1);

            /*for(m=0;m<lenvol;m++) printf("temp_float[%d]=%f\n",m,temp_float[m]);*/

            #if 0
            for(m2=m1=m=0;m<dim4;m++,m2+=nregions) {
                for(n1=m2+kk,n=0;n<ifh->dim1;n++,m1++,n1++) temp_double[n1] = (double)temp_float[m1];
                }
            #endif
            for(m2=m1=m=0;m<dim4;m++,m2+=nregions) {
                for(n1=m2+kk,n=0;n<dim1[k2+j];n++,m1++,n1++) {
                    /*printf("dim1[%d]=%d dim4=%d lenvol=%d temp_float[%d]=%f\n",k2+j,dim1[k2+j],dim4,lenvol,m1,temp_float[m1]);*/
                    temp_double[n1] = (double)temp_float[m1];
                    }
                } 
            kk += vol[k2+j];
            }
        sprintf(string,"%s_b%d.4dfp.img",rootname,j+1);
        if(!crosscorr_guts((char*)NULL,string,corr,nregions,dim4,maxlik_unbias,temp_double)) exit(-1);
        if(!write_ifh(string,ifh,0)) return 0;
        printf("Output written to %s\n",string);
        }
    }
}

#if 0
/*KEEP*/
if(!(temp_int=malloc(sizeof*temp_int*nruns))) {
    printf("Error: Unable to malloc temp_int\n");
    exit(-1);
    }
for(i=0;i<nruns;i++) temp_int[i] = nconcs;
if(!(tcs=read_tc_string_new(nruns,temp_int,argc_runs,argv))) exit(-1);
free(temp_int);
#endif
#if 0
printf("tcs->num_tc=%d nruns=%d tcs->total=%d\n",tcs->num_tc,nruns,tcs->total);
printf("tcs->num_tc_to_sum="); for(i=0;i<tcs->num_tc;i++) printf("%d ",tcs->num_tc_to_sum[i][0]); printf("\n");
printf("tcs->each="); for(i=0;i<tcs->num_tc;i++) printf("%d ",tcs->each[i]); printf("\n");
printf("tcs->tc=\n");
for(i=0;i<tcs->num_tc;i++) {
    for(j=0;j<tcs->num_tc_to_sum[i][0];j++) printf("%d ",(int)tcs->tc[i][0][j]);
    printf("\n");
    }
#endif
#if 0
for(i=0;i<tcs->num_tc;i++) {
    if(tcs->each[i]!=nconcs) {
        printf("nconcs=%d tcs->each[%d]=%d  Must be the same. Abort!\n",nconcs,i,tcs->each[i]);
        exit(-1);
        }
    }
if(!(tdim_tc=malloc(sizeof*tdim_tc*tcs->num_tc))) {
    printf("Error: Unable to malloc tdim_tc\n");
    exit(-1);
    }
if(!(dof=malloc(sizeof*dof*tcs->total))) {
    printf("Error: Unable to malloc dof\n");
    exit(-1);
    }
#endif
#if 0
KEEP
for(i=0;i<tcs->num_tc;i++) {
    for(j1=j=0;j<nconcpersub;j1+=concpersub[j++]) {
        for(l=0;l<tcs->num_tc_to_sum[i][j1];l++) {
            for(k1=j1,kk=k=0;k<concpersub[j];k++,k1++) {

                /*idx = (int)tcs->tc[i][k1][l]-1;*/
                /*printf("concss[%d]->files[%d]=%s\n",k1,idx,concss[k1]->files[idx]);*/

                strptr = concss[k1]->files[(int)tcs->tc[i][k1][l]-1];
                printf("strptr=%s\n",strptr);
                if(!(ifh=read_ifh(strptr))) exit(-1);
                lenvol = ifh->dim1*ifh->dim2*ifh->dim3*ifh->dim4;
                if(!readstack(strptr,(float*)temp_float,sizeof(float),(size_t)lenvol,SunOS_Linux)) exit(-1);
                if(shouldiswap(SunOS_Linux,ifh->bigendian)) swap_bytes((unsigned char *)temp_float,sizeof(float),(size_t)lenvol);
               for(m2=m1=m=0;m<dim4;m++,m2+=nregions) {
                    for(n1=m2+kk,n=0;n<ifh->dim1;n++,m1++,n1++) temp_double[n1] = (double)temp_float[m1];
                    }
                }
            kk += ifh->dim1;
            }
        }
    }
#endif
