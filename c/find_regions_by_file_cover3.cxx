/* Copyright 9/2/21 Washington University.  All Rights Reserved.
   find_regions_by_file_cover3.cxx  $Revision: 1.1 $*/

#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <set>
#include <gsl/gsl_sort_int.h>

//#include "find_regions_by_file_cover.h"
//START210901
#include "find_regions_by_file_cover3.h"

#include "d2intvar.h"
#include "constants.h"

//Regions_By_File *find_regions_by_file_cover(int num_region_files,int num_regions,Regions **reg,int *roi)
//START210901
Regions_By_File *find_regions_by_file_cover(int num_region_files,int num_regions,Regions **reg,int *roi,Files_Struct* roistr)

{
    int *nregions=NULL,i,j,m,n,*roinew=NULL;
    size_t i1,j1,*index=NULL;
    Regions_By_File *rbf;
    if(!num_region_files) {

        if(!(rbf=(Regions_By_File*)malloc(sizeof*rbf))) {
            printf("Error: Unable to allocate rbf\n");
            return NULL;
            }

        #if 0
        rbf->nvoxels = num_regions;
        if(!(rbf->indices=malloc(sizeof*rbf->indices*rbf->nvoxels))) {
            printf("Error: Unable to allocate rbf->indices\n");
            return NULL;
            }
        for(i=0;i<rbf->nvoxels;i++) rbf->indices[i] = i;
        #endif
        //START200224
        if(num_regions){ 
            rbf->nvoxels = num_regions;
            if(!(rbf->indices=(int*)malloc(sizeof*rbf->indices*rbf->nvoxels))) {
                printf("Error: Unable to allocate rbf->indices\n");
                return NULL;
                }
            for(i=0;i<rbf->nvoxels;i++) rbf->indices[i] = i;
            }
 
        }
    else {

        #if 0
        if(!(nregions=malloc(sizeof*nregions*num_region_files))) {
            printf("Error: Unable to allocate nregions\n");
            return NULL;
            }
        if(!(roinew=malloc(sizeof*roinew*num_regions))) {
            printf("Error: Unable to allocate roinew\n");
            return NULL;
            }
        if(!(index=malloc(sizeof*index*num_regions))) {
            printf("Error: Unable to allocate index\n");
            return NULL;
            }
        gsl_sort_int_index(index,roi,(size_t)1,(size_t)num_regions);
        for(i=0;i<num_regions;i++) roinew[i] = roi[index[i]];
        for(i=0;i<num_region_files;i++) nregions[i] = reg[i]->nregions;
        if(!(rbf=find_regions_by_file(num_region_files,num_regions,nregions,roinew)))return NULL;
        #endif
        //START210901
        if(!(rbf=init_rbf(num_region_files,num_regions)))return NULL;
        if(!(index=(size_t*)malloc(sizeof*index*num_regions))) {
            printf("fidlError: Unable to allocate index\n");
            return NULL;
            }
        if(roi){ 
            if(!(nregions=(int*)malloc(sizeof*nregions*num_region_files))) {
                printf("fidlError: Unable to allocate nregions\n");
                return NULL;
                }
            if(!(roinew=(int*)malloc(sizeof*roinew*num_regions))) {
                printf("fidlError: Unable to allocate roinew\n");
                return NULL;
                }
            gsl_sort_int_index(index,roi,(size_t)1,(size_t)num_regions);
            for(i=0;i<num_regions;i++) roinew[i] = roi[index[i]];
            for(i=0;i<num_region_files;i++) nregions[i] = reg[i]->nregions;
            if(!(rbf=find_regions_by_file(num_region_files,num_regions,nregions,roinew,rbf)))return NULL;
            }
        else{
            if(!(rbf=find_regions_by_file_roistr(num_region_files,num_regions,reg,roistr,rbf)))return NULL;
            for(i=0;i<num_regions;i++) index[i]=i;
            }

        //START190910
        for(i=0;i<3;++i)rbf->c_orient[i]=reg[0]->c_orient[i];
        for(i=1;i<num_region_files;++i){
            if(rbf->c_orient[0]!=reg[i]->c_orient[0]||rbf->c_orient[1]!=reg[i]->c_orient[1]||rbf->c_orient[2]!=reg[i]->c_orient[2]){
                for(int j=0;j<3;++j)rbf->c_orient[j]=-1;
                break;
                }
            }
#if 0
    for(i=0;i<3;++i)dp->c_orientall[i]=dp->c_orient[0][i];
    for(i=1;i<nfiles;++i){
        if(dp->c_orient[i][0]!=dp->dp->c_orientall[i][0]||dp->c_orient[i][1]!=dp->dp->c_orientall[i][1]||
            dp->c_orient[i][2]!=dp->dp->c_orientall[i][2]){
            for(j=0;j<3;j++)dp->c_orientall[j]=-1;
            }
        }
#endif

        rbf->nreg = num_regions;
        if(!(rbf->nvoxels_region=(int*)malloc(sizeof*rbf->nvoxels_region*num_regions))) {
            printf("Error: Unable to allocate rbf->nvoxels_region\n");
            return NULL;
            } 
        if(!(rbf->harolds_num=(int*)malloc(sizeof*rbf->harolds_num*num_regions))) {
            printf("Error: Unable to allocate rbf->harolds_num\n");
            return NULL;
            }
        for(n=m=0;m<num_region_files;m++) if(reg[m]->length) n++;

        if(n==num_region_files) {
            if(!(rbf->region_names_ptr=(char**)malloc(sizeof*rbf->region_names_ptr*num_regions))) { 
                printf("Error: Unable to allocate rbf->region_names_ptr\n");
                return NULL;
                }
            if(!(rbf->length=(int*)malloc(sizeof*rbf->length*num_regions))) {
                printf("Error: Unable to allocate rbf->length\n");
                return NULL;
                }
            for(rbf->max_length=n=m=0;m<num_region_files;m++) {
                for(j=0;j<rbf->num_regions_by_file[m];j++,n++) {
                    rbf->region_names_ptr[index[n]] = reg[m]->region_names[rbf->roi_by_file[m][j]];
                    if((rbf->length[index[n]]=reg[m]->length[rbf->roi_by_file[m][j]]) > rbf->max_length)
                        rbf->max_length = rbf->length[n];
                    }
                }
            }
        else {
            rbf->region_names_ptr = (char**)NULL;
            rbf->length = (int*)NULL;
            }

        //printf("here100\n");fflush(stdout);
        //for(j=0;j<num_regions;j++){printf("rbf->region_names_ptr[%d]=%s\n",j,rbf->region_names_ptr[j]);fflush(stdout);}

        //START190909
        if(!reg[0]->voxel_indices)return rbf;

        #if 0
        for(rbf->nvoxels_region_max=rbf->nvoxels=j=m=0;m<num_region_files;m++) {
            for(i=0;i<rbf->num_regions_by_file[m];i++,j++) {
                rbf->nvoxels += rbf->nvoxels_region[index[j]] = reg[m]->nvoxels_region[rbf->roi_by_file[m][i]];
                rbf->harolds_num[index[j]] = reg[m]->harolds_num[rbf->roi_by_file[m][i]];
                if(rbf->nvoxels_region_max < reg[m]->nvoxels_region[rbf->roi_by_file[m][i]]) 
                    rbf->nvoxels_region_max = reg[m]->nvoxels_region[rbf->roi_by_file[m][i]];
                }
            }
        if(!(rbf->indices=malloc(sizeof*rbf->indices*rbf->nvoxels))) {
            printf("Error: Unable to allocate rbf->indices\n");
            return NULL;
            }
        if(reg[0]->voxel_indices) {
            for(j1=0,m=0;m<num_region_files;m++) {
                for(i=0;i<rbf->num_regions_by_file[m];i++,j1++) {
                    for(n=0,i1=0;i1<index[j1];i1++)n+=rbf->nvoxels_region[i1]; 
                    for(j=0;j<reg[m]->nvoxels_region[rbf->roi_by_file[m][i]];j++) {
                        rbf->indices[n+j] = reg[m]->voxel_indices[rbf->roi_by_file[m][i]][j];
                        }
                    }
                }
            }
        if(!(rbf->indices_uniqsort=malloc(sizeof*rbf->indices_uniqsort*rbf->nvoxels))) {
            printf("Error: Unable to allocate rbf->indices_uniqsort\n");
            return NULL;
            }
        for(m=0;m<rbf->nvoxels;m++) rbf->indices_uniqsort[m]=rbf->indices[m];
        gsl_sort_int(rbf->indices_uniqsort,1,rbf->nvoxels);
        for(n=rbf->nvoxels,m=0;m<n-1;) {
            if(rbf->indices_uniqsort[m]==rbf->indices_uniqsort[m+1]) {
                for(i=m+1;i<n;i++) rbf->indices_uniqsort[i-1]=rbf->indices_uniqsort[i];
                n--;
                }
            else {
                m++;
                }
            }
        rbf->nindices_uniqsort=n;
        if(n<rbf->nvoxels)rbf->indices_uniqsort=realloc(rbf->indices_uniqsort,n*sizeof*rbf->indices_uniqsort);
        #endif
        //START190909
        if(reg[0]->voxel_indices){

            //printf("here101\n");fflush(stdout);

            for(rbf->nvoxels_region_max=rbf->nvoxels=j=m=0;m<num_region_files;m++) {
                for(i=0;i<rbf->num_regions_by_file[m];i++,j++) {
                    rbf->nvoxels += rbf->nvoxels_region[index[j]] = reg[m]->nvoxels_region[rbf->roi_by_file[m][i]];
                    rbf->harolds_num[index[j]] = reg[m]->harolds_num[rbf->roi_by_file[m][i]];
                    if(rbf->nvoxels_region_max < reg[m]->nvoxels_region[rbf->roi_by_file[m][i]]) 
                        rbf->nvoxels_region_max = reg[m]->nvoxels_region[rbf->roi_by_file[m][i]];
                    }
                }

            //printf("here102\n");fflush(stdout);

            if(!(rbf->indices=(int*)malloc(sizeof*rbf->indices*rbf->nvoxels))) {
                printf("Error: Unable to allocate rbf->indices\n");
                return NULL;
                }
            if(reg[0]->voxel_indices) {
                for(j1=0,m=0;m<num_region_files;m++) {
                    for(i=0;i<rbf->num_regions_by_file[m];i++,j1++) {
                        for(n=0,i1=0;i1<index[j1];i1++)n+=rbf->nvoxels_region[i1]; 
                        for(j=0;j<reg[m]->nvoxels_region[rbf->roi_by_file[m][i]];j++) {
                            rbf->indices[n+j] = reg[m]->voxel_indices[rbf->roi_by_file[m][i]][j];
                            }
                        }
                    }
                }

            //printf("here103\n");fflush(stdout);


            #if 0
//Replace all this code with a set, see how fast it runs
            if(!(rbf->indices_uniqsort=(int*)malloc(sizeof*rbf->indices_uniqsort*rbf->nvoxels))) {
                printf("Error: Unable to allocate rbf->indices_uniqsort\n");
                return NULL;
                }
            for(m=0;m<rbf->nvoxels;m++) rbf->indices_uniqsort[m]=rbf->indices[m];

            printf("here104\n");fflush(stdout);

            gsl_sort_int(rbf->indices_uniqsort,1,rbf->nvoxels);

            printf("here105 rbf->nvoxels=%d\n",rbf->nvoxels);fflush(stdout);

            for(n=rbf->nvoxels,m=0;m<n-1;) {
                if(rbf->indices_uniqsort[m]==rbf->indices_uniqsort[m+1]) {
                    for(i=m+1;i<n;i++) rbf->indices_uniqsort[i-1]=rbf->indices_uniqsort[i];
                    n--;
                    }
                else {
                    m++;
                    }
                }

            printf("here106\n");fflush(stdout);

            rbf->nindices_uniqsort=n;
            if(n<rbf->nvoxels)rbf->indices_uniqsort=(int*)realloc(rbf->indices_uniqsort,n*sizeof*rbf->indices_uniqsort);
            #endif
            //START210909
            std::set<int> s(rbf->indices,rbf->indices+rbf->nvoxels);            
            rbf->nindices_uniqsort=s.size();
            if(!(rbf->indices_uniqsort=(int*)malloc(sizeof*rbf->indices_uniqsort*rbf->nindices_uniqsort))) {
                printf("fidlError: Unable to allocate rbf->indices_uniqsort\n");
                return NULL;
                }
            m=0;
            for(auto it=s.begin();++it!=s.end();){ 
                rbf->indices_uniqsort[m++]=*it;
                }
            //printf("here105 rbf->nvoxels=%d rbf->nindices_uniqsort=%d\n",rbf->nvoxels,rbf->nindices_uniqsort);fflush(stdout);
            

            }



        #if 0
        free(index);
        free(roinew);
        free(nregions);
        #endif
        //START190909
        if(index)free(index);
        if(roinew)free(roinew);
        if(nregions)free(nregions);

        }
    return rbf;
}

