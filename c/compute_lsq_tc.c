/* Copyright 11/7/01 Washington University.  All Rights Reserved.
   compute_lsq_tc.c  $Revision: 1.14 $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>
#include <nrutil.h>

enum{Truncate = 7,Modulo=50000};

typedef struct Design_struct {
    int nregion,nvoxel,how_many,*ntc,*ntimepoints,max_ntc;
    char **region_str,**tc_str,**tc_file;
    float *TR,***tc;
    }
Design;

typedef struct Parameter_struct {
    char **gamma_names;
    int *ngamma;
    float ***stimlen_sec,***delay_sec;
    }
Parameter;

Design *read_design(char *design_file);
Parameter *read_parameter(char *parameter_file,Design *d,char *design_file);

static char rcsid[] = "$Header: /home/hannah/mcavoy/idl/clib/RCS/compute_lsq_tc.c,v 1.14 2003/04/23 21:53:31 mcavoy Exp $";

/************************/
main(int argc,char **argv)
/************************/
{
char *design_file,*parameter_file,*output_file,filename[MAXNAME],*str_ptr;
int i,j,k,m,n,r,s,t,lengthy,lengthb,lc_check_regressors=(int)FALSE,lc_scale_column_globally=(int)FALSE,lc_check_design=(int)FALSE,
    lc_check_parameters=(int)FALSE,*lenvol,*unsampled_mask;
float time,*regressor,min,**max,global_max,*R2_stack,*R2a_stack,**yhat_stack,**B_stack;
double **Y,**X,**XTX,**XTXm1,**XTY,**B,**Yhat,mean,temp,SSE,SSyy,R2,R2a;
Design *d;
Parameter *p;
Memory_Map **mm;
Interfile_header *ifh;
FILE *fp;

print_version_number(rcsid);
if(argc < 3) {
    fprintf(stderr,"        -design:               Text file that contains the design.\n");
    fprintf(stderr,"        -parameters:           Text file that contains the boynton parameters (stimulus duration and delay).\n");
    fprintf(stderr,"        -output:               Output filename that regional beta values are written to.\n");
    fprintf(stderr,"        -scale_column_globally Each regressor is a column in the design matrix. If different durations are\n");
    fprintf(stderr,"                               used for each timecourse, the gamma function of longest duration will have the\n");
    fprintf(stderr,"                               largest raw amplitude. Dividing by the maximum raw amplitude for the entire\n");
    fprintf(stderr,"                               column preserves this amplitude difference.\n");
    fprintf(stderr,"        -check_regressors      Echos the design matrix.\n");
    fprintf(stderr,"        -check_design          Echos the design as read from the design file.\n");
    fprintf(stderr,"        -check_parameters      Echos the parameters as read from the parameter file.\n");
    exit(-1);
    }
for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-design") && argc > i+1)
        design_file = argv[++i];
    if(!strcmp(argv[i],"-parameters") && argc > i+1)
        parameter_file = argv[++i];
    if(!strcmp(argv[i],"-output") && argc > i+1)
        output_file = argv[++i];
    if(!strcmp(argv[i],"-scale_column_globally"))
        lc_scale_column_globally = (int)TRUE;
    if(!strcmp(argv[i],"-check_regressors"))
        lc_check_regressors = (int)TRUE;
    if(!strcmp(argv[i],"-check_design"))
        lc_check_design = (int)TRUE;
    if(!strcmp(argv[i],"-check_parameters"))
        lc_check_parameters = (int)TRUE;
    }

if(!(d = (Design *)read_design(design_file))) exit(-1);
if(lc_check_design == (int)TRUE) {
    for(m=i=0;i<d->nregion;i++) {
        printf("%sEND\nTR = %f\n",d->region_str[i],d->TR[i]);
        for(j=0;j<d->ntc[i];j++) {
            printf("%s := ",d->tc_str[m++]); 
            for(k=0;k<d->ntimepoints[i];k++) printf("%g ",d->tc[i][j][k]);
            printf("\n");
            }
        printf("\n");
        }
    for(m=i=0;i<d->nvoxel;i++) {
        printf("TR = %f\n",d->TR[i]);
        for(j=0;j<d->ntc[i];j++) {
            printf("%s := ",d->tc_str[m]);
            for(k=0;k<d->ntimepoints[i];k++) printf("%s ",d->tc_file[m++]);
            printf("\n");
            }
        printf("\n");
        }
    }

