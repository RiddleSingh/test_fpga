#pragma once

#ifdef __SYNTHESIS__
#include <hls_math.h>
#else
#include <math.h>
#endif

#include "definitions.h"
#include "perf.h"


/** Structs */
typedef struct hw_env_t
{
	real alpha;
	real beta;
	real delta;
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

	real km_egm[NKM_GRID];
	real k_egm[NKGRID];

	// real trate[NSTATES_AG];
	real ag[NSTATES_AG];
	real ag2[NSTATES_AG];
	real epsilon[NSTATES_ID];
	real P[SIZE_P];
	real wealth[NSTATES];

#if AST_UNROLL && ACCEL
	real k[NUM_KCROSS][NKGRID];
	real epsilon2[NUM_KCROSS][NSTATES_ID];
#else
	real k[NKGRID];
	real epsilon2[NSTATES_ID];
#endif

	real ur[NSTATES_AG];
	real er[NSTATES_AG];
	real er_inv[NSTATES_AG];

	real irate_factor[NSTATES_AG];
	real wage_factor[NSTATES_AG];
	real cons2_factor[NSTATES_AG * NSTATES_ID];
	real log_env_km[NKM_GRID];
} hw_env_t;




// #pragma HLS array_partition variable=kprimes complete dim=1
// #pragma HLS array_partition variable=env.P complete


// typedef struct hw_i4_t
// {
// 	real tz;
// 	small_idx_t i12_min_min;
// 	small_idx_t i12_min_max;
// 	small_idx_t i12_max_min;
// 	small_idx_t i12_max_max;
// 	small_idx_t i3_min_base;
// 	small_idx_t i3_max_base;
// 	real a[4];
// 	real b[4];
// 	real fv[16];
// } hw_i4_t;

/** Initialization */
// void hw_init_all(hw_env_t &env,
// 				 real kprime[NSTATES],
// 				 real kcross[N_AGENTS]);

void force_init_env(void);

/** Utilities */
real hw_pow(real x, real y);

real hw_exp(real b);

void hw_rail_values(real *val, const real max, const real min);

#ifdef __SYNTHESIS__
void hw_rail_values(kc_t *val, const kc_t max, const kc_t min);
#endif

/** Linear interpolation */
range_t hw_findrange(real p, const real *src, int n_elem);

range_t hw_findrange_n2(real p, const real src[2]);

range_t hw_findrange_n(real p, const real *src, int n_elem);

// range_t hw_findrange_n4(real p, const real src[4]);

// range_t hw_findrange_n100(real p, const real src[100]);

// range_t hw_bfindrange_n100(real p, const real src[100]);

// void hw_fr_n100_8(range_t result[8], real p[8], const real src[100]);

// void hw_li_4d_update_dimm12(hw_i4_t &m, const hw_env_t &env, shock_t p0, real p1);

// void hw_li_4d_update_dimm3(hw_i4_t &m, const hw_env_t &env, real p2);

// void hw_li_4d_update_dimm4(hw_i4_t &m,
// 						   const hw_env_t &env,
// 						   real p3,
// 						   const real kprime[NSTATES]);

// real hw_li_2d(const real *x1,
// 			  const real *x2,
// 			  int d1,
// 			  int d2,
// 			  const real *f,
// 			  const real p[2],
// 			  small_idx_t idx_base,
// 			  unsigned short i1_idx_factor);

real hw_li_2d_aux(const real p0, // kmp
				  const real p1, // kp
				  const real *x1, // km
				  const real *x2, // k
				  const real f[NUM_KPRIMES][NSTATES], // st_kprimes
				  small_idx_t f_bank,
				  small_idx_t idx_base);

// real hw_li_2d_base(const real *x1,
// 			  const real *x2,
// 			  const real *f1,
// 			  const real *f2,
// 			  const real p[2],
// 			  small_idx_t idx_base);

// real hw_li_4d(const real *fv,
// 			  const real *a,
// 			  const real *b);

/** Simulation */
void hw_sim_alm(
				real kmprime[NSTATES_AG * NKM_GRID],
				const real coeff[NCOEFF]);
#if OUT_DEBUG_DATA
void hw_sim_egm(
				const real kmprime[NSTATES_AG * NKM_GRID],
				int *egms);
#else
#if OUTPUT_KP
void hw_sim_egm(const real kmprime[NSTATES_AG * NKM_GRID],
				real kp_final[NSTATES]);
#else
void hw_sim_egm(const real kmprime[NSTATES_AG * NKM_GRID]);
#endif
#endif


void hw_sim_ast(
				const unsigned char agshock[SIM_STEPS],
				const ids_sw_t idshock[IDSHOCK_HW_ARR_SIZE],
				// const real kprimes[NUM_KPRIMES][NSTATES],
				// real kcross[N_AGENTS],
#if OUT_DEBUG_DATA
				int *asts,
#endif
				real kmts[SIM_STEPS]);

/** Top-level */
void hw_top_init(bool first,
				 const real kp_in[NSTATES],
				 const real kcross_init);

void hw_top(bool first,
			const real coeff[NCOEFF],
#if OUT_DEBUG_DATA
			int *egms,
			int *asts,
#endif
			const unsigned char agshock[SIM_STEPS],
			const ids_sw_t idshock[IDSHOCK_HW_ARR_SIZE],
			const real kp_in[NSTATES],
			const real kcross_init,
#if OUTPUT_KP
			real kp_final[NSTATES],
#endif
			real kmts[SIM_STEPS]);

