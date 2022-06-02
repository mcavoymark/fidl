/* Copyright 2/14/07 Washington University. All Rights Reserved.
   fidl_crosscorr.c  $Revision: 1.20 $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>
#include <gsl/gsl_eigen.h>

static char rcsid[] = "$Header: /home/hannah/mcavoy/idl/clib/RCS/fidl_crosscorr.c,v 1.20 2012/11/14 22:39:01 mcavoy Exp $";

main(int argc,char **argv)
{
char *scratchdir=NULL,string[MAXNAME],string2[MAXNAME],*strptr,*mask_file=NULL;
int i,j,k,l,m,n,o,ii,nconcs=0,nruns=0,nroots=0,nregions=0,argc_runs,*temp_int,*tdim_tc,tdim_tcmax,vol,lenvol,nsub,lcclean_up=0,
    lcttest_driver=0,max_each,lcpooledsd=0,atlas=0,SunOS_Linux;
float *temp_float;
double TR,*temp_double,*avg,*subr=NULL,*sum,*sum2,*sem,*dof,*temp_double2,*r,td,td1;
Files_Struct *concs,*roots,*regions,**concss;
TC *tcs;
Interfile_header *ifh;
Memory_Map *mm;
FILE *fp;
Mask_Struct *ms;
Atlas_Param *ap;

/*START19*/
gsl_matrix_view mcov;
gsl_vector_view evec_i;
gsl_vector *eval;
gsl_matrix *evec;
gsl_eigen_symmv_workspace *w;

