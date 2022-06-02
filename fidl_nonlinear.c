/* Copyright 9/1/04 Washington University.  All Rights Reserved.
   fidl_nonlinear.c  $Revision: 1.2 $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*#include <ctype.h>*/
#include <math.h>
/*#include <nan.h>*/
#include <fidl.h>
#include <nrutil.h>

static char rcsid[] = "$Header: /home/hannah/mcavoy/idl/clib/RCS/fidl_nonlinear.c,v 1.2 2004/11/16 20:49:46 mcavoy Exp $";

void single_exp_rise_to_max(double x,double *a,double *yfit, double *dyda,int na);
void single_exp_decay(double x,double *a,double *yfit, double *dyda,int na);
void expfn(double x,double *a,double *yfit, double *dyda,int na);

main(int argc,char **argv)
{
char string[MAXNAME],*string_ptr,*driver_file,*directory=NULL,*regional_ttest_name=NULL,regional_anova_name_str[MAXNAME],
     space_str[9],*glm_list_file=NULL,*group_name=NULL,firststr[MAXNAME],*scratchdir=NULL,scrapstr[7],*Nimage_name=NULL,
     *mask_file=NULL,**files_ptr,string_t[MAXNAME],string_z[MAXNAME],string2[4];

int i,j,k,l,m,n,p,lenvol,*fidl_voxel=(int*)NULL,index_fidl_voxel,space,
    lccleanup=(int)FALSE,lccleanup_only=(int)FALSE,num_region_names=0,how_many,flag,*unsampled,
    num_unsampled_voxels=0,lc_Z_uncorrected=0,lc_Z_corrected=0,text=(int)FALSE,modulo,
    num_tc=0,num_contrasts=0,argc_tc,argc_c,**contrast,frames=0,are_variances_heterogeneous=(int)FALSE,
    lc_monte_carlo=(int)FALSE,lc_one_condition=(int)FALSE,*lt2subjects_voxels_idx,num_voxels_lt2subjects,**dferror,min,max,
    **dferror_min_and_max,lc_print_cov_matrix=(int)FALSE,n_threshold_extent=0,*extent,num_YYT_files=0,lc_Z_monte_carlo=0,
    lenvol_whole,*subject_unsampled_count,num_Y_files=0,lc_statview=0,t,mode,*df,*nsub_per_test,*nparam,
    f1index,f2index,covindex,files_ptr_index,*nsubjects,lc_T_uncorrected=(int)FALSE,
    lc_T_monte_carlo=(int)FALSE,*lc_test_type,loop,n4dfps,nharolds_num=0,*harolds_num,SunOS_Linux,
    single_exp_rise_to_max_and_decay=0,nfn,np,ma,nca,*ia,lcsingle_exp_rise_to_max=0,count,maxit;

long *indx;

float *temp_float;

double *zstat,temp_double,*dferror_stack,*threshold,*tstat,*pval,*tstat_uneqvar,*df_uneqvar,mag,mag2,difference,avg1,avg2,var1,
    var2,temp1,temp2,sum,sp2,*behav_var,sumx,sumx2,sumy,sumy2,sumxy,SSxx,SSxy,SSyy,*r,**cov_var,**X,**Y,**XTX,**XTXm1,**XTY,**B,
    **YTY,**BTXTY,cond,var,*a,**covar,**alpha,*x,*y,*sig,*logy,xtx,xty,b2,chisq,alamda,chisqold,alamdamin,y0,xlow,xhigh,ylow,yhigh;

Interfile_header *ifh;
Memory_Map *mm,**mm1,**mm2;
LinearModel *glm;
Mask_Struct *ms;
FILE *fprn=NULL;
Files_Struct *region_names;
TtestDesign *td;

print_version_number(rcsid);
if (argc < 3) {
    fprintf(stderr,"     -driver:              File that specifies the factor levels and associated files for the anova.\n");
    fprintf(stderr,"     -uncompress:          Specify mask file to put back into image format.\n");
    fprintf(stderr,"                           The glm_atlas_mask is used by default.\n\n");
    fprintf(stderr,"     -regions:             List of region names for regional T tests.\n");
    fprintf(stderr,"     -harolds_num:         One for each regions.\n");
    fprintf(stderr,"     -directory:           Specify directory for output z statistic files.\n\n");
    fprintf(stderr,"     -fidl_voxel:          Three numbers from fidl: slice col row [fidl coordinates = (col,row)]\n\n");
    fprintf(stderr,"     -clean_up             Delete the files used in the anova computation.\n");
    fprintf(stderr,"     -clean_up ONLY        Don't do any computation, just delete the files used in the anova computation.\n\n");
    fprintf(stderr,"     -output:              Output options: T_uncorrected Z_uncorrected T_monte_carlo Z_monte_carlo\n\n");
    fprintf(stderr,"     -statview             Output text file for statview input. (Region is a factor.)\n");
    fprintf(stderr,"     -text                 Print out text file of regional analysis. (Region is a factor.)\n\n");
    fprintf(stderr,"     -tc:                  List of time courses for computation of box correction. The first tc is 1.\n");
    fprintf(stderr,"     -contrasts:           List of contrasts for computation of box correction. The first contrast is 1.\n");
    fprintf(stderr,"     -frames:              This option is used with -output box_correction_only\n");
    fprintf(stderr,"     -number_of_delays:    If magnitudes are being analyzed, the p-value is automatically corrected for the\n");
    fprintf(stderr,"                           number of delays. The default is set to 1.\n");
    fprintf(stderr,"     -regional_ttest_name: Output filename for a regional ttest.\n");
    fprintf(stderr,"     -Nimage_name:         Output filename for number of subjects at each voxel. Default is Nimage.4dfp.img\n");
    fprintf(stderr,"     -monte_carlo          Some processes skipped to speed up Monte Carlo simulation.\n");
    fprintf(stderr,"     -print_cov_matrix     Covariance matrix will be printed.\n");
    fprintf(stderr,"     -threshold_extent     Monte Carlo thresholds with spatial extent.\n");
    fprintf(stderr,"                           Ex. -threshold_extent \"3.75 18\" \"4 12\"\n");
    fprintf(stderr,"     -glm_list_file:       If present this goes into the ifh 'name of data file' field of the output files.\n");
    fprintf(stderr,"     -group_name:          Group name for subjects. Added to output filename.\n");
    fprintf(stderr,"     -scratchdir:          Location of precomputed scratchfiles. Include the backslash at the end.\n");
    fprintf(stderr,"     -single_exp_rise_to_max_and_decay  Nonlinear regression function.\n");
    fprintf(stderr,"     -single_exp_rise_to_max  Nonlinear regression function.\n");
    exit(-1);
    }

for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-driver") && argc > i+1)
        driver_file = argv[++i];
    if(!strcmp(argv[i],"-uncompress")) {
        if(argc > i+1 && strchr(argv[i+1],'-') != argv[i+1]) {
            mask_file = argv[++i];
            }
        else {
            printf("No mask file specified for -uncompress option.\n");
            exit(-1);
            }
        }
    if(!strcmp(argv[i],"-regions") && argc > i+1) {
        for(j=1;i+j<argc;j++) {
            string_ptr = argv[i+j] + 1;
            if(strchr(argv[i+j],'-') == argv[i+j] && !isdigit(*string_ptr)) break;
            ++num_region_names;
            }
        if(!(region_names=get_files(num_region_names,&argv[i+1]))) exit(-1);
        i += num_region_names;
        }
    if(!strcmp(argv[i],"-harolds_num") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nharolds_num;
        if(!(harolds_num=malloc(sizeof*harolds_num*nharolds_num))) {
            printf("Error: Unable to malloc harolds_num\n");
            exit(-1);
            }
        for(j=0;j<nharolds_num;j++) harolds_num[j] = atoi(argv[++i]);
        }
    if(!strcmp(argv[i],"-directory") && argc > i+1) {
	j = strlen(argv[i+1]);
        if(!(directory=(char*)d1(sizeof(char),(size_t)(j+1)))) exit(-1);
        strcpy(directory,argv[++i]);
	if(directory[j-2] == '/') directory[j-2] = 0;
        }
    if(!strcmp(argv[i],"-fidl_voxel") && argc > i+1) {
	for(k=0,j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++k;
	if(k != 3) {
	    fprintf(stderr,"Must specify slice, col, and row for -fidl_voxel\n");
	    exit(-1);
	    }
	else {
            if(!(fidl_voxel=(int*)d1(sizeof(int),3))) exit(-1);
	    for(j=0;j<3;j++) fidl_voxel[j] = atoi(argv[++i]);
	   }
	}
    if(!strcmp(argv[i],"-clean_up")) {
	lccleanup = TRUE;
        if(argc > i+1 && !strcmp(argv[i+1],"ONLY")) {
            lccleanup_only = TRUE;
            ++i;
            }
        }
    if(!strcmp(argv[i],"-output") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) {
	    if(!strcmp(argv[i+j],"T_uncorrected")) lc_T_uncorrected = 1;
            else if(!strcmp(argv[i+j],"Z_uncorrected")) lc_Z_uncorrected = 1; 
            else if(!strcmp(argv[i+j],"T_monte_carlo")) lc_T_monte_carlo = 1; 
            else if(!strcmp(argv[i+j],"Z_monte_carlo")) lc_Z_monte_carlo = 1; 
	    }
	i += lc_T_uncorrected+lc_Z_uncorrected+lc_T_monte_carlo+lc_Z_monte_carlo;
	}
    if(!strcmp(argv[i],"-statview"))
	lc_statview = 1;
    if(!strcmp(argv[i],"-text"))
	text = (int)TRUE;
    if(!strcmp(argv[i],"-tc") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_tc;
        argc_tc = i+1;
        i += num_tc;
        }
    if(!strcmp(argv[i],"-contrasts") && argc > i+1) {
        for(j=1;i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_contrasts;
        argc_c = i+1;
        i += num_contrasts;
        }
    if(!strcmp(argv[i],"-frames") && argc > i+1)
        frames = atoi(argv[++i]);
    if(!strcmp(argv[i],"-monte_carlo"))
	lc_monte_carlo = (int)TRUE;
    if(!strcmp(argv[i],"-regional_ttest_name") && argc > i+1)
        regional_ttest_name = argv[++i];
    if(!strcmp(argv[i],"-Nimage_name") && argc > i+1)
        Nimage_name = argv[++i];
    if(!strcmp(argv[i],"-one_condition"))
	lc_one_condition = (int)TRUE;
    if(!strcmp(argv[i],"-print_cov_matrix"))
	lc_print_cov_matrix = (int)TRUE;
    if(!strcmp(argv[i],"-threshold_extent") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++n_threshold_extent;
        if(!(threshold=(double*)d1(sizeof(double),(size_t)n_threshold_extent))) exit(-1);
        if(!(extent=(int*)d1(sizeof(int),(size_t)n_threshold_extent))) exit(-1);
        for(j=0;j<n_threshold_extent;j++) sscanf(argv[++i],"%lf %d",&threshold[j],&extent[j]);
        /*for(j=0;j<n_threshold_extent;j++) printf("threshold=%f extent=%d\n",threshold[j],extent[j]);*/
        }
    if(!strcmp(argv[i],"-glm_list_file") && argc > i+1)
        glm_list_file = argv[++i];
    if(!strcmp(argv[i],"-group_name") && argc > i+1)
        group_name = argv[++i];
    if(!strcmp(argv[i],"-scratchdir") && argc > i+1)
        scratchdir = argv[++i];
    if(!strcmp(argv[i],"-single_exp_rise_to_max_and_decay"))
	single_exp_rise_to_max_and_decay = 1;
    if(!strcmp(argv[i],"-single_exp_rise_to_max"))
	lcsingle_exp_rise_to_max = 1;
    }
if((SunOS_Linux=checkOS())==-1) exit(-1);
firststr[0] = 0;
if(group_name) {
    strcat(firststr,group_name);
    strcat(firststr,"_");
    }
if(Nimage_name) *strstr(Nimage_name,".4dfp.img") = 0;

if(!(td = (TtestDesign *)read_ttest_driver(driver_file))) exit(-1);
#if 0
/*BEGIN CHECK*/
for(k=l=m=i=0;i<td->ntests;i++) {
    printf("td->nfiles1=%d\n",td->nfiles1[i]);
    printf("TTEST := %s %s\n",td->type[i],td->name[i]);
    for(j=0;j<td->nfiles1[i];j++) {
        printf("FIRST: %sEND\n",td->files1[k++]);
        }
    printf("td->nfiles2=%d\n",td->nfiles2[i]);
    for(j=0;j<td->nfiles2[i];j++) {
        printf("SECOND: %sEND\n",td->files2[m++]);
        }
    printf("l=%d\n",l);
    for(j=0;j<td->ncov[i];j++) {
        printf("COVARIATES: %sEND\n",td->cov[l++]);
        }
    }
/*END CHECK*/
#endif
printf("Successfully read %s\n",driver_file);


if(!(files_ptr=malloc(sizeof*files_ptr*td->total_nfiles))) {
    printf("Unable to malloc files_ptr\n");
    exit(-1);
    }
for(n=k=m=i=0;i<td->ntests;i++) {
    for(j=0;j<td->nfiles1[i];j++,k++) {
        if(strstr(td->files1[k],".4dfp.img")) files_ptr[n++] = td->files1[k];
        }
    for(j=0;j<td->nfiles2[i];j++,m++) {
        if(strstr(td->files2[m],".4dfp.img")) files_ptr[n++] = td->files2[m];
        }
    }
n4dfps = n;

if(lccleanup_only == (int)FALSE) {
    if(!(ifh = read_ifh(files_ptr[0]))) exit(-1);
    lenvol = ifh->dim1*ifh->dim2*ifh->dim3;
    #ifndef MONTE_CARLO
        if(!check_dimensions(n4dfps,files_ptr,lenvol)) exit(-1);
        if(glm_list_file) assign_glm_list_file_to_ifh(ifh,glm_list_file);
    #endif
    if(!(ms=get_mask_struct(mask_file,lenvol))) exit(-1);
    how_many = ms->lenvol;
    set_ifh_and_constants(how_many,ifh,&space,space_str);

    /*mode = 0, print z; mode = 1, print t; mode = 2, print both*/
    if(lc_Z_monte_carlo) mode = 0;
    if(lc_T_monte_carlo) mode = 1;
    if(lc_Z_monte_carlo && lc_T_monte_carlo) mode = 2;

    if(!(temp_float=malloc(sizeof*temp_float*how_many))) {
        printf("Error: Unable to malloc temp_float\n");
        exit(-1);
        }
    if(!(df=malloc(sizeof*df*td->ntests))) {
        printf("Error: Unable to malloc df\n");
        exit(-1);
        }
    if(!(nsub_per_test=malloc(sizeof*nsub_per_test*td->ntests))) {
        printf("Error: Unable to malloc nsub_per_test\n"); 
        exit(-1);
        }

    for(i=0;i<td->ntests;i++) nsub_per_test[i] = td->nfiles1[i];

    #if 0
    nfn = 0;
    if(single_exp_rise_to_max_and_decay) nfn++;
    if(!(nparam=malloc(sizeof*nparam*nfn))) {
        printf("Error: Unable to malloc nparam\n");
        exit(-1);
        }
    i = 0;
    if(single_exp_rise_to_max_and_decay) nparam[i++] = 3;
    #endif



    if(num_region_names || text) {
        if(directory)
            sprintf(regional_anova_name_str,"%s/%s",directory,regional_ttest_name);
        else
            strcpy(regional_anova_name_str,regional_ttest_name);
        if(!(fprn = (FILE *)fopen_sub(regional_anova_name_str,"w"))) exit(-1);
        strcpy(scrapstr,"region");
        modulo = 1;
        }
    else {
        strcpy(scrapstr,"voxel");
        modulo = 20000;
        }
    lenvol_whole = lenvol;
    if(!(dferror_stack=malloc(sizeof*dferror_stack*lenvol_whole))) {
        printf("Error: Unable to malloc dferror_stack\n");
        exit(-1);
        }
    if(!(nsubjects=malloc(sizeof*nsubjects*ms->lenbrain))) {
        printf("Error: Unable to malloc nsubjects\n"); 
        exit(-1);
        }
    for(f1index=f2index=files_ptr_index=t=0;t<td->ntests;t++) {
        for(j=0;j<ms->lenbrain;j++) nsubjects[j] = td->nfiles1[t];
        if(!(subject_unsampled_count=malloc(sizeof*subject_unsampled_count*nsub_per_test[t]))) {
            printf("Error: Unable to malloc subject_unsampled_count\n");
            exit(-1);
            }
        for(j=0;j<nsub_per_test[t];j++) subject_unsampled_count[j]=0;
        for(k=files_ptr_index,i=0;i<td->nfiles1[t];i++,k++) {
            if(!(mm = map_disk(files_ptr[k],lenvol,0))) exit(-1);
            for(j=0;j<lenvol;j++) {
                if(mm->ptr[j] == (float)UNSAMPLED_VOXEL || isnan(mm->ptr[j])) {
                    --nsubjects[j];
                    subject_unsampled_count[i]++;
                    }
                }
            unmap_disk(mm);
            }
        files_ptr_index += td->nfiles1[t];
        printf("\nNumber of unsampled voxels for each subject.\n");
        for(i=0;i<nsub_per_test[t];i++) printf("    subject%d %d\n",i+1,subject_unsampled_count[i]);
        printf("\n");
        free(subject_unsampled_count);
        if(Nimage_name) {
            for(j=0;j<ms->lenbrain;j++) temp_float[ms->brnidx[j]] = (float)(nsubjects[j]);
            if(directory) {
                sprintf(regional_anova_name_str,"%s/%s.4dfp.img",directory,Nimage_name);
                }
            else {
                sprintf(regional_anova_name_str,"%s.4dfp.img",Nimage_name);
                }
            if(!writestack(regional_anova_name_str,temp_float,sizeof*temp_float,(size_t)how_many,SunOS_Linux)) exit(-1);
            min_and_max(temp_float,how_many,&ifh->global_min,&ifh->global_max);
            if(!write_ifh(regional_anova_name_str,ifh,(int)FALSE)) exit(-1);
            fprintf(stdout,"Nimage written to %s\n",regional_anova_name_str);
            }
        if(!(tstat=malloc(sizeof*tstat*lenvol_whole))) {
            printf("Error: Unable to malloc tstat\n");
            exit(-1);
            }
        if(!(zstat=malloc(sizeof*zstat*lenvol_whole))) {
            printf("Error: Unable to malloc zstat\n");
            exit(-1);
            }
        if(!(pval=malloc(sizeof*pval*lenvol_whole))) {
            printf("Error: Unable to malloc pval\n");
            exit(-1);
            }
        if(regional_ttest_name) {
            strcpy(string,"region");
            }
        else {
            strcpy(string,"voxel");
            }
        fprintf(stdout,"A total of %d %s(s) per scratch file will be processed.\n",lenvol_whole,string);
        if(!(mm1=malloc(sizeof*mm1*td->nfiles1[t]))) {
            printf("Error: Unable to malloc mm1\n");
            exit(-1);
            }
        for(i=0;i<td->nfiles1[t];i++) if(!(mm1[i] = map_disk(td->files1[f1index+i],lenvol,0))) exit(-1);
        if(!(behav_var=malloc(sizeof*behav_var*td->nfiles2[t]))) {
            printf("Error: Unable to malloc behav_var\n");
            exit(-1);
            }
        for(i=0;i<td->nfiles2[t];i++) behav_var[i] = atof(td->files2[f2index+i]);
        if(td->nfiles1[t] != td->nfiles2[t]) {
            printf("Error: %s %s  %d files  %d behavioral values. Must be equal. Abort!\n",
                td->type[t],td->name[t],td->nfiles1[t],td->nfiles2[t]);
            exit(-1);
            }

/*START*/
        #if 0
        if(single_exp_rise_to_max_and_decay) {
            #if 0
            if(!(beta=malloc(sizeof*beta*nparam[p]*lenvol))) {
                printf("Error: Unable to malloc beta\n");
                exit(-1);
                }
            for(i=0;i<nparam[p]*lenvol;i++) beta[i] = (double)UNSAMPLED_VOXEL;
            #endif

            np = 2;
            XTX = dmatrix(1,np,1,np);
            XTY = dmatrix(1,np,1,1);
            B = dmatrix(1,np,1,1);

            if(single_exp_rise_to_max_and_decay) {
                ma=nca=3;
                }
            a = dvector(1,ma);
            ia = ivector(1,ma);
            covar = dmatrix(1,nca,1,nca);
            alpha = dmatrix(1,nca,1,nca);

            for(i=0;i<lenvol;i++) {
                if(!(i%modulo)) fprintf(stdout,"Processing %s = %d\n",scrapstr,i);
                printf("i=%d nsubjects=%d\n",i,nsubjects[i]);
                /*if(nsubjects[i] > 3) {*/
                if(nsubjects[i] == td->nfiles1[t]) {
                    X = dmatrix(1,nsubjects[i],1,np);
                    x = dvector(1,nsubjects[i]);
                    y = dvector(1,nsubjects[i]);
                    for(k=j=0;j<td->nfiles1[t];j++) {
                        if(mm1[j]->ptr[i]!=(float)UNSAMPLED_VOXEL && !isnan(mm1[j]->ptr[i])) {
                            k++;
                            X[k][1] = 1.;
                            X[k][2] = x[k] = behav_var[j];
                            y[k] = (double)mm1[j]->ptr[i];
                            }
                        }
                    if(k!=nsubjects[i]) {
                        printf("Error: Problem HERE0. k=%d nsubjects=%d. Should be equal.\n",k,nsubjects[i]);
                        exit(-1);
                        }
                    dmatrix_mult_nomem(X,X,nsubjects[i],np,nsubjects[i],np,(int)TRANSPOSE_FIRST,XTX);
                    cond = condition_number(XTX,np);
                    if(cond < 10000)
                        XTXm1 = inverse(XTX,np);
                    else
                        XTXm1 = pinv(XTX,np,np,(double)0.);
                    for(j=1;j<=np;j++) {
                        XTY[j][1] = 0.;
                        for(k=1;k<=nsubjects[i];k++) XTY[j][1] += X[k][j]*y[k];
                        }
                    dmatrix_mult_nomem(XTXm1,XTY,np,np,np,1,(int)TRANSPOSE_NONE,B);
                    free_dmatrix(X,1,nsubjects[i],1,np);
                    free_dmatrix(XTXm1,1,np,1,np);

                    if(!(logy=malloc(sizeof*logy*nsubjects[i]))) {
                        printf("Error: Unable to malloc logy\n");
                        exit(-1);
                        }

                    sig = dvector(1,nsubjects[i]);
                    for(j=1;j<=nsubjects[i];j++) {
                        sig[j] = x[j]+y[j];
                        sig[j] = sqrt(x[j]*x[j]+y[j]*y[j]-.5*sig[j]*sig[j]);
                        }

                    #if 0
                    if(!(logy=malloc(sizeof*y*nsubjects[i]))) {
                        printf("Error: Unable to malloc y\n");
                        exit(-1);
                        }
                    if(B[2][1]>0.) {
                        printf("here0\n");
                        for(k=1,j=0;j<nsubjects[i];j++,k++) logy[j] = -log((B[1][1]-y[k])/B[2][1]+1);
                        }
                    else {
                        printf("here1 B[1][1]=%f B[2][1]=%f\n",B[1][1],B[2][1]);
                        /*for(k=1,j=0;j<nsubjects[i];j++,k++) logy[j] = -log((y[k]-B[1][1])/B[2][1]);*/
                        for(k=1,j=0;j<nsubjects[i];j++,k++) {
                            logy[j] = -log((y[k]-B[1][1])/B[2][1]);
                            printf("y[%d]=%f logy[%d]=%f\n",k,y[k],j,logy[j]);
                            }
                        }
                    for(xtx=xty=0.,k=1,j=0;j<nsubjects[i];j++,k++) {
                        xtx += x[k]*x[k];
                        xty += x[k]*logy[j];
                        /*printf("logy[%d]=%f\n",j,logy[j]);*/
                        }
                    printf("xtx=%f xty=%f\n",xtx,xty);
                    b2 = xty/xtx; 
                    free(logy);
                    #endif
                    #if 1
                    printf("i=%d B[1][1]=%f B[2][1]=%f\n",i,B[1][1],B[2][1]);
                    if(!(logy=malloc(sizeof*y*nsubjects[i]))) {
                        printf("Error: Unable to malloc y\n");
                        exit(-1);
                        }
                    if(B[2][1]>0.) {
                        printf("here0a\n");
                        for(k=1,j=0;j<nsubjects[i];j++,k++) logy[j] = -log((B[1][1]-y[k])/B[2][1]+1);
                        }
                    else {
                        printf("here0b\n");
                        for(k=1,j=0;j<nsubjects[i];j++,k++) {
                            logy[j] = y[k]<B[1][1] ? -log((y[k]-B[1][1])/B[2][1]) : log((B[1][1]-y[k])/B[2][1]+1.);
                            printf("y[%d]=%f logy[%d]=%f\n",k,y[k],j,logy[j]);
                            }
                        }
                    #endif

                    #if 1
                    for(xtx=xty=0.,k=1,j=0;j<nsubjects[i];j++,k++) {
                        xtx += x[k]*x[k];
                        xty += x[k]*logy[j];
                        /*printf("logy[%d]=%f\n",j,logy[j]);*/
                        }
                    printf("xtx=%f xty=%f\n",xtx,xty);
                    b2 = xty/xtx;
                    free(logy);
                    #endif



                    #if 1
                    ma=nca=3;
                    a = dvector(1,ma);
                    ia = ivector(1,ma);
                    covar = dmatrix(1,nca,1,nca);
                    alpha = dmatrix(1,nca,1,nca);
                    a[1]=B[1][1]; a[2]=B[2][1]; a[3]=b2;
                    ia[1]=1; ia[2]=1; ia[3]=1;
                    alamda=-1.;
                    #endif

                    #if 0
                    ma=3;
                    nca=3;
                    a = dvector(1,ma);
                    ia = ivector(1,ma);
                    covar = dmatrix(1,nca,1,nca);
                    alpha = dmatrix(1,nca,1,nca);
                    a[1]=B[1][1]; a[2]=B[2][1]; a[3]=b2;
                    ia[1]=0; ia[2]=1; ia[3]=1;
                    alamda=-1.;
                    #endif


                    #if 0
                    STARTLOOP
                        mrqmin(x,y,sig,nsubjects[i],a,ia,ma,covar,alpha,&chisq,
                            void (*funcs)(double, double [], double *, double [], int), double *alamda);
                    ENDLOOP
                    one final call to mrqmin
                    #endif
/*HERE*/
                  
                    for(j=1;j<=nsubjects[i];j++) printf("%d x=%f y=%f\n",j,x[j],y[j]);
                    printf("a="); for(j=1;j<=3;j++) printf("%f ",a[j]); printf("\n");
                    if(B[2][1] > 0.) {
                        printf("here1a\n");
                        for(j=0;j<1000;j++) {
                            mrqmin(x,y,sig,nsubjects[i],a,ia,ma,covar,alpha,&chisq,&single_exp_rise_to_max,&alamda);
                            printf("j=%d chisq=%f alamda=%f\n",j,chisq,alamda);
                            }
                        }
                    else {
                        printf("here1b\n");
                        #if 0
                        for(j=0;j<1000;j++) {
                            mrqmin(x,y,sig,nsubjects[i],a,ia,ma,covar,alpha,&chisq,&single_exp_decay,&alamda);
                            printf("j=%d chisq=%.15f alamda=%.15f\n",j,chisq,alamda);
                            }
                        #endif

                        for(alamdamin=10,chisqold=1.e20,j=0;j<1000;j++) {
                            mrqmin(x,y,sig,nsubjects[i],a,ia,ma,covar,alpha,&chisq,&single_exp_decay,&alamda);
                            printf("j=%d chisq=%.20f alamda=%.20f chisqold-chisq=%.20f\n",j,chisq,alamda,chisqold-chisq);
                            if(alamda < alamdamin) {
                                /*alamda = alamdamin;*/
                                alamdamin = alamda;
                                if(chisqold-chisq < .001) {
                                    printf("HERE\n");
                                    }
                                }
                            chisqold = chisq;
                            }
                        alamda=0.;
                        mrqmin(x,y,sig,nsubjects[i],a,ia,ma,covar,alpha,&chisq,&single_exp_decay,&alamda);
                        printf("a="); for(j=1;j<=3;j++) printf("%f ",a[j]); printf("\n");

                        printf("covar=\n");
                        for(j=1;j<=nca;j++) {
                            for(k=1;k<=nca;k++) printf("%f ",covar[j][k]);
                            printf("\n");
                            }
                        printf("alpha=\n");
                        for(j=1;j<=nca;j++) {
                            for(k=1;k<=nca;k++) printf("%f ",alpha[j][k]);
                            printf("\n");
                            }


                        }

                    exit(-1);




                    free_dvector(sig,1,nsubjects[i]);
                    }
                }
            free_dmatrix(XTX,1,np,1,np);
            free_dmatrix(XTY,1,np,1,1);
            free_dmatrix(B,1,np,1,1);
        #endif

        /*START100*/
        if(single_exp_rise_to_max_and_decay) { /*exp decay only*/
            np = 2;
            XTX = dmatrix(1,np,1,np);
            XTY = dmatrix(1,np,1,1);
            B = dmatrix(1,np,1,1);
            if(single_exp_rise_to_max_and_decay) {
                ma=nca=3;
                }
            a = dvector(1,ma);
            ia = ivector(1,ma);
            covar = dmatrix(1,nca,1,nca);
            alpha = dmatrix(1,nca,1,nca);
            for(i=0;i<lenvol;i++) {
                if(!(i%modulo)) fprintf(stdout,"Processing %s = %d\n",scrapstr,i);
                printf("i=%d nsubjects=%d\n",i,nsubjects[i]);
                /*if(nsubjects[i] > 3) {*/
                if(nsubjects[i] == td->nfiles1[t]) {
                    X = dmatrix(1,nsubjects[i],1,np);
                    x = dvector(1,nsubjects[i]);
                    y = dvector(1,nsubjects[i]);
                    for(y0=0.,k=j=0;j<td->nfiles1[t];j++) {
                        if(mm1[j]->ptr[i]!=(float)UNSAMPLED_VOXEL && !isnan(mm1[j]->ptr[i])) {
                            k++;
                            X[k][1] = 1.;
                            X[k][2] = x[k] = behav_var[j];
                            if((y[k]=(double)mm1[j]->ptr[i])<y0) y0 = y[k];
                            }
                        }
                    if(k!=nsubjects[i]) {
                        printf("Error: Problem HERE0. k=%d nsubjects=%d. Should be equal.\n",k,nsubjects[i]);
                        exit(-1);
                        }
                    if(y0<0.) {
                        y0-=.1;
                        for(j=1;j<=nsubjects[i];j++) y[j] -= y0;
                        }
                    for(j=1;j<=nsubjects[i];j++) y[j] = log(y[j]);

                    dmatrix_mult_nomem(X,X,nsubjects[i],np,nsubjects[i],np,(int)TRANSPOSE_FIRST,XTX);
                    cond = condition_number(XTX,np);
                    if(cond < 10000)
                        XTXm1 = inverse(XTX,np);
                    else
                        XTXm1 = pinv(XTX,np,np,(double)0.);
                    for(j=1;j<=np;j++) {
                        XTY[j][1] = 0.;
                        for(k=1;k<=nsubjects[i];k++) XTY[j][1] += X[k][j]*y[k];
                        }
                    dmatrix_mult_nomem(XTXm1,XTY,np,np,np,1,(int)TRANSPOSE_NONE,B);
                    free_dmatrix(X,1,nsubjects[i],1,np);
                    free_dmatrix(XTXm1,1,np,1,np);
                    printf("%s\n",region_names->files[i]);
                    printf("bo=%f b1=%f\n",B[1][1],B[2][1]);
                    printf("a=%f b=%f y0=%f\n\n",exp(B[1][1]),-B[2][1],y0);
                    }
                }
                free_dmatrix(XTX,1,np,1,np);
                free_dmatrix(XTY,1,np,1,1);
                free_dmatrix(B,1,np,1,1);
            }

        /*START101*/
        if(lcsingle_exp_rise_to_max) { 
            ma=nca=2;
            a = dvector(1,ma);
            ia = ivector(1,ma);
            ia[1]=ia[2]=1;
            covar = dmatrix(1,nca,1,nca);
            alpha = dmatrix(1,nca,1,nca);

            indx = lvector(1,(long)td->nfiles1[t]);
            x = dvector(1,td->nfiles1[t]);
            y = dvector(1,td->nfiles1[t]);
            sig = dvector(1,td->nfiles1[t]);
            for(i=0;i<lenvol;i++) {
                if(!(i%modulo)) fprintf(stdout,"Processing %s = %d\n",scrapstr,i);
                printf("i=%d nsubjects=%d\n",i,nsubjects[i]);
                /*if(nsubjects[i] > 3) {*/
                if(nsubjects[i] == td->nfiles1[t]) {
                    for(k=j=0;j<td->nfiles1[t];j++) {
                        if(mm1[j]->ptr[i]!=(float)UNSAMPLED_VOXEL && !isnan(mm1[j]->ptr[i])) {
                            k++;
                            x[k] = behav_var[j];
                            y[k]=(double)mm1[j]->ptr[i];
                            }
                        }
                    if(k!=nsubjects[i]) {
                        printf("Error: Problem HERE0. k=%d nsubjects=%d. Should be equal.\n",k,nsubjects[i]);
                        exit(-1);
                        }
                    indexx((unsigned long)nsubjects[i],x,indx);
                    for(xlow=ylow=0.,k=0,j=1;j<nsubjects[i]/3;j++,k++) {
                        xlow += x[indx[j]];
                        ylow += y[indx[j]];
                        }
                    xlow /= (double)k; ylow /= (double)k;
                    printf("xlow=%f ylow=%f nsubjects=%d\n",xlow,ylow,k);
                    for(xhigh=yhigh=0.,k=0,j=2*nsubjects[i]/3;j<nsubjects[i];j++,k++) {
                        xhigh += x[indx[j]];
                        yhigh += y[indx[j]];
                        }
                    xhigh /= (double)k; yhigh /= (double)k;
                    printf("xhigh=%f yhigh=%f nsubjects=%d\n",xhigh,yhigh,k);
                    a[1] = 1.1*yhigh;
                    a[2] = 1/(xhigh-xlow)*log((a[1]-ylow)/(a[1]-yhigh)); 
                    printf("a[1]=%f a[2]=%f\n",a[1],a[2]);

                    for(j=1;j<=nsubjects[i];j++) {
                        sig[j] = x[j]+y[j];
                        sig[j] = sqrt(x[j]*x[j]+y[j]*y[j]-.5*sig[j]*sig[j]);
                        }
                    printf("here1\n");

                    printf("i=%d\n",i);
                    maxit=100;
                    for(count=0,alamda=-1.,alamdamin=10,chisqold=1.e20,j=0;j<maxit;j++) {
                        mrqmin(x,y,sig,nsubjects[i],a,ia,ma,covar,alpha,&chisq,&single_exp_rise_to_max,&alamda);
                        printf("j=%d chisq=%.20f alamda=%.20f chisqold-chisq=%.20f\n",j,chisq,alamda,chisqold-chisq);

                        #if 0
                        if(alamda < alamdamin) {
                            /*alamda = alamdamin;*/
                            alamdamin = alamda;
                            if(chisqold-chisq < .001) {
                                printf("HERE\n");
                                }
                            }
                        #endif
                                     
                        if(chisq < chisqold) {
                            if(fabs(chisqold-chisq) < .01) {
                                count++;
                                printf("HERE count=%d\n",count);
                                if(count==3) break;
                                }
                            }


                        chisqold = chisq;
                        }
                    if(j==maxit) printf("ERROR: DID NOT CONVERGE\n");
                    alamda=0.;
                    mrqmin(x,y,sig,nsubjects[i],a,ia,ma,covar,alpha,&chisq,&single_exp_rise_to_max,&alamda);
                    printf("a="); for(j=1;j<=ma;j++) printf("%f ",a[j]); printf("\n");

                    printf("covar=\n");
                    for(j=1;j<=nca;j++) {
                        for(k=1;k<=nca;k++) printf("%f ",covar[j][k]);
                        printf("\n");
                        }
                    printf("alpha=\n");
                    for(j=1;j<=nca;j++) {
                        for(k=1;k<=nca;k++) printf("%f ",alpha[j][k]);
                        printf("\n");
                        }
                    printf("\n\n");


                    }
                }
            }







            #if 0
            loop = lc_test_type[t]==2 ? 2 : 1;
            for(i=0;i<loop;i++) {
                if(i == 1) {
                    for(j=0;j<lenvol;j++) {
                        tstat[j] = tstat_uneqvar[j];
                        dferror_stack[j] = df_uneqvar[j];
                        }
                    }
                t_to_z(tstat,zstat,lenvol,dferror_stack,pval);
                if(!fprn) {
                    string[0] = 0;
                    if(directory) sprintf(string,"%s/",directory);
                    strcat(string,firststr);
                    strcat(string,td->name[t]);
                    regional_anova_name_str[0] = 0;
                    if(lc_test_type[t] == 2) {
                        i==0 ? strcat(regional_anova_name_str,"_eqvar") : strcat(regional_anova_name_str,"_uneqvar");
                        }
                    strcat(regional_anova_name_str,space_str);
                    if(lc_test_type[t] == 3) {
                        for(j=0;j<ms->lenbrain;j++) temp_float[ms->brnidx[j]] = (float)r[j];
                        min_and_max(temp_float,how_many,&ifh->global_min,&ifh->global_max);
                        sprintf(string_t,"%s_corcoeff%s.4dfp.img",string,regional_anova_name_str);
                        if(!write_float(string_t,temp_float,how_many)) exit(-1);
                        printf("Correlation coefficient written to %s\n",string_t);
                        if(!write_ifh(string_t,ifh,(int)FALSE)) exit(-1);
                        }
 
                    if(lc_T_uncorrected || lc_T_monte_carlo) {
                        for(j=0;j<num_voxels_lt2subjects;j++) tstat[lt2subjects_voxels_idx[j]] = (double)UNSAMPLED_VOXEL;
                        sprintf(string_t,"%s_tstat%s.4dfp.img",string,regional_anova_name_str);
                        }
                    if(lc_T_uncorrected) {
                        for(j=0;j<ms->lenbrain;j++) temp_float[ms->brnidx[j]] = (float)tstat[j];
                        min_and_max(temp_float,how_many,&ifh->global_min,&ifh->global_max);
                        if(!write_float(string_t,temp_float,how_many)) exit(-1);
                        printf("T statistics written to %s\n",string_t);
                        if(!write_ifh(string_t,ifh,(int)FALSE)) exit(-1);
                        }
                    if(lc_Z_uncorrected || lc_Z_monte_carlo) {
                        for(j=0;j<num_voxels_lt2subjects;j++) zstat[lt2subjects_voxels_idx[j]] = (double)UNSAMPLED_VOXEL;
                        sprintf(string_z,"%s_zstat%s.4dfp.img",string,regional_anova_name_str);
                        }
                    if(lc_Z_uncorrected) {
                        for(j=0;j<ms->lenbrain;j++) temp_float[ms->brnidx[j]] = (float)zstat[j];
                        min_and_max(temp_float,how_many,&ifh->global_min,&ifh->global_max);
                        if(!write_float(string_z,temp_float,how_many)) exit(-1);
                        printf("Z statistics written to %s\n",string_z);
                        if(!write_ifh(string_z,ifh,(int)FALSE)) exit(-1);
                        }
                    if(lc_Z_monte_carlo || lc_T_monte_carlo) {
                        #if 0
                        monte_carlo_mult_comp(zstat,ms->lenbrain,ms->brnidx,ifh->dim1,ifh->dim2,ifh->dim3,threshold,extent,
                            n_threshold_extent,string_z,ifh,mode,tstat,string_t);
                        #endif
                        monte_carlo_mult_comp(zstat,ms->lenbrain,ms->brnidx,ifh->dim1,ifh->dim2,ifh->dim3,threshold,extent,
                            n_threshold_extent,string_z,ifh,mode,tstat,string_t,SunOS_Linux);
                        }
                    if(lc_T_uncorrected || lc_T_monte_carlo) {
                        sprintf(string_t,"%s_df%s.4dfp.img",string,regional_anova_name_str);
                        for(j=0;j<ms->lenbrain;j++) temp_float[ms->brnidx[j]] = (float)dferror_stack[j];
                        min_and_max(temp_float,how_many,&ifh->global_min,&ifh->global_max);
                        if(!write_float(string_t,temp_float,how_many)) exit(-1);
                        printf("Degrees of freedom written to %s\n",string_t);
                        if(!write_ifh(string_t,ifh,(int)FALSE)) exit(-1);
                        }
                    }
                else {
                    if(!nharolds_num) {
                        fprintf(fprn,"%s %s df = %d\n",td->type[t],td->name[t],df[t]);
                        if(lc_test_type[t] == 2) {
                            i==0 ? fprintf(fprn,"EQUAL VARIANCES\n") : fprintf(fprn,"UNEQUAL VARIANCES\n");
                            }
                        strcpy(string,"REGION");
                        fprintf(fprn,"%*-sT        Z     p (two-tailed)\n",region_names->max_length+6,string);
                        for(k=0;k<region_names->max_length+6;k++) fprintf(fprn,"=");
                        fprintf(fprn,"=============================\n");
                        for(j=0;j<lenvol;j++) {
                            fprintf(fprn,"%s%*.4f %8.4f %12.8f\n",region_names->files[j],region_names->max_length-
                                region_names->strlen_files[j]+9,tstat[j],zstat[j],pval[j]*2);
                            }
                        fprintf(fprn,"\n\n");
                        }
                    else {
                        fprintf(fprn,"%s %s df = %d\n",td->type[t],td->name[t],(int)dferror_stack[0]);
                        if(lc_test_type[t] == 2) {
                            i==0 ? fprintf(fprn,"EQUAL VARIANCES\n") : fprintf(fprn,"UNEQUAL VARIANCES\n");
                            }
                        if(lc_test_type[t] == 3) {
                            strcpy(string,"REGION");
                            /*fprintf(fprn,"%*-sr        T        Z       p(two-tailed)\n",region_names->max_length+6,string);*/
                            /*fprintf(fprn,"%*-sr        T(df=%-4d)Z       p(two-tailed)\n",region_names->max_length+6,string,*/

                            if(dferror_stack[0]<10.) k=1;
                            else if(dferror_stack[0]<100.) k=2;
                            else if(dferror_stack[0]<1000.) k=3; 
                            else k=4;
                            for(string2[0]=0,j=k+1;j<4;j++) strcat(string2," ");
                            fprintf(fprn,"%*-sr        T(df=%d)%sZ       p(two-tailed)\n",region_names->max_length+6,string,
                                (int)dferror_stack[0],string2);

                            for(k=0;k<region_names->max_length+6;k++) fprintf(fprn,"=");
                            fprintf(fprn,"=======================================\n");
                            for(j=0;j<lenvol;j++) {
                                fprintf(fprn,"%-3d %s%*.4f %8.4f %8.4f  %e\n",harolds_num[j],region_names->files[j],
                                    region_names->max_length-region_names->strlen_files[j]+9,r[j],tstat[j],zstat[j],pval[j]*2);
                                }
                            fprintf(fprn,"\n\n");
                            }
                        else {
                            strcpy(string,"REGION");
                            fprintf(fprn,"%*-sT        Z     p (two-tailed)\n",region_names->max_length+6,string);
                            for(k=0;k<region_names->max_length+6;k++) fprintf(fprn,"=");
                            fprintf(fprn,"=================================\n");
                            for(j=0;j<lenvol;j++) {
                                fprintf(fprn,"%-3d %s%*.4f %8.4f %12.8f\n",harolds_num[j],region_names->files[j],
                                    region_names->max_length-region_names->strlen_files[j]+9,tstat[j],zstat[j],pval[j]*2);
                                }
                            fprintf(fprn,"\n\n");
                            }
                        }
                    }
                }
            #endif
           /*}*/
#if 0        
        if(num_region_names || text || lc_statview) {
            }
        else {
            free(lt2subjects_voxels_idx);
            }
        free(tstat);
        free(zstat);
        free(pval);
        free(temp_float);
        for(i=0;i<td->nfiles1[t];i++) unmap_disk(mm1[i]); 
        free(mm1);

        /*if(lc_test_type[t] != 3) {*/
        if(lc_test_type[t] < 3) {
            for(i=0;i<td->nfiles2[t];i++) unmap_disk(mm2[i]); 
            free(mm2);
            }

        f1index += td->nfiles1[t];
        f2index += td->nfiles2[t];
        /*covindex += td->ncov[t];*/


        #ifndef MONTE_CARLO
            fprintf(stdout,"\n");
        #endif

        if(lc_test_type[t] >= 3) {
            free(behav_var);
            free(r);
            if(lc_test_type[t] == 4) free_d2double(cov_var);
            }
#endif



        } /*for(fiindex=f2index=f1=f2=files_ptr_index=t=0;t<td->ntests;t++)*/
    } /*if(lccleanup_only == (int)FALSE)*/
if(num_region_names) {
    fclose(fprn);
    printf("T-test regional output written to %s\n",regional_anova_name_str);
    }

if(lccleanup == (int)TRUE || lccleanup_only == (int)TRUE) {
    if(scratchdir) {
        sprintf(string,"rm -r %s",scratchdir);
        if(system(string) == -1) printf("Error: unable to %s\n",string);
        }
    else {
        delete_scratch_files(files_ptr,n4dfps); 
        }
    }
}


#if 0
void single_exp_rise_to_max(double x,double *a,double *yfit, double *dyda,int na)
{
    double td;
    td = exp(-a[3]*x);
    *yfit = a[1]+a[2]*(1-td);
    dyda[1] = 1.;
    dyda[2] = 1 - td;
    dyda[3] = a[2]*x*td;
}
#endif
void single_exp_rise_to_max(double x,double *a,double *yfit, double *dyda,int na)
{
    double td;
    td = exp(-a[2]*x);
    *yfit = a[1]*(1-td);
    dyda[1] = 1 - td;
    dyda[2] = a[1]*x*td;
}


void single_exp_decay(double x,double *a,double *yfit, double *dyda,int na)
{
    double td;
    td = exp(-a[3]*x);
    *yfit = a[1]+a[2]*td;
    dyda[1] = 1.;
    dyda[2] = td;
    dyda[3] = -a[2]*x*td;
}

void expfn(double x,double *a,double *yfit, double *dyda,int na)
{
    double td;
    td = exp(a[3]*x);
    *yfit = a[1]+a[2]*td;
    dyda[1] = 1.;
    dyda[2] = td;
    dyda[3] = a[2]*x*td;
}
