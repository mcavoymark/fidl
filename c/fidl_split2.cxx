/* Copyright 9/30/19 Washington University.  All Rights Reserved.
   fidl_split2.cxx  $Revision: 1.1 $ */

#include <iostream>
#include <vector>
//#include <stdlib.h>
#include <string.h>

#include "files_struct.h"
#include "coor.h"
#include "filetype.h"
#include "fidl.h"
#include "subs_util.h"
#include "lut.h"
#include "subs_nifti.h"

int main(int argc,char **argv)
{
char string[MAXNAME],string2[MAXNAME],filename[MAXNAME],*strptr,*rroot=NULL,*rrootptr;
int i,j,nfiles=0,nroots=0,LRregonly=0;
Files_Struct *files=NULL,*roots=NULL;

if(argc<5) {
    std::cout<<"fidl_split2"<<std::endl;
    std::cout<<"    -files: Files to split by hemisphere, anterior/posterior and inferior/superior. Conc, list or anything."<<std::endl;
    std::cout<<"    -roots: Roots are used for the region filenames and the region names."<<std::endl;
    std::cout<<"    -rroot: Used in place of -roots for region names. One word."<<std::endl;
    std::cout<<"    -LRregonly: Only output the hemispherical region file."<<std::endl;
    std::cout<<"        Ex. set FILES = (-files /data/nil-bluearc/vlassenko/mcavoy/HCP/sub/AGBR-048/vglab/T1w/brainmask_fs.nii.gz)"<<std::endl;
    std::cout<<"        Ex. set ROOTS = (-roots brainmask_fs)"<<std::endl;
    std::cout<<"        Ex. nice +19 $BIN/fidl_split2 $FILES $ROOTS -LRregonly"<<std::endl;
    std::cout<<"        Region file will be an unsigned short nifti named brainmask_fs_LRreg.nii.gz with values 65534 for Lbrainmask and 65535 for Rbrainmask."<<std::endl;
    exit(-1);
    }
for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-files") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nfiles;
        if(!(files=read_files(nfiles,&argv[i+1]))) exit(-1);
        i += nfiles;
        }
    if(!strcmp(argv[i],"-roots") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nroots;
        if(!(roots=get_files(nroots,&argv[i+1]))) exit(-1);
        i += nroots;
        }
    if(!strcmp(argv[i],"-rroot") && argc > i+1)
        rroot = argv[++i];
    if(!strcmp(argv[i],"-LRregonly"))
        LRregonly=1;
    }
if(!nfiles) {
    std::cout<<"fidlError: Need to specify -files"<<std::endl;
    exit(-1);
    }
if(!nroots) {
    std::cout<<"fidlError: Need to specify -roots"<<std::endl;
    exit(-1);
    }

