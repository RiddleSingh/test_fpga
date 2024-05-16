/*
 Date: 2017-12-13
 File: usr_decl.h (Version: 1.0)
 Description: User Main Header File.  
 Copyright: Alessandro Peri, peri.alessandro@gmail.com
 */

#ifndef __FILE_USRDECL_H_SEEN__
#define __FILE_USRDECL_H_SEEN__

//#define FPGA_MODE_ON

#ifdef FPGA_MODE_ON
#include <time.h>
#include <fpga_pci.h>
#endif

#ifdef FPGA_MODE_ON
//#define FPGA_DEBUG_MODE_ON
const int FPGA_ITERATIONS;
#endif

typedef double REAL;

typedef struct{
    
    uint32_t addr_low;
    uint32_t addr_high;
#ifdef FPGA_MODE_ON
    pci_bar_handle_t pci_bar_handle;
#endif
    int nk;
    int nz;
    double TOLL_V;
    REAL *K;
    REAL *P;
    REAL *V0;
    REAL *W;
    REAL *V;
    int *G;
    double beta;
    double eta;
    int Iterations;
    clock_t tic;
    double startTime;
    double solTime;
    double ReadingBackTime;
    
} FPGA_setting_type;

typedef struct{
    // Parameters
    REAL alpha;  // Capital Share
    REAL beta;   // Discount Factor
    REAL delta;  // Depreciation Rate
    REAL eta;    // Relative Risk Aversion
    REAL mu;     // TFP (Mean)
    REAL rho;    // TFP (Persistency AR1)
    REAL sigma;  // TFP (Volatility)
    REAL lambda; // Tauchen # (size TFP grid)
    // Grids
    int nz;      // # Points in TFP Grid
    int nk;      // # Points in Capital Grid
    // Tollerance
    REAL TOLL_V; // Tolerance for Convergence (VFI Algorithm)
    
} parameters;

// Functions Declaration
void Parameters_load(parameters *param);
void Tauchen(parameters *param, REAL* Z, REAL* P);
void InitializationK(parameters *param, const REAL* Z, REAL* K);
void InitializationV(parameters *param, const REAL* Z, REAL* V);
void InitializationW(parameters *param, const REAL* Z,const REAL* K, REAL* W);
int fpgamemcpyandgo(parameters *params,FPGA_setting_type *FPGA_);

int Model1SobolNodes(REAL **);

#define MAINPATH "/home/centos/src/project_data/fpga-c/"
#define GLOBAL_SEARCH 0
#define N_MODEL     1
#define N_PARMS_TBC 5


//#define READWRITE_INIT
//#define READ_INIT
//#define WRITE_INIT
// #define BINARY_SEARCH // Binary Search otherwise Grid Search
//#define READ_V0 // Read Value function from file

#endif
