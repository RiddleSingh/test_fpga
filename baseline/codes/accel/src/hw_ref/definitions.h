#pragma once
#include <stdio.h>
#include <stdlib.h> // must be prior to sds_lib.h
#include <string.h>
#include <assert.h>
// #include <cstdint>

/** Development options */
#include "dev_options.h"

/** Hardware includes */
#ifdef __SYNTHESIS__
#include <ap_int.h>
#include <ap_fixed.h>
#include <hls_stream.h>
#endif

#ifdef __SDSCC__
#include <sds_lib.h>
#endif


/** Typedefs */
#if PRECISION_FLOAT
typedef float real;
#elif __SYNTHESIS__ && PRECISION_FIXED
typedef ap_fixed<32, 24> real;
#else
typedef double real;
#endif

typedef unsigned short small_idx_t;
typedef unsigned int idx_t;

#if defined(__SYNTHESIS__) && PRECISION_EXTRA_FIXED
// #if PRECISION_EXTRA_FIXED

typedef ap_fixed<32, 24> kc_t;
typedef ap_fixed<32, 3> emu_s_t; // 48 bits
typedef ap_uint<4> pidx_t;
typedef ap_uint<8> fr_idx_t;
typedef ap_uint<4> apu4_t;
#else

typedef real kc_t;
typedef real emu_s_t;
typedef small_idx_t pidx_t;
typedef small_idx_t fr_idx_t;
typedef small_idx_t apu4_t;

#endif

#ifdef __SYNTHESIS__
typedef ap_uint<8 * IDS_AGG_X> ids_hw_t;
typedef ap_uint<8> shock_t; // ags shock t
#else
typedef unsigned char shock_t; // fix
typedef shock_t ids_hw_t;
#endif

#if IDS_AGG_PACK && (IDS_AGG_X == 2)
// typedef uint16_t ids_sw_t;
typedef unsigned short ids_sw_t;
#else
// typedef uint8_t ids_sw_t;
typedef unsigned char ids_sw_t;
#endif


typedef struct range_t
{
	small_idx_t min;
	small_idx_t max;
	real min_val;
	real max_val;
#ifndef __SDSCC__
	bool operator==(const range_t& rhs) const
	{
		return (min == rhs.min)
				&& (max == rhs.max)
				&& (min_val == rhs.min_val)
				&& (max_val == rhs.max_val);
	}
	bool operator!=(const range_t& rhs) const
	{
		return !range_t::operator==(rhs);
	}
#endif
} range_t;

/** Paths */
#define SHOCKPATH			"shocks/"
#define KPRIME_FILE_NAME	"kprime.txt"
#define IDSHOCK_FILE_NAME	"idshock.txt"
#define AGSHOCK_FILE_NAME	"agshock.txt"
#define IDSHOCK_BIN_FILE	"idshock.dat"
#define AGSHOCK_BIN_FILE	"agshock.dat"

/** Programming constants */
#define MAX_FILENAME_LEN	150
#define RES_OK				0
#define RES_ERR				-1
#define MAX_EGM_ITER		5000

/** Auxiliary constants */
#define XMIN				((real)0.0)
#define XMAX				((real)0.5)
#define BYTE_PACK_FACTOR	8 // 8 bits to byte
#define AGS_PACK_FACTOR		8 // data width may change
#define IDS_PACK_FACTOR		8 // data width may change
#define NUM_KPRIMES			9
#define NUM_KPINTERPS		2



/** Simulation settings */
// todo enum n/stochastic simulation mode
#define N_AGENTS			10000
#define N_AGENTS_INV		((real)0.0001) // (1. / N_AGENTS)
#define SIM_STEPS			1100
#define AGSHOCK_ARR_SIZE	138 // (ceil(1100/8))
#define IDSHOCK_ARR_SIZE	((N_AGENTS / IDS_PACK_FACTOR) * SIM_STEPS)
// Better name above as "number of bytes"

#if IDS_AGG_PACK
#define IDSHOCK_HW_ARR_SIZE	((N_AGENTS / IDS_PACK_FACTOR) * SIM_STEPS) / IDS_AGG_X
#else
#define IDSHOCK_HW_ARR_SIZE	((N_AGENTS / IDS_PACK_FACTOR) * SIM_STEPS)
#endif
#define NDISCARD			100
#define CC_NORM				1

/** Application constants */
#define NSTATES_ID			2 // 4
#define NSTATES_AG			2 // 0
#define NKGRID				100 // 65,536
#define NKM_GRID			4 // 0
#define NCOEFF				4
#define NSTATES				(NSTATES_ID * NSTATES_AG * NKGRID * NKM_GRID)
#define L4D_D3				(NKM_GRID * NSTATES_ID * NKGRID)
#define L4D_D2				(NSTATES_ID * NKGRID)
#define L4D_D1				NKGRID