if(!(p = (Parameter *)read_parameter(parameter_file,d,design_file))) exit(-1);
if(lc_check_parameters == (int)TRUE) {
    /*for(m=i=0;i<d->nregion;i++) {*/
    for(m=i=0;i<d->how_many;i++) {
        printf("%s\nTR = %f\nstimlen_secs\n",d->region_str[i],d->TR[i]);
        for(j=0;j<d->ntc[i];j++) {
            printf("%s := ",d->tc_str[m++]);
            for(k=0;k<p->ngamma[i];k++) printf("%g ",p->stimlen_sec[i][j][k]);
            printf("\n");
            }
        printf("delay_sec\n");
        for(m-=d->ntc[i],j=0;j<d->ntc[i];j++) {
            printf("%s := ",d->tc_str[m++]);
            for(k=0;k<p->ngamma[i];k++) printf("%g ",p->delay_sec[i][j][k]);
            printf("\n");
            }
        printf("\n");
        }
    }

GETMEM(lenvol,d->how_many,int);
if(d->nregion) {
    if(!(fp = fopen(output_file,"w"))) {
        printf("Error opening %s in compute_lsq_tc.\n",output_file);
        exit(-1);
        }
    for(j=0;j<d->how_many;j++) lenvol[j] = 1; 
    }
else {
    for(m=i=0;i<d->nvoxel;i++) {
        for(j=0;j<d->ntc[i];j++,m++) {
            if(!(ifh = read_ifh(d->tc_file[m]))) exit(-1);
            if(!j) {
                lenvol[i] = ifh->dim1*ifh->dim2*ifh->dim3;
                d->ntimepoints[i] = ifh->dim4;
                }
            else if(lenvol[i]*d->ntimepoints[i] != ifh->dim1*ifh->dim2*ifh->dim3*ifh->dim4) {
                printf("Timecourse files are not all the same size. Abort!\n");
                exit(-1);
                }
            free_ifh(ifh,(int)FALSE);
            }
        }
    }

