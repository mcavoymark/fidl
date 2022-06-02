/* Copyright 12/31/99 Washington University.  All Rights Reserved.
   anova_header.h  $Revision: 12.108 $*/

#define RANDOM      0       /*zeroeth column is RANDOM factor for Perlman_levels*/
#define FZERO       1e-30   /*fidl and atlas voxels are returned for voxels whose mse is below this value*/
#define FZERO_SSEFFECT 1e-5 
/*#define F_THRESHOLD 1.8*/     /*When outputing Box corrected zstats, any F below this threshold has its respective
                              corrected zstat set to UNSAMPLED VOXEL. The reason for this is that there is some
                              numerical problem that results in Box corrected zstats with F's lower than the threshold
                              having slightly higher corrected z's when they should of course be lower.
			      I rationalize that this is all ok because F's higher than F_THRESHOLD do yield reduced
                              z's, and any F as low as the threshold have respective p's ~.2 so don't represent
                              activated regions.*/ 

#define Z_THRESHOLD .62     /*For large degrees of freedom, the above is not true in that small F's yield significant p's.
                              Thus, by applying the criterion to gaussianzed F's it is hoped to alleviate the problem*/


#define REALLOC(A,B,type) if(!(A=(type *)realloc((A),B*sizeof(type)))) { \
                              fprintf(stdout,"Error allocating memory for " #A "\n"); \
                              exit(-1); \
                              }
/*#define MAXL 100*/

#define member(factor,source)     (((1 << (factor)) & (source)) != 0)
#define join(a,b)                 ((a) | (b))
#define subset(a,b)               (((a)&(b)) == (a))
#define enter(factor,source)      ((source) |= (1 << (factor)))
#define signedterm(nterms,sub,Nfactors,value) \
        ((((nterms)-setsize(sub,Nfactors))%2) ? -(value) : (value))

typedef struct {
    int chunks;
    int num_factors;
    int num_sources;		/* num_sources = 2**num_factors */
    int num_datafiles;
    int num_cells,num_cells_notRANDOM;
    int between,within;         /* holds between subject factors */
    int *Perlman_levels;	/* length = num_factors */
    int *address;		/* length = num_datafiles */
    int *replications;		/* length = num_cells */
    int *level;			/* length = num_factors, computational tool */
    int *sumcount;		/* length = num_factors, computational tool */
    int *dfeffect;		/* length = num_sources/2 - 1 */
    double *dfeffectSPH;
    int *dferror;               /* length = num_sources/2 - 1 */
    double *dferrorSPH;    
    int *dferror_within;	/* length = num_sources/2 - 1, df from within subjects factors contributing to dferror */
    int betprod,withprod;  
    float *data;		/* length = num_cells */
    float *driver_data;		/* length = nlines */
    double *fstat,*chat,*htildaprimeprime,*htilda; /* length = num_sources/2 - 1 */

    double *bracket;		/* length = num_sources */
    char **datafiles;		/* length = num_datafiles */

    char *fstatfiles;
    char **fstatfilesptr;       /* num_sources */ 
    int *fstatfilesl;           /* num_sources */
    int max_length;

    char *fn,**fnptr,*datafilesstack,*ln,**lnptr;
    int aa,aa1,nwithin,nbetween,*within_between,*nterms,within_neffects,*within_effects,*betweeni,*withini,Perlman_levelst;
    double *sseffect,*mseffect,*sserror,*mserror;
    }
AnovaDesign;

typedef struct {
    int ncol,ncol_between;
    int *box_dfeffect,*box_dfeffect_between;
    double **contrasts,*AATstack,**Cr,*Gstack;
    MAT *AAT,*scalarAAT,*CrCrTNm1Crm1CrT,*G,*Sstar,*GSstar;
    VEC *VecXbarCrCrTNm1Crm1CrTkronprodAAT;
    } Helmert; 

typedef struct {
    double SSWCELL,*weight;
    int *n,ntreatments,**where,*nind;
    VEC *uhat,*uhat_rearranged;
    } CellMeansModel;

typedef struct {
    int *error,*nerror;
    } Classical;
Classical *classical(AnovaDesign *ad);

typedef struct {
    int *n,*addressord,*nsubpergroup;
    MAT *Sp,*S,*s,*vec,*outprod;
    double *sstack,*ag,*bg,*uhat,*data;
    } Algina;
Algina *algina_init(AnovaDesign *ad,Helmert *h);

int algina(Algina *al,CellMeansModel *cmm,AnovaDesign *ad);

int algina_guts(AnovaDesign *ad,Helmert *h,int nsubjects,int *nsubpergroup,double *sstack,Algina *al,int lcGstackonly,
    int classical_WCM_GIGA);


/*AnovaDesign *read_driver(char *driver_file,int nreg);*/
/*START53*/
AnovaDesign *read_driver(char *driver_file,int nreg,char *scratchdir);


int offset(int *level, int num_factors, int *Perlman_levels);

int cellmeans(AnovaDesign *ad,int nsubjects,CellMeansModel *cmm,FILE *fprn,int lccellmeans,int cellmeansn,int *cellmeansi,int *check);

int setsize(int set, int maxsize);
int nextlevel(int *level, int source, int sourceflag, int num_factors, int *Perlman_levels);
int nonprop(AnovaDesign *ad);
int anova(AnovaDesign *ad,FILE *fprn,int *error_source,double *epsilon1,double *epsilon2,int *dferror,Helmert *h,CellMeansModel *cmm,
    double SSE,int dfSSE,int goose,double *Sstackinv,double var,Classical *c,int classical_WCM_GIGA,int nsubjects,Algina *al,
    int lcGIGA,double *chat);
Helmert *helmert(AnovaDesign *ad,int GIGA,int design);
Helmert *kronecker(AnovaDesign *ad);
void cellcounts(AnovaDesign *ad,CellMeansModel *cmm);
void cmm_design(AnovaDesign *ad,CellMeansModel *cmm);

void pcellheader(int source,FILE *fprn,AnovaDesign *ad);
void pcellstats(int count,double sum,double sumsq,FILE *fprn,AnovaDesign *ad,int source);

void get_transform(TC *tcs,int tc_contrast,int j,MAT *ATA,int Mcol,float *c,MAT *transform);
double *get_Sstackinv(int j,LinearModel *glm,TC *tcs,int tc_contrast,AnovaDesign *ad,Helmert *h);

void anova_fprn(FILE *fprn,int goose,AnovaDesign *ad,int classical_WCM_GIGA,CellMeansModel *cmm,int flag_empty_cells,int df1start,
    int start,int *dferror,double *epsilon1,double *epsilon,int lcGIGA,double *chat);

typedef struct {
    char *t,**tptr;
    int n;
    } Treats;
Treats* get_treatment_str(AnovaDesign *ad,int nidx,int *idx);