#define SIZE_P				16

#define KMIN				((real)0.0)
#define KMAX				((real)1000.0)
#define NKPRIME_SCALE		((real)0.9)
#define CONS2_MIN			((real)0.0000000001) // pow(10., -10.);

#define KM_MIN				((real)30.0)
#define KM_MAX				((real)50.0)

// newly added magic numbers // @@ what does float cast do with this?
#define VERY_SMALL_SCALAR	((real)-99999999999999)

/** Environment variables */
#define env__beta			((real)0.99)	//.95 to .99
#define env__gamma			((real)1.0)		// most of the time == 2, sometimes 4 or 5 (even up to 40)
#define env__alpha			((real)0.36)	// 
#define env__alpha_c		((real)(-0.64)) // alpha - 1.
#define env__delta			((real)0.025)	// 0.1 or 0.025 (by year or by quarter)
#define env__delta_c		((real)0.975) // 1. - delta
#define env__delta_a		((real)0.01)
#define env__mu				((real)0.15)
#define env__l_bar			((real)(1 / 0.9))
#define env__l_bar_inv		((real)0.9)
#define env__gamma_inv		((real)1.0)
#define env__gamma_neg		((real)(-1.0))
#define env__gamma_neg_inv	((real)(-1.0))
#define env__epsilon_u		((real)0.0)
#define env__epsilon_e		((real)1.0)
#define env__epsilon_0		((real)0.0)
#define env__epsilon_1		((real)1.0)
#define env__ur_0			((real)0.1)
#define env__er_0			((real)0.9)
#define env__ur_1			((real)0.04)
#define env__er_1			((real)0.96)
#define env__er_inv_0		((real)(1 / 0.9))
#define env__er_inv_1		((real)(1 / 0.96))
// pow((1./beta-(1.-delta))/alpha,1./(alpha-1))
#define env__kss			((real)37.9892535815222)


/** Macros */
#define IXP(a, i, ap, ip)	(NSTATES_ID * NSTATES_AG * NSTATES_ID * a \
							 + NSTATES_AG * NSTATES_ID * i \
							 + NSTATES_ID * ap \
							 + ip)
#define IXEV(a, i, j)		(NSTATES_ID * NKGRID * a \
							 + NKGRID * i \
							 + j)
#define IXV(a, km, id, k)	(NKM_GRID * NSTATES_ID * NKGRID * a \
							 + NSTATES_ID * NKGRID * km \
							 + NKGRID * id \
							 + k)
#define IX(t, j)			(t * N_AGENTS + j)
#define IXID(i, k)			(i * NKGRID + k)
#define IXAKM(a, km)		(a * NKM_GRID + km)
#define IXAI(a, i)			(a * NSTATES_ID + i)

/** Convergence */
#if PRECISION_FLOAT
#define TOLL_K				((real)(1e-3))
#define TOLL_COEFF			((real)(1e-3))
#else
#define TOLL_K				((real)(1e-8))
#define TOLL_COEFF			((real)(1e-8))
#endif

#define UPDATE_K			((real)0.7)
#define UPDATE_K_C			((real)0.3)
#define	UPDATE_B			((real)0.3)
#define REGRESSORS			2

/** Sigh */
#define SIZE_OF_ENV			1866

/** Containers for parameters */
typedef struct env_t
{
	// full: 1618+16+10+100+4+100+4+14
	real alpha;
	real beta;
	real delta;
	real gamma;
	real mu;
	real l_bar;

	real l_bar_inv;
	real gamma_inv;
	real gamma_neg;
	real gamma_neg_inv;

	real delta_a;

	real epsilon_u;
	real epsilon_e;

	real kss;

	real km[NKM_GRID];
	real k[NKGRID];

	real km_egm[NKM_GRID];
	real k_egm[NKGRID];

	real trate[NSTATES_AG];
	real ag[NSTATES_AG];
	real ag2[NSTATES_AG];
	real epsilon[NSTATES_ID];
	real epsilon2[NSTATES_ID];
	real P[SIZE_P];
	real wealth[NSTATES];

	real ur[NSTATES_AG];
	real er[NSTATES_AG];
	real er_inv[NSTATES_AG];

	real irate_factor[NSTATES_AG];
	real wage_factor[NSTATES_AG];
	real cons2_factor[NSTATES_AG * NSTATES_ID];
	real log_env_km[NKM_GRID];
} env_t;

typedef struct input_t
{
	ids_sw_t *idshock;
	unsigned char *agshock;
} input_t;

typedef struct vars_t
{
	real *kmprime;

	real *kprime_a;
	real *kprime_b;

	real *kcross_a;
	real *kcross_b;

	real *coeff;
	real *coeff1;

	real *kmts;
} var_t;