float *fptr,*coorf;
Interfile_header *ifh;
size_t i1,st[2];
int nL,nR,nant,npos,ninf,nsup,*brnidx,lenbrain,vol,filetype,c_orient[3];
int16_t datatype;
for(i1=0;i1<files->nfiles;++i1){
    coor c0;
    if(!(coorf=c0.coor0(files->files[i1])))exit(-1);
    brnidx=c0.get_brnidx(lenbrain,vol);
    c0.assign(filetype,c_orient,datatype);
    if(filetype==(int)IMG){
        std::vector<float> L(vol,0),R(vol,0),LR(vol,0),ant(vol,0),pos(vol,0),antpos(vol,0),inf(vol,0),sup(vol,0),infsup(vol,0);
        float *temp_float;
        if(!(temp_float=c0.stack0()))exit(-1);
        //L.resize(vol,0);R.resize(vol,0);LR.resize(vol,0);ant.resize(vol,0);pos.resize(vol,0);antpos.resize(vol,0);inf.resize(vol,0);sup.resize(vol,0);infsup.resize(vol,0);
        for(fptr=coorf,nL=nR=npos=nant=ninf=nsup=j=0;j<lenbrain;++j){
            if((*fptr++)<0) {
                L[brnidx[j]] = temp_float[brnidx[j]];
                LR[brnidx[j]] = 2.;
                nL++;
                }
            else {
                R[brnidx[j]] = temp_float[brnidx[j]];
                LR[brnidx[j]] = 3.;
                nR++;
                }
            if((*fptr++)<0) {
                pos[brnidx[j]] = temp_float[brnidx[j]];
                antpos[brnidx[j]] = 2.;
                npos++;
                }
            else {
                ant[brnidx[j]] = temp_float[brnidx[j]];
                antpos[brnidx[j]] = 3.;
                nant++;
                }
            if((*fptr++)<0) {
                inf[brnidx[j]] = temp_float[brnidx[j]];
                infsup[brnidx[j]] = 2.;
                ninf++;
                }
            else {
                sup[brnidx[j]] = temp_float[brnidx[j]];
                infsup[brnidx[j]] = 3.;
                nsup++;
                }
            }
        ifh=c0.getifh();
        if(ifh->file_name)free(ifh->file_name);
        ifh->bigendian=0;
        if(!(ifh->file_name=(char*)malloc(sizeof*ifh->file_name*files->strlen_files[i1]))) {
            std::cout<<"fidlError: Unable to malloc ifh->file_name"<<std::endl;
            exit(-1);
            }
        strcpy(ifh->file_name,files->files[i1]);
        strcpy(string,roots->files[i1]);
        if(!(strptr=get_tail_sans_ext(string))) exit(-1);
        if(!LRregonly) {
            sprintf(filename,"%s_L.4dfp.img",strptr);
            if(!writestack(filename,L.data(),sizeof(float),(size_t)vol,0)) exit(-1);
            if(!write_ifh(filename,ifh,0)) exit(-1);
            std::cout<<"L written to "<<filename<<std::endl;
            sprintf(filename,"%s_R.4dfp.img",strptr);
            if(!writestack(filename,R.data(),sizeof(float),(size_t)vol,0)) exit(-1);
            if(!write_ifh(filename,ifh,0)) exit(-1);
            std::cout<<"R written to "<<filename<<std::endl;
            sprintf(filename,"%s_pos.4dfp.img",strptr);
            if(!writestack(filename,pos.data(),sizeof(float),(size_t)vol,0)) exit(-1);
            if(!write_ifh(filename,ifh,0)) exit(-1);
            std::cout<<"Posterior written to "<<filename<<std::endl;
            sprintf(filename,"%s_ant.4dfp.img",strptr);
            if(!writestack(filename,ant.data(),sizeof(float),(size_t)vol,0)) exit(-1);
            if(!write_ifh(filename,ifh,0)) exit(-1);
            std::cout<<"Anterior written to "<<filename<<std::endl;
            sprintf(filename,"%s_inf.4dfp.img",strptr);
            if(!writestack(filename,inf.data(),sizeof(float),(size_t)vol,0)) exit(-1);
            if(!write_ifh(filename,ifh,0)) exit(-1);
            std::cout<<"Inferior written to n"<<filename<<std::endl;
            sprintf(filename,"%s_sup.4dfp.img",strptr);
            if(!writestack(filename,sup.data(),sizeof(float),(size_t)vol,0)) exit(-1);
            if(!write_ifh(filename,ifh,0)) exit(-1);
            std::cout<<"Superior written to "<<filename<<std::endl;
            }
        rrootptr = rroot ? rroot : roots->files[i1];
        sprintf(string2,"0 L%s %d",rrootptr,nL);
        st[0] = strlen(string2) + 1;
        sprintf(filename,"1 R%s %d",rrootptr,nR);
        st[1] = strlen(filename) + 1;
        if(ifh->region_names) free_ifhregnames(ifh);
        if(!(ifh->region_names=d2charvar(2,st))) return 0;
        ifh->nregions = 2;
        strcpy(ifh->region_names[0],string2);
        strcpy(ifh->region_names[1],filename);
        sprintf(filename,"%s_LRreg.4dfp.img",strptr);
        if(c0.flip(LR.data())==-1)exit(-1);
        if(!writestack(filename,LR.data(),sizeof(float),(size_t)vol,0))exit(-1);
        if(!write_ifh(filename,ifh,0)) exit(-1);
        std::cout<<"LRreg written to "<<filename<<std::endl;
        if(!LRregonly) {
            sprintf(string2,"0 pos%s %d",rrootptr,npos);
            st[0] = strlen(string2) + 1;
            sprintf(filename,"1 ant%s %d",rrootptr,nant);
            st[1] = strlen(filename) + 1;
            if(ifh->region_names) free_ifhregnames(ifh);
            if(!(ifh->region_names=d2charvar(2,st)))exit(-1);
            ifh->nregions = 2;
            strcpy(ifh->region_names[0],string2);
            strcpy(ifh->region_names[1],filename);
            sprintf(filename,"%s_antposreg.4dfp.img",strptr);
            if(!writestack(filename,antpos.data(),sizeof(float),(size_t)vol,0))exit(-1);
            if(!write_ifh(filename,ifh,0))exit(-1);
            std::cout<<"antposreg written to "<<filename<<std::endl;
            sprintf(string2,"0 inf%s %d",rrootptr,ninf);
            st[0] = strlen(string2) + 1;
            sprintf(filename,"1 sup%s %d",rrootptr,nsup);
            st[1] = strlen(filename) + 1;
            if(ifh->region_names) free_ifhregnames(ifh);
            if(!(ifh->region_names=d2charvar(2,st)))exit(-1);
            ifh->nregions = 2;
            strcpy(ifh->region_names[0],string2);
            strcpy(ifh->region_names[1],filename);
            sprintf(filename,"%s_infsupreg.4dfp.img",strptr);
            if(!writestack(filename,infsup.data(),sizeof(float),(size_t)vol,0))exit(-1);
            if(!write_ifh(filename,ifh,0))exit(-1);
            std::cout<<"infsupreg written to "<<filename<<std::endl;
            }
        }
    else if(filetype==(int)NIFTI){
        //std::vector<unsigned short> LRus;
        //unsigned short* temp_us;
        //if(!(temp_us=c0.stack0us()))exit(-1);
        //LRus.resize(vol,0);
        std::vector<unsigned short> LRus(vol,0);
        lut l0;unsigned short Lval,Rval;
        l0.lut0(files->files[i1]);Lval=l0.lutLval();Rval=l0.lutRval();

        std::cout<<"Lval="<<Lval<<" Rval="<<Rval<<std::endl;
        std::cout<<"lenbrain="<<lenbrain<<" vol="<<vol<<std::endl;

        for(fptr=coorf,nL=nR=npos=nant=ninf=nsup=j=0;j<lenbrain;++j){
            if((*fptr++)<0) {
                LRus[brnidx[j]] = Lval;
                nL++;
                }
            else {
                LRus[brnidx[j]] = Rval;
                nR++;
                }
            if((*fptr++)<0) {
                npos++;
                }
            else {
                nant++;
                }
            if((*fptr++)<0) {
                ninf++;
                }
            else {
                nsup++;
                }
            }


        std::cout<<"nL="<<nL<<" nR="<<nR<<std::endl;

        #if 0
        strcpy(string,roots->files[i1]);
        if(!(strptr=get_tail_sans_ext(string))) exit(-1);
        sprintf(filename,"%s_LRreg.nii",strptr);
        #endif
        sprintf(filename,"%s_LRreg.nii",roots->files[i1]);

        if(!(nifti_write2us(filename,files->files[i1],LRus.data())))exit(-1);
        std::string str="gzip -f "+(std::string)filename;
        if(system(str.c_str())==-1)
            std::cout<<"fidlError: "<<str.c_str()<<std::endl;
        else
            strcat(filename,".gz");
        std::cout<<"LRreg written to "<<filename<<std::endl;
        }
    }
}
