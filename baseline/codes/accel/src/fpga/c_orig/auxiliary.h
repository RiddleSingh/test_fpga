//
//  auxiliary.h
//  fpga-ks-c
//
//  Created by Alessandro Peri on 08/09/2019.
//  Copyright © 2019 Alessandro Peri. All rights reserved.
//

#ifndef auxiliary_h
#define auxiliary_h

#define KCTEST 0
#define KCTEST_MEAN 0

#include "usr_decl.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <gsl/gsl_multifit.h>
#include "perf.h"

#include <gsl/gsl_statistics_double.h>
void printkprimeinterp(Parameters_type * Parms_){
    printf("\nkprimeinterp\n");
    for(int ik=0;ik<NKGRID;ik++){
        for(int iid=0;iid<NSTATES_ID;iid++)
            printf("%lf ", Parms_->kprime_interp[IXID(iid,ik)]);
        printf("\n");
    }
}
void printkcross(Parameters_type * Parms_){
    printf("\nkcross\n");
    for(int j=0;j<N_AGENTS;j++){
        printf("%18.15lf ", Parms_->kcross[j]);
    }
}
void loadprime(Parameters_type * Parms_){
    
    // filescanf kprime
    char filename[150] ={'\0'};
    int i;
    REAL value;
    FILE *fp;
    // --------------------------------------------------------------------------------------------
    strcpy(filename,SHOCKPATH);
    strcat(filename,"kprime.txt");
    fp = fopen(filename, "r");
    if(fp == NULL)
    {
        fprintf(stderr,"Cannot read filename: %s",filename);
        exit(EXIT_FAILURE);
    }
    else
        fprintf(stderr,"\nOpen filename: %s",filename);
    i=0;
    while(fscanf(fp,"%lf",&value)== 1)
    {
        // ngridk,ngridkm,nstates_ag,nstates_id
        int residual;
        int ia  = i/(NKM_GRID*NSTATES_ID*NKGRID);
        residual = i%(NKM_GRID*NSTATES_ID*NKGRID);
        int ikm = residual/(NSTATES_ID*NKGRID);
        residual = residual%(NSTATES_ID*NKGRID);
        int iid = residual/NKGRID;
        int ik  = residual%NKGRID;
        Parms_->kprime[IXV(ia,ikm,iid,ik)]=value;//Parms_->kprime[IXV2(ik,ikm,ia,iid)]
        i = i+1;
    }
    printf("\n1.2. Check number of elements read from file is consistent:");
    if (i!= NSTATES)
    {
        printf(" ERROR: %d != %d\n",i,NSTATES);
        exit(EXIT_FAILURE);
    }
    else
        printf(" DONE\n");
    fclose(fp);
   
    printf("kprime\n");
    // ngridk,ngridkm,nstates_ag,nstates_id
    for(int iid=0;iid<NSTATES_ID;iid++)
        for(int ia=0;ia<NSTATES_AG;ia++)
        {
            printf("val(:,:,%d,%d) =\n",ia+1,iid+1);
            
            for(int ik=0;ik<NKGRID;ik++)
            {
                for(int ikm=0;ikm<NKM_GRID;ikm++)
                    printf("%lf ",Parms_->kprime[IXV(ia,ikm,iid,ik)]);
                printf("\n");
            }
            printf("\n\n");
            
        }
    
    printf("Checkpoint");
    
}
/**
 * Compares elementwise the equilibrium values of the following:
 *  ag[]
 *  ag2[]
 *  agshock[]
 *  epsilon[]
 *  epsilon2[]
 *  idshock[]
 *  k[]
 *  kcross[]
 *  km[]
 *  P[]
 *  agshock[]
 *  coeff[]
 * 
 * But also compares the constant parameters.
 */
void checkwithmatlab(Parameters_type * Parms_){
    printf("\nCompare Equilibrium objects with Matlab\n");
    printf("\nag: ");
    for(int i=0;i<NSTATES_AG;i++)
        printf("%lf ",Parms_->ag[i]);
    printf("\na2: ");
    for(int i=0;i<NSTATES_AG;i++)
        printf("%lf ",Parms_->ag2[i]);
    printf("\nag: ");
    for(int i=0;i<T;i++)
        printf("%d ",Parms_->agshock[i]+1);
    printf("\nepsilon: ");
    for(int i=0;i<NSTATES_ID;i++)
        printf("%lf ",Parms_->epsilon[i]);
    printf("\nepsilon2: ");
    for(int i=0;i<NSTATES_ID;i++)
        printf("%lf ",Parms_->epsilon2[i]);
    printf("\nig: \n");
    for(int i=0;i<T;i++)
    {
        for(int j=0;j<N_AGENTS;j++)
        {
            printf("%d ",Parms_->idshock[IX(i,j)]+1);
        }
        printf("\n");
    }
    printf("\nk: \n");
    for(int i=0;i<NKGRID;i++)
        printf("%10.8lf ",Parms_->k[i]);
    printf("\nkcross: \n");
    for(int i=0;i<N_AGENTS;i++)
        printf("%10.8lf ",Parms_->kcross[i]);
    printf("\nkm: \n");
    for(int i=0;i<NKM_GRID;i++)
        printf("%10.8lf ",Parms_->km[i]);
    printf("\nprob: \n");
    for(int ia=0;ia<NSTATES_AG;ia++)
        for(int iid=0;iid<NSTATES_ID;iid++)
        {
            for(int iap=0;iap<NSTATES_AG;iap++)
                for(int iidp=0;iidp<NSTATES_ID;iidp++)
                    printf("%18.15lf ",Parms_->P[IXP(ia,iid,iap,iidp)]);
            printf("\n");
        }
    
    
    printf("\nParameters\n");
    printf("\nalpha %lf ",Parms_->alpha);
    printf("\nbeta %lf ",Parms_->beta);
    printf("\ngamma %lf ",Parms_->gamma);
    printf("\ndelta %lf ",Parms_->delta);
    printf("\ndelta_a %lf ",Parms_->delta_a);
    printf("\nmu %lf ",Parms_->mu);
    printf("\nl_bar %lf ",Parms_->l_bar);
    printf("\nepsilon_u %lf ",Parms_->epsilon_u);
    printf("\nepsilon_e %lf ",Parms_->epsilon_e);
    printf("\nur0 %lf ",Parms_->ur[0]);
    printf("\ner0 %lf ",Parms_->er[0]);
    printf("\nur1 %lf ",Parms_->ur[1]);
    printf("\ner1 %lf ",Parms_->er[1]);
    printf("\nkss %lf ",Parms_->kss);
    for(int i=0;i<T;i++)
        printf("%d ",Parms_->agshock[i]);
    printf("\nCoefficients\n");
    for(int i=0;i<REGRESSORS*NSTATES_AG;i++)
        printf("%lf ",Parms_->coeff[i]);
    printf("\nCriter_K %10.8lf ",TOLL_K);
    printf("\nCriter_B %10.8lf ",TOLL_COEFF);
}
/**
 * x maybe stream; p and n pass as values,
 * return a packed int64
 * avoid xmin, xmax being put in DRAM
 */
