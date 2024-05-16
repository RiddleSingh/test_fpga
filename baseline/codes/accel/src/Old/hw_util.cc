#include "hw.h"

real hw_exp(real b)
{
	#pragma HLS inline
	#if PRECISION_FLOAT
	return expf(b);
	#else
	return exp(b);
	#endif
}

real hw_pow(real x, real y)
{
	#pragma HLS inline

	#if PRECISION_FLOAT
	#if USE_EXP_AS_POW
		return expf(y * logf(x));
		if (x == 0.0)
			return 0;
		else
			return expf(y * logf(x));
	#else
		return powf(x, y);
	#endif
	#else
		return pow(x, y);
	#endif
}

void hw_rail_values(real *val, const real max, const real min)
{
	#pragma HLS inline
	real src = *val;
	bool over_max = (src > max);
	bool under_min = (src < min);

	if (over_max)
		*val = max;
	else if (under_min)
		*val = min;
	return;
}

#ifdef __SYNTHESIS__
void hw_rail_values(kc_t *val, const kc_t max, const kc_t min)
{
	#pragma HLS inline
	kc_t src = *val;
	bool over_max = (src > max);
	bool under_min = (src < min);

	if (over_max)
		*val = max;
	else if (under_min)
		*val = min;
	return;
}
#endif

range_t hw_findrange_n2(real p, const real src[2])
{
	#pragma HLS inline
	range_t result;
	result.min = 0;
	result.max = 1;
	result.min_val = src[0];
	result.max_val = src[1];
	return result;
}

range_t hw_findrange_n4(real p, const real *src)
{
	#pragma HLS inline
	range_t result;
	small_idx_t idx = 0;
	real last, curr;

	// preload src[0]
	curr = src[idx++];

	// idx loop over [0: 1]
	while (idx < 4) {
		#pragma HLS unroll factor=2
		#pragma HLS pipeline
		last = curr;
		curr = src[idx++]; // from 1 to N-3
		if (p < curr)
			break;
	}

	result.min = idx - 2;
	result.max = idx - 1;
	result.min_val = last;
	result.max_val = curr;
	return result;
}

range_t hw_findrange_n100(real p, const real *src)
{
	#pragma HLS inline
	range_t result;
	small_idx_t idx = 0;
	real last, curr;

	// preload src[0]
	curr = src[idx++];

	// idx loop over [0: 1]
	while (idx < 100) {
		#pragma HLS unroll factor=2
		#pragma HLS pipeline
		last = curr;
		curr = src[idx++]; // from 1 to N-3
		if (p < curr)
			break;
	}

	result.min = idx - 2;
	result.max = idx - 1;
	result.min_val = last;
	result.max_val = curr;
	return result;
}

range_t hw_findrange(real p, const real *src, int n_elem)
{
	#pragma HLS inline
	assert(n_elem >= 2 && n_elem <= 100);
	if (n_elem == 2)
		return hw_findrange_n2(p, src);
	else if (n_elem == 4)
		return hw_findrange_n4(p, src);
	else if (n_elem == 100)
		return hw_findrange_n100(p, src);
	else
		return hw_findrange_n(p, src, n_elem);
}

range_t hw_findrange_n(real p, const real *src, int n_elem)
{
	#pragma HLS inline
	assert(n_elem > 2 && n_elem <= 100);
	range_t result;
	small_idx_t idx = 0;
	real last, curr;

	// preload src[0]
	curr = src[idx++];

	// idx loop over [0: 1]
	while (idx < n_elem) {
		#pragma HLS unroll factor=2
		#pragma HLS loop_tripcount min=4 max=100
		#pragma HLS pipeline
		last = curr;
		curr = src[idx++]; // from 1 to N-3
		if (p < curr)
			break;
	}

	result.min = idx - 2;
	result.max = idx - 1;
	result.min_val = last;
	result.max_val = curr;
	return result;
}



/**
 * binary findrange 100
 *	Returns first elem strictly greater than p in src
 *	as 'max'; one before that as 'min'
 */
range_t hw_bfindrange_n100(real p, const real src[100])
{
	#pragma HLS inline
	range_t result;
	fr_idx_t lo = 0;
	fr_idx_t hi = 99;
	result.min = 0;
	result.max = 1;
	
	while (lo <= hi) {
		#pragma HLS loop_tripcount min=7 max=7
		#pragma HLS pipeline
		#pragma HLS unroll factor=2
		fr_idx_t mid = (lo + hi) >> 1; // / 2?
		real curr = src[mid];

		if (p >= curr)
			lo = mid + 1;
		else {
			// p < curr; speculatively update output
			result.max = mid;
			result.min = mid - 1;
			result.max_val = curr;
			hi = mid - 1;
		}
	}

	// Spare some src[] reads by grabbing min_val only when finalized
	result.min_val = src[result.min];
	return result;
}

/**
 * linear findrange 100
 */
void hw_fr_n100_8(range_t result[8], real p[8], const real src[100]) {
	#pragma HLS inline
	// #pragma HLS array_partition variable=result complete
	// #pragma HLS array_partition variable=p complete

	small_idx_t idx = 0;
	real last;
	real curr;

	// preload src[0]
	curr = src[idx++];
	
	// idx loop over [0: 1]
	while (idx < 100) {
		#pragma HLS unroll factor=2
		#pragma HLS pipeline
		// n_elem == 2 case: last becomes [0], curr becomes[1]
		// and idx == 2
		last = curr;
		curr = src[idx++]; // from 1 to N-3
		for (int i = 0; i < 8; ++i) {
			#pragma HLS unroll
			if (p[i] < curr) {
				result[i].min = idx - 2;
				result[i].max = idx - 1;
				result[i].min_val = last;
				result[i].max_val = curr;
			}
		}
	}
	return;
}

