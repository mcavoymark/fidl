/* Copyright 2/6/07 Washington University. All Rights Reserved.
   fidl_ac.c  $Revision: 1.4 $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>

static char rcsid[] = "$Header: /home/hannah/mcavoy/idl/clib/RCS/fidl_ac.c,v 1.4 2008/01/17 00:01:20 mcavoy Exp $";

main(int argc,char **argv)
{
char *scratchdir=NULL,string[MAXNAME];
int i,j,k,l,m,nconcs=0,nruns=0,nroots=0,nregions=0,argc_runs,*temp_int,*tdim_tc,tdim_tcmax,vol,lenvol,nsub,lcsem=0;
double TR,*temp_double,*avg,*sum,*sum2,*sem,td;
Files_Struct *concs,*roots,*regions,**concss;
TC *tcs;
Interfile_header *ifh;
Memory_Map *mm;
FILE *fp;

if(argc < 5) {
    fprintf(stderr,"This program averages and assembles autocorrelation files computed by autocorr_guts.\n");
    fprintf(stderr,"    -concs:    Conc files for precomputed autocorrelation files.\n");
    fprintf(stderr,"    -runs:     Which runs to be assembled and averaged.\n");
    fprintf(stderr,"    -roots:    Output roots.\n");
    fprintf(stderr,"    -regions:  Region names.\n");
    fprintf(stderr,"    -TR:       Used to determine the lag.\n");
    fprintf(stderr,"    -sem:      Output standard error of the mean.\n");
    fprintf(stderr,"    -clean_up: Name of scratch directory to remove.\n");
    exit(-1);
    }
for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-concs") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nconcs;
        if(!(concs=get_files(nconcs,&argv[i+1]))) exit(-1);
        i += nconcs;
        }
    if(!strcmp(argv[i],"-runs") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nruns;
        argc_runs = i+1;
        i += nruns;
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
    if(!strcmp(argv[i],"-TR") && argc > i+1)
        TR = atof(argv[++i]);
    if(!strcmp(argv[i],"-sem"))
        lcsem = 1;
    if(!strcmp(argv[i],"-clean_up") && argc > i+1)
        scratchdir = argv[++i];
    }
print_version_number(rcsid,stdout);
if(!nconcs) {
    printf("Error: Need to specify -concs\n");
    exit(-1);
    }
if(!nruns) {
    printf("Error: Need to specify -runs\n");
    exit(-1);
    }
if(!nroots) {
    printf("Error: Need to specify -roots\n");
    exit(-1);
    }
if(!nregions) {
    printf("Error: No output set up for a voxel level analysis.\n");
    exit(-1);
    }

if(!(concss=malloc(sizeof*concss*nconcs))) {
    printf("Error: Unable to allocate concss\n");
    exit(-1);
    }
for(i=0;i<nconcs;i++) if(!(concss[i]=read_conc(concs->files[i]))) exit(-1); 

if(!(temp_int=malloc(sizeof*temp_int*nruns))) {
    printf("Error: Unable to malloc temp_int\n");
    exit(-1);
    }
for(i=0;i<nruns;i++) temp_int[i] = nconcs;
if(!(tcs=read_tc_string_new(nruns,temp_int,argc_runs,argv))) exit(-1);
free(temp_int);

#if 0
printf("tcs->num_tc=%d nruns=%d\n",tcs->num_tc,nruns);
printf("tcs->num_tc_to_sum="); for(i=0;i<tcs->num_tc;i++) printf("%d ",tcs->num_tc_to_sum[i][0]); printf("\n");
printf("tcs->each="); for(i=0;i<tcs->num_tc;i++) printf("%d ",tcs->each[i]); printf("\n");
printf("tcs->tc=\n");
for(i=0;i<tcs->num_tc;i++) {
    for(j=0;j<tcs->num_tc_to_sum[i][0];j++) printf("%d ",(int)tcs->tc[i][0][j]);
    printf("\n");
    }
#endif

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
for(j=k=0;k<tcs->num_tc;k++) {
    for(l=m=0;m<tcs->each[k];m++) {
        if((int)tcs->tc[k][m][0]==-1) continue;
        for(i=0;i<tcs->num_tc_to_sum[k][m];i++,j++,l++) {
            if(!(ifh=read_ifh(concss[m]->files[(int)tcs->tc[k][m][i]-1]))) exit(-1);
            if(!j) {
                vol = ifh->dim4;
                if(nregions) {
                    if(vol!=nregions) {
                        printf("vol=%d nregions=%d  Must be the same. Abort!\n",vol,nregions);
                        exit(-1);
                        }
                    }
                }
            else if(ifh->dim4!=vol) {
                printf("Error: vol=%d ifh->dim4=%d  Must be the same. Abort!\n",vol,ifh->dim4);
                exit(-1);
                }
            if(!l) {
                tdim_tc[k] = ifh->dim1;
                }
            else if(ifh->dim1!=tdim_tc[k]) {
                printf("Error: tdim_tc[k]=%d ifh->dim1=%d  Must be the same. Abort!\n",k,tdim_tc[k],ifh->dim1);
                exit(-1);
                }
            free_ifh(ifh,(int)FALSE);
            }
        }
    }
for(tdim_tcmax=k=0;k<tcs->num_tc;k++) if(tdim_tc[k]>tdim_tcmax) tdim_tcmax=tdim_tc[k];
if(!(temp_double=malloc(sizeof*temp_double*tdim_tcmax*vol))) {
    printf("Error: Unable to malloc temp_double\n");
    exit(-1);
    }


#if 0
for(k=0;k<tcs->num_tc;k++) {
    /*printf("k=%d\n",k);*/
    lenvol = tdim_tc[k]*vol; 
    if(!(avg=malloc(sizeof*avg*lenvol))) {
        printf("Error: Unable to malloc avg\n");
        exit(-1);
        }
    for(m=0;m<lenvol;m++) avg[m]=0.;

    for(nsub=m=0;m<tcs->each[k];m++) {
        if((int)tcs->tc[k][m][0]==-1) continue;
        for(i=0;i<lenvol;i++) temp_double[i]=0.;
        for(i=0;i<tcs->num_tc_to_sum[k][m];i++) {
            l = (int)tcs->tc[k][m][i]-1;
            if(!(mm=map_disk_double(concss[m]->files[l],lenvol,0))) exit(-1);
            for(j=0;j<lenvol;j++) temp_double[j] += mm->dptr[j];        
            if(!unmap_disk(mm)) exit(-1);
            }
        for(i=0;i<lenvol;i++) avg[i]+=temp_double[i]/(double)tcs->num_tc_to_sum[k][m];
        nsub++;
        }
    for(m=0;m<lenvol;m++) avg[m]/=(double)nsub;

    if(nregions) {
        sprintf(string,"%s.txt",roots->files[k]);
        if(!(fp=fopen_sub(string,"w"))) exit(-1); 
        fprintf(fp,"lag\t");
        for(m=0;m<nregions;m++) fprintf(fp,"%s\t",regions->files[m]);
        fprintf(fp,"\n");
        for(m=0;m<tdim_tc[k];m++) {
            fprintf(fp,"%.4f\t",(double)m*TR);
            for(l=m,i=0;i<nregions;i++,l+=tdim_tc[k]) fprintf(fp,"%.4f\t",avg[l]);
            fprintf(fp,"\n");
            }
        fclose(fp);
        printf("Output written to %s\n",string);
        }

    free(avg);
    }