if(argc < 5) {
    fprintf(stderr,"This program averages and assembles autocorrelation files computed by autocorr_guts.\n");
    fprintf(stderr,"    -concs:       Conc files for precomputed autocorrelation files.\n");
    fprintf(stderr,"    -runs:        Which runs to be assembled and averaged.\n");
    fprintf(stderr,"    -roots:       Output roots.\n");
    fprintf(stderr,"    -regions:     Region names.\n");
    fprintf(stderr,"    -TR:          Used to determine the lag.\n");
    fprintf(stderr,"    -scratchdir:  Name of scratch directory to remove.\n");
    fprintf(stderr,"    -clean_up     Remove scratch directory to remove.\n");
    fprintf(stderr,"    -ttest_driver Ouput lag 0 correlations in a T test driver format.\n");
    fprintf(stderr,"                  The default is each subject a row; each region a column.\n");
    fprintf(stderr,"    -pooledsd     Compute the pooled standard deviation.\n");
    fprintf(stderr,"                  The variance from compute_residuals is a weighted (by dof) average across runs.\n");
    fprintf(stderr,"                  Then the sqrt is taken. This gives it the feel of a rms measure and I hope a proper chi dist.\n");
    fprintf(stderr,"                  Also this is consistent with how rms amplitude power is computed by compute_zstat.\n");
    fprintf(stderr,"    -mask:        The input files are already compressed by compute_residuals.\n");
    fprintf(stderr,"                  For a voxel analysis, including the mask decompresses them at output.\n");
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
    if(!strcmp(argv[i],"-scratchdir") && argc > i+1)
        scratchdir = argv[++i];
    if(!strcmp(argv[i],"-clean_up"))
        lcclean_up=1;
    if(!strcmp(argv[i],"-ttest_driver"))
        lcttest_driver=1;
    if(!strcmp(argv[i],"-pooledsd"))
        lcpooledsd=1;
    if(!strcmp(argv[i],"-mask") && argc > i+1)
        mask_file = argv[++i];
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
if(!nregions) {
    printf("Voxel level analysis.\n");
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
printf("tcs->num_tc=%d nruns=%d tcs->total=%d\n",tcs->num_tc,nruns,tcs->total);
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
if(!(dof=malloc(sizeof*dof*tcs->total))) {
    printf("Error: Unable to malloc dof\n");
    exit(-1);
    }

for(tdim_tcmax=max_each=j=k=0;k<tcs->num_tc;k++) {
    if(tcs->each[k]>max_each) max_each=tcs->each[k];
    for(l=m=0;m<tcs->each[k];m++) {
        if((int)tcs->tc[k][m][0]==-1) continue;
        for(i=0;i<tcs->num_tc_to_sum[k][m];i++,j++,l++) {
            if(!(ifh=read_ifh(concss[m]->files[(int)tcs->tc[k][m][i]-1],(Interfile_header*)NULL))) exit(-1);
            if(!j) {
                vol = ifh->dim4;
                if(nregions) {
                    if(vol!=nregions*nregions) {
                        printf("%s\n",concss[m]->files[(int)tcs->tc[k][m][i]-1]);
                        printf("vol=%d nregions*nregions=%d  Must be the same. Abort!\n",vol,nregions*nregions);
                        fflush(stdout);
                        exit(-1);
                        }
                    }
                }
            else if(ifh->dim4!=vol) {
                printf("%s\n",concss[m]->files[(int)tcs->tc[k][m][i]-1]);
                printf("Error: vol=%d ifh->dim4=%d  Must be the same. Abort!\n",vol,ifh->dim4);
                fflush(stdout);
                exit(-1);
                }
            if(!l) {
                if((tdim_tc[k]=ifh->dim1)>tdim_tcmax) tdim_tcmax=tdim_tc[k];
                }
            else if(ifh->dim1!=tdim_tc[k]) {
                printf("Error: tdim_tc[k]=%d ifh->dim1=%d  Must be the same. Abort!\n",k,tdim_tc[k],ifh->dim1);
                exit(-1);
                }
            dof[j] = (double)ifh->dof_condition;
            free_ifh(ifh,0);
            }
        }
    }

if(!(temp_double=malloc(sizeof*temp_double*tdim_tcmax*vol))) {
    printf("Error: Unable to malloc temp_double\n");
    exit(-1);
    }
if(!(temp_double2=malloc(sizeof*temp_double2*tdim_tcmax*vol))) {
    printf("Error: Unable to malloc temp_double2\n");
    exit(-1);
    }
if(nregions) {
    if(!(subr=malloc(sizeof*subr*max_each*nregions*nregions))) {
        printf("Error: Unable to malloc subr\n");
        exit(-1);
        }
    if(!(r=malloc(sizeof*r*nregions*nregions))) {
        printf("Error: Unable to malloc r in fidl_crosscorr\n");
        return 0;
        }
    mcov = gsl_matrix_view_array(r,nregions,nregions);
    eval = gsl_vector_alloc(nregions);
    evec = gsl_matrix_alloc(nregions,nregions);
    w = gsl_eigen_symmv_alloc(nregions);
    }
else {
    if(!mask_file) {
        printf("Error: Need to specify -mask\n");
        exit(-1);
        }
    if(!(ms=get_mask_struct(mask_file,0,(int*)NULL,SunOS_Linux,(LinearModel*)NULL,0))) exit(-1);
    if(ms->lenbrain!=vol) {
        printf("ms->lenbrain=%d vol=%d Must be equal.\n",ms->lenbrain,vol);
        exit(-1);
        }
    atlas=get_atlas(ms->lenvol);
    if(!(ap=get_atlas_param(atlas,(Interfile_header*)NULL))) exit(-1);

    #if 0
    if(!(temp_float=malloc(sizeof*temp_float*ms->lenvol))) {
        printf("Error: Unable to malloc temp_float\n");
        exit(-1);
        }
    #endif
    /*START21*/
    if(!(temp_float=malloc(sizeof*temp_float*ms->lenvol*tdim_tcmax))) {
        printf("Error: Unable to malloc temp_float\n");
        exit(-1);
        }

    for(i=0;i<ms->lenvol;i++) temp_float[i]=0.;
    if(!(ifh=init_ifh(4,ap->xdim,ap->ydim,ap->zdim,1,ap->voxel_size[0],ap->voxel_size[1],ap->voxel_size[2],!SunOS_Linux?1:0)))
        exit(-1);
    strcpy(string,concs->files[0]);
    if(!(strptr=get_tail_sans_ext(string))) exit(-1);
    }

if(!(avg=malloc(sizeof*avg*tdim_tcmax*vol))) {
    printf("Error: Unable to malloc avg\n");
    exit(-1);
    }
if(!(sum=malloc(sizeof*sum*tdim_tcmax*vol))) {
    printf("Error: Unable to malloc sum\n");
    exit(-1);
    }
if(!(sum2=malloc(sizeof*sum2*tdim_tcmax*vol))) {
    printf("Error: Unable to malloc sum2\n");
    exit(-1);
    }
if(!(sem=malloc(sizeof*sem*tdim_tcmax*vol))) {
    printf("Error: Unable to malloc sem\n");
    exit(-1);
    }

for(k=0;k<tcs->num_tc;k++) {
    if(subr) for(m=0;m<tcs->each[k]*nregions*nregions;m++) subr[m]=0.;
    lenvol = tdim_tc[k]*vol;
    for(m=0;m<lenvol;m++) sum[m]=sum2[m]=0.;
    for(nsub=o=m=0;m<tcs->each[k];m++,l++) {
        if((int)tcs->tc[k][m][0]==-1) {
            o+=nregions*nregions;
            continue;
            }
        for(i=0;i<lenvol;i++) temp_double[i]=temp_double2[i]=0.;
        for(i=0;i<tcs->num_tc_to_sum[k][m];i++) {
            l = (int)tcs->tc[k][m][i]-1;
            printf("Mapping %s\n",concss[m]->files[l]);
            if(!(mm=map_disk_double(concss[m]->files[l],lenvol,0))) exit(-1);
            for(j=0;j<lenvol;j++) {
                temp_double[j] += mm->dptr[j]*dof[l];
                temp_double2[j] += dof[l];
                }
            if(!unmap_disk(mm)) exit(-1);
            }
        for(i=0;i<lenvol;i++) {
            temp_double[i]/=temp_double2[i];
            sum[i]+=temp_double[i];
            sum2[i]+=temp_double[i]*temp_double[i];
            }
        if(subr) {
            for(n=j=0;j<nregions;j++,n+=tdim_tc[k]*nregions) {
                for(l=i=0;i<nregions;i++,l+=tdim_tc[k],o++) subr[o]=temp_double[n+l];
                }
            }
        if(lcpooledsd) {
            for(i=0;i<lenvol;i++) temp_double[i]=sqrt(temp_double[i]);
            strcpy(string2,concs->files[m]);
            if(!(strptr=get_tail_sans_ext(string2))) exit(-1);
            if(nroots) {
                sprintf(string,"%s_%s_pooledsd.4dfp.img",strptr,roots->files[k]);
                }
            else {
                sprintf(string,"%s_pooledsd.4dfp.img",strptr);
                }
            if(ms->lenbrain!=lenvol) {
                printf("Error: ms->lenbrain=%d lenvol=%d Must be equal.\n",ms->lenbrain,lenvol);
                exit(-1);
                } 
            for(i=0;i<ms->lenbrain;i++) temp_float[ms->brnidx[i]] = (float)temp_double[i];
            if(!writestack(string,temp_float,sizeof(float),(size_t)(ap->vol),SunOS_Linux)) exit(-1);
            if(!write_ifh(string,ifh,(int)FALSE)) exit(-1);
            printf("    Output written to %s\n",string);
            } 
        nsub++;
        }
    for(m=0;m<lenvol;m++) avg[m]=sum[m]/(double)nsub;
    if(nsub>1) for(m=0;m<lenvol;m++) sem[m]=sqrt((sum2[m]-sum[m]*sum[m]/nsub)/(nsub*(nsub-1)));
    if(nregions) {
        if(subr) {
            if(!lcttest_driver) {
                for(o=n=j=0;j<nregions;j++,n+=nregions,o+=tdim_tc[k]*nregions) {
                    if(nroots) {
                        sprintf(string,"%s_lag0_%s.txt",roots->files[k],regions->files[j]);
                        }
                    else {
                        sprintf(string,"lag0_%s.txt",regions->files[j]);
                        }
                    if(!(fp=fopen_sub(string,"w"))) exit(-1);
                    fprintf(fp,"subject\t");
                    for(m=0;m<nregions;m++) fprintf(fp,"%s\t",regions->files[m]);
                    fprintf(fp,"\n");
                    for(l=n,m=0;m<tcs->each[k];m++,l+=nregions*nregions) {
                        strcpy(string2,concs->files[m]);
                        if(!(strptr=get_tail_sans_ext(string2))) exit(-1);
                        fprintf(fp,"%s\t",strptr);
                        for(i=0;i<nregions;i++) fprintf(fp,"%.4f\t",subr[l+i]);
                        fprintf(fp,"\n");
                        }
                    fprintf(fp,"mean\t");
                    for(l=0,i=0;i<nregions;i++,l+=tdim_tc[k]) fprintf(fp,"%.4f\t",avg[o+l]);
                    fprintf(fp,"\n");
                    if(nsub>1) {
                        fprintf(fp,"sem\t");
                        for(l=0,i=0;i<nregions;i++,l+=tdim_tc[k]) fprintf(fp,"%.4f\t",sem[o+l]);
                        fprintf(fp,"\n");
                        }
                    fclose(fp);
                    printf("Output written to %s\n",string);
                    }
                }
            else {
                for(n=j=0;j<nregions;j++,n+=nregions) {
                    sprintf(string,"%s_lag0_%s_driver.txt",roots->files[k],regions->files[j]);
                    if(!(fp=fopen_sub(string,"w"))) exit(-1);
                    for(i=0;i<nregions;i++) {
                        fprintf(fp,"%s\nr\n",regions->files[i]);
                        for(l=n,m=0;m<tcs->each[k];m++,l+=nregions*nregions) fprintf(fp,"%s := %f\n",!k?"FIRST":"SECOND",subr[l+i]);
                        fprintf(fp,"\nfisher z\n");
                        for(l=n,m=0;m<tcs->each[k];m++,l+=nregions*nregions) fprintf(fp,"%s := %f\n",!k?"FIRST":"SECOND",
                            .5*log((1.+subr[l+i])/(1.-subr[l+i])));
                        fprintf(fp,"\n");
                        }
                    fclose(fp);
                    printf("Output written to %s\n",string);
                    }
                }
            }
        if(tdim_tc[k]>1) {
            for(n=j=0;j<nregions;j++,n+=tdim_tc[k]*nregions) {
                if(nroots) {
                    sprintf(string,"%s_SHIFT_%s.txt",roots->files[k],regions->files[j]);
                    }
                else {
                    sprintf(string,"SHIFT_%s.txt",regions->files[j]);
                    }
                if(!(fp=fopen_sub(string,"w"))) exit(-1);
                fprintf(fp,"lag\t");
                for(m=0;m<nregions;m++) fprintf(fp,"%s\t",regions->files[m]);
                fprintf(fp,"\n");
                for(m=0;m<tdim_tc[k];m++) {
                    fprintf(fp,"%.4f\t",(double)m*TR);
                    for(l=m,i=0;i<nregions;i++,l+=tdim_tc[k]) fprintf(fp,"%.4f\t",avg[n+l]);
                    fprintf(fp,"\n");
                    }
                fclose(fp);
                printf("Output written to %s\n",string);
                }
            if(nsub>1) {
                for(o=n=j=0;j<nregions;j++,n+=tdim_tc[k]*nregions,o+=tdim_tc[k]) {
                    if(nroots) {
                        sprintf(string,"%s_SHIFT_%s_sem.txt",roots->files[k],regions->files[j]);
                        }
                    else {
                        sprintf(string,"SHIFT_%s_sem.txt",regions->files[j]);
                        }
                    if(!(fp=fopen_sub(string,"w"))) exit(-1);
                    fprintf(fp,"At neg lags, the region leads %s. At pos lags, the region lags %s.\n",regions->files[j],
                        regions->files[j]);
                    fprintf(fp,"lag"); for(m=0;m<nregions;m++) fprintf(fp,"\t%s",regions->files[m]); fprintf(fp,"\n");
                    for(m=tdim_tc[k];--m>0;) {
                        fprintf(fp,"-%.4f_%s\t",(double)m*TR,roots->files[k]);
                        for(l=m,i=0;i<nregions;i++,l+=tdim_tc[k]) fprintf(fp,"%.4f\t%.4f\t",avg[n+l],sem[n+l]);
                        fprintf(fp,"\n");
                        }
                    for(m=0;m<tdim_tc[k];m++) {
                        fprintf(fp,"%.4f_%s\t",(double)m*TR,roots->files[k]);
                        for(l=0,i=0;i<nregions;i++,l+=tdim_tc[k]*nregions)fprintf(fp,"%.4f\t%.4f\t",avg[o+m+l],sem[o+m+l]);
                        fprintf(fp,"\n");
                        }
                    fclose(fp);
                    printf("Output written to %s\n",string);
                    }
                }
            }

        string[0]=0;
        if(nroots) sprintf(string,"%s_",roots->files[k]); 
        strcat(string,"lag0_mean.txt");
        if(!(fp=fopen_sub(string,"w"))) exit(-1);
        for(n=j=0;j<nregions;j++,n+=tdim_tc[k]*nregions) {
            fprintf(fp,"%s\t",regions->files[j]);
            for(l=n,i=0;i<nregions;i++,l+=tdim_tc[k]) fprintf(fp,"%.4f\t",avg[l]);
            fprintf(fp,"\n");
            }
        fclose(fp);
        printf("Output written to %s\n",string);

        string[0]=0;
        if(nroots) sprintf(string,"%s_",roots->files[k]); 
        strcat(string,"lag0_eig.txt");
        if(!(fp=fopen_sub(string,"w"))) exit(-1);
        fprintf(fp,"Lag 0 correlation matrix\n");
        for(ii=n=j=0;j<nregions;j++,n+=tdim_tc[k]*nregions) {
            fprintf(fp,"%s\t",regions->files[j]);
            for(l=n,i=0;i<nregions;i++,l+=tdim_tc[k],ii++) {
                fprintf(fp,"%.4f\t",avg[l]);
                r[ii] = avg[l];
                }
            fprintf(fp,"\n");
            }
        fprintf(fp,"\n");
        gsl_eigen_symmv(&mcov.matrix,eval,evec,w);
        gsl_eigen_symmv_sort(eval,evec,GSL_EIGEN_SORT_ABS_DESC);
        fprintf(fp,"Eigenvectors are in columns.\n");
        for(j=0;j<nregions;j++) {
            fprintf(fp,"%s\t",regions->files[j]);
            evec_i = gsl_matrix_row(evec,j);
            for(i=0;i<nregions;i++) fprintf(fp,"%f\t",gsl_vector_get(&evec_i.vector,i));
            fprintf(fp,"\n");
            }
        fprintf(fp,"\neigenvalue");
        for(td=0.,j=0;j<nregions;j++) {
            td1 = gsl_vector_get(eval,j);
            td += fabs(td1);
            fprintf(fp,"\t%g",td1); 
            }
        fprintf(fp,"\n");
        fprintf(fp,"\npercent var");
        for(j=0;j<nregions;j++) fprintf(fp,"\t%.2f%%",fabs(gsl_vector_get(eval,j))/td*100.);
        fprintf(fp,"\npercent culm var");
        for(td1=0.,j=0;j<nregions;j++) {
            td1 += fabs(gsl_vector_get(eval,j));
            fprintf(fp,"\t%.2f%%",td1/td*100.);
            }
        fprintf(fp,"\n");
        fclose(fp);
        printf("Output written to %s\n",string);
        }
    else {
        for(l=i=0;i<tdim_tc[k];i++,l+=ms->lenvol) for(m=i,j=0;j<vol;j++,m+=tdim_tc[k]) temp_float[l+ms->brnidx[j]] = (float)avg[m];

        /*sprintf(string2,"%s_%s.4dfp.img",strptr,roots->files[k]);*/
        /*START121112*/
        sprintf(string2,"%s%s%s.4dfp.img",strptr,nroots?"_":"",nroots?roots->files[k]:"");

        if(!writestack(string2,temp_float,sizeof(float),(size_t)(ms->lenvol*tdim_tc[k]),0)) exit(-1);
        ifh->dim4 = tdim_tc[k];
        min_and_max_init(&ifh->global_min,&ifh->global_max);
        min_and_max_doublestack(avg,lenvol,&ifh->global_min,&ifh->global_max);
        if(!write_ifh(string2,ifh,0)) return 0;
        printf("Output written to %s\n",string2);
        fflush(stdout);

        }
    }
if(lcclean_up) {
    if(nregions) {
        sprintf(string,"rm -rf %s",scratchdir);
        if(system(string) == -1) printf("Error: Unable to %s\n",string);
        }    
    else {
        delete_scratch_files(concss[0]->files,concss[0]->nfiles);
        }
    }
}
