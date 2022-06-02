/* Copyright 8/23/10 Washington University. All Rights Reserved.
   fidl_pca.c  $Revision: 1.5 $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>
#include <gsl/gsl_eigen.h>

static char rcsid[] = "$Header: /home/hannah/mcavoy/idl/clib/RCS/fidl_pca.c,v 1.5 2010/12/23 22:52:39 mcavoy Exp $";

main(int argc,char **argv)
{
char *scratchdir=NULL,filename[MAXNAME];
int i,j,k,m,i1,j1,k1,m1,nconcs=0,nruns=0,nroots=0,nregions=0,argc_runs,*temp_int,*tdim_tc,vol,nsub,lcclean_up=0,max_each,SunOS_Linux,
    maxlik_unbias=1,lcpercentchange=0,lccor=0,*tdim_tc_all,tdim_sum,lcscale_by_var=0,nsubroots=0;
double *temp_double,*stat,*cov,*eval,**evec,td,td1,ss,sum,sum2,var;
Files_Struct *concs,*roots,*regions,**concss,*subroots;
TC *tcs;
Interfile_header *ifh;
Memory_Map *mm;
FILE *fp;
if(argc < 5) {
    fprintf(stderr,"    -concs:         Conc files for precomputed autocorrelation files.\n");
    fprintf(stderr,"    -runs:          Which runs to be assembled and averaged.\n");
    fprintf(stderr,"    -roots:         Output roots for -runs.\n");
    fprintf(stderr,"    -regions:       Region names.\n");
    fprintf(stderr,"    -scratchdir:    Name of scratch directory to remove.\n");
    fprintf(stderr,"    -percent_change Concs are in percent change. Invokes a different normalizing scalar.\n");
    fprintf(stderr,"    -scale_by_var   Grand covariance matrix is formed by normalizing to each subject's overall variance.\n");
    fprintf(stderr,"    -cor            Compute correlation matrix instead of covariance matrix.\n");
    fprintf(stderr,"    -subroots:      Individual subject identifiers.\n");
    fprintf(stderr,"    -clean_up       Remove scratch directory to remove.\n");
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
    if(!strcmp(argv[i],"-scratchdir") && argc > i+1)
        scratchdir = argv[++i];
    if(!strcmp(argv[i],"-percent_change"))
        lcpercentchange=1;
    if(!strcmp(argv[i],"-scale_by_var"))
        lcscale_by_var=1;
    if(!strcmp(argv[i],"-cor"))
        lccor=1;
    if(!strcmp(argv[i],"-subroots") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nsubroots;
        if(!(subroots=get_files(nsubroots,&argv[i+1]))) exit(-1);
        i += nsubroots;
        }
    if(!strcmp(argv[i],"-clean_up"))
        lcclean_up=1;
    }
print_version_number(rcsid,stdout);
if((SunOS_Linux=checkOS())==-1) exit(-1);
if(!nconcs) {
    printf("Error: Need to specify -concs\n");
    exit(-1);
    }
if(!nruns) {
    printf("Error: Need to specify -runs\n");
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

#if 1
printf("tcs->num_tc=%d nruns=%d tcs->total=%d\n",tcs->num_tc,nruns,tcs->total);
printf("tcs->num_tc_to_sum="); for(i=0;i<tcs->num_tc;i++) printf("%d ",tcs->num_tc_to_sum[i][0]); printf("\n");
printf("tcs->each="); for(i=0;i<tcs->num_tc;i++) printf("%d ",tcs->each[i]); printf("\n");
printf("tcs->tc=\n");
for(i=0;i<tcs->num_tc;i++) {
    for(j=0;j<tcs->num_tc_to_sum[i][0];j++) printf("%d ",(int)tcs->tc[i][0][j]);
    printf("\n");
    }
#endif

if(lcpercentchange) {
    printf("Data is in percent change.\n");
    maxlik_unbias=-1;
    }
printf("maxlik_unbias = %d\n",maxlik_unbias);
printf("We will be computing %s matrices.\n",!lccor?"covariance":"correlation");
if(lcscale_by_var) printf("Will be scaling cov matrices by each subject's variance.\n");
fflush(stdout);

for(i=0;i<tcs->num_tc;i++) {
    if(tcs->each[i]!=nconcs) {
        printf("nconcs=%d tcs->each[%d]=%d  Must be the same. Abort!\n",nconcs,i,tcs->each[i]);
        exit(-1);
        }
    }

for(j=k=0;k<tcs->num_tc;k++) for(m=0;m<tcs->each[k];m++) j+=tcs->num_tc_to_sum[k][m]; 
/*printf("j=%d\n",j); fflush(stdout);*/
if(!(tdim_tc_all=malloc(sizeof*tdim_tc_all*j))) {
    printf("Error: Unable to malloc tdim_tc_all\n");
    exit(-1);
    }