//START210901
Regions_By_File *init_rbf(int num_region_files,int num_regions){
    Regions_By_File *rbf;
    if(!(rbf=(Regions_By_File*)malloc(sizeof*rbf))) {
        printf("fidlError: Unable to allocate rbf\n");
        return NULL;
        }
    if(!(rbf->num_regions_by_file=(int*)malloc(sizeof*rbf->num_regions_by_file*num_region_files))) {
        printf("fidlError: Unable to allocate rbf->num_regions_by_file\n");
        return NULL;
        }
    for(int i=0;i<num_region_files;i++) rbf->num_regions_by_file[i]=0;
    if(!(rbf->file_index=(int*)malloc(sizeof*rbf->file_index*num_regions))) {
        printf("fidlError: Unable to allocate rbf->file_index\n");
        return NULL;
        }
    if(!(rbf->roi_index=(int*)malloc(sizeof*rbf->roi_index*num_regions))) {
        printf("Error: Unable to allocate rbf->roi_index\n");
        return NULL;
        }
    rbf->nvoxels_region = NULL;
    rbf->length = NULL;
    rbf->indices = NULL;
    rbf->indices_uniqsort = NULL;
    rbf->indices_uniqsort = NULL;
    rbf->harolds_num = NULL;
    rbf->region_names_ptr = NULL;
    return rbf;
    }