#endif

for(k=0;k<tcs->num_tc;k++) {
    lenvol = tdim_tc[k]*vol;
    if(!(sum=malloc(sizeof*sum*lenvol))) {
        printf("Error: Unable to malloc sum\n");
        exit(-1);
        }
    for(m=0;m<lenvol;m++) sum[m]=0.;
    if(!(sum2=malloc(sizeof*sum2*lenvol))) {
        printf("Error: Unable to malloc sum2\n");
        exit(-1);
        }
    for(m=0;m<lenvol;m++) sum2[m]=0.;
    if(!(avg=malloc(sizeof*avg*lenvol))) {
        printf("Error: Unable to malloc avg\n");
        exit(-1);
        }
    for(m=0;m<lenvol;m++) avg[m]=0.;
    if(!(sem=malloc(sizeof*sem*lenvol))) {
        printf("Error: Unable to malloc sem\n");
        exit(-1);
        }
    for(m=0;m<lenvol;m++) sem[m]=0.;

    for(nsub=m=0;m<tcs->each[k];m++) {
        if((int)tcs->tc[k][m][0]==-1) continue;
        for(i=0;i<lenvol;i++) temp_double[i]=0.;
        for(i=0;i<tcs->num_tc_to_sum[k][m];i++) {
            l = (int)tcs->tc[k][m][i]-1;
            if(!(mm=map_disk_double(concss[m]->files[l],lenvol,0))) exit(-1);
            for(j=0;j<lenvol;j++) temp_double[j] += mm->dptr[j];
            if(!unmap_disk(mm)) exit(-1);
            }
        for(i=0;i<lenvol;i++) {
            td=temp_double[i]/(double)tcs->num_tc_to_sum[k][m];
            sum[i]+=td;
            sum2[i]+=td*td;
            }
        nsub++;
        }
    for(m=0;m<lenvol;m++) {
        avg[m]=sum[m]/(double)nsub;
        sem[m]=sqrt((sum2[m]-sum[m]*sum[m]/nsub)/(nsub*(nsub-1)));
        }
    if(nregions) {
        sprintf(string,"%s.txt",roots->files[k]);
        if(!(fp=fopen_sub(string,"w"))) exit(-1);
        fprintf(fp,"lag\t");
        for(m=0;m<nregions;m++) fprintf(fp,"%s\t",regions->files[m]);
        fprintf(fp,"\n");
        for(m=0;m<tdim_tc[k];m++) {
            fprintf(fp,"%.4f\t",(double)m*TR);
            for(l=m,i=0;i<nregions;i++,l+=tdim_tc[k]) {
                fprintf(fp,"%.4f\t",avg[l]);
                if(lcsem) fprintf(fp,"%.4f\t",sem[l]);
                }
            fprintf(fp,"\n");
            }
        fclose(fp);
        printf("Output written to %s\n",string);
        }
    free(sem);
    free(avg);
    free(sum2);
    free(sum);
    }










if(scratchdir) {
    sprintf(string,"rm -rf %s",scratchdir);
    if(system(string) == -1) printf("Error: Unable to %s\n",string);
    }
}
