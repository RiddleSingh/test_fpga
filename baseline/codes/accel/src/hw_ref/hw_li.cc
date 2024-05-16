#include "hw.h"

// void hw_li_4d_update_dimm12(hw_i4_t &m, const hw_env_t &env, shock_t p0, real p1)
// {
// 	#pragma HLS inline
// 	range_t i1 = hw_findrange_n2(p0, env.ag2);
// 	range_t i2 = hw_findrange_n4(p1, env.km);
// 	real tx = (p0 - i1.min_val) / (i1.max_val - i1.min_val);
// 	real ty = (p1 - i2.min_val) / (i2.max_val - i2.min_val);
	
// 	m.a[0] = (1.0 - tx) * (1.0 - ty);
// 	m.a[1] = (1.0 - tx) * ty;
// 	m.a[2] = tx * (1.0 - ty);
// 	m.a[3] = tx * ty;

// 	small_idx_t i1_min_base = L4D_D3 * i1.min;
// 	small_idx_t i1_max_base = L4D_D3 * i1.max;
// 	small_idx_t i2_min_base = L4D_D2 * i2.min;
// 	small_idx_t i2_max_base = L4D_D2 * i2.max;

// 	m.i12_min_min = i1_min_base + i2_min_base;
// 	m.i12_min_max = i1_min_base + i2_max_base;
// 	m.i12_max_min = i1_max_base + i2_min_base;
// 	m.i12_max_max = i1_max_base + i2_max_base;
// 	return;
// }

// void hw_li_4d_update_dimm3(hw_i4_t &m, const hw_env_t &env, real p2)
// {
// 	#pragma HLS inline
// 	range_t i3 = hw_findrange_n2(p2, env.epsilon2);
// 	m.tz = (p2 - i3.min_val) / (i3.max_val - i3.min_val);

// 	m.i3_min_base = L4D_D1 * i3.min;
// 	m.i3_max_base = L4D_D1 * i3.max;
// 	return;
// }

// void hw_li_4d_update_dimm4(hw_i4_t &m,
// 						   const hw_env_t &env,
// 						   real p3,
// 						   const real kprime[NSTATES])
// {
// 	#pragma HLS inline
// 	range_t i4 = hw_findrange_n100(p3, env.k);
// 	real tw = (p3 - i4.min_val) / (i4.max_val - i4.min_val);

// 	m.b[0] = (1.0 - m.tz) * (1.0 - tw);
// 	m.b[1] = (1.0 - m.tz) * tw;
// 	m.b[2] = m.tz * (1.0 - tw);
// 	m.b[3] = m.tz * tw;

// 	m.fv[0]  = kprime[i4.min + m.i3_min_base + m.i12_min_min];
// 	m.fv[1]  = kprime[i4.max + m.i3_min_base + m.i12_min_min];
// 	m.fv[2]  = kprime[i4.min + m.i3_max_base + m.i12_min_min];
// 	m.fv[3]  = kprime[i4.max + m.i3_max_base + m.i12_min_min];
// 	m.fv[4]  = kprime[i4.min + m.i3_min_base + m.i12_min_max];
// 	m.fv[5]  = kprime[i4.max + m.i3_min_base + m.i12_min_max];
// 	m.fv[6]  = kprime[i4.min + m.i3_max_base + m.i12_min_max];
// 	m.fv[7]  = kprime[i4.max + m.i3_max_base + m.i12_min_max];
// 	m.fv[8]  = kprime[i4.min + m.i3_min_base + m.i12_max_min];
// 	m.fv[9]  = kprime[i4.max + m.i3_min_base + m.i12_max_min];
// 	m.fv[10] = kprime[i4.min + m.i3_max_base + m.i12_max_min];
// 	m.fv[11] = kprime[i4.max + m.i3_max_base + m.i12_max_min];
// 	m.fv[12] = kprime[i4.min + m.i3_min_base + m.i12_max_max];
// 	m.fv[13] = kprime[i4.max + m.i3_min_base + m.i12_max_max];
// 	m.fv[14] = kprime[i4.min + m.i3_max_base + m.i12_max_max];
// 	m.fv[15] = kprime[i4.max + m.i3_max_base + m.i12_max_max];

// 	return;
// }
/**
 * 
 * for 2d non-aux: idx_base = 0
 * for 2d aux: i1_idx_factor = NSTATES_ID * NKGRID
 * for 2d: i1_idx_factor = NKGRID
 */