//Regions_By_File *find_regions_by_file(int num_region_files,int num_regions,int *nregions,int *roi)
//START210901
Regions_By_File *find_regions_by_file(int num_region_files,int num_regions,int *nregions,int *roi,Regions_By_File *rbf)

{
    int i,j,k,m,n,region_count=0;

    //START210902
    if(!rbf)if(!(rbf=init_rbf(num_region_files,num_regions)))return NULL;

    //START20901
    #if 0
    Regions_By_File *rbf;
    if(!(rbf=malloc(sizeof*rbf))) {
        printf("Error: Unable to allocate rbf\n");
        return NULL;
        }
    if(!(rbf->num_regions_by_file=malloc(sizeof*rbf->num_regions_by_file*num_region_files))) {
        printf("Error: Unable to allocate rbf->num_regions_by_file\n");
        return NULL;
        }
    for(i=0;i<num_region_files;i++) rbf->num_regions_by_file[i]=0;
    if(!(rbf->file_index=malloc(sizeof*rbf->file_index*num_regions))) {
        printf("Error: Unable to allocate rbf->file_index\n");
        return NULL;
        }
    if(!(rbf->roi_index=malloc(sizeof*rbf->roi_index*num_regions))) {
        printf("Error: Unable to allocate rbf->roi_index\n");
        return NULL;
        }
    #endif


    for(region_count=i=0;region_count<num_regions;) {
        for(n=m=0;m<num_region_files&&region_count<num_regions;m++) {
            k=0;
            if(roi[i]==-1) {
                rbf->file_index[region_count]=-1;
                rbf->roi_index[region_count]=-1;
                k++;i++;region_count++;
                }
            else {
                /*printf("hereA roi[%d]=%d nregions[%d]=%d n=%d\n",i,roi[i],m,nregions[m],n); fflush(stdout);*/
                for(;roi[i]>=n&&roi[i]<nregions[m]+n;) {
                    /*printf("hereB roi[%d]=%d nregions[%d]=%d n=%d\n",i,roi[i],m,nregions[m],n); fflush(stdout);*/
                    for(j=n;j<nregions[m]+n;j++) {
                        if(j == roi[i]) {
                            rbf->file_index[region_count]=m;
                            rbf->roi_index[region_count]=roi[i]-n;
                            k++;i++;region_count++;
                            break;
                            }
                        }
                    /*printf("hereC region_count=%d num_regions=%d\n",region_count,num_regions);fflush(stdout);*/
                    if(region_count == num_regions) break;
                    }
                }
            /*printf("hereD\n");fflush(stdout);*/
            rbf->num_regions_by_file[m] += k;
            n += nregions[m]; /*n must be incremented before m*/
            }
        }

    //std::cout<<"here100 num_region_files="<<num_region_files<<std::endl;
    //std::cout<<"rbf->num_regions_by_file=";for(i=0;i<num_region_files;++i)std::cout<<rbf->num_regions_by_file[i]<<" ";std::cout<<std::endl;


    if(!(rbf->roi_by_file=d2intvar(num_region_files,rbf->num_regions_by_file))) return NULL;
    for(m=0;m<num_region_files;m++) rbf->num_regions_by_file[m]=0;

    #if 1
    for(region_count=i=0;region_count<num_regions;) {
        for(n=m=0;m<num_region_files&&region_count<num_regions;m++) {
            k=0;
            if(roi[i]==-1) {
                rbf->roi_by_file[m][rbf->num_regions_by_file[m]++] = roi[i++];
                k++;
                }
            else {
                for(;roi[i]>=n&&roi[i]<nregions[m]+n;) {
                    for(j=n;j<nregions[m]+n;j++) {
                        if(j == roi[i]) {
                            rbf->roi_by_file[m][rbf->num_regions_by_file[m]++] = roi[i++] - n;
                            k++;
                            break;
                            }
                        }
                    if(k+region_count == num_regions) break;
                    }
                }
            region_count += k;
            n += nregions[m]; /*n must be incremented before m*/
            }
        }
    #endif
    //START210902
    #if 0 
    for(int i=0;i<num_regions;++i){
        if(roi[i]==-1) {
            }
        else{
            rbf->roi_by_file[rbf->file_index[i]][rbf->num_regions_by_file[rbf->file_index[i]]++]=rbf->roi_index[i];
            }
        }
    #endif


    //for(m=0;m<num_region_files;++m){
    //    for(i=0;i<rbf->num_regions_by_file[m];++i){
    //        std::cout<<"here100 rbf->roi_by_file["<<m<<"]["<<i<<"]="<<rbf->roi_by_file[m][i]<<std::endl;
    //        }
    //    }














    for(rbf->num_regions_by_file_max=i=0;i<num_region_files;i++) 
        if(rbf->num_regions_by_file[i]>rbf->num_regions_by_file_max) rbf->num_regions_by_file_max=rbf->num_regions_by_file[i];

    //START210901
    #if 0
    rbf->nvoxels_region = NULL;
    rbf->length = NULL;
    rbf->indices = NULL;
    rbf->indices_uniqsort = NULL;
    rbf->indices_uniqsort = NULL;
    rbf->harolds_num = NULL;
    rbf->region_names_ptr = NULL;
    #endif

    return rbf;
}