for(t=r=i=0;i<d->how_many;i++) {
    GETMEM(regressor,d->ntimepoints[i],float);
    max = (float **)matrix(1,p->ngamma[i],1,d->ntc[i]);
    lengthy = d->ntc[i]*d->ntimepoints[i];
    lengthb = p->ngamma[i]+1;
    X = (double **)dmatrix(1,lengthy,1,lengthb);
    Y = (double **)dmatrix(1,lengthy,1,1); /*segmentation fault occurs when freeing allocate_2Dfloat unless it is last*/

    if(d->nregion) {
        for(m=1,j=0;j<d->ntc[i];j++) for(k=0;k<d->ntimepoints[i];k++,m++) Y[m][1] = (double)d->tc[i][j][k];
        }
    else {
        if(!(yhat_stack = (float **)d2float(d->ntc[i],d->ntimepoints[i]*lenvol[i]))) exit(-1);
        GETMEM_0(unsampled_mask,lenvol[i],int);
        GETMEM(mm,d->ntc[i],Memory_Map *);
        for(j=0;j<d->ntc[i];j++,t++) {
            if(!(mm[j] = map_disk(d->tc_file[t],d->ntimepoints[i]*lenvol[i],0))) exit(-1);
            for(k=0;k<lenvol[i];k++) { /*unsampled voxels are marked with a 1*/
                if(mm[j]->ptr[k] == (float)UNSAMPLED_VOXEL) unsampled_mask[k] = 1;
                }
            }
        t -= d->ntc[i];
        printf("Files memory mapped.\n");
        GETMEM(R2_stack,lenvol[i],float);
        GETMEM(R2a_stack,lenvol[i],float);
        if(!(B_stack = (float **)d2float(lengthb,lenvol[i]))) exit(-1);
        }

    for(n=j=0;j<d->ntc[i];j++,n+=d->ntimepoints[i]) {
        if(lc_check_regressors == (int)TRUE) printf("%s\n",d->tc_str[j]);
        for(k=0;k<p->ngamma[i];k++) {
            if(p->delay_sec[i][j][k]<0 && p->stimlen_sec[i][j][k]<0) {
                for(m=0;m<d->ntimepoints[i];m++) regressor[m] = 0;
                max[k+1][j+1] = lc_scale_column_globally==(int)FALSE ? 1 : 0;
                }
            else {
                if(lc_check_regressors == (int)TRUE && p->gamma_names) printf("%s\n",p->gamma_names[k]);
                for(m=0;m<d->ntimepoints[i];m++) {
                    if((time = d->TR[i]*m - p->delay_sec[i][j][k]) < 0.) time = 0;
                    regressor[m] = boynton_model(time,p->stimlen_sec[i][j][k],(float)HRF_DELTA,(float)HRF_TAU,(int)TRUE);
                    if(lc_check_regressors == (int)TRUE)
                        printf("time=%f stimlen=%f hrf=%f\n",time,p->stimlen_sec[i][j][k],regressor[m]);
                    }
                if(lc_check_regressors == (int)TRUE) printf("\n");
                min_and_max(regressor,d->ntimepoints[i],&min,&max[k+1][j+1]);
                }
            global_max = lc_scale_column_globally==(int)FALSE ? max[k+1][j+1] : 1;
            for(m=0;m<d->ntimepoints[i];m++) {
                X[m+1+n][k+1] = (double)(regressor[m]/global_max);
                }
            }
        }

    if(lc_scale_column_globally == (int)TRUE) {
        for(k=0;k<p->ngamma[i];k++) {
            min_and_max(&max[k+1][1],d->ntc[i],&min,&global_max);
            if(lc_check_regressors == (int)TRUE && p->gamma_names) printf("%s global_max = %f\n",p->gamma_names[k],global_max);
            for(m=1;m<=d->ntimepoints[i]*d->ntc[i];m++) X[m][k+1] /= (double)global_max;
            }
        }
    for(j=1;j<=lengthy;j++) X[j][lengthb] = 1;
    if(lc_check_regressors == (int)TRUE) {
        for(j=1;j<=lengthy;j++) {
            for(k=1;k<=lengthb;k++) printf("%f ",X[j][k]);
            printf("\n");
            if(!(j%d->ntimepoints[i])) printf("\n");
            }
        printf("\n");
        }
    XTX = (double **)dmatrix_mult(X,X,lengthy,lengthb,lengthy,lengthb,(int)TRANSPOSE_FIRST);
    XTXm1 = (double **)pinv(XTX,lengthb,lengthb,(double)0.);

    if(d->nvoxel) printf("A total of %d voxels will be processed over %d timepoints.\n",lenvol[i],d->ntimepoints[i]);

    for(s=0;s<lenvol[i];s++) {

        if(d->nvoxel) {
            if(!(s%Modulo)) fprintf(stdout,"Processing voxel = %d\n",s);
            if(unsampled_mask[s]) continue; 
            for(m=1,j=0;j<d->ntc[i];j++) {
                for(k=0;k<d->ntimepoints[i];k++,m++) {
                    Y[m][1] = (double)mm[j]->ptr[lenvol[i]*k+s];
                    }
                }
            }


        XTY = (double **)dmatrix_mult(X,Y,lengthy,lengthb,lengthy,1,(int)TRANSPOSE_FIRST);
        B = (double **)dmatrix_mult(XTXm1,XTY,lengthb,lengthb,lengthb,1,(int)TRANSPOSE_NONE);
        Yhat = (double **)dmatrix_mult(X,B,lengthy,lengthb,lengthb,1,(int)TRANSPOSE_NONE);
        mean = SSE = SSyy = 0;
        for(j=1;j<=lengthy;j++) mean += Y[j][1];
        mean /= lengthy; 
        for(j=1;j<=lengthy;j++) {
            temp = Y[j][1] - Yhat[j][1];        
            SSE += temp*temp;
            temp = Y[j][1] - mean;
            SSyy += temp*temp;
            }
        R2 = 1 - SSE/SSyy;
        R2a = 1 - (double)(lengthy-1)/(lengthy-lengthb)*SSE/SSyy;

        /*printf("SSE=%f SSyy=%f lengthy=%d lengthb=%d R2a=%f\n",SSE,SSyy,lengthy,lengthb,R2a);*/

        if(d->nregion) {
            fprintf(fp,"REGION : %s\n",d->region_str[i]);
            fprintf(fp,"BETA = "); for(j=1;j<=lengthb;j++) fprintf(fp,"%g ",B[j][1]); fprintf(fp,"<-constant\n");
            n = r;
            for(j=0;j<d->ntc[i];j++,r++) fprintf(fp,"%-20s",d->tc_str[r]); fprintf(fp,"\n");
            for(j=0;j<d->ntc[i];j++) fprintf(fp,"actual predicted    "); fprintf(fp,"\n");
            for(j=0;j<d->ntc[i];j++) fprintf(fp,"------ ---------    "); fprintf(fp,"\n");
            for(k=1;k<=d->ntimepoints[i];k++) {
                for(m=k,j=0;j<d->ntc[i];j++,m+=d->ntimepoints[i]) fprintf(fp,"%7.4f  %7.4f    ",Y[m][1],Yhat[m][1]);
                fprintf(fp,"\n");
                }
            fprintf(fp,"R2 = %.4f\nR2(adjusted) = %.4f\n\n",R2,R2a);
            for(r=n,k=(Truncate+1)*p->ngamma[i]+2,j=0;j<d->ntc[i];j++,r++) fprintf(fp,"%-*s",k,d->tc_str[r]); fprintf(fp,"\n");
            if(p->gamma_names) {


                /*for(j=0;j<d->ntc[i];j++) {
                    for(k=0;k<p->ngamma[i];k++) fprintf(fp,"%-*s ",Truncate,p->gamma_names[k]);
                    fprintf(fp,"  ");
                    }*/

                for(j=0;j<d->ntc[i];j++) {
                    for(k=0;k<p->ngamma[i];k++) { 
                        strcpy(filename,p->gamma_names[k]);
                        filename[Truncate] = 0;
                        fprintf(fp,"%-*s ",Truncate,filename);
                        }
                    fprintf(fp,"  ");
                    }

                fprintf(fp,"\n");
                }
            for(m=0;m<d->ntimepoints[i];m++) {
                for(n=j=0;j<d->ntc[i];j++,n+=d->ntimepoints[i]) {
                    for(k=0;k<p->ngamma[i];k++) fprintf(fp,"%-7.4f ",X[m+1+n][k+1]*B[k+1][1]);
                    fprintf(fp,"  ");
                    }
                fprintf(fp,"\n");
                }
            fprintf(fp,"\n");
            }
        else {
            R2_stack[s] = (float)R2;
            R2a_stack[s] = (float)R2a;
            for(m=1,j=0;j<d->ntc[i];j++) {
                for(k=0;k<d->ntimepoints[i];k++,m++) {
                    yhat_stack[j][lenvol[i]*k+s] = (float)Yhat[m][1]; 
                    }
                }
            for(j=1;j<=lengthb;j++) B_stack[j-1][s] = (float)B[j][1];
            }
        free_dmatrix(XTY,1,lengthb,1,1);
        free_dmatrix(B,1,lengthb,1,1);
        free_dmatrix(Yhat,1,lengthy,1,1);

        } /*for(s=0;s<lenvol[i];s++) {*/

    free_dmatrix(XTX,1,lengthb,1,lengthb);
    free_dmatrix(XTXm1,1,lengthb,1,lengthb);
    free(regressor);
    free_matrix(max,1,p->ngamma[i],1,d->ntc[i]);
    free_dmatrix(X,1,lengthy,1,lengthb); 
    free_dmatrix(Y,1,lengthy,1,1);

    if(d->nvoxel) {
        if(!(ifh = read_ifh(d->tc_file[t]))) exit(-1);
        ifh->dim4 = 1;

        for(j=0;j<lenvol[i];j++) if(unsampled_mask[j]) R2_stack[j] = (float)UNSAMPLED_VOXEL;
        strcpy(filename,"R2.4dfp.img");
        if(!write_float(filename,R2_stack,lenvol[i])) exit(-1); 
        printf("Output written to %s\n",filename);
        min_and_max(R2_stack,lenvol[i],&ifh->global_min,&ifh->global_max);
        if(!write_ifh(filename,ifh,(int)FALSE)) exit(-1);

        for(j=0;j<lenvol[i];j++) if(unsampled_mask[j]) R2a_stack[j] = (float)UNSAMPLED_VOXEL;
        strcpy(filename,"R2a.4dfp.img");
        if(!write_float(filename,R2a_stack,lenvol[i])) exit(-1); 
        printf("Output written to %s\n",filename);
        min_and_max(R2a_stack,lenvol[i],&ifh->global_min,&ifh->global_max);
        if(!write_ifh(filename,ifh,(int)FALSE)) exit(-1);

        /*START*/
        for(j=0;j<lengthb;j++) {
            for(k=0;k<lenvol[i];k++) if(unsampled_mask[k]) B_stack[j][k] = (float)UNSAMPLED_VOXEL;
            if(j < lengthb -1) {
                sprintf(filename,"%s.4dfp.img",p->gamma_names[j]);
                }
            else {
                strcpy(filename,"constant.4dfp.img");
                }
            if(!write_float(filename,B_stack[j],lenvol[i])) exit(-1); 
            printf("Output written to %s\n",filename);
            min_and_max(B_stack[j],lenvol[i],&ifh->global_min,&ifh->global_max);
            if(!write_ifh(filename,ifh,(int)FALSE)) exit(-1);
            }

        free_ifh(ifh,(int)FALSE);

        for(j=0;j<d->ntc[i];j++,t++) {
            for(k=0;k<lenvol[i];k++) {
                if(unsampled_mask[k]) {
                    for(m=0;m<d->ntimepoints[i];m++) yhat_stack[j][m*lenvol[i]+k] = (float)UNSAMPLED_VOXEL;
                    }
                }
            strcpy(filename,d->tc_file[t]);
            *strstr(filename,".4dfp.img") = 0;
            strcat(filename,"_predicted.4dfp.img");

            if(str_ptr=strrchr(filename,'/')) {
                str_ptr++;
                }
            else {
                str_ptr = filename;
                }

            if(!write_float(str_ptr,yhat_stack[j],lenvol[i]*d->ntimepoints[i])) exit(-1);
            printf("Output written to %s\n",str_ptr);
            if(!(ifh = read_ifh(d->tc_file[t]))) exit(-1);
            min_and_max(yhat_stack[j],lenvol[i]*d->ntimepoints[i],&ifh->global_min,&ifh->global_max);
            if(!write_ifh(str_ptr,ifh,(int)FALSE)) exit(-1);
            free_ifh(ifh,(int)FALSE);

            }
        free(R2_stack);
        free(R2a_stack);
        free_d2float(yhat_stack);
        free_d2float(B_stack);
        for(j=0;j<d->ntc[i];j++) unmap_disk(mm[j]); 
        free(mm);
        free(unsampled_mask);
        }


    } /*for(r=i=0;i<d->how_many;i++) {*/