void findrange (REAL *p,REAL *x,int *xmin,int *xmax,int *n){
    for(int i=2;i<=*n-1;i++)
        if (*p<x[i-1])
        {
            *xmin = i - 2;
            *xmax = i - 1;
            return;
        }
    
    *xmin = *n - 2;
    *xmax = *n - 1;
    return;
}
void linear_interp4D(Interp4_type *M_){
    
    // Find Extremes
    int i1min,i1max,i2min,i2max,i3min,i3max,i4min,i4max;
    findrange(&M_->p[0],&(M_->x1[0]),&i1min,&i1max,&M_->d1);
    findrange(&M_->p[1],&(M_->x2[0]),&i2min,&i2max,&M_->d2);
    findrange(&M_->p[2],&(M_->x3[0]),&i3min,&i3max,&M_->d3);
    findrange(&M_->p[3],&(M_->x4[0]),&i4min,&i4max,&M_->d4);
    
    // Declarations
    REAL
    f0000,
    f0001,
    f0010,
    f0011,
    f0100,
    f0101,
    f0110,
    f0111,
    f1000,
    f1001,
    f1010,
    f1011,
    f1100,
    f1101,
    f1110,
    f1111;
    
    f0000 = M_->f[IXV(i1min,i2min,i3min,i4min)];
    f0001 = M_->f[IXV(i1min,i2min,i3min,i4max)];
    f0010 = M_->f[IXV(i1min,i2min,i3max,i4min)];
    f0011 = M_->f[IXV(i1min,i2min,i3max,i4max)];
    f0100 = M_->f[IXV(i1min,i2max,i3min,i4min)];
    f0101 = M_->f[IXV(i1min,i2max,i3min,i4max)];
    f0110 = M_->f[IXV(i1min,i2max,i3max,i4min)];
    f0111 = M_->f[IXV(i1min,i2max,i3max,i4max)];
    f1000 = M_->f[IXV(i1max,i2min,i3min,i4min)];
    f1001 = M_->f[IXV(i1max,i2min,i3min,i4max)];
    f1010 = M_->f[IXV(i1max,i2min,i3max,i4min)];
    f1011 = M_->f[IXV(i1max,i2min,i3max,i4max)];
    f1100 = M_->f[IXV(i1max,i2max,i3min,i4min)];
    f1101 = M_->f[IXV(i1max,i2max,i3min,i4max)];
    f1110 = M_->f[IXV(i1max,i2max,i3max,i4min)];
    f1111 = M_->f[IXV(i1max,i2max,i3max,i4max)];
    
    // Weights
    REAL tx = (M_->p[0] - M_->x1[i1min])/(M_->x1[i1max]-M_->x1[i1min]);
    REAL ty = (M_->p[1] - M_->x2[i2min])/(M_->x2[i2max]-M_->x2[i2min]);
    REAL tz = (M_->p[2] - M_->x3[i3min])/(M_->x3[i3max]-M_->x3[i3min]);
    REAL tw = (M_->p[3] - M_->x4[i4min])/(M_->x4[i4max]-M_->x4[i4min]);
    
    // Interpolated Value
    *(M_->fp) =
    f0000 * (1-tx) * (1-ty) * (1-tz) * (1-tw) +
    f0001 * (1-tx) * (1-ty) * (1-tz) *    tw  +
    f0010 * (1-tx) * (1-ty) *    tz  * (1-tw) +
    f0011 * (1-tx) * (1-ty) *    tz  *    tw  +
    f0100 * (1-tx) *    ty  * (1-tz) * (1-tw) +
    f0101 * (1-tx) *    ty  * (1-tz) *    tw  +
    f0110 * (1-tx) *    ty  *    tz  * (1-tw) +
    f0111 * (1-tx) *    ty  *    tz  *    tw  +
    f1000 *    tx  * (1-ty) * (1-tz) * (1-tw) +
    f1001 *    tx  * (1-ty) * (1-tz) *    tw  +
    f1010 *    tx  * (1-ty) *    tz  * (1-tw) +
    f1011 *    tx  * (1-ty) *    tz  *    tw  +
    f1100 *    tx  *    ty  * (1-tz) * (1-tw) +
    f1101 *    tx  *    ty  * (1-tz) *    tw  +
    f1110 *    tx  *    ty  *    tz  * (1-tw) +
    f1111 *    tx  *    ty  *    tz  *    tw;
}
void linear_interp2D(Interp2_type *M_){
    
    // Find Extremes
    int i1min,i1max,i2min,i2max;

    findrange(&M_->p[0],&(M_->x1[0]),&i1min,&i1max,&M_->d1);
    findrange(&M_->p[1],&(M_->x2[0]),&i2min,&i2max,&M_->d2);
    
    // Declarations
    REAL
    f00,
    f01,
    f10,
    f11;
    //#define IXID(i,k) (i*NKGRID+k)
    f00 = M_->f[IXID(i1min,i2min)];
    f01 = M_->f[IXID(i1min,i2max)];
    f10 = M_->f[IXID(i1max,i2min)];
    f11 = M_->f[IXID(i1max,i2max)];
    
    // Weights
    REAL tz = (M_->p[0] - M_->x1[i1min])/(M_->x1[i1max]-M_->x1[i1min]);
    REAL tw = (M_->p[1] - M_->x2[i2min])/(M_->x2[i2max]-M_->x2[i2min]);
    
    // Interpolated Value
    *(M_->fp) =
    f00 * (1-tz) * (1-tw) +
    f01 * (1-tz) *    tw  +
    f10 *    tz  * (1-tw) +
    f11 *    tz  *    tw;
    
}
void linear_interp2Daux(Interp2_type *M_){
    
    // Find Extremes
    int i1min,i1max,i2min,i2max;
    findrange(&M_->p[0],&(M_->x1[0]),&i1min,&i1max,&M_->d1);
    findrange(&M_->p[1],&(M_->x2[0]),&i2min,&i2max,&M_->d2);
    
    // Declarations
    REAL
    f00,
    f01,
    f10,
    f11;
    //#define IXID(i,k) (i*NKGRID+k)
    // Interpolating only with respect to first two dimensions.
    f00 = M_->f[IXV(M_->i1,i1min,M_->i3,i2min)];
    f01 = M_->f[IXV(M_->i1,i1min,M_->i3,i2max)];
    f10 = M_->f[IXV(M_->i1,i1max,M_->i3,i2min)];
    f11 = M_->f[IXV(M_->i1,i1max,M_->i3,i2max)];
    
    // Weights
    REAL tz = (M_->p[0] - M_->x1[i1min])/(M_->x1[i1max]-M_->x1[i1min]);
    REAL tw = (M_->p[1] - M_->x2[i2min])/(M_->x2[i2max]-M_->x2[i2min]);
    
    // Interpolated Value
    *(M_->fp) =
    f00 * (1-tz) * (1-tw) +
    f01 * (1-tz) *    tw  +
    f10 *    tz  * (1-tw) +
    f11 *    tz  *    tw;
    
}
void linear_interp1D(Interp1_type *M_){
    
    // Find Extremes
    int i1min,i1max;
    findrange(&M_->p[0],&(M_->x1[0]),&i1min,&i1max,&M_->d1);
    
    // Declarations
    REAL
    f0,
    f1;
    //#define IXID(i,k) (i*NKGRID+k)
    // Interpolating only with respect to first two dimensions.
    f0 = M_->f[IXV(M_->i1,i1min,M_->i3,M_->i4)];
    f1 = M_->f[IXV(M_->i1,i1max,M_->i3,M_->i4)];
    
    // Weights
    REAL tz = (M_->p[0] - M_->x1[i1min])/(M_->x1[i1max]-M_->x1[i1min]);
    
    // Interpolated Value
    *(M_->fp) = f0 * (1-tz)  + f1 *    tz;
}
void linear_interpolation(REAL *xmin,REAL *xmax,REAL *x,REAL *fxmin,REAL *fxmax,REAL *fx){
    if(*xmin==*xmax)
    {
        *fx = *fxmin;
    }
    else
    {
        REAL w = (*x-*xmin)/(*xmax-*xmin);
        *fx = (1-w)*(*fxmin) + w*(*fxmax);
    }
}
void linear_space(const REAL *x_min,const REAL*x_max,const int* ngrid,REAL *grid){   int i;
    REAL increment;
    
    increment=(REAL) ((*x_max)-(*x_min))/((*ngrid)-1);
    
    for(i=0;i<(*ngrid);i++) grid[i]=(*x_min)+i*increment;
}
void InitParms(Parameters_type * Parms_){
    Parms_->beta=0.99;       // discount factor
    Parms_->gamma=1;         // utility-function parameter
    Parms_->alpha=0.36;      // share of capital in the production function
    Parms_->delta=0.025;     // depreciation rate
    Parms_->delta_a=0.01;    // (1-delta_a) is the productivity level in a bad state,
    // and (1+delta_a) is the productivity level in a good state
    Parms_->mu = 0.15;       // unemployment benefits as a share of wage
    Parms_->l_bar=1/0.9;     // time endowment; normalizes labor supply to 1 in a bad state
    
    Parms_->epsilon_u=0;     // idiosyncratic shock if the agent is unemployed
    Parms_->epsilon_e=1;     // idiosyncratic shock if the agent is employed
    
    Parms_->ur[0]=0.1;        // unemployment rate in a bad aggregate state
    Parms_->er[0]=(1-Parms_->ur[0]);   // employment rate in a bad aggregate state
    Parms_->ur[1]=0.04;        // unemployment rate in a good aggregate state
    Parms_->er[1]=(1-Parms_->ur[1]);   // employment rate in a good aggregate state
    
    Parms_->kss=pow((1./Parms_->beta-(1.-Parms_->delta))/Parms_->alpha,1./(Parms_->alpha-1)); // steady-state capital in a
    Parms_->kss_init_sum = N_AGENTS * (Parms_->kss);
    printf("- Initialization of Parameters: Check\n");
    
}
void InitTransition(Parameters_type * Parms_){
    // Transition Matrix (set as in  Den Haan, Judd, Juillard (2008))
    Parms_->P[IXP(0,0,0,0)] =0.525;
    Parms_->P[IXP(0,0,0,1)] =0.35;
    Parms_->P[IXP(0,0,1,0)] =0.03125;
    Parms_->P[IXP(0,0,1,1)] =0.09375;
    Parms_->P[IXP(0,1,0,0)] =0.038889;
    Parms_->P[IXP(0,1,0,1)] =0.836111;
    Parms_->P[IXP(0,1,1,0)] =0.002083;
    Parms_->P[IXP(0,1,1,1)] =0.122917;
    Parms_->P[IXP(1,0,0,0)] =0.09375;
    Parms_->P[IXP(1,0,0,1)] =0.03125;
    Parms_->P[IXP(1,0,1,0)] =0.291667;
    Parms_->P[IXP(1,0,1,1)] =0.583333;
    Parms_->P[IXP(1,1,0,0)] =0.009115;
    Parms_->P[IXP(1,1,0,1)] =0.115885;
    Parms_->P[IXP(1,1,1,0)] =0.024306;
    Parms_->P[IXP(1,1,1,1)] =0.850694;
    
#ifdef PRINTIT
    // Check prob sum
    for(int ia=0;ia<NSTATES_AG;ia++)
        for(int iid=0;iid<NSTATES_ID;iid++)
        {
            printf("\nSum[%d,%d] =  ",ia,iid);
            REAL sum=0.;
            for(int iap=0;iap<NSTATES_AG;iap++){
                for(int iidp=0;iidp<NSTATES_ID;iidp++)
                {
                    printf("%lf ",Parms_->P[IXP(ia,iid,iap,iidp)]);
                    sum+=Parms_->P[IXP(ia,iid,iap,iidp)];
                }
            }
            printf(" = %lf\n ",sum);
            
        }
    
#endif
    /*
     Same thing as:
    int i=0;
    Parms_->P[i] = 0.525;i++;
    Parms_->P[i] = 0.35;i++;
    Parms_->P[i] = 0.03125;i++;
    Parms_->P[i] = 0.09375;i++;
    Parms_->P[i] = 0.038889;i++;
    Parms_->P[i] = 0.836111;i++;
    Parms_->P[i] = 0.002083;i++;
    Parms_->P[i] = 0.122917;i++;
    Parms_->P[i] = 0.09375;i++;
    Parms_->P[i] = 0.03125;i++;
    Parms_->P[i] = 0.291667;i++;
    Parms_->P[i] = 0.583333;i++;
    Parms_->P[i] = 0.009115;i++;
    Parms_->P[i] = 0.115885;i++;
    Parms_->P[i] = 0.024306;i++;
    Parms_->P[i] = 0.850694;
     */
    printf("- Initialization of Transition Matrix: Check\n");
}
void InitIdAg(Parameters_type *Parms_){
    
    char filename[150] ={'\0'};
    int i,value;
    FILE *fp;
    
    // --------------------------------------------------------------------------------------------
    // Idiosyncratic Shocks
    strcpy(filename,SHOCKPATH);
    strcat(filename,"idshock.txt");
    
    fp = fopen(filename, "r");
    if(fp == NULL)
    {
        fprintf(stderr,"Shock filename: %s",filename);
        fprintf(stderr,"\nCannot be read");
        exit(EXIT_FAILURE);
    }
    else
    {
        fprintf(stderr,"\nOpen: %s",filename);
    }
    
    i=0;
    while(fscanf(fp,"%d",&value)== 1)
    {
        i = i+1;
    }
    
    printf("\n1.2. Check number of elements read from file is consistent:");
    if (i!= N_AGENTS*T)
    {
        printf(" ERROR: %d != %d\n",i, N_AGENTS*T);
        exit(EXIT_FAILURE);
    }
    else{
        
        printf(" DONE\n");
        
        // Rewind the fp. Sets the file position to the beginning of the file of the given stream.
        rewind(fp);
        
        
        int i=0;
        while(fscanf(fp,"%d",&value)== 1)
        {
            int t = i/N_AGENTS;
            int j = i%N_AGENTS;

#ifdef PRINTIT
           // printf("%d %d %d %d\n",i,t,j,value);
#endif
            Parms_->idshock[t*N_AGENTS+j] = value-1;
            i++;
        }
        
    }
#ifdef PRINTIT
    for(int t=T-1;t<T;t++)
    for(int j=0;j<N_AGENTS;j++)
        printf("%d ",Parms_->idshock[t*N_AGENTS+j]);
#endif
    fclose(fp);
    //*/
    
    // --------------------------------------------------------------------------------------------
    // Aggregate Shocks
    strcpy(filename,SHOCKPATH);
    strcat(filename,"agshock.txt");
    
    fp = fopen(filename, "r");
    if(fp == NULL)
    {
        fprintf(stderr,"Shock filename: %s",filename);
        fprintf(stderr,"\nCannot be read");
        exit(EXIT_FAILURE);
    }
    else
    {
        fprintf(stderr,"\nOpen: %s",filename);
    }
    
    i=0;
    while(fscanf(fp,"%d",&value)== 1)
    {
        Parms_->agshock[i] = value-1;
        i = i+1;
    }
    
    printf("\n1.2. Check number of elements read from file is consistent:");
    if (i!=T)
    {
        printf(" ERROR: %d != %d\n",i,T);
        exit(EXIT_FAILURE);
    }
    else{
        
        printf(" DONE\n");
    }
#ifdef PRINTIT
    for(int t=0;t<T;t++)
        printf("%d ",Parms_->agshock[t]);
#endif
     fclose(fp);
    
    printf("- Initialized Idiosyncratic and Aggregate Shocks: Check\n");
    
}
void Init_parmshocks(Parameters_type *Parms_){
    
    Parms_->epsilon[0] =Parms_->epsilon_u;
    Parms_->epsilon[1] =Parms_->epsilon_e;
    
    Parms_->epsilon2[0] = 0;
    Parms_->epsilon2[1] = 1;
    
    Parms_->ag[0] = 1-Parms_->delta_a;
    Parms_->ag[1] = 1+Parms_->delta_a;
    
    Parms_->ag2[0] = 0;
    Parms_->ag2[1] = 1;
    
    printf("- Initialized Parameters Idiosyncratic and Aggregate Shocks: Check\n");
    
}
void InitPrealloc(Parameters_type *Parms_){
    
    for(int ia=0;ia<NSTATES_AG;ia++)
    {
        Parms_->trate[ia] = Parms_->mu * (1.-Parms_->er[ia]) / (Parms_->l_bar*Parms_->er[ia]);
        REAL taxrate = Parms_->trate[ia];
        
        for(int ikm=0;ikm<NKM_GRID;ikm++)
        {
            // Interest Rate
            Parms_->irate[IXAKM(ia,ikm)] = Parms_->alpha*(Parms_->ag[ia]*pow(Parms_->km[ikm]/Parms_->er[ia]/Parms_->l_bar,Parms_->alpha-1));
            // Wage
            Parms_->wage[IXAKM(ia,ikm)]  =(1.-Parms_->alpha)*(Parms_->ag[ia]*pow(Parms_->km[ikm]/Parms_->er[ia]/Parms_->l_bar,Parms_->alpha));
            
            REAL irate = Parms_->irate[IXAKM(ia,ikm)];
            REAL wage  = Parms_->wage[IXAKM(ia,ikm)];
            
            for(int iid=0;iid<NSTATES_ID;iid++)
                for(int ik=0;ik<NKGRID;ik++)
                {
                    Parms_->wealth[IXV(ia, ikm, iid, ik)] = (irate + 1.-Parms_->delta) * Parms_->k[ik] +
                    wage * ( (1.-taxrate)* Parms_->epsilon[iid]*Parms_->l_bar+
                            (1.-Parms_->epsilon[iid]) * Parms_->mu
                           );
                    Parms_->V[IXV(ia, ikm, iid, ik)]=Parms_->wealth[IXV(ia, ikm, iid, ik)];
                    Parms_->kprime[IXV(ia, ikm, iid, ik)]=0.9*Parms_->k[ik];
                }
        }
    }
    
    // Interpolation Matrix for Agents Problem
    Parms_->M4_.x1 = Parms_->ag2;
    Parms_->M4_.d1 = NSTATES_AG;
    Parms_->M4_.x2 = Parms_->km;
    Parms_->M4_.d2 = NKM_GRID;
    Parms_->M4_.x3 = Parms_->epsilon2;
    Parms_->M4_.d3 = NSTATES_ID;
    Parms_->M4_.x4 = Parms_->k;
    Parms_->M4_.d4 = NKGRID;
    Parms_->M4_.f  = Parms_->kprime;
    
    Parms_->M2_.x1 = Parms_->epsilon2;
    Parms_->M2_.d1 = NSTATES_ID;
    Parms_->M2_.x2 = Parms_->k;
    Parms_->M2_.d2 = NKGRID;
    Parms_->M2_.f  = Parms_->kprime_interp;
    
    Parms_->I4_.x1 = Parms_->ag2;
    Parms_->I4_.d1 = NSTATES_AG;
    Parms_->I4_.x2 = Parms_->km;
    Parms_->I4_.d2 = NKM_GRID;
    Parms_->I4_.x3 = Parms_->epsilon2;
    Parms_->I4_.d3 = NSTATES_ID;
    Parms_->I4_.x4 = Parms_->k;
    Parms_->I4_.d4 = NKGRID;
    Parms_->I4_.f  = Parms_->V;
    
    Parms_->I2_.x1 = Parms_->km;
    Parms_->I2_.d1 = NKM_GRID;
    Parms_->I2_.x2 = Parms_->k;
    Parms_->I2_.d2 = NKGRID;
    Parms_->I2_.f  = Parms_->kprime;
    
    Parms_->I1_.x1 = Parms_->km;
    Parms_->I1_.d1 = NKM_GRID;
    Parms_->I1_.f  = Parms_->V;
#ifdef PRINTCHECK
    printf("\nWealth\n");
    for(int ia=0;ia<NSTATES_AG;ia++)
        for(int iid=0;iid<NSTATES_ID;iid++)
        {
            printf("(:,:,%d,%d)\n",ia+1,iid+1);
            for(int ik=0;ik<NKGRID;ik++)
            {
                for(int ikm=0;ikm<NKM_GRID;ikm++)
                    printf("%19.15lf ",Parms_->wealth[IXV(ia,ikm,iid,ik)]);
                printf("\n");
            }
            printf("\n");
            
        }
#endif
}
void InitK(Parameters_type *Parms_){
    
    REAL *x = (REAL *) malloc(NKGRID*sizeof(REAL));
    REAL *y = (REAL *) malloc(NKGRID*sizeof(REAL));
    
    REAL xmin = 0;
    REAL xmax = 0.5;
    
    linear_space(&xmin,&xmax,&NKGRID,x); // generate a grid of ngridk points on [0,0.5]
    
    // Polynomial distribution of grid points
    for(int i=0;i<NKGRID;i++)
    {
        y[i] = pow(x[i],7.)/pow(x[NKGRID-1],7.);
        Parms_->k[i] = KMIN+(KMAX-KMIN)*y[i];   // transformation of grid points from [0,0.5]
#ifdef PRINTIT
        printf("x %lf, y %lf k %lf\n",x[i],y[i],Parms_->k[i]);
#endif
    }
    
    // Grid for mean of capital
    linear_space(&KM_MIN,&KM_MAX,&NKM_GRID,Parms_->km); // generate a grid of ngridk points on [0,0.5]
    // Grid kprime: next-period individual capital (k')
    for(int ja=0;ja<NSTATES_AG;ja++)
        for(int jkm=0;jkm<NKM_GRID;jkm++)
            for(int jid=0;jid<NSTATES_ID;jid++)
                for(int jk=0;jk<NKGRID;jk++)
                {
                    //int indx = NKM_GRID*NSTATES_ID*NKGRID*ja+NSTATES_ID*NKGRID*jkm+NKGRID*jid+jk;
                    //int indx = IXV(ja,jkm,jid,jk);
                    
#ifdef PRINTIT
                    printf("%d: KM: %d, AG: %d, ID: %d, k: %d\n",IXV(ja,jkm,jid,jk),jkm,ja,jid,jk);
#endif
                    Parms_->kprime[IXV(ja,jkm,jid,jk)] = NKPRIME_SCALE*Parms_->k[jk];
                }
    
    for(int j=0;j<N_AGENTS;j++)
        Parms_->kcross[j] = Parms_->kss;
    free(x);
    free(y);
    
    printf("- Initalized of capital grids: Check.\n");
    
}
void AggregatesLawMotion(Parameters_type *Parms_){
#ifdef PRINTIT
    printf("\n");
#endif
    for(int ia = 0;ia<NSTATES_AG;ia++)
    {
        int const_coef = ia*REGRESSORS;
        int km_coef    = ia*REGRESSORS+1;
        
        for(int ikm = 0;ikm<NKM_GRID;ikm++)
        {
            Parms_->kmprime[IXAKM(ia,ikm)] =exp(Parms_->coeff[const_coef]+ Parms_->coeff[km_coef]*log(Parms_->km[ikm]));
            if(Parms_->kmprime[IXAKM(ia,ikm)]>KM_MAX)
                Parms_->kmprime[IXAKM(ia,ikm)]=KM_MAX;
            if(Parms_->kmprime[IXAKM(ia,ikm)]<KM_MIN)
                Parms_->kmprime[IXAKM(ia,ikm)]=KM_MIN;
#ifdef PRINTIT
            printf("Coefficients[%d,%d]=%lf ",ia,ikm,Parms_->kmprime[IXAKM(ia,ikm)]);
#endif
        }
#ifdef PRINTIT
        printf("\n");
#endif
    }
#ifdef PRINTIT
    printf("- ALM: Check.\n");
#endif
}
void Agents(Parameters_type *Parms_){
    
    int iter_dV=0;
    REAL metric=1;
    while(metric>TOLL_K)
    {
#ifdef PRINTIT
        printf("\n");
#endif
        // Expected Value
        for(int ia=0;ia<NSTATES_AG;ia++)
            for(int ikm=0;ikm<NKM_GRID;ikm++)
            {
                REAL kmprime = Parms_->kmprime[IXAKM(ia,ikm)]; // Interpolation Value
                for(int iid=0;iid<NSTATES_ID;iid++)
                    for(int ikp=0;ikp<NKGRID;ikp++)
                    {
                        Parms_->EV[IXV(ia,ikm,iid,ikp)] = 0;
                        //REAL sum =0.;
                        for(int iap=0;iap<NSTATES_AG;iap++)
                            for(int iidp=0;iidp<NSTATES_ID;iidp++)
                            {
                                Parms_->I1_.i1 = iap;
                                //Parms_->I1_.i2 = ikm;//not used
                                Parms_->I1_.i3 = iidp;
                                Parms_->I1_.i4 = ikp;
                                REAL value;
                                Parms_->I1_.fp = &value;
                                Parms_->I1_.p[0] = kmprime;
                                linear_interp1D(&Parms_->I1_);
                                //printf("Interpolated V: %lf ia %d  ikm %d iid  %d ikp %d\n ",value,ia,ikm,iid,ikp);
                                Parms_->EV[IXV(ia,ikm,iid,ikp)] += Parms_->P[IXP(ia,iid,iap,iidp)] * value;
                                //sum+=Parms_->P[IXP(ia,iid,iap,iidp)];
                            }
                        //printf("\nProb: %lf ia %d  ikm %d iid  %d ikp %d\n ",sum,ia,ikm,iid,ikp);
                        
                    }
            }

        REAL spread_scalar = -99999999999999;
        for(int is=0;is<NSTATES;is++){
            
            int residual;
            int ia  = is/(NKM_GRID*NSTATES_ID*NKGRID);
            residual = is%(NKM_GRID*NSTATES_ID*NKGRID);
            int ikm = residual/(NSTATES_ID*NKGRID);
            residual = residual%(NSTATES_ID*NKGRID);
            int iid = residual/NKGRID;
            //int ik  = residual%NKGRID;
            
            REAL wealth = Parms_->wealth[is];
            
            // Maximization Loop
            REAL V_scalar = -99999999999999;
            int ikpstar = -1;
            for(int ikp=0;ikp<NKGRID;ikp++)
            {
                
                REAL consumption = wealth - Parms_->k[ikp];
                if(consumption<0)
                    consumption=1*pow(10.,-10.);
                REAL ret_func;
if(Parms_->gamma != 1)
                ret_func = 1./(1.-Parms_->gamma)*(pow(consumption,1-Parms_->gamma)-1.);
else
                ret_func = log(consumption);
                
                REAL V_actual = ret_func + Parms_->beta  * Parms_->EV[IXV(ia,ikm,iid,ikp)];
                
                if (V_actual > V_scalar)
                {
                    V_scalar = V_actual;
                    ikpstar = ikp;
                }
            }
            
            Parms_->Vimp[is] = V_scalar;
            // -----------------------------------------------------------------------------------------------------
            // CHECK CONVERGENCE
            //Set the Spread
            REAL spread = fabs(Parms_->V[is]-Parms_->Vimp[is]);
            
            if (spread>spread_scalar)
                spread_scalar = spread;
            
            //Update the Guess
            Parms_->V[is] = Parms_->Vimp[is];
            Parms_->G[is] = ikpstar;
            Parms_->kprimen[is] = Parms_->k[ikpstar];
            Parms_->kprime[is] = UPDATE_K*Parms_->kprimen[is] + (1-UPDATE_K)*Parms_->kprime[is];
        } // is
#ifdef PRINTCHECK
        printf("\nV\n");
        for(int ia=0;ia<NSTATES_AG;ia++)
            for(int iid=0;iid<NSTATES_ID;iid++)
            {
                printf("(:,:,%d,%d)\n",ia+1,iid+1);
                for(int ik=0;ik<NKGRID;ik++)
                {
                    for(int ikm=0;ikm<NKM_GRID;ikm++)
                        printf("%19.15lf ",Parms_->V[IXV(ia,ikm,iid,ik)]);
                    printf("\n");
                }
                printf("\n");
                
            }
#endif
        // Update the Metric
        metric = spread_scalar;
        
        iter_dV++;
#ifdef SHOWMETRIC_DV
        printf("Iteration dV: %d metric %lf\n",iter_dV,metric);
#endif
    }
}
void AgentsEGM(Parameters_type *Parms_){

    // BABA
    int iter_dK=0;
    REAL metric=1;
    // ~Outer Loop
    // precision is bounded by tolerance; introducing noise may take more iterations
    int num_iter_egm = 0;
    int count = 0;
    while(metric>TOLL_K)
    
    // for (int i = 0; i < num_iter_egm; i++)
    {
        count++;
        if (count > num_iter_egm & num_iter_egm != 0) {
            break;
        }
#ifdef PRINTIT
        printf("\n");
#endif

        REAL spread_scalar = -99999999999999;

        // ~Loop A
        for(int ia=0;ia<NSTATES_AG;ia++)
            // ~Loop B
            for(int ikm=0;ikm<NKM_GRID;ikm++)
            {
                REAL kmprime = Parms_->kmprime[IXAKM(ia,ikm)]; // Interpolation Value
                //printf("\nkmprime[%d,%d]= %18.15lf\n",ia,ikm,Parms_->kmprime[IXAKM(ia,ikm)]);
                // ~Loop C
                for(int iid=0;iid<NSTATES_ID;iid++)
                    // ~Loop D
                    for(int ik=0;ik<NKGRID;ik++)
                    { // @@in any order
                        //printf("\nkprime[%d]= %18.15lf\n",ik,Parms_->kprime[IXV(ia,ikm,iid,ik)]);
                        REAL kprime = Parms_->kprime[IXV(ia,ikm,iid,ik)];
                        REAL EMUs = 0.;
                        // Future
                        // ~Loop E
                        for(int iap=0;iap<NSTATES_AG;iap++)
                        {
                            REAL taxrate = Parms_->trate[iap];
                            REAL irate   = Parms_->alpha*(Parms_->ag[iap]*pow(kmprime/Parms_->er[iap]/Parms_->l_bar,Parms_->alpha-1.));
                            REAL IMRT     = (1.-Parms_->delta)+irate;
                            REAL wage    = (1.-Parms_->alpha)*(Parms_->ag[iap]*pow(kmprime/Parms_->er[iap]/Parms_->l_bar,Parms_->alpha));
                            
                            // ~Loop F
                            #if PERF_SPECIAL
                            perf_ctr_start(&pctr_f);
                            #endif
                            for(int iidp=0;iidp<NSTATES_ID;iidp++)
                            {
                                REAL kprime2_interpolated;
                                Parms_->I2_.i1   = iap;
                                Parms_->I2_.i3   = iidp;
                                Parms_->I2_.fp   = &kprime2_interpolated;
                                Parms_->I2_.p[0] = kmprime;
                                Parms_->I2_.p[1] = kprime;
                                linear_interp2Daux(&Parms_->I2_);
                                
                                REAL wealth2 = (irate + 1.-Parms_->delta) * kprime +
                                                wage * ( (1.-taxrate)* Parms_->epsilon[iidp]*Parms_->l_bar+
                                                         (1.-Parms_->epsilon[iidp]) * Parms_->mu
                                                        );
                                
                                REAL cons2 = wealth2 - kprime2_interpolated;
                                // consumption restricted to be positive @@
                                if(cons2<0)
                                    cons2=1*pow(10.,-10.);
                                // range of gamma @@ between 1 and 5
                                REAL MU2   = pow(cons2,-Parms_->gamma);
                                
                                // weighted sum
                                EMUs += Parms_->P[IXP(ia,iid,iap,iidp)] * IMRT * MU2;
                            } //iidp
                            #if PERF_SPECIAL
                            perf_ctr_update(&pctr_f);
                            #endif
                            
                        } // iap
                        
                        // Today
                        REAL consn = pow(Parms_->beta * EMUs,-1/Parms_->gamma);
                        Parms_->kprimen[IXV(ia,ikm,iid,ik)] = Parms_->wealth[IXV(ia,ikm,iid,ik)]-consn;
                        if(Parms_->kprimen[IXV(ia,ikm,iid,ik)]>KMAX)
                            Parms_->kprimen[IXV(ia,ikm,iid,ik)]=KMAX;
                        if(Parms_->kprimen[IXV(ia,ikm,iid,ik)]<KMIN)
                            Parms_->kprimen[IXV(ia,ikm,iid,ik)]=KMIN;
                        // -----------------------------------------------------------------------------------------------------
                        // CHECK CONVERGENCE
                        //Set the Spread
                        REAL spread = fabs(Parms_->kprimen[IXV(ia,ikm,iid,ik)]-Parms_->kprime[IXV(ia,ikm,iid,ik)]);
                        
                        if (spread>spread_scalar)
                            spread_scalar = spread;
                    }
                        
            } // ikm,ia

        
        // Updating the Guess (cannot be done inside because kprime needed for interpolation)
        // ~Loop G
        #if PERF_SPECIAL
        perf_ctr_start(&pctr_g);
        #endif
        for(int is=0;is<NSTATES;is++){
            Parms_->cons[is] = Parms_->wealth[is]-Parms_->kprime[is];
            //Update the Guess
            Parms_->kprime[is] = UPDATE_K*Parms_->kprimen[is] + (1-UPDATE_K)*Parms_->kprime[is];
            //Parms_->kprime[IXV(ia,ikm,iid,ik)] = UPDATE_K*Parms_->kprimen[IXV(ia,ikm,iid,ik)] + (1-UPDATE_K)*Parms_->kprime[IXV(ia,ikm,iid,ik)];
        }
        #if PERF_SPECIAL
        perf_ctr_update(&pctr_g);
        #endif
        
        // Update the Metric
        metric = spread_scalar;
        
        iter_dK++;
#ifdef SHOWMETRIC_DV
        printf("Iteration dK: %d metric %lf\n",iter_dK,metric);
#endif
    }
#ifdef PRINTCHECK
    printf("\nkprime\n");
    for(int ia=0;ia<NSTATES_AG;ia++)
        for(int iid=0;iid<NSTATES_ID;iid++)
        {
            printf("(:,:,%d,%d)\n",ia+1,iid+1);
            for(int ik=0;ik<NKGRID;ik++)
            {
                for(int ikm=0;ikm<NKM_GRID;ikm++)
                    printf("%19.15lf ",Parms_->kprime[IXV(ia,ikm,iid,ik)]);
                printf("\n");
            }
            printf("\n");
            
        }
#endif
}
void Aggregate_ST(Parameters_type *Parms_){
#ifdef PRINTIT
    printkcross(Parms_);
#endif
    // FILE* tfile = fopen("checks/test.txt", "w");        
    REAL test = 0;
    for(int j=0;j<N_AGENTS;j++){
            test += Parms_->kcross[j];
    }

    printf("~~~~ Initial kcross sum: %18.15lf\n", test);

    // ~Loop A
    for(int t=0;t<T;t++){ //T
        // Cross-Sectional Average of Capital

        #if KCTEST_MEAN
        REAL mean;
        if (t == 0)
            mean = Parms_->kss_init_sum;
        #else
        REAL mean = 0;
        #endif

        // ~Loop B
        #if PERF_SPECIAL
        perf_ctr_start(&pctr_b);
        #endif

        //kcross_l is similar to kcross in MATLAB
        #if !KCTEST_MEAN
        for(int j=0;j<N_AGENTS;j++){
            // mean=mean+Parms_->kcross[j];
            mean=mean+Parms_->kcross_l[j];
        }//j
        #endif

        #if PERF_SPECIAL
        perf_ctr_update(&pctr_b);
        #endif

        Parms_->kmts[t] = mean/N_AGENTS;
        FILE* cfile = fopen("checks/mean.txt", "a");
        fprintf(cfile, "%lf\n", mean/N_AGENTS);
        fclose(cfile);
        if(Parms_->kmts[t]>KM_MAX)
            Parms_->kmts[t]=KM_MAX;
        if(Parms_->kmts[t]<KM_MIN)
            Parms_->kmts[t]=KM_MIN;

        #if KCTEST_MEAN
        mean = 0;
        #endif
        
        // ~Loop C
        #if PERF_SPECIAL
        perf_ctr_start(&pctr_c);
        #endif
        for(int ik=0;ik<NKGRID;ik++)
        {
            // agshock[t] kmts[t] findrange, get it out of the loop
            // hoist d3 and d4 outside of the timesteps loop
            // ~Loop C.2
            for(int iid=0;iid<NSTATES_ID;iid++)
            {
                REAL value;
                Parms_->M4_.fp = &value;
                //#define IXV(a,km,id,k) (NKM_GRID*NSTATES_ID*NKGRID*a+NSTATES_ID*NKGRID*km+NKGRID*id+k)
                Parms_->M4_.p[0] = Parms_->agshock[t];// 0 or 1
                Parms_->M4_.p[1] = Parms_->kmts[t];
                Parms_->M4_.p[2] = Parms_->epsilon2[iid]; // 0 or 1
                Parms_->M4_.p[3] =Parms_->k[ik];
                linear_interp4D(&Parms_->M4_);
                Parms_->kprime_interp[IXID(iid,ik)] = value;
                //kprimet4=interpn(k,km,a2,epsilon2,kprime,k, kmts(t),agshock(t),epsilon2,'linear');
            }
        }
        #if PERF_SPECIAL
        perf_ctr_update(&pctr_c);
        #endif

#ifdef PRINTIT
        printkprimeinterp(Parms_);
#endif
        // ~Loop D
        #if PERF_SPECIAL
        perf_ctr_start(&pctr_d);
        #endif
        for (int j=0;j<N_AGENTS;j++) {
            REAL value;
            REAL kc1val;
            Parms_->M2_.fp = &value;
            // 0 and 1 pipelines for idshock
            Parms_->M2_.p[0] = Parms_->idshock[IX(t,j)];
            Parms_->M2_.p[1] = Parms_->kcross_l[j];
            // Parms_->M2_.p[1] = Parms_->kcross[j];
            linear_interp2D(&Parms_->M2_);

            /* Params_->kcross_n is same as kcrossn in MATLAB */
            // old:
            #if !KCTEST
            Parms_->kcross_l[j]=value;
            //kcrossn=interpn(k,epsilon2,kprimet,kcross,idshock(t,:),'linear');
            if(Parms_->kcross_l[j]>KMAX)
                Parms_->kcross_l[j]=KMAX;
            if(Parms_->kcross_l[j]<KMIN)
                Parms_->kcross_l[j]=KMIN;
            #else
            // new: test no need for kcross1
            kc1val = value;
            if (kc1val > KMAX)
                kc1val = KMAX;
            if (kc1val < KMIN)
                kc1val = KMIN;
            Parms_->kcross[j] = kc1val;
            // fprintf(tfile, "%lf\n",  Parms_->kcross[j]);
            #if KCTEST_MEAN
            mean += kc1val;
            #endif
            #endif
        } // j
        #if PERF_SPECIAL
        perf_ctr_update(&pctr_d);
        #endif
        
        // ~Loop E: obsolete
        #if !KCTEST
        #if PERF_SPECIAL
        perf_ctr_start(&pctr_e);
        #endif
        #if PERF_SPECIAL
        perf_ctr_update(&pctr_e);
        #endif
        #endif
    }//t
    
#ifdef PRINTCHECK
    //printkcross(Parms_);
    for(int t=0;t<T;t++){
        printf("%18.15lf\n", Parms_->kmts[t]);
    }
#endif
    // fclose(tfile);
    
}