//START210901
Regions_By_File *find_regions_by_file_roistr(int num_region_files,int num_regions,Regions **reg,Files_Struct* roistr,Regions_By_File *rbf)
{
    int i,j,k=0;
    for(i=0;i<num_regions;++i){
        for(j=0;j<num_region_files;++j){
            for(k=0;k<reg[j]->nregions;++k){
                //printf("reg[%d]->region_names[%d]=%s roistr->files[%d]=%s\n",j,k,reg[j]->region_names[k],i,roistr->files[i]);
                if(!strcmp(reg[j]->region_names[k],roistr->files[i])){
                    //break;
                    goto superbird; 
                    }
                }
            }
        superbird:
        if(j==num_region_files){
            printf("fidlError: %s not found in find_regions_by_file_roistr\n",roistr->files[i]);
            return NULL;
            }
        rbf->file_index[i]=j;
        rbf->roi_index[i]=k;
        rbf->num_regions_by_file[j]++;
        }
    if(!(rbf->roi_by_file=d2intvar(num_region_files,rbf->num_regions_by_file)))return NULL;
    for(i=0;i<num_region_files;i++)rbf->num_regions_by_file[i]=0;
    for(i=0;i<num_regions;++i){
        rbf->roi_by_file[rbf->file_index[i]][rbf->num_regions_by_file[rbf->file_index[i]]++]=rbf->roi_index[i];
        }

    #if 1 
    for(i=0;i<num_region_files;++i){
        for(j=0;j<rbf->num_regions_by_file[i];++j){
            //printf("rbf->roi_by_file[%d][%d]=%d\n",i,j,rbf->roi_by_file[i][j]);
            fprintf(stderr,"rbf->roi_by_file[%d][%d]=%d\n",i,j,rbf->roi_by_file[i][j]);
            }
        }
    fflush(stderr);
    #endif

    for(rbf->num_regions_by_file_max=i=0;i<num_region_files;i++) 
        if(rbf->num_regions_by_file[i]>rbf->num_regions_by_file_max)rbf->num_regions_by_file_max=rbf->num_regions_by_file[i];
    return rbf;
}