for(max_each=j=k=0;k<tcs->num_tc;k++) {
    if(tcs->each[k]>max_each) max_each=tcs->each[k];
    for(m=0;m<tcs->each[k];m++) {
        if((int)tcs->tc[k][m][0]==-1) continue;
        for(i=0;i<tcs->num_tc_to_sum[k][m];i++,j++) {
            if(!(ifh=read_ifh(concss[m]->files[(int)tcs->tc[k][m][i]-1],(Interfile_header*)NULL))) exit(-1);
            if(!j) {
                vol = ifh->dim1*ifh->dim2*ifh->dim3;
                }
            else if((ifh->dim1*ifh->dim2*ifh->dim3)!=vol) {
                printf("%s\n",concss[m]->files[(int)tcs->tc[k][m][i]-1]);
                printf("Error: vol=%d ifh->dim1*ifh->dim2*ifh->dim3=%d Must be the same. Abort!\n",vol,ifh->dim1*ifh->dim2*ifh->dim3);
                fflush(stdout);
                exit(-1);
                }
            tdim_tc_all[j]=ifh->dim4;
            free_ifh(ifh,0);
            }
        }
    }
if(!(temp_double=malloc(sizeof*temp_double*vol*vol))) {
    printf("Error: Unable to malloc temp_double\n");
    exit(-1);
    }
if(!(stat=malloc(sizeof*stat*vol))) {
    printf("Error: Unable to malloc stat\n");
    exit(-1);
    }
if(!(cov=malloc(sizeof*cov*vol*vol*max_each))) {
    printf("Error: Unable to malloc cov\n");
    exit(-1);
    }
if(!(eval=malloc(sizeof*eval*vol))) {
    printf("Error: Unable to malloc eval\n");
    exit(-1);
    }
gsl_vector_view geval = gsl_vector_view_array(eval,vol);

#if 0
if(!(evec=malloc(sizeof*evec*vol*vol))) {
    printf("Error: Unable to malloc evec\n");
    exit(-1);
    }
gsl_matrix_view gevec = gsl_matrix_view_array(evec,vol,vol);
#endif
/*START12*/
if(!(evec=d2double(vol,vol))) exit(-1); 
gsl_matrix_view gevec = gsl_matrix_view_array(evec[0],vol,vol);