if(d->nregion) {
    fclose(fp);
    printf("Output written to %s\n",output_file);
    }
}



/************************************/
Design *read_design(char *design_file)
/************************************/
{
char line[MAXNAME],write_back[MAXNAME],*str_ptr;
int i,j,k,count_TR=0,nstrings,max_ntimepoints=0;
FILE *fp;
Design *d;

GETMEM(d,1,Design);
if(!(fp = fopen(design_file,"r"))) {
    printf("Error opening %s in read_design.\n",design_file);
    return (Design *)NULL;
    }
for(d->nregion=0,d->nvoxel=0;fgets(line,sizeof(line),fp);) {
    if(strstr(line,"TR")) {
        count_TR++;
        }
    else if(strstr(line,"REGION")) {
        d->nregion++;
        }
    else if(strstr(line,"VOXEL")) {
        d->nvoxel++;
        }
    }
if(d->nregion && d->nvoxel) {
    printf("You may do either a regional or voxel analysis but not both simultaneously. Abort!\n");
    return (Design *)NULL;
    }


d->nregion ? strcpy(line,"REGION") : strcpy(line,"VOXEL");
d->how_many = d->nregion ? d->nregion : d->nvoxel;


/*if(count_TR!=1 && count_TR!=d->nregion) {
    printf("You have %d TR fields %d REGION fields.\n",count_TR,d->nregion);
    printf("You need to have either a single TR field or one for each REGION field. Abort!\n");
    return (Design *)NULL;
    }*/

if((count_TR!=1 && count_TR!=d->nregion) && (count_TR!=1 && count_TR!=d->nvoxel)) {
    printf("You have %d TR fields %d %s fields.\n",count_TR,d->nregion,line);
    printf("You need to have either a single TR field or one for each %s field. Abort!\n",line);
    return (Design *)NULL;
    }

GETMEM(d->region_str,d->nregion,char *);


/*GETMEM(d->TR,d->nregion,float);
GETMEM_0(d->ntc,d->nregion,int);
GETMEM_0(d->ntimepoints,d->nregion,int);*/

GETMEM(d->TR,d->how_many,float);
GETMEM_0(d->ntc,d->how_many,int);
GETMEM_0(d->ntimepoints,d->how_many,int);


for(rewind(fp),i=j=0;fgets(line,sizeof(line),fp);) {
    if(strstr(line,"TR")) {
        d->TR[i++] = atof(getstr(line));
        }
    else if(strstr(line,"REGION")) {
        str_ptr = getstr(line);
        GETMEM(d->region_str[j],strlen(str_ptr)+1,char);
        strcpy(d->region_str[j++],str_ptr);
        }
    else if(strstr(line,"VOXEL")) {
        j++;
        }
    else if(nstrings=count_strings(line,write_back,' ')) {  /*line could be blank*/
        if(!(nstrings=count_strings(getstr(line),write_back,' '))) {  /*line could be "FIELD:=" or "FIELD :="*/
            printf("FILE := %s\nREGION := %s\nTIMECOURSE := %s\nNo timecourse is listed. Abort!\n",design_file,d->region_str[j-1],
                line);
            return (Design *)NULL;
            }
        d->ntc[j-1]++;
        if(!d->ntimepoints[j-1]) {
            d->ntimepoints[j-1] = nstrings;
            }
        else if(d->ntimepoints[j-1] != nstrings) {
            printf("%s\nNot all timecourses have the same number of points. Abort!\n",d->region_str[j-1]);
            return (Design *)NULL;
            }
        }
    }

/*for(j=i;j<d->nregion;j++) d->TR[j] = d->TR[0];*/
for(j=i;j<d->how_many;j++) d->TR[j] = d->TR[0];

/*for(d->max_ntc=j=i=0;i<d->nregion;i++) {*/
for(d->max_ntc=j=i=0;i<d->how_many;i++) {
    j += d->ntc[i];
    if(d->ntc[i] > d->max_ntc) d->max_ntc = d->ntc[i];
    if(d->ntimepoints[i] > max_ntimepoints) max_ntimepoints = d->ntimepoints[i];
    /*printf("d->ntimepoints=%d\n",d->ntimepoints[i]);*/
    }

if(d->nregion) {
    if(!(d->tc = (float ***)d3float(d->nregion,d->max_ntc,max_ntimepoints))) exit(-1);
    }
else {
    GETMEM(d->tc_file,j,char *);
    }


GETMEM(d->tc_str,j,char *);
for(rewind(fp),k=0,i=-1;fgets(line,sizeof(line),fp);) {
    if(strstr(line,"TR")) {
        /*do nothing*/
        }
    /*else if(strstr(line,"REGION")) {*/
    else if(strstr(line,"REGION") || strstr(line,"VOXEL")) {
        /*do nothing*/
        i++;
        j=0;
        }
    else if(count_strings(line,write_back,' ')) {
        /*grab_string(write_back,line);
        GETMEM(d->tc_str[k],strlen(line)+1,char);
        strcpy(d->tc_str[k++],line);
        str_ptr = getstr(write_back);
        strings_to_float(str_ptr,d->tc[i][j++],d->ntimepoints[i]);*/

        grab_string(write_back,line);
        GETMEM(d->tc_str[k],strlen(line)+1,char);
        strcpy(d->tc_str[k],line);
        str_ptr = getstr(write_back);
        if(d->nregion) {
            strings_to_float(str_ptr,d->tc[i][j++],d->ntimepoints[i]);
            }
        else {
            /*GETMEM(d->tc_file[k],strlen(str_ptr)+1,char);
            strcpy(d->tc_file[k],str_ptr);*/
            /*printf("d->tc_file=%s\n",d->tc_file[k]);*/

            grab_string(str_ptr,line); /*grab_string is needed to prevent an extra space at the end*/
            GETMEM(d->tc_file[k],strlen(line)+1,char);
            strcpy(d->tc_file[k],line);
            }
        k++;
        }
    }
fclose(fp);
return (Design *)d;
}