void free_regions_by_file(Regions_By_File *rbf)
{
    if(rbf->nvoxels_region) free(rbf->nvoxels_region);
    if(rbf->length) free(rbf->length);
    if(rbf->indices) free(rbf->indices);
    if(rbf->indices_uniqsort) free(rbf->indices_uniqsort);
    if(rbf->harolds_num) free(rbf->harolds_num);
    if(rbf->region_names_ptr) free(rbf->region_names_ptr);
    free(rbf->roi_by_file[0]);
    free(rbf->roi_by_file);
    free(rbf->file_index);
    free(rbf->roi_index);
    free(rbf->num_regions_by_file);
    free(rbf);
}

int crs(double *temp_double,double *region_stack,Regions_By_File *rbf,char *glmfile)
{
    int i,j,k,count;
    for(k=i=0;i<rbf->nreg;i++) {
        for(region_stack[i]=0.,count=j=0;j<rbf->nvoxels_region[i];j++,k++) {
            if(temp_double[rbf->indices[k]]!=(double)UNSAMPLED_VOXEL&&temp_double[rbf->indices[k]]!= 0.&&
                isfinite(temp_double[rbf->indices[k]])){
                region_stack[i] += temp_double[rbf->indices[k]];
                count++;
                }
            }
        count ? (region_stack[i]/=(double)count) : (region_stack[i]=(double)UNSAMPLED_VOXEL);
        if(!count&&glmfile) {
            printf("Error: Region %s has zero voxels in %s\n",rbf->region_names_ptr[i],glmfile);
            return 0;
            }
        }
    return 1;
}

