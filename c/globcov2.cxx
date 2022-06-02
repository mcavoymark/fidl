/* Copyright 9/9/21 Washington University.  All Rights Reserved.
   globcov2.cxx  $Revision: 1.1 $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <gsl/gsl_eigen.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_linalg.h>
#include "globcov2.h"
#include "map_disk.h"
#include "trend.h"
#include "constants.h"
#include "read_xform.h"
#include "twoA_or_twoB.h"
#include "subs_util.h"
#include "shouldiswap.h"
#include "t4_atlas.h"

int globcov_guts(Files_Struct *fi,Dim_Param2 *dp,int *valid_frms,double *globcov,int *scalingv,
    Regions_By_File *rbf,Files_Struct *xform_file,int *t4_identify,Atlas_Param *ap,double *weights,int *weightsi,int weightsc,
    int lccompressed,int *tdim){

    size_t i,k1;
    int j,k,l,m,ii,jj,kk,i1,i2,index,nframes,*iframes,*A_or_B_or_U=NULL,nbold_index,*bold_index=NULL,col,ncol; //swapbytes
    float *temp_float=NULL,*t4=NULL;
    double *temp_double,*stat,*mean,*max,*min,*den=NULL,*r=NULL,td,td1,*slope=NULL,*Astack=NULL,*QRstack=NULL,*bstack=NULL,
        *residualstack=NULL,*dptr,*dptr1;
    Memory_Map *mm;
    gsl_matrix_view mcov,A,QR;
    gsl_vector_view b,residual;
    gsl_vector *eval=NULL,*tau=NULL,*x=NULL;
    gsl_matrix *evec=NULL;
    gsl_eigen_symmv_workspace *w=NULL;
    FILE *fp=NULL;
    if(weightsc==0) weightsc=1;
    if(!(temp_double=(double*)malloc(sizeof*temp_double*ap->vol))) {
        printf("fidlError: Unable to malloc temp_double in globov_guts\n");
        return 0;
        }
    if(!(stat=(double*)malloc(sizeof*stat*ap->vol))) {
        printf("fidlError: Unable to malloc stat in globov_guts\n");
        return 0;
        }
    if(!(mean=(double*)malloc(sizeof*mean*rbf->nreg*weightsc))) {
        printf("fidlError: Unable to malloc mean in globov_guts\n");
        return 0;
        }
    if(!(max=(double*)malloc(sizeof*max*rbf->nreg*weightsc))) {
        printf("fidlError: Unable to malloc max in globov_guts\n");
        return 0;
        }
    if(!(min=(double*)malloc(sizeof*min*rbf->nreg))) {
        printf("fidlError: Unable to malloc min in globov_guts\n");
        return 0;
        }
    if(!(iframes=(int*)malloc(sizeof*iframes*dp->tdim_max))) {
        printf("fidlError: Unable to malloc iframes in globov_guts\n");
        return 0;
        }
    if(scalingv[0]==5) {
        ncol = 2;
        if(!(slope=(double*)malloc(sizeof*slope*dp->tdim_total))) {
            printf("fidlError: Unable to malloc slope in globcov_guts\n");
            return 0;
            }
        if(!(Astack=(double*)malloc(sizeof*Astack*dp->tdim_max*ncol))) {
            printf("fidlError: Unable to malloc Astack in globcov_guts\n");
            return 0;
            }
        if(!(QRstack=(double*)malloc(sizeof*QRstack*dp->tdim_max*ncol))) {
            printf("fidlError: Unable to malloc QRstack in globcov_guts\n");
            return 0;
            }
        if(!(bstack=(double*)malloc(sizeof*bstack*dp->tdim_max))) {
            printf("fidlError: Unable to malloc bstack in globcov_guts\n");
            return 0;
            }
        if(!(residualstack=(double*)malloc(sizeof*residualstack*dp->tdim_max))) {
            printf("fidlError: Unable to malloc residualstack in globcov_guts\n");
            return 0;
            }
        tau = gsl_vector_alloc(ncol);
        x = gsl_vector_alloc(ncol);
        trend((int)fi->nfiles,dp->tdim_total,tdim,valid_frms,slope,iframes);
        }
    else if(scalingv[0]==6&&!weights) {
        if(!(den=(double*)malloc(sizeof*den*rbf->nreg))) {
            printf("fidlError: Unable to malloc den in globov_guts\n");
            return 0;
            }
        if(!(r=(double*)malloc(sizeof*r*rbf->nreg*rbf->nreg))) {
            printf("fidlError: Unable to malloc r in globov_guts\n");
            return 0;
            }
        mcov = gsl_matrix_view_array(r,rbf->nreg,rbf->nreg);
        eval = gsl_vector_alloc(rbf->nreg);
        evec = gsl_matrix_alloc(rbf->nreg,rbf->nreg);
        w = gsl_eigen_symmv_alloc(rbf->nreg);
        }
    if(xform_file) {
        if(!(t4=(float*)malloc(sizeof*t4*(size_t)T4SIZE*xform_file->nfiles))) {
            printf("fidlError: Unable to malloc t4\n");
            return 0;
            }
        if(!(A_or_B_or_U=(int*)malloc(sizeof*A_or_B_or_U*xform_file->nfiles))) {
            printf("fidlError: Unable to malloc A_or_B_or_U\n");
            return 0;
            }
        for(i=0;i<xform_file->nfiles;i++) {
            if(!read_xform(xform_file->files[i],&t4[i*(int)T4SIZE])) return 0;
            if((A_or_B_or_U[i]=twoA_or_twoB(xform_file->files[i]))==2) return 0;
            }
        nbold_index = dp->volall;
        if(!(bold_index=(int*)malloc(sizeof*bold_index*nbold_index))) {
            printf("fidlError: Unable to malloc bold_index\n");
            exit(-1);
            }
        for(ii=0;ii<nbold_index;ii++) bold_index[ii]=ii;
        }
    else if(lccompressed) {
        if(rbf->nindices_uniqsort!=dp->volall) {
            printf("fidlError: rbf->nindices_uniqsort=%d dp->volall=%d Must be equal.\n",rbf->nindices_uniqsort,dp->volall);
            return 0;
            }
        nbold_index = 0;
        }
    else {
        nbold_index = rbf->nvoxels;
        bold_index = rbf->indices;
        }

    if(nbold_index) if(!(temp_float=(float*)malloc(sizeof*temp_float*nbold_index))) {
        printf("fidlError: Unable to malloc temp_float in globov_guts\n");
        return 0;
        }
    for(ii=0;ii<dp->tdim_total*rbf->nreg;ii++) globcov[ii]=0.;
    if(scalingv[0]==6&&!weights) if(!(fp=fopen_sub("fidl_gio.txt","a"))) return 0;



    //printf("rbf->indices_uniqsort rbf->nindices_uniqsort=%d\n",rbf->nindices_uniqsort);for(k=0;k<rbf->nindices_uniqsort;k++)printf("%d ",rbf->indices_uniqsort[k]);printf("\n");exit(-1);



    for(kk=jj=i=0;i<fi->nfiles;i++) {

        //printf("here100 i=%zd\n",i);fflush(stdout);


        //if(!(mm=map_disk(fi->files[i],dp->vol[i]*dp->tdim[i],0,sizeof(float)))) return 0;
        //START190322
        if(!(mm=map_disk(fi->files[i],dp->vol[i]*dp->tdim[i],0,dp->number_format[i]==(int)DOUBLE_IF?sizeof(double):sizeof(float)))) return 0;


        //START190322
        //swapbytes=shouldiswap(SunOS_Linux,dp->bigendian[i]);

        for(j=0;j<rbf->nreg;j++) {
            mean[j]=0.;
            max[j]=-1.e20;
            min[j]=1.e20;
            }
        for(k1=nframes=ii=j=0;j<tdim[i];j++,jj++,ii+=dp->vol[i]) {
            if(!valid_frms[jj]) {
                kk += rbf->nreg*weightsc;
                }
            else {
                if(!lccompressed) {
                    for(k=0;k<nbold_index;k++) temp_float[k] = mm->ptr[ii+bold_index[k]];

                    //if(swapbytes) swap_bytes((unsigned char *)temp_float,sizeof(float),(size_t)nbold_index);
                    //START190322
                    if(dp->swapbytes[i]) swap_bytes((unsigned char *)temp_float,sizeof(float),(size_t)nbold_index);

                    for(k=0;k<nbold_index;k++) temp_double[bold_index[k]] =
                        temp_float[k]==(float)UNSAMPLED_VOXEL ? (double)UNSAMPLED_VOXEL : (double)temp_float[k];
                    }
                else {

                    //if(swapbytes) {
                    //START190322
                    if(dp->swapbytes[i]) {

                        for(k=0;k<rbf->nindices_uniqsort;k++,k1++) stat[k] = mm->dptr[k1];
                        swap_bytes((unsigned char *)stat,sizeof*stat,(size_t)rbf->nindices_uniqsort);
                        for(k=0;k<rbf->nindices_uniqsort;k++) temp_double[rbf->indices_uniqsort[k]] = stat[k];
                        }
                    else { 

                        //printf("here101 rbf->nindices_uniqsort=%d\n",rbf->nindices_uniqsort);fflush(stdout);


                        for(k=0;k<rbf->nindices_uniqsort;k++,k1++) temp_double[rbf->indices_uniqsort[k]] = mm->dptr[k1];

                        
                        //for(k=0;k<rbf->nindices_uniqsort;k++,k1++) temp_double[rbf->indices_uniqsort[k]] = 0.;
                        //for(k=0;k<rbf->nindices_uniqsort;k++,k1++) printf("mm->dptr[%zd]=%f\n",k1,mm->dptr[k1]); 
                        //printf("mm->dptr %d\n",j);for(k=0;k<rbf->nindices_uniqsort;k++,k1++)printf("%f ",mm->dptr[k1]);printf("\n");fflush(stdout);

                        //printf("temp_double %d\n",j);for(k=0;k<rbf->nindices_uniqsort;k++)printf("%g ",temp_double[rbf->indices_uniqsort[k]]);printf("\n"); 
                        
                        } 
                    }
                if(xform_file) {
                    if(!t4_atlas(temp_double,stat,&t4[t4_identify[i]*(int)T4SIZE],dp->xdim[i],dp->ydim[i],dp->zdim[i],dp->dx[i],
                        dp->dz[i],A_or_B_or_U[t4_identify[i]],dp->orientation[i],ap,(double*)NULL)) return 0;
                    for(k=0;k<ap->vol;k++) temp_double[k] = stat[k];
                    }
                if(!weightsi) {
                    crs(temp_double,stat,rbf,(char*)NULL);

                    //for(k=0;k<rbf->nreg*weightsc;k++)printf("stat[%d]=%f ",k,stat[k]);printf("\n");

                    }
                else {       


                    for(dptr1=stat,dptr=weights,k=0;k<weightsc;k++,dptr+=rbf->nvoxels,dptr1+=rbf->nreg) {
                        crsw(temp_double,dptr1,rbf,dptr,weightsi);
                        }


                    }
                for(k=0;k<rbf->nreg*weightsc;k++,kk++) {
                    mean[k] += globcov[kk] = stat[k];
                    if(globcov[kk]>max[k]) max[k] = globcov[kk];
                    if(globcov[kk]<min[k]) min[k] = globcov[kk];
                    //printf("stat[%d]=%f globcov[%d]=%f\n",k,stat[k],kk,globcov[kk]);
                    }
                iframes[nframes++] = jj;
                }
            }

        //printf("scalingv[0]=%d\n",scalingv[0]);fflush(stdout);

        if(scalingv[0]<5) {
            for(j=0;j<rbf->nreg;j++) {

                //printf("scalingv[%d]=%d\n",j,scalingv[j]);fflush(stdout);


                if(scalingv[j]==1) {
                    mean[j] /= nframes;
                    max[j] -= mean[j];
                    for(k=0;k<nframes;k++) {
                        index = iframes[k]*rbf->nreg+j;
                        globcov[index] -= mean[j];
                        globcov[index] /= max[j];
                        }
                    }
                else if(scalingv[j]==2) {
                    max[j] -= min[j];
                    for(k=0;k<nframes;k++) {
                        index = iframes[k]*rbf->nreg+j;
                        globcov[index] -= min[j];
                        globcov[index] /= max[j];
                        }
                    }
                else if(scalingv[j]==3||scalingv[j]==4) {
                    mean[j] /= nframes;

                    //printf("mean[%d]=%f\n",j,mean[j]);

                    for(k=0;k<nframes;k++) {
                        index = iframes[k]*rbf->nreg+j;
                        globcov[index] -= mean[j];

                        //printf("globcov[%d]=%f\n",index,globcov[index]);fflush(stdout);

                        }
                    }
                }
            }
        else if(scalingv[0]==5) {
            A = gsl_matrix_view_array(Astack,nframes,ncol);
            QR = gsl_matrix_view_array(QRstack,nframes,ncol);
            b = gsl_vector_view_array(bstack,nframes);
            residual = gsl_vector_view_array(residualstack,nframes);
            for(j=0;j<rbf->nreg;j++) {
                for(l=0;l<weightsc;l++) {
                    for(dptr=Astack,dptr1=QRstack,k=0;k<nframes;k++) {
                        *dptr++ = *dptr1++ = 1.;
                        *dptr++ = *dptr1++ = slope[iframes[k]];
                        index = iframes[k]*rbf->nreg*weightsc+l*rbf->nreg+j;
                        bstack[k] = globcov[index];
                        /*printf("l=%d bstack[%d]=%f\n",l,k,bstack[k]);*/
                        /*printf("globcov[%d]=%f\n",index,globcov[index]);*/
                        }
                    gsl_linalg_QR_decomp(&QR.matrix,tau);
                    gsl_linalg_QR_lssolve(&QR.matrix,tau,&b.vector,x,&residual.vector);
                    for(k=0;k<nframes;k++) {
                        index = iframes[k]*rbf->nreg*weightsc+l*rbf->nreg+j;
                        globcov[index] = residualstack[k];
                        }
                    }
                }
            }
        else {
            if(!weights) {
                for(j=0;j<rbf->nreg;j++) {
                    mean[j] /= nframes;
                    den[j] = 0.;
                    for(k=0;k<nframes;k++) {
                        index = iframes[k]*rbf->nreg+j;
                        globcov[index] -= mean[j];
                        den[j] += globcov[index]*globcov[index];
                        }
                    den[j] = sqrt(den[j]);
                    }
                for(j=0;j<rbf->nreg-1;j++) {
                    for(k=j+1;k<rbf->nreg;k++) {
                        for(td=0.,m=0;m<nframes;m++) {
                            i1 = iframes[k]*rbf->nreg+j;
                            i2 = iframes[k]*rbf->nreg+k;
                            td += globcov[i1]*globcov[i2];
                            }
                        td/=den[j]*den[k];
                        r[j*rbf->nreg+k]=r[k*rbf->nreg+j]=td;
                        }
                    }
                for(k=j=0;j<rbf->nreg;j++,k+=rbf->nreg+1) r[k]=1.;
                gsl_eigen_symmv(&mcov.matrix,eval,evec,w);
                gsl_eigen_symmv_sort(eval,evec,GSL_EIGEN_SORT_ABS_DESC);
                #if 1
                for(td=0.,k=0;k<rbf->nreg;k++) td += fabs(gsl_vector_get(eval,k));
                fprintf(fp,"run %zd",i+1);
                for(td1=0.,k=0;k<rbf->nreg;k++) {
                    td1+=fabs(gsl_vector_get(eval,k));
                    fprintf(fp,"\teigenvalue = %g  %.2f%% %.2f%%\n",gsl_vector_get(eval,k),fabs(gsl_vector_get(eval,k))/td*100.,
                        td1/td*100.);
                    }
                #endif

                #if 1
                /*This one is just the first eigenvector.*/
                /*gsl_vector_view evec_i = gsl_matrix_column(evec,0);
                for(k=0;k<nframes;k++) {
                    for(index=iframes[k]*rbf->nreg,td=0.,j=0;j<rbf->nreg;j++,index++) {
                        td += gsl_vector_get(&evec_i.vector,j)*globcov[index];
                        }
                    globcov[iframes[k]*rbf->nreg] = td;
                    }*/
    
                /*This one is all the eigenvectors*/
                gsl_vector_view evec_i;
                for(col=0;col<rbf->nreg;col++) {
                    evec_i = gsl_matrix_column(evec,col);
                    for(k=0;k<nframes;k++) {
                        for(index=iframes[k]*rbf->nreg+col,td=0.,j=0;j<rbf->nreg;j++,index++) {
                            td += gsl_vector_get(&evec_i.vector,j)*globcov[index];
                            }
                        globcov[iframes[k]*rbf->nreg+col] = td;
                        }
                    }
                #endif

                #if 0
                for(k=0;k<nframes;k++) {
                    for(index=iframes[k]*rbf->nreg,td=0.,j=0;j<rbf->nreg;j++,index++) td += globcov[index];
                    globcov[iframes[k]*rbf->nreg] = td/(double)rbf->nreg;
                    }
                #endif
                }
            else {
                fprintf(stderr,"Using weights\n");
                for(j=0;j<rbf->nreg;j++) {
                    mean[j] /= nframes;
                    for(k=0;k<nframes;k++) {
                        index = iframes[k]*rbf->nreg+j;
                        globcov[index] -= mean[j];
                        }
                    }
                for(k=0;k<nframes;k++) {
                    for(index=iframes[k]*rbf->nreg,td=0.,j=0;j<rbf->nreg;j++,index++) {
                        td += weights[j]*globcov[index];
                        }
                    globcov[iframes[k]*rbf->nreg] = td;
                    } 
                }
            fflush(stderr);
            }

        //printf("here148\n");fflush(stdout);

        //if(!unmap_disk(mm)) return 0;
        //START200826
        if(unmap_disk(mm))return 0;

        //printf("here149\n");fflush(stdout);
        }

    //printf("here150\n");fflush(stdout);


    if(temp_float) free(temp_float);
    if(xform_file) {
        free(t4);
        free(A_or_B_or_U);
        free(bold_index);
        }
    if(scalingv[0]==6&&!weights) {
        fprintf(fp,"\n");
        fclose(fp);
        gsl_eigen_symmv_free(w);
        gsl_matrix_free(evec);
        gsl_vector_free(eval);
        free(r);
        free(den);
        }
    if(x) gsl_vector_free(x);
    if(tau) gsl_vector_free(tau);
    if(residualstack) free(residualstack);
    if(bstack) free(bstack);
    if(QRstack) free(QRstack);
    if(Astack) free(Astack);
    if(slope) free(slope);
    free(iframes);
    free(max);
    free(min);
    free(mean);
    free(stat);
    free(temp_double);
    return 1;
}
