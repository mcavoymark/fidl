/* Copyright 7/28/20 Washington University.  All Rights Reserved.
   fidl_logic.cxx  $Revision: 1.1 $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "fidl.h"
#include "subs_util.h"
#include "minmax.h"
static char rcsid[] = "$Header: /home/hannah/mcavoy/idl/clib/RCS/fidl_logic.c,v 1.6 2015/11/16 21:29:29 mcavoy Exp $";
int main(int argc,char **argv)
{
char *file1=NULL,*file2=NULL,*name1ANDNOT2=NULL,*nameNOT1AND2=NULL,*name1AND2=NULL,*strptr; //,filename[MAXNAME];
int i,j,SunOS_Linux,vol,nscalars=0,outvalAND=3;
float *temp_float1,*temp_float2,*temp_float3,scalars[3]={1.,1.,1.};
Interfile_header *ifh1,*ifh2;
if(argc < 5) {
    fprintf(stderr,"    -file1:    4dfp\n");
    fprintf(stderr,"    -file2:    4dfp\n"); 
    fprintf(stderr,"    -1ANDNOT2: Output name.\n");
    fprintf(stderr,"    -NOT1AND2: Output name.\n");
    fprintf(stderr,"    -1AND2:    Output name.\n");
    fprintf(stderr,"    -scalars:  Three numbers - one for each output. Divide output by these numbers. Default is 1.\n");
    fprintf(stderr,"    -outvalAND: Output values for AND: map1, map2, 1, sum. Default is sum.\n");
    exit(-1);
    }
for(i=1;i<argc;i++) { 
    if(!strcmp(argv[i],"-file1") && argc > i+1)
        file1 = argv[++i];
    if(!strcmp(argv[i],"-file2") && argc > i+1)
        file2 = argv[++i];
    if(!strcmp(argv[i],"-1ANDNOT2") && argc > i+1)
        name1ANDNOT2 = argv[++i];
    if(!strcmp(argv[i],"-NOT1AND2") && argc > i+1)
        nameNOT1AND2 = argv[++i]; if(!strcmp(argv[i],"-1AND2") && argc > i+1)
        name1AND2 = argv[++i];
    if(!strcmp(argv[i],"-scalars") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nscalars;
        if(nscalars!=3) {
            printf("Error: 3 numbers must be provided for -scalars. Abort!\n");
            exit(-1);
            } 
        for(j=0;j<3;j++) scalars[j] = atoi(argv[++i]);
        }
    if(!strcmp(argv[i],"-outvalAND") && argc > i+1) {
        if(!strcmp(strptr=argv[++i],"map1")) outvalAND=1;
        else if(!strcmp(strptr=argv[i],"map2")) outvalAND=2;
        else if(!strcmp(strptr=argv[i],"1")) outvalAND=0;
        else if(!strcmp(strptr=argv[i],"sum")) outvalAND=3;
        }
    }
print_version_number(rcsid,stderr);
if(!file1){printf("fidlError: -file1 not specified. Abort!\n");fflush(stdout);exit(-1);}
if(!file2){printf("fidlError: -file2 not specified. Abort!\n");fflush(stdout);exit(-1);}
if(!name1ANDNOT2&&!nameNOT1AND2&&!name1AND2){printf("fidlError: No output specified. Abort!\n");fflush(stdout);exit(-1);}


#if 0
if((SunOS_Linux=checkOS())==-1) exit(-1);
if(!(ifh1=read_ifh(file1,(Interfile_header*)NULL))) exit(-1);
if(!(ifh2=read_ifh(file2,(Interfile_header*)NULL))) exit(-1);
if((vol=ifh1->dim1*ifh1->dim2*ifh1->dim3)!=(ifh2->dim1*ifh2->dim2*ifh2->dim3)) {
    printf("fidlError: %s and %s differ in dimension. Abort!\n",file1,file2);fflush(stdout);exit(-1);
    }
if(!(temp_float1=malloc(sizeof*temp_float1*vol))) {
    printf("fidlError: Unable to malloc temp_float1\n");
    exit(-1);
    }
if(!(temp_float2=malloc(sizeof*temp_float2*vol))) {
    printf("fidlError: Unable to malloc temp_float2\n");
    exit(-1);
    }
if(!(temp_float3=malloc(sizeof*temp_float3*vol))) {
    printf("fidlError: Unable to malloc temp_float3\n");
    exit(-1);
    }
if(!readstack(file1,(float*)temp_float1,sizeof(float),(size_t)vol,SunOS_Linux,ifh1->bigendian)) exit(-1);
if(!readstack(file2,(float*)temp_float2,sizeof(float),(size_t)vol,SunOS_Linux,ifh2->bigendian)) exit(-1);
ifh1->bigendian = !SunOS_Linux?1:0;
if(ifh1->file_name)free(ifh1->file_name);
if(!(ifh1->file_name=malloc(sizeof*ifh1->file_name*(strlen(file1)+strlen(file2)+2)))) {
    printf("fidlError: Unable to malloc ifh1->file_name\n");
    exit(-1);
    }
sprintf(ifh1->file_name,"%s %s",file1,file2);
if(name1ANDNOT2) {
    for(i=0;i<vol;i++) temp_float3[i] = (fabs(temp_float1[i])>1.e-35 && fabs(temp_float2[i])<1.e-35) ? temp_float1[i]/scalars[0] : 0.;
    if(!writestack(name1ANDNOT2,temp_float3,sizeof(float),(size_t)vol,0)) exit(-1);
    min_and_max_init(&ifh1->global_min,&ifh1->global_max);
    min_and_max_floatstack(temp_float3,vol,&ifh1->global_min,&ifh1->global_max);
    if(!write_ifh(name1ANDNOT2,ifh1,0)) exit(-1);
    printf("1ANDNOT2 written to %s\n",name1ANDNOT2);
    }
#endif
//START200728
stack h1,h2;
float *f1,*f2;
if(!(f1=h1.stack0(file1)))exit(-1);
if(!(f2=h2.stack0(file2)))exit(-1);
if(h1.dim[0]!=h2.dim[0]||h1.dim[1]!=h2.dim[1]||h1.dim[2]!=h2.dim[2]){
    std::cout<<"fidlError: "<<file1<<" and "<<file2<<" differ in dimension. Abort!"<<std::endl;
    exit(-1);
    }

if(h1.get_cf_flip()!=h2.get_cf_flip()){ 
    std::cout<<"fidlError: "<<file1<<" "<<h1.get_cf_flip()<<std::endl;
    std::cout<<"fidlError: "<<file2<<" "<<h2.get_cf_flip()<<std::endl;
    std::cout<<"fidlError: Must be equal. Code must added to handle different orientations."<<std::endl;
    exit(-1);
    }
std::cout<<"h1.get_cf_flip()="<<h1.get_cf_flip()<<" h2.get_cf_flip()="<<h2.get_cf_flip()<<std::endl;

float* f3=new float[h1.vol];
if(name1ANDNOT2) {
    for(int i=0;i<h1.vol;++i)f3[i]=(fabsf(f1[i])>1.e-35 && fabsf(f2[i])<1.e-35) ? f1[i]/scalars[0] : 0.;
    if(!writestack(name1ANDNOT2,f3,sizeof(float),(size_t)h1.vol,0))exit(-1);
    //STARTHERE



if(name1ANDNOT2) {
    for(i=0;i<vol;i++) temp_float3[i] = (fabs(temp_float1[i])>1.e-35 && fabs(temp_float2[i])<1.e-35) ? temp_float1[i]/scalars[0] : 0.;
    if(!writestack(name1ANDNOT2,temp_float3,sizeof(float),(size_t)vol,0)) exit(-1);
    min_and_max_init(&ifh1->global_min,&ifh1->global_max);
    min_and_max_floatstack(temp_float3,vol,&ifh1->global_min,&ifh1->global_max);
    if(!write_ifh(name1ANDNOT2,ifh1,0)) exit(-1);
    printf("1ANDNOT2 written to %s\n",name1ANDNOT2);
    }
if(nameNOT1AND2) {
    for(i=0;i<vol;i++) temp_float3[i] = (fabs(temp_float1[i])<1.e-35 && fabs(temp_float2[i])>1.e-35) ? temp_float2[i]/scalars[1] : 0.;
    if(!writestack(nameNOT1AND2,temp_float3,sizeof(float),(size_t)vol,0)) exit(-1);
    min_and_max_init(&ifh1->global_min,&ifh1->global_max);
    min_and_max_floatstack(temp_float3,vol,&ifh1->global_min,&ifh1->global_max);
    if(!write_ifh(nameNOT1AND2,ifh1,0)) exit(-1);
    printf("NOT1AND2 written to %s\n",nameNOT1AND2);
    }
if(name1AND2) {
    if(outvalAND==0) 
        for(i=0;i<vol;i++) temp_float3[i] = (fabs(temp_float1[i])>1.e-35 && fabs(temp_float2[i])>1.e-35) ? 1./scalars[2] : 0.;
    else if(outvalAND==1)
        for(i=0;i<vol;i++) temp_float3[i]=(fabs(temp_float1[i])>1.e-35 && fabs(temp_float2[i])>1.e-35) ? temp_float1[i]/scalars[2]:0.;
    else if(outvalAND==2)
        for(i=0;i<vol;i++) temp_float3[i]=(fabs(temp_float1[i])>1.e-35 && fabs(temp_float2[i])>1.e-35) ? temp_float2[i]/scalars[2]:0.;
    else
        for(i=0;i<vol;i++) temp_float3[i] = (fabs(temp_float1[i])>1.e-35 && fabs(temp_float2[i])>1.e-35) ? 
            (temp_float1[i]+temp_float2[i])/scalars[2] : 0.;
    if(!writestack(name1AND2,temp_float3,sizeof(float),(size_t)vol,0)) exit(-1);
    min_and_max_init(&ifh1->global_min,&ifh1->global_max);
    min_and_max_floatstack(temp_float3,vol,&ifh1->global_min,&ifh1->global_max);
    if(!write_ifh(name1AND2,ifh1,0)) exit(-1);
    printf("1AND2 written to %s\n",name1AND2);
    }
}