/*START130815*/
#if 0
KEEP
int crs(double *temp_double,double *region_stack,int nreg,int *nvoxels_region,int *indices,int *maskidx,char **region_names_ptr,
    char *glmfile) {
    int i,j,k,count;
    for(k=i=0;i<nreg;i++) {
        for(region_stack[i]=0.,count=j=0;j<nvoxels_region[i];j++,k++) {
            td = !maskidx ? temp_double[indices[k]] : temp_double[maskidx[indices[k]]];
            if(td!=(double)UNSAMPLED_VOXEL && td!= 0. &&
              #ifdef __sun__
                !IsNANorINF(td)
              #else
                (!isnan(td) || !isinf(td))
              #endif
                ) {
                region_stack[i] += td;
                count++;
                }
            }
        count ? (region_stack[i]/=(double)count) : (region_stack[i]=(double)UNSAMPLED_VOXEL);
        if(!count&&glmfile) {
            printf("Error: Region %s has zero voxels in %s\n",region_names_ptr[i],glmfile);
            return 0;
            }
        }
    return 1;
    }
#endif

//START211103
int crs_rms(double *temp_double,double *region_stack,Regions_By_File *rbf,char *glmfile)
{
    int i,j,k,count;
    for(k=i=0;i<rbf->nreg;i++) {
        for(region_stack[i]=0.,count=j=0;j<rbf->nvoxels_region[i];j++,k++) {
            if(temp_double[rbf->indices[k]]!=(double)UNSAMPLED_VOXEL&&temp_double[rbf->indices[k]]!= 0.&&
                isfinite(temp_double[rbf->indices[k]])){
                region_stack[i] += temp_double[rbf->indices[k]]*temp_double[rbf->indices[k]];
                count++;
                }
            }

        //count ? (region_stack[i]/=(double)count) : (region_stack[i]=(double)UNSAMPLED_VOXEL);
        //START211103
        if(!count){
            region_stack[i]=(double)UNSAMPLED_VOXEL;
            }
        else{
            region_stack[i]/=(double)count;
            region_stack[i]=sqrt(region_stack[i]);
            }

        if(!count&&glmfile) {
            printf("fidlError: Region %s has zero voxels in %s\n",rbf->region_names_ptr[i],glmfile);
            return 0;
            }
        }
    return 1;
}

                                                                                        /*rbf->indices*/
