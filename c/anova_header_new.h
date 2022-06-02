/* Copyright 12/31/99 Washington University.  All Rights Reserved.
   anova_header_new.h  $Revision: 1.15 $*/
#define FZERO       1e-30   /*fidl and atlas voxels are returned for voxels whose mse is below this value*/

//#define member(factor,source)     (((1 << (factor)) & (source)) != 0)
//START160407
#define member(factor,source) (((1<<factor)&source)!=0)

#define join(a,b)                 ((a) | (b))
#define subset(a,b)               (((a)&(b)) == (a))
#define enter(factor,source)      ((source) |= (1 << (factor)))
#define signedterm(nterms,sub,Nfactors,value) \
        ((((nterms)-setsize(sub,Nfactors))%2) ? -(value) : (value))
#include "read_driver2.h"
typedef struct {
    int ncol,ncol_between,aa1;
    int *box_dfeffect,*box_dfeffect_between;
    double *contrasts,*AATstack,**Cr;
    } Helmert;
typedef struct {
    double SSWCELL,*weight,*uhat;
    int *n,ntreatments,**where,*nind;
    } CellMeansModel;
typedef struct {
    int *error,*nerror;
    } Classical;
Classical *classical(AnovaDesign *ad);
typedef struct {
    int *n,*addressord,*nsubpergroup,*nold;
    double *ATs,*Sp,*S,*s,*vec,*sstack,*ag,*bg,*uhat,*data,*CrT,*CrTNm1,*CrTNm1Cr,*CrCrTNm1Crm1,*CrCrTNm1Crm1CrT,*scalarAAT,*Gstack,
        *uhat_rearranged,*ATsA,*VecXbarCrCrTNm1Crm1CrTkronprodAAT,*G,*Sstar,*GSstar,*temp_double,*temp_double2,*V,*Sgsl,*work,*Cr;
    gsl_matrix_view gslCrCrTNm1Crm1CrT,gslscalarAAT,gslSp,gslG,gslSstar,gslGSstar;
    gsl_vector_view gsluhat_rearranged,gslVecXbarCrCrTNm1Crm1CrTkronprodAAT;
    } Algina;
Algina *algina_init(AnovaDesign *ad,Helmert *h);

/*int algina(Algina *al,CellMeansModel *cmm,AnovaDesign *ad,Helmert *h);*/
/*START150522*/
int algina(Algina *al,AnovaDesign *ad,Helmert *h);

void free_algina(Algina *al);
typedef struct {
    double *epsilon1,*epsilon2,*chatv,*dferror;
    int goose,lcGIGA;
    FILE *fprn,*shortfp;
    CellMeansModel *cmm;
    } Anovas;
int algina_guts_anova(AnovaDesign *ad,Helmert *h,int nsubjects,int *nsubpergroup,double *sstack,Algina *al,int lcGstackonly,
    int classical_WCM_GIGA,Anovas *anovas);
int cellmeans(AnovaDesign *ad,int nsubjects,CellMeansModel *cmm,FILE *fprn,int lccellmeans,int cellmeansn,int *cellmeansi,int *check);
int nonprop(AnovaDesign *ad);
Helmert *helmert(AnovaDesign *ad,int GIGA,int design);
Helmert *kronecker(AnovaDesign *ad);
void cellcounts(AnovaDesign *ad,CellMeansModel *cmm);
void cmm_design(AnovaDesign *ad,CellMeansModel *cmm);

void pcellheader(int source,FILE *fprn,AnovaDesign *ad);
void pcellstats(int count,double sum,double sumsq,FILE *fprn,AnovaDesign *ad,int source);
double *get_Sstackinv(int j,LinearModel **glm,TC *tcs,int tc_contrast,AnovaDesign *ad,Helmert *h,int glmpersub0);

typedef struct {
    char *t,**tptr;
    int n;
    } Treats;
Treats* get_treatment_str(AnovaDesign *ad,int nidx,int *idx);

int statview_guts(int lenvol,Algina *al,AnovaDesign *ad,char *regional_anova_name);

typedef struct {
    double *ybar,*Sinvybar,*Sstackinv,*eqvar,*uneqvar,*fstatuneqvar,dfeqvar,*dfuneqvar;
    } SS;
SS *init_ss(Helmert *h);

//void anova_fprn(FILE *fprn,int goose,AnovaDesign *ad,int classical_WCM_GIGA,CellMeansModel *cmm,int flag_empty_cells,int df1start,
//    int start,double *dferror,double *epsilon1,double *epsilon2,int lcGIGA,double *chat,FILE *shortfp,SS *ss,int v);
//START211119
void anova_fprn(FILE *fprn,int goose,AnovaDesign *ad,int classical_WCM_GIGA,CellMeansModel *cmm,int flag_empty_cells,int df1start,
    int start,double *dferror,double *epsilon1,double *epsilon2,int lcGIGA,double *chat,FILE *shortfp,FILE *shortfp_p,FILE *shortfp_z,SS *ss,int v);

//int anova(AnovaDesign *ad,FILE *fprn,double *epsilon1,double *epsilon2,double *dferror,Helmert *h,CellMeansModel *cmm,double SSE,
//    int dfSSE,int goose,Classical *c,int classical_WCM_GIGA,int lcGIGA,double *chat,SS *ss,int v,FILE *shortfp);
//START211119
int anova(AnovaDesign *ad,FILE *fprn,double *epsilon1,double *epsilon2,double *dferror,Helmert *h,CellMeansModel *cmm,double SSE,
    int dfSSE,int goose,Classical *c,int classical_WCM_GIGA,int lcGIGA,double *chat,SS *ss,int v,FILE *shortfp,FILE *shortfp_p,FILE *shortfp_z);
