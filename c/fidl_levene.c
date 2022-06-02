/* Copyright 1/18/12 Washington University.  All Rights Reserved.
   fidl_levene.c  $Revision: 1.2 $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>
static char rcsid[] = "$Header: /home/hannah/mcavoy/idl/clib/RCS/fidl_levene.c,v 1.2 2015/01/30 21:57:34 mcavoy Exp $";
main(int argc,char **argv)
{
char *out=NULL,*xform_file=NULL,*mask_file=NULL,*scrub_file=NULL,*strptr,filename[MAXNAME];
int i,j,k,l,m,p,ll,SunOS_Linux,nfiles=0,lenbrain,*brnidx,A_or_B_or_U,atlas=222,N,nvalid,*valid_frames,*Ni;
float *t4,*temp_float;
double **z,*zdotdot,num,*den,*ybar,*temp_double,*stat,td,*df1,*df2;
Files_Struct *files,*xformfiles;
Dim_Param *dp;
Mask_Struct *ms1,*ms2;
Atlas_Param *ap;
Memory_Map *mm;
Data *scrub;
Interfile_header *ifh;
if(argc < 5) {
    fprintf(stderr,"Levene's test for equality of variance across runs.\n");
    fprintf(stderr,"    -files:      4dfp files or conc.\n");
    fprintf(stderr,"    -out:        Gaussianized F statistic.\n");
    fprintf(stderr,"    -xform_file: t4 file.\n");
    fprintf(stderr,"    -atlas:      Either 111, 222 or 333. Default is 222. Used with -xform_file option.\n");
    fprintf(stderr,"    -mask:       Only voxels in the mask are analyzed.\n");
    fprintf(stderr,"    -scrub:      Valid frame = 1, skip = 0. Single column. Length matches the total time dimension of files.\n");
    fprintf(stderr,"                 If not included, all frames are used.\n");
    exit(-1);
    }
for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-files") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nfiles;
        if(nfiles>1) {
            if(!(files=get_files(nfiles,&argv[i+1]))) exit(-1);
            }
        else {
            strptr = strrchr(argv[i+1],'.');
            if(!strcmp(strptr,".conc")) {
                if(!(files=read_conc(argv[i+1]))) exit(-1);
                if(files->nfiles==1) {
                    printf("Error: You need at least two runs for a Levene's test.\n");fflush(stdout);
                    exit(-1);
                    }
                }
            else if(!strcmp(strptr,".img")){
                printf("Error: You need at least two runs for a Levene's test.\n");fflush(stdout);
                exit(-1);
                }
            else {
                printf("Error: -conc not conc or img. Abort!\n");fflush(stdout);
                exit(-1);
                }
            }
        i += nfiles;
        }
    if(!strcmp(argv[i],"-out") && argc > i+1)
        out=argv[++i];
    if(!strcmp(argv[i],"-xform_file") && argc > i+1)
        xform_file=argv[++i];
    if(!strcmp(argv[i],"-atlas") && argc > i+1)
        atlas = atoi(argv[++i]);
    if(!strcmp(argv[i],"-mask") && argc > i+1)
        mask_file=argv[++i];
    if(!strcmp(argv[i],"-scrub") && argc > i+1)
        scrub_file=argv[++i];
    }
print_version_number(rcsid,stdout);
if((SunOS_Linux=checkOS())==-1) exit(-1);
if(!nfiles) {
    printf("Error: Need to specify 4dfps or concs with -files\n");fflush(stdout);
    exit(-1);
    }
if(!out) {
    printf("Error: Need to specify output file with -out\n");fflush(stdout);
    exit(-1);
    }
if(!(dp=dim_param(files->nfiles,files->files,SunOS_Linux,0))) exit(-1);
if(!(valid_frames=malloc(sizeof*valid_frames*dp->tdim_total))) {
    printf("fidlError: Unable to malloc dp->tdim_total\n");
    exit(-1);
    }
if(!scrub_file) {
    for(i=0;i<dp->tdim_total;i++) valid_frames[i] = 1;
    }
else {
    if(!(scrub=read_data(scrub_file,0,0,0,0))) exit(-1);
    if(scrub->nsubjects!=dp->tdim_total) {
        printf("%s has %d lines. dp->tdim_total=%d Must be equal.\n",scrub_file,scrub->nsubjects,dp->tdim_total);fflush(stdout);
        exit(-1);
        }
    }
if(!(ms1=get_mask_struct(!xform_file?mask_file:(char*)NULL,dp->vol,(int*)NULL,SunOS_Linux,(LinearModel*)NULL))) exit(-1);


#if 0
if(xform_file) if(!(ms2=get_mask_struct(mask_file,dp->vol,(int*)NULL,SunOS_Linux,(LinearModel*)NULL))) exit(-1);
lenbrain = !xform_file? ms1->lenbrain : ms2->lenbrain;
brnidx = !xform_file? ms1->brnidx : ms2->brnidx;
if(xform_file) {
    if(!(t4=malloc(sizeof*t4*(size_t)T4SIZE))) {
        printf("Error: Unable to malloc t4\n");
        exit(-1);
        }
    if(!read_xform(xform_file,t4)) exit(-1);
    if((A_or_B_or_U=twoA_or_twoB(xform_file)) == 2) exit(-1);
    }
else {
    atlas = get_atlas(dp->vol);
    }
#endif
/*START120131*/
lenbrain = !xform_file? ms1->lenbrain : ms2->lenbrain;
brnidx = !xform_file? ms1->brnidx : ms2->brnidx;
if(xform_file) {
    if(!(ms2=get_mask_struct(mask_file,dp->vol,(int*)NULL,SunOS_Linux,(LinearModel*)NULL))) exit(-1);
    lenbrain = ms2->lenbrain;
    brnidx = ms2->brnidx;
    k = ms1->lenbrain>ms2->lenbrain?ms1->lenbrain:ms2->lenbrain;
    if(!(t4=malloc(sizeof*t4*(size_t)T4SIZE))) {
        printf("Error: Unable to malloc t4\n");
        exit(-1);
        }
    if(!read_xform(xform_file,t4)) exit(-1);
    if((A_or_B_or_U=twoA_or_twoB(xform_file)) == 2) exit(-1);
    }