gsl_eigen_symmv_workspace *w = gsl_eigen_symmv_alloc(vol);
for(k1=k=0;k<tcs->num_tc;k++) {
    for(m=0;m<vol*vol*tcs->each[k];m++) cov[m]=0.;
    for(m1=m=0;m<tcs->each[k];m++,m1+=vol*vol) {
        for(sum=sum2=0.,tdim_sum=i=0;i<tcs->num_tc_to_sum[k][m];i++,k1++) {
            if(!lccor) {
                if(!cov_guts(concss[m]->files[(int)tcs->tc[k][m][i]-1],(char*)NULL,temp_double,vol,tdim_tc_all[k1],maxlik_unbias)) 
                    exit(-1);
                }
            else {
                if(!corr_guts(concss[m]->files[(int)tcs->tc[k][m][i]-1],(char*)NULL,temp_double,stat,vol,tdim_tc_all[k1])) exit(-1);
                }
            for(j1=m1,j=0;j<vol*vol;j++,j1++) cov[j1] += temp_double[j];
            if(lcscale_by_var) {
                if(!(mm=map_disk_double(concss[m]->files[(int)tcs->tc[k][m][i]-1],vol*tdim_tc_all[k1],0))) return 0;
                for(j=0;j<tdim_tc_all[k1]*vol;j++) {sum+=mm->dptr[j];sum2+=mm->dptr[j]*mm->dptr[j];}
                if(!unmap_disk(mm)) exit(-1);
                }
            tdim_sum += tdim_tc_all[k1];
            }
        if(!lcpercentchange||lccor) {
            for(i1=m1,i=0;i<vol*vol;i++,i1++) cov[i1] /= (double)tcs->num_tc_to_sum[k][m];
            }
        else {
            td = (double)(tdim_sum-1);
            for(i1=m1,i=0;i<vol*vol;i++,i1++) cov[i1] /= td; 
            }
        if(lcscale_by_var) {
            var = (sum2-sum*sum/(tdim_sum*vol))/(tdim_sum*vol-1);
            for(i1=m1,i=0;i<vol*vol;i++,i1++) cov[i1] /= var;
            }
        }
    for(m=0;m<vol*vol;m++) temp_double[m]=0.;
    for(m1=m=0;m<tcs->each[k];m++) for(i=0;i<vol*vol;i++,m1++) temp_double[i]+=cov[m1];
    for(m=0;m<vol*vol;m++) temp_double[m]/=(double)tcs->each[k];
    gsl_matrix_view gcov = gsl_matrix_view_array(temp_double,vol,vol);
    gsl_eigen_symmv(&gcov.matrix,&geval.vector,&gevec.matrix,w);
    gsl_eigen_symmv_sort(&geval.vector,&gevec.matrix,GSL_EIGEN_SORT_ABS_DESC);


    sprintf(filename,"%s_eval.txt",roots->files[k]);
    if(!(fp=fopen_sub(filename,"w"))) exit(-1);
    fprintf(fp,"%s\n",roots->files[k]); fflush(stdout);
    for(td=0.,i=0;i<vol;i++) td += fabs(eval[i]);
    for(td1=0.,i=0;i<vol;i++) {
        td1+=fabs(eval[i]);
        fprintf(fp,"\teigenvalue = %g  %.2f%% %.2f%%\n",eval[i],fabs(eval[i])/td*100.,td1/td*100.);
        }
    fclose(fp);
    printf("Output written to %s\n",filename); fflush(stdout);


    #if 0
    sprintf(filename,"%s_evec.txt",roots->files[k]);
    if(!(fp=fopen_sub(filename,"w"))) exit(-1);
    for(i=0;i<vol;i++) fprintf(fp,"evec%d\t",i+1);
    fprintf(fp,"\n");
    for(i=0;i<vol;i++) {
        for(j=0;j<vol;j++) fprintf(fp,"%f\t",evec[i][j]);
        fprintf(fp,"\n");
        }
    fclose(fp);
    printf("Output written to %s\n",filename); fflush(stdout);
    #endif
    /*START17*/
    sprintf(filename,"%s_evec.txt",roots->files[k]);
    if(!(fp=fopen_sub(filename,"w"))) exit(-1);
    for(i=0;i<vol;i++) fprintf(fp,"evec%d\t",i+1);
    fprintf(fp,"\n");
    for(i=0;i<vol;i++) {
        if(regions->nfiles==vol) fprintf(fp,"%s\t",regions->files[i]);
        for(j=0;j<vol;j++) fprintf(fp,"%f\t",evec[i][j]);
        fprintf(fp,"\n");
        }
    fclose(fp);
    printf("Output written to %s\n",filename); fflush(stdout);




    if(nsubroots) {
        for(m1=m=0;m<tcs->each[k];m++,m1+=vol*vol) {

            gsl_matrix_view gcov = gsl_matrix_view_array(&cov[m1],vol,vol);
            gsl_eigen_symmv(&gcov.matrix,&geval.vector,&gevec.matrix,w);
            gsl_eigen_symmv_sort(&geval.vector,&gevec.matrix,GSL_EIGEN_SORT_ABS_DESC);

            #if 0
            printf("%s %s\n",subroots->files[m],roots->files[k]); fflush(stdout);
            for(td=0.,i=0;i<vol;i++) td += fabs(eval[i]);
            for(td1=0.,i=0;i<vol;i++) {
                td1+=fabs(eval[i]);
                printf("\teigenvalue = %g  %.2f%% %.2f%%\n",eval[i],fabs(eval[i])/td*100.,td1/td*100.);
                }
            #endif
            sprintf(filename,"%s_%s_eval.txt",subroots->files[m],roots->files[k]);
            if(!(fp=fopen_sub(filename,"w"))) exit(-1);
            fprintf(fp,"%s %s\n",subroots->files[m],roots->files[k]); fflush(stdout);
            for(td=0.,i=0;i<vol;i++) td += fabs(eval[i]);
            for(td1=0.,i=0;i<vol;i++) {
                td1+=fabs(eval[i]);
                fprintf(fp,"\teigenvalue = %g  %.2f%% %.2f%%\n",eval[i],fabs(eval[i])/td*100.,td1/td*100.);
                }
            fclose(fp);
            printf("Output written to %s\n",filename); fflush(stdout);

            #if 0
            sprintf(filename,"%s_%s_evec.txt",subroots->files[m],roots->files[k]);
            if(!(fp=fopen_sub(filename,"w"))) exit(-1);
            for(i=0;i<vol;i++) fprintf(fp,"evec%d\t",i+1); 
            fprintf(fp,"\n");
            for(i=0;i<vol;i++) {
                for(j=0;j<vol;j++) fprintf(fp,"%f\t",evec[i][j]); 
                fprintf(fp,"\n");
                }
            fclose(fp);
            printf("Output written to %s\n",filename); fflush(stdout);
            #endif
            /*START17*/
            sprintf(filename,"%s_%s_evec.txt",subroots->files[m],roots->files[k]);
            if(!(fp=fopen_sub(filename,"w"))) exit(-1);
            for(i=0;i<vol;i++) fprintf(fp,"evec%d\t",i+1);
            fprintf(fp,"\n");
            for(i=0;i<vol;i++) {
                if(regions->nfiles==vol) fprintf(fp,"%s\t",regions->files[i]);
                for(j=0;j<vol;j++) fprintf(fp,"%f\t",evec[i][j]);
                fprintf(fp,"\n");
                }
            fclose(fp);
            printf("Output written to %s\n",filename); fflush(stdout);


            }
        }


    }
}