//@@ matrix inversion check Rosetta stones
void ALMCoefficients(Parameters_type *Parms_){
    
    // Time series for the ALM regression
    int ndimbad=0;    // count how many times the aggregate shock was bad
    int ndimgood=0;   // count how many times the aggregate shock was good

    double test = 0;
	for (int t = 0; t < T; ++t)
		test += Parms_->kmts[t];

	printf("~~~~ Initial alm_coeff kmts[t] sum: %18.15lf\n", test);

    for(int t=NDISCARD;t<T-1;t++)
        if(Parms_->agshock[t]==0)
            ndimbad=ndimbad+1;
        else
            ndimgood=ndimgood+1;
    
    double SSRgood,SSRbad;
    gsl_matrix *xgood = gsl_matrix_alloc (ndimgood,REGRESSORS);
    gsl_matrix *xbad  = gsl_matrix_alloc (ndimbad,REGRESSORS);
    gsl_vector *ygood = gsl_vector_alloc (ndimgood);
    gsl_vector *ybad  = gsl_vector_alloc (ndimbad);
    //gsl_vector *wgood = gsl_vector_alloc (ndimgood);
    //gsl_vector *wbad  = gsl_vector_alloc (ndimbad);
    gsl_vector *cgood = gsl_vector_alloc (REGRESSORS);
    gsl_vector *cbad = gsl_vector_alloc (REGRESSORS);
    gsl_matrix *covgood = gsl_matrix_alloc (REGRESSORS,REGRESSORS);
    gsl_matrix *covbad = gsl_matrix_alloc (REGRESSORS,REGRESSORS);
    
    //c = gsl_vector_alloc (REGRESSORS);
    //cov = gsl_matrix_alloc (REGRESSORS,REGRESSORS);
    int ibad = 0;
    int igood = 0;
    for(int t=NDISCARD;t<T-1;t++)
        if(Parms_->agshock[t]==0){
            
            gsl_vector_set (ybad,ibad,log(Parms_->kmts[t+1]));
            //gsl_vector_set (wbad,ibad,1.0);
            gsl_matrix_set (xbad,ibad,0,1.0);
            gsl_matrix_set (xbad,ibad,1,log(Parms_->kmts[t]));
            ibad++;
        }
        else{
            gsl_vector_set (ygood,igood,log(Parms_->kmts[t+1]));
            //gsl_vector_set (wgood,igood,1.0);
            gsl_matrix_set (xgood,igood,0,1.0);
            gsl_matrix_set (xgood,igood,1,log(Parms_->kmts[t]));
            igood++;
        }
    
    double TSSgood = gsl_stats_tss(ygood->data,ygood->stride,ygood->size);
    gsl_multifit_linear_workspace * workgood = gsl_multifit_linear_alloc (ndimgood, REGRESSORS);
    gsl_multifit_linear (xgood, ygood, cgood, covgood,&SSRgood, workgood);
    gsl_multifit_linear_free (workgood);
    
    double TSSbad = gsl_stats_tss(ybad->data,ybad->stride,ybad->size);
    gsl_multifit_linear_workspace * workbad = gsl_multifit_linear_alloc (ndimbad, REGRESSORS);
    gsl_multifit_linear (xbad, ybad, cbad, covbad,&SSRbad, workbad);
    gsl_multifit_linear_free (workbad);
    
    Parms_->coeff1[0]=gsl_vector_get(cbad,0);
    Parms_->coeff1[1]=gsl_vector_get(cbad,1);
    Parms_->coeff1[2]=gsl_vector_get(cgood,0);
    Parms_->coeff1[3]=gsl_vector_get(cgood,1);
    
    Parms_->R2[0] = 1- SSRbad / TSSbad;
    Parms_->R2[1] = 1- SSRgood/TSSgood;
    
    printf ("\n# Good State Best Fit: logK' = %18.15lf + %18.15lf logk,  R2 = %18.15lf",gsl_vector_get(cgood,0),gsl_vector_get(cgood,1),Parms_->R2[1]);
    printf ("\n# Bad  State Best Fit: logK' = %18.15lf + %18.15lf logK,  R2 = %18.15lf",gsl_vector_get(cbad,0),gsl_vector_get(cbad,1)  ,Parms_->R2[0]);
    
    FILE* cfile = fopen("checks/coeffs.txt", "a");
    fprintf(cfile, "%lf\n", gsl_vector_get(cbad,0));
    fprintf(cfile, "%lf\n", gsl_vector_get(cbad,1));
    fprintf(cfile, "%lf\n", gsl_vector_get(cgood,0));
    fprintf(cfile, "%lf\n", gsl_vector_get(cgood,1));
    fclose(cfile);

    for(int i=0;i<2;i++){
        FILE* cfile = fopen("checks/r2bg.txt", "a");
        fprintf(cfile, "%lf\n", Parms_->R2[i]);
        fclose(cfile);
    }

    gsl_matrix_free (xgood);
    gsl_matrix_free (xbad);
    gsl_vector_free (ygood);
    gsl_vector_free (ybad);
    //gsl_vector_free (wgood);
    //gsl_vector_free (wbad);
    gsl_vector_free (cgood);
    gsl_vector_free (cbad);
    gsl_matrix_free (covgood);
    gsl_matrix_free (covbad);
}
#endif /* auxiliary_h */