else {
    k = lenbrain = ms1->lenbrain;
    brnidx = ms1->brnidx;
    atlas = get_atlas(dp->vol);
    }


if(!(ap=get_atlas_param(atlas,(Interfile_header*)NULL))) exit(-1);
if(!(temp_float=malloc(sizeof*temp_float*ap->vol))) {
    printf("fidlError: Unable to malloc temp_float\n");
    exit(-1);
    }

#if 0
if(!(temp_double=malloc(sizeof*temp_double*(ms1->lenbrain>ms2->lenbrain?ms1->lenbrain:ms2->lenbrain)))) {
    printf("fidlError: Unable to malloc temp_double\n");
    exit(-1);
    }
#endif
/*START120131*/
if(!(temp_double=malloc(sizeof*temp_double*k))) {
    printf("fidlError: Unable to malloc temp_double\n");
    exit(-1);
    }

if(!(stat=malloc(sizeof*stat*(xform_file?ap->vol:lenbrain)))) {
    printf("fidlError: Unable to malloc stat\n");
    exit(-1);
    }
if(!(z=d2double(files->nfiles,lenbrain))) exit(-1);
if(!(zdotdot=malloc(sizeof*zdotdot*lenbrain))) {
    printf("fidlError: Unable to malloc zdotdot\n");
    exit(-1);
    }
for(i=0;i<lenbrain;i++) zdotdot[i] = 0.;
if(!(den=malloc(sizeof*den*lenbrain))) {
    printf("fidlError: Unable to malloc den\n");
    exit(-1);
    }
for(i=0;i<lenbrain;i++) den[i] = 0.;
if(!(ybar=malloc(sizeof*ybar*lenbrain))) {
    printf("fidlError: Unable to malloc ybar\n");
    exit(-1);
    }

if(!(Ni=malloc(sizeof*Ni*files->nfiles))) {
    printf("fidlError: Unable to malloc Ni\n");
    exit(-1);
    }
if(!(df1=malloc(sizeof*df1*lenbrain))) {
    printf("fidlError: Unable to malloc df1\n");
    exit(-1);
    }
if(!(df2=malloc(sizeof*df2*lenbrain))) {
    printf("fidlError: Unable to malloc df2\n");
    exit(-1);
    }