/*************************************************************************/
Parameter *read_parameter(char *parameter_file,Design *d,char *design_file)
/*************************************************************************/
{
char line[MAXNAME],write_back[MAXNAME],*str_ptr,gamma_str[MAXNAME];
int nregion=0,i,j,k,m,nstrings,max_ngamma;
float TR_stimlen,TR_delay;
FILE *fp;
Parameter *p;

GETMEM(p,1,Parameter);
p->gamma_names = (char **)NULL;
if(!(fp = fopen(parameter_file,"r"))) {
    printf("Error opening %s in read_parameter.\n",parameter_file);
    return (Parameter *)NULL;
    }
for(j=nregion=0;fgets(line,sizeof(line),fp);) {
    if(strstr(line,"GAMMA_NAMES")) {
        if(!(nstrings = count_strings(getstr(line),write_back,' '))) {
            printf("GAMMA_NAMES := has no names listed. Abort!\n");
            return (Parameter *)NULL;
            }
        GETMEM(p->gamma_names,nstrings,char *);
        get_strings(write_back,p->gamma_names,nstrings);
        /*for(i=0;i<nstrings;i++) p->gamma_names[i][Truncate] = 0;*/
        }
    else if(strstr(line,"ALL")) {
        j = 1;
        } 
    else if(strstr(line,"REGION")) {
        nregion++;
        }
    }


/*if(j) nregion = d->nregion;
if(nregion != d->nregion) {
    printf("%s has %d regions but %s has %d regions.\n",design_file,d->nregion,parameter_file,nregion);
    printf("Both files must contain the same number of regions. Abort!\n");
    return (Parameter *)NULL;
    }
GETMEM_0(p->ngamma,d->nregion,int);*/

if(j) nregion = d->how_many;
if(nregion != d->how_many) {
    printf("%s has %d regions but %s has %d regions.\n",design_file,d->nregion,parameter_file,nregion);
    printf("Both files must contain the same number of regions. Abort!\n");
    return (Parameter *)NULL;
    }
GETMEM_0(p->ngamma,d->how_many,int);



for(rewind(fp),i=-1,gamma_str[0]=0;fgets(line,sizeof(line),fp);) {
    if(strstr(line,"GAMMA_NAMES")) {
        /*do nothing*/
        }
    else if(strstr(line,"ALL")) {
        i = k = 0;
        }
    else if(strstr(line,"REGION")) {
        str_ptr = getstr(line);
        for(k=i=0;i<d->nregion;i++) {
            if(!strcmp(d->region_str[i],str_ptr)) break;
            k += d->ntc[i];
            }
        if(i == d->nregion) {
            printf("REGION := %s is in %s, but not %s. Abort!\n",str_ptr,parameter_file,design_file);
            return (Parameter *)NULL;
            }
        }
    else if(strstr(line,"STIMLEN_IN_FRAMES")) {
        grab_string(line,gamma_str);
        }
    else if(strstr(line,"DELAY_IN_FRAMES")) {
        grab_string(line,gamma_str);
        }
    else if(strstr(line,"STIMLEN_IN_SEC")) {
        grab_string(line,gamma_str);
        }
    else if(strstr(line,"DELAY_IN_SEC")) {
        grab_string(line,gamma_str);
        }
    else if(count_strings(line,write_back,' ')) {
        if(i == -1 || !gamma_str[0]) {
            printf("%s is not ordered correctly. Abort at line = %s\ni=%d gamma_str=%sEND\n",parameter_file,line,i,gamma_str); 
            return (Parameter *)NULL;
            }
        grab_string(write_back,line);
        for(j=k;j<k+d->ntc[i];j++) if(!strcmp(d->tc_str[j],line)) break;
        if(j == k+d->ntc[i]) {
            printf("REGION := %s\n%s is not listed as a timecourse in %s. Abort!\n",d->region_str[i],line,design_file);
            return (Parameter *)NULL;
            }
        if(!(nstrings = count_strings(getstr(write_back),write_back,' '))) {
            printf("REGION := %s\n%s has no parameters listed for %s in %s. Abort!\n",d->region_str[i],d->tc_str[j],
                gamma_str,parameter_file);
            return (Parameter *)NULL;
            }
        if(!p->ngamma[i]) {
            p->ngamma[i] = nstrings;
            }
        else if(p->ngamma[i] != nstrings) {
            printf("FILE := %s\nREGION := %s\nPARAMETER := %s\nTIMECOURSE := %s\n",parameter_file,d->region_str[i],gamma_str,
                d->tc_str[j]);
            printf("%d parameters are listed. However an earlier timecourse had %d parameters listed. Abort!\n",nstrings,
                p->ngamma[i]);
            return (Parameter *)NULL;
            }
        }
    }

for(max_ngamma=i=0;i<d->how_many;i++) if(p->ngamma[i] > max_ngamma) max_ngamma = p->ngamma[i];
if(!(p->stimlen_sec = (float ***)d3float(d->how_many,d->max_ntc,max_ngamma))) exit(-1);
if(!(p->delay_sec = (float ***)d3float(d->how_many,d->max_ntc,max_ngamma))) exit(-1);

for(rewind(fp);fgets(line,sizeof(line),fp);) {
    if(strstr(line,"GAMMA_NAMES")) {
        /*do nothing*/
        }
    else if(strstr(line,"ALL")) {
        i = k = 0;
        }
    else if(strstr(line,"REGION")) {
        str_ptr = getstr(line);
        for(k=i=0;i<d->nregion;i++) {
            if(!strcmp(d->region_str[i],str_ptr)) break;
            k += d->ntc[i];
            }
        }
    else if(strstr(line,"STIMLEN_IN_FRAMES")) {
        TR_stimlen = d->TR[i];
        TR_delay = (float)UNSAMPLED_VOXEL;
        }
    else if(strstr(line,"DELAY_IN_FRAMES")) {
        TR_stimlen = (float)UNSAMPLED_VOXEL;
        TR_delay = d->TR[i];
        }
    else if(strstr(line,"STIMLEN_IN_SEC")) {
        TR_stimlen = 1;
        TR_delay = (float)UNSAMPLED_VOXEL;
        }
    else if(strstr(line,"DELAY_IN_SEC")) {
        TR_stimlen = (float)UNSAMPLED_VOXEL;
        TR_delay = 1;
        }
    else if(count_strings(line,write_back,' ')) {
        grab_string(write_back,line);
        for(j=0;j<d->ntc[i];j++) if(!strcmp(d->tc_str[j+k],line)) break;
        str_ptr = getstr(write_back);
        if(TR_stimlen != (float)UNSAMPLED_VOXEL) {
            strings_to_float(str_ptr,p->stimlen_sec[i][j],p->ngamma[i]);
            if(TR_stimlen != 1) for(m=0;m<p->ngamma[i];m++) p->stimlen_sec[i][j][m] *= TR_stimlen;
            }
        else if(TR_delay != (float)UNSAMPLED_VOXEL) {
            strings_to_float(str_ptr,p->delay_sec[i][j],p->ngamma[i]);
            if(TR_stimlen != 1) for(m=0;m<p->ngamma[i];m++) p->delay_sec[i][j][m] *= TR_delay;
            }
        }
    }
if(!i && !k) {
    /*for(i=1;i<d->nregion;i++) {*/
    for(i=1;i<d->how_many;i++) {
        p->ngamma[i] = p->ngamma[0];
        for(j=0;j<d->ntc[i];j++) { 
            for(m=0;m<p->ngamma[i];m++) { 
                p->stimlen_sec[i][j][m] = p->stimlen_sec[0][j][m];
                p->delay_sec[i][j][m] = p->delay_sec[0][j][m];
                }
            }
        }
    }
fclose(fp);
return (Parameter *)p;
}
