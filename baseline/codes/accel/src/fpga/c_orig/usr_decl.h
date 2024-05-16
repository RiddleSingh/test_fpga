//
//  usr_decl.h
//  fpga-ks-c
//
//  Created by Alessandro Peri on 08/09/2019.
//  Copyright © 2019 Alessandro Peri. All rights reserved.
//

#ifndef usr_decl_h
#define usr_decl_h


#include <math.h>

#define __PETALINUX__

#ifdef __PETALINUX__
#define SHOCKPATH "shocks/"
#else
#define SHOCKPATH "../shocks/"
#endif

typedef double REAL;
typedef struct{
    // Domain Grid
    int d1;
    int d2;
    int d3;
    int d4;
    REAL * x1;
    REAL * x2;
    REAL * x3;
    REAL * x4;
    // Interpolation point in Domain Grid
    REAL *p;
    // Function
    REAL *f;
    // Interpolated Value
    REAL *fp;
} Interp4_type;
typedef struct{
    // Domain Grid
    int d1;
    int d2;
    REAL * x1;
    REAL * x2;
    // Interpolation point in Domain Grid
    REAL *p;
    // Function
    REAL *f;
    // Interpolated Value
    REAL *fp;
    int i1,i2,i3,i4;
} Interp2_type;
typedef struct{
    // Domain Grid
    int d1;
    REAL * x1;
    // Interpolation point in Domain Grid
    REAL *p;
    // Function
    REAL *f;
    // Interpolated Value
    REAL *fp;
    int i1;
    int i2;
    int i3;
    int i4;
} Interp1_type;

typedef struct{
    REAL beta;       // discount factor
    REAL gamma;      // utility-function parameter
    REAL alpha;      // share of capital in the production function
    REAL delta;      // depreciation rate
    REAL delta_a;    // (1-delta_a) is the productivity level in a bad state, and (1+delta_a) is the productivity level in a good state
    REAL mu;         // unemployment benefits as a share of wage
    REAL l_bar;      // time endowment; normalizes labor supply to 1 in a bad state
    
    REAL epsilon_u;  // idiosyncratic shock if the agent is unemployed
    REAL epsilon_e;  // idiosyncratic shock if the agent is employed
    
    REAL *ur;        // unemployment rate in aggregate state
    REAL *er;        // employment rate in aggregate state
    /*
    REAL ur_b;       // unemployment rate in a bad aggregate state
    REAL er_b;       // employment rate in a bad aggregate state
    REAL ur_g;       // unemployment rate in a good aggregate state
    REAL er_g;       // employment rate in a good aggregate state
    */
    
    REAL kss;
    
    REAL *P;
    int *idshock;
    int *agshock;
    
    REAL *k;
    REAL *km;
    REAL *kmprime;
    REAL *kprime;
    REAL *kprimen;
    REAL *cons;
    REAL *kcross;
    REAL *kcross_l;
    REAL *kprime_interp;
    
    REAL *wealth;
    REAL *wage;
    REAL *irate;
    REAL *trate;
    REAL *V;
    REAL *EV;
    REAL *Vimp;
    REAL *Vinterp;
    int *G;
    
    REAL *epsilon;
    REAL *epsilon2;
    REAL *ag;
    REAL *ag2;
    
    REAL *coeff;
    REAL *coeff1;
    REAL *R2;
    
    REAL *kmts;
    
    // Interpolation Matrixes
    Interp4_type I4_;
    Interp2_type I2_;
    Interp1_type I1_;
    Interp4_type M4_;
    Interp2_type M2_;
    
    // Performance
    REAL solTime;

    // Extensions
    REAL kss_init_sum;
    
} Parameters_type;

// STATES
const int NSTATES_ID = 2; // number of states for the idiosyncratic shock
const int NSTATES_AG = 2; // number of states for the aggregate shock
const int NKGRID=100;                // number of grid points
const int NKM_GRID = 4;            // number of grid points for the mean of capital distribution grid
const int NCOEFF = 4;      // Number of ceofficient estimates
const int NSTATES = NSTATES_ID*NSTATES_AG*NKGRID*NKM_GRID; //NSTATES_ID*NSTATES_AG*NKGRID*NKM_GRID

// Capital Grid
const REAL KMIN=0;                   // minimum grid-value of capital
const REAL KMAX=1000;                // maximum grid-value of capital
const REAL NKPRIME_SCALE = 0.9;   // scaling factor inititalization kprime cross-sectional distribution

// Mean Capital Distribution
const REAL KM_MIN = 30;             // minimum grid-value of the mean of capital distribution, km
const REAL KM_MAX = 50;             // maximum grid-value of the mean of capital distribution, km

const int SETSYM = 1; // 1 stochastic simulation; 2. non-stochastic simulation;
// Stochastic Simulation
const int N_AGENTS = 10000;   // N. agents in stochastic simulation
const int T=1100;             // Simulation length
const int NDISCARD=100;       // Periods to discard

// Non-Stochastic Simulation
const int J=1000;             // number of grid points for the non-stochastic simulation
const REAL Kmin_val=0.;      // minimum grid value for the non-stochastic simulation
const REAL Kmax_val=100.; // maximum grid value for the non-stochastic simulation

#define IXP(a,i,ap,ip) (NSTATES_ID*NSTATES_AG*NSTATES_ID * a + NSTATES_AG*NSTATES_ID*i + NSTATES_ID*ap + ip)
#define IXEV(a,i,j) (NSTATES_ID*NKGRID*a+NKGRID*i+j)
#define IXV(a,km,id,k) (NKM_GRID*NSTATES_ID*NKGRID*a+NSTATES_ID*NKGRID*km+NKGRID*id+k)
#define IX(t,j) (t*N_AGENTS+j)
#define IXID(i,k) (i*NKGRID+k)
#define IXAKM(a,km) (a*NKM_GRID+km)
#define IXAI(a,i) (a*NSTATES_ID+i)

// Convergence Parameters
//
const REAL TOLL_K = 1e-8; // convergence criterion for the individual capital function
const REAL UPDATE_K=0.7;  // updating parameter for the individual capital function

const REAL TOLL_COEFF=1e-8; // convergence criterion for the coefficients B in the ALM
const REAL UPDATE_B=0.3;  // updating parameter for the coefficients B in the ALM

// Regressions
const REAL REGRESSORS =2; // Number of Regressors: constant + logkmts
//#define PRINTIT
//#define PRINTCHECK


//#define SHOWMETRIC_DV
#define SHOWMETRIC_DCOEF
// #define VFISOLVER     // if not define Endogenous grid method
#define CONVERGENCE_COEF_NORM 1 // 1. Euclidean 2. Max(abs)

#endif /* usr_decl_h */