for(nfiles=N=l=m=0;m<files->nfiles;l+=dp->tdim[m++]) {
    if(dp->number_format[m]==(int)FLOAT_IF) {
        if(!(mm=map_disk(files->files[m],dp->vol*dp->tdim[m],0))) exit(-1);
        }
    else if(dp->number_format[m]==(int)DOUBLE_IF) {
        if(!(mm=map_disk_double(files->files[m],dp->vol*dp->tdim[m],0))) exit(-1);
        }
    else {
        printf("Error: Unrecognized number format dp->number_format[%d]=%d\n",m,dp->number_format[m]);
        printf("Error: %s\n",files->files[m]);
        fflush(stdout);exit(-1);
        }
    for(i=0;i<lenbrain;i++) ybar[i]=0.;
    for(k=0;k<3;k++) {
        for(ll=l,nvalid=p=i=0;i<dp->tdim[m];i++,p+=dp->vol,ll++) {
            if(!valid_frames[ll]) continue; else nvalid++;
            if(dp->number_format[m]==(int)FLOAT_IF) {
                for(j=0;j<ms1->lenbrain;j++) temp_float[j] = mm->ptr[p+ms1->brnidx[j]];
                if(dp->swapbytes[m]) swap_bytes((unsigned char *)temp_float,sizeof(float),(size_t)ms1->lenbrain);
                for(j=0;j<ms1->lenbrain;j++)
                    temp_double[j] = temp_float[j]==(float)UNSAMPLED_VOXEL ? (double)UNSAMPLED_VOXEL : (double)temp_float[j];
                }
            else {
                for(j=0;j<ms1->lenbrain;j++) temp_double[j] = mm->ptr[p+ms1->brnidx[j]];
                if(dp->swapbytes[m]) swap_bytes((unsigned char *)temp_double,sizeof(double),(size_t)ms1->lenbrain);
                }
            if(xform_file) {
                if(!t4_atlas(temp_double,stat,t4,dp->xdim,dp->ydim,dp->zdim,dp->dxdy,dp->dz,A_or_B_or_U,dp->orientation,ap)) exit(-1);
                for(j=0;j<ms2->lenbrain;j++) temp_double[j] = stat[ms2->brnidx[j]];
                }
            if(!k) {
                for(j=0;j<lenbrain;j++) ybar[j]+=temp_double[j]; 
                }
            else if(k==1) {
                for(j=0;j<lenbrain;j++) {
                    z[m][j]+=td=fabs(temp_double[j]-ybar[j]); 
                    zdotdot[j]+=td;
                    }
                }
            else {
                for(j=0;j<lenbrain;j++) {
                    td = fabs(temp_double[j]-ybar[j]) - z[m][j]; 
                    den[j] += td*td;
                    }
                }
            }
        if(!k) {
            if(!nvalid) break;
            nfiles++;
            for(i=0;i<lenbrain;i++) ybar[i]/=(double)nvalid;
            }
        else if(k==1) {
            for(i=0;i<lenbrain;i++) z[m][i]/=(double)nvalid;
            N += Ni[m] = nvalid;
            }
        }
    if(!unmap_disk(mm)) exit(-1);
    }
for(i=0;i<lenbrain;i++) {
    zdotdot[i] /= (double)N;
    for(num=0.,m=0;m<files->nfiles;m++) {
        if(Ni[m]) {
            td = z[m][i]-zdotdot[i];
            num += Ni[m]*td*td;
            }
        }
    temp_double[i] = (double)(N-nfiles)*num/(double)(nfiles-1)/den[i]; 
    df1[i] = (double)(nfiles-1);
    df2[i] = (double)(N-nfiles); 
    }
f_to_z(temp_double,stat,lenbrain,df1,df2);
if(!(ifh=init_ifh(4,ap->xdim,ap->ydim,ap->zdim,1,ap->voxel_size[0],ap->voxel_size[1],ap->voxel_size[2],SunOS_Linux?0:1))) exit(-1);
min_and_max_init(&ifh->global_min,&ifh->global_max);
min_and_max_doublestack(stat,lenbrain,&ifh->global_min,&ifh->global_max);
for(i=0;i<ap->vol;i++) temp_float[i] = 0.;
for(i=0;i<lenbrain;i++) temp_float[brnidx[i]] = (float)stat[i];
strcpy(filename,out);
if(!(strptr=strstr(filename,"fstat"))) {
    *(strrchr(filename,'.')-5)=0;
    strcat(filename,"_zstat.4dfp.img");
    }
if(!writestack(filename,temp_float,sizeof(float),(size_t)ap->vol,0)) exit(-1);
if(!write_ifh(filename,ifh,0)) exit(-1);
printf("Z statistics written to %s\n",filename);
}