real hw_li_2d(const real *x1,
			  const real *x2,
			  int d1,
			  int d2,
			  const real *f,
			  const real p[2],
			  small_idx_t idx_base,
			  unsigned short i1_idx_factor)
{
	#pragma HLS inline
	range_t i1 = hw_findrange(p[0], x1, d1); // d1 is sometimes 2
	range_t i2 = hw_findrange(p[1], x2, d2);
	small_idx_t i1_min_base = idx_base + ((i1_idx_factor) * i1.min);
	small_idx_t i1_max_base = idx_base + ((i1_idx_factor) * i1.max);
	
	// sometimes called with d1 == 4, so bitshift not possible

	real fv0 = f[i1_min_base + i2.min];
	real fv1 = f[i1_min_base + i2.max];
	real fv2 = f[i1_max_base + i2.min];
	real fv3 = f[i1_max_base + i2.max];

	real tz = (p[0] - i1.min_val) / (i1.max_val - i1.min_val);
	real tw = (p[1] - i2.min_val) / (i2.max_val - i2.min_val);

	real fp = fv0 * (1.0 - tz) * (1.0 - tw) + 
		 fv1 * (1.0 - tz) *      tw  +
		 fv2 *        tz  * (1.0 - tw) +
		 fv3 *        tz  *      tw;

	return fp;
}

// real hw_li_2d_base(const real *x1,
// 			  const real *x2,
// 			//   int d1, NSTATES_ID
// 			//   int d2, NKGRID
// 			  const real *f1,
// 			  const real *f2,
// 			  const real p[2],
// 			  small_idx_t idx_base)
// {
// 	#pragma HLS inline
// 	unsigned short i1_idx_factor = NKGRID;
// 	real fp;
// 	real fval[4];
// 	#pragma HLS array_partition variable=fval complete
// 	range_t i1 = hw_findrange_n2(p[0], x1);
// 	range_t i2 = hw_findrange_n100(p[1], x2);
// 	small_idx_t i1_min_base = idx_base + ((i1_idx_factor) * i1.min);
// 	small_idx_t i1_max_base = idx_base + ((i1_idx_factor) * i1.max);
	
// 	// sometimes called with d1 == 4, so bitshift not possible

// 	fval[0] = f1[i1_min_base + i2.min];
// 	fval[1] = f1[i1_min_base + i2.max];
// 	fval[2] = f2[i1_max_base + i2.min];
// 	fval[3] = f2[i1_max_base + i2.max];

// 	real tz = (p[0] - i1.min_val) / (i1.max_val - i1.min_val);
// 	real tw = (p[1] - i2.min_val) / (i2.max_val - i2.min_val);

// 	fp = fval[0] * (1.0 - tz) * (1.0 - tw) + 
// 		 fval[1] * (1.0 - tz) *      tw  +
// 		 fval[2] *        tz  * (1.0 - tw) +
// 		 fval[3] *        tz  *      tw;

// 	return fp;
// }

real hw_li_2d_aux(const real p0, // kmp
				  const real p1, // kp
				  const real *x1, // km
				  const real *x2, // k
				  const real f[NUM_KPRIMES][NSTATES], // st_kprimes
				  small_idx_t f_bank,
				  small_idx_t idx_base)
{
	#pragma HLS inline
	unsigned short i1_idx_factor = NSTATES_ID * NKGRID;
	range_t i1 = hw_findrange(p0, x1, NKM_GRID);
	range_t i2 = hw_findrange(p1, x2, NKGRID);
	small_idx_t i1_min_base = idx_base + ((i1_idx_factor) * i1.min);
	small_idx_t i1_max_base = idx_base + ((i1_idx_factor) * i1.max);

	real fv0 = f[f_bank+0][i1_min_base+i2.min];
	real fv1 = f[f_bank+1][i1_min_base+i2.max];
	real fv2 = f[f_bank+2][i1_max_base+i2.min];
	real fv3 = f[f_bank+3][i1_max_base+i2.max];

	real tz = (p0 - i1.min_val) / (i1.max_val - i1.min_val);
	real tw = (p1 - i2.min_val) / (i2.max_val - i2.min_val);

	real fp = fv0 * (1.0 - tz) * (1.0 - tw) +
			  fv1 * (1.0 - tz) *        tw  +
			  fv2 *        tz  * (1.0 - tw) +
			  fv3 *        tz  *        tw;
	return fp;
}

// real hw_li_4d(const real *fv,
// 			  const real *a,
// 			  const real *b)
// {
// 	#pragma HLS inline
// 	real fp;
// 	real P, Q, R, S;
// 	real p, q, r, s;

// 	P = a[0];
// 	Q = a[1];
// 	R = a[2];
// 	S = a[3];

// 	p = b[0];
// 	q = b[1];
// 	r = b[2];
// 	s = b[3];

// 	fp = fv[0] * P * p +
// 		 fv[1] * P * q +
// 		 fv[2] * P * r +
// 		 fv[3] * P * s +
// 		 fv[4] * Q * p +
// 		 fv[5] * Q * q +
// 		 fv[6] * Q * r +
// 		 fv[7] * Q * s +
// 		 fv[8] * R * p +
// 		 fv[9] * R * q +
// 		 fv[10] * R * r +
// 		 fv[11] * R * s +
// 		 fv[12] * S * p +
// 		 fv[13] * S * q +
// 		 fv[14] * S * r +
// 		 fv[15] * S * s;
// 	return fp;
// }