void crsw(double *temp_double,double *region_stack,Regions_By_File *rbf,double *weights,int *weightsi)
{
    int i,j,k;
    double weights_sum;
    for(k=i=0;i<rbf->nreg;i++) {
        for(weights_sum=region_stack[i]=0.,j=0;j<rbf->nvoxels_region[i];j++,k++) {

            /*printf("%e ",temp_double[rbf->indices[k]]);
            if(fabs(temp_double[rbf->indices[k]])>(double)UNSAMPLED_VOXEL) printf("yes "); else printf("no ");*/

            #if 0
            if(temp_double[rbf->indices[k]] != (double)UNSAMPLED_VOXEL && temp_double[rbf->indices[k]] != 0. &&

              /*#ifndef LINUX*/
              #ifdef __sun__

                !IsNANorINF(temp_double[rbf->indices[k]])
              #else
                (!isnan(temp_double[rbf->indices[k]]) || !isinf(temp_double[rbf->indices[k]]))
              #endif
                ) {
                region_stack[i] += temp_double[rbf->indices[k]]*weights[weightsi[k]];
                weights_sum += weights[weightsi[k]];
                }
            #endif
            //START170724
            if(temp_double[rbf->indices[k]]!=(double)UNSAMPLED_VOXEL && temp_double[rbf->indices[k]]!=0. &&
                isfinite(temp_double[rbf->indices[k]])){
                region_stack[i] += temp_double[rbf->indices[k]]*weights[weightsi[k]];
                weights_sum += weights[weightsi[k]];
                }


            }
        /*printf("\ni=%d weights_sum=%f region_stack=%f\n",i,weights_sum,region_stack[i]);*/
        weights_sum ? (region_stack[i]/=weights_sum) : (region_stack[i]=(double)UNSAMPLED_VOXEL);
        /*printf("after i=%d weights_sum=%f region_stack=%f\n",i,weights_sum,region_stack[i]);*/
        }
}

