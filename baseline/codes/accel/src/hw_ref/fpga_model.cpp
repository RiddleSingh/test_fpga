#include <iostream>
#include <ctime>
#include <utility>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "hls_stream_utils.h"
#include "fpga_model.h"


#define hash_bloom 0x7ffff 
#define bloom_size 14
#define docTag 0xffffffff

extern "C" {

    void perf_ctr_start(perf_ctr *pctr) {
        pctr->start = clock();
        return;
        }

    /**
    * perf_time_update:
    *  Updates current cycle count since last start()
    */
    void perf_ctr_update(perf_ctr *pctr) {
        if (pctr->start == 0) {
            perf_ctr_start(pctr);
        } else {
            // Update relative to the last start
            pctr->last = (unsigned long long)(clock() - pctr->start);
            pctr->cycles += pctr->last;
            pctr->iters += 1;
        }
        return;
    }

    double get_pctr_secs(const perf_ctr pctr) {
        return (double)(pctr.cycles) / CLOCKS_PER_SEC;
    }

    double get_pctr_last_secs(const perf_ctr pctr) {
        return (double)(pctr.last) / CLOCKS_PER_SEC;
    }

    /**
    * get_pctr_avg_cycles:
    *  Gets average cycles per iteration
    */
    double get_pctr_avg_cycles(const perf_ctr pctr) {
        return (double)(pctr.cycles) / (pctr.iters);
    }
	
    void sim_ast(
				const unsigned char agshock[AGSHOCK_ARR_SIZE],
				const ids_sw_t idshock[IDSHOCK_HW_ARR_SIZE],
				// const real kprimes[NUM_KPRIMES][NSTATES],
				// real kcross[N_AGENTS],
				#if OUT_DEBUG_DATA
				int *asts,
				#endif
				real kmts[SIM_STEPS])
    {
	#pragma HLS array_partition variable=st_kprimes complete dim=1

	#if AST_UNROLL && ACCEL
	real kprime_interp0[NUM_KCROSS][NSTATES_ID * NKGRID];
	real kprime_interp1[NUM_KCROSS][NSTATES_ID * NKGRID];
	#else
	real kprime_interp0[NSTATES_ID * NKGRID];
	real kprime_interp1[NSTATES_ID * NKGRID];
	#endif

	#if AST_UNROLL && ACCEL
	// #pragma HLS array_partition variable=st_env.k complete dim=1
	#pragma HLS array_partition variable=st_env.epsilon2 complete dim=1
	#pragma HLS array_partition variable=st_kcross complete dim=1
	#pragma HLS array_partition variable=kprime_interp0 complete dim=1
	#pragma HLS array_partition variable=kprime_interp1 complete dim=1
	#endif

	real curr_kmts = st_kmts_init;
	idx_t idshock_idx = 0;
	idx_t agshock_idx = 0;
	ids_hw_t curr_ids;
	shock_t curr_ags;
	apu4_t ags_phase = AGS_PACK_FACTOR;

	#if OUT_DEBUG_DATA
	int curr_sum_cnt = 0;
	#endif

	// Loop 1
	for (int t = 0; t < SIM_STEPS; ++t) {
		kc_t curr_sum = 0;
		kmts[t] = curr_kmts;

		// Read next packed agshock value when needed
		if (++ags_phase >= AGS_PACK_FACTOR) {
			curr_ags = agshock[agshock_idx++];
			ags_phase = 0;
		}

		#ifdef __SYNTHESIS__
		real p0 = curr_ags.get_bit(ags_phase) ? (real)1.0 : (real)0.0;
		#else
		real p0 = (curr_ags & 0b1) ? (real)1.0 : (real)0.0;
		curr_ags >>= 1;
 		#endif

		/** 
		 * (C): AST kprime_interp over kprime
		 *	f(env, agshock[t], curr_kmts, kprimes)
		 */
		// real p0 = agshock[t] ? (real)1.0 : (real)0.0;
		real p1 = curr_kmts;

		range_t i1 = hw_findrange(p0, st_env.ag2, NSTATES_AG);
		range_t i2 = hw_findrange(p1, st_env.km, NKM_GRID);
		real tx = (p0 - i1.min_val) / (i1.max_val - i1.min_val);
		real ty = (p1 - i2.min_val) / (i2.max_val - i2.min_val);

		real P = ((real)1.0 - tx) * ((real)1.0 - ty);
		real Q = ((real)1.0 - tx) * ty;
		real R = tx * ((real)1.0 - ty);
		real S = tx * ty;

		small_idx_t i1_min_base = L4D_D3 * i1.min;
		small_idx_t i1_max_base = L4D_D3 * i1.max;
		small_idx_t i2_min_base = L4D_D2 * i2.min;
		small_idx_t i2_max_base = L4D_D2 * i2.max;

		small_idx_t i12_min_min = i1_min_base + i2_min_base;
		small_idx_t i12_min_max = i1_min_base + i2_max_base;
		small_idx_t i12_max_min = i1_max_base + i2_min_base;
		small_idx_t i12_max_max = i1_max_base + i2_max_base;
		/** Produces[1100]
		 *	i12_min_min (11 bits: max should be 800 + 600
		 *	i12_min_max (11 bits):
		 *	i12_max_min (11 bits):
		 *	i12_max_max (11 bits):
		 *	P, Q, R, S
		 */

		small_idx_t kpi_idx = 0;

		// Loop 1.1
		for (int iid = 0; iid < NSTATES_ID; ++iid) {
			// Can pipeline this too if we double the # of kprimes; see EGM
			// ** UPDATE DIMM3
			/*** Move to initialization
			 * Produces:
			 *	i3_min_base[2]
			 *	i3_max_base[2]
			 *	tz[2]
			 */
			real p2 = st_env.epsilon[iid];
			#if AST_UNROLL && ACCEL
			range_t i3 = hw_findrange(p2, st_env.epsilon2[0], NSTATES_ID);
			#else
			range_t i3 = hw_findrange(p2, st_env.epsilon2, NSTATES_ID);
			#endif
			real tz = (p2 - i3.min_val) / (i3.max_val - i3.min_val);
			small_idx_t i3_min_base = L4D_D1 * i3.min;
			small_idx_t i3_max_base = L4D_D1 * i3.max;
			
			// Loop 1.1.1
			for (int ik = 0; ik < NKGRID; ++ik) {
				#if !SMALL_PL && ACCEL
				#pragma HLS pipeline
				#else
				// #pragma HLS pipeline II=1
				#endif
				// ** UPDATE DIMM4

				real p3 = st_env.k_egm[ik];
				range_t i4 = hw_findrange(p3, st_env.k_egm, NKGRID);

//				printf("---%d %d %d---\n", ik, i4.min, i4.max);
				//assert((i4.min == ik && i4.max ==ik+1) ||
					   //(i4.min == ik - 1 && i4.max == ik));

//				range_t i4;
//				if (ik == NKGRID - 1) {
//					i4.min = ik - 1;
//					i4.max = ik;
//					i4.min_val = st_env.k_egm[i4.min];
//					i4.max_val = st_env.k_egm[i4.max];
//				} else {
//					i4.min = ik;
//					i4.max = ik + 1;
//					i4.min_val = st_env.k_egm[i4.min];
//					i4.max_val = st_env.k_egm[i4.max];
//				}
				
				real tw = (p3 - i4.min_val) / (i4.max_val - i4.min_val);

				real p = ((real)1.0 - tz) * ((real)1.0 - tw);
				real q = ((real)1.0 - tz) * tw;
				real r = tz * ((real)1.0 - tw);
				real s = tz * tw;

				small_idx_t kp_idx_0  = i4.min + i3_min_base + i12_min_min;
				small_idx_t kp_idx_1  = i4.max + i3_min_base + i12_min_min;
				small_idx_t kp_idx_2  = i4.min + i3_max_base + i12_min_min;
				small_idx_t kp_idx_3  = i4.max + i3_max_base + i12_min_min;
				small_idx_t kp_idx_4  = i4.min + i3_min_base + i12_min_max;
				small_idx_t kp_idx_5  = i4.max + i3_min_base + i12_min_max;
				small_idx_t kp_idx_6  = i4.min + i3_max_base + i12_min_max;
				small_idx_t kp_idx_7  = i4.max + i3_max_base + i12_min_max;
				small_idx_t kp_idx_8  = i4.min + i3_min_base + i12_max_min;
				small_idx_t kp_idx_9  = i4.max + i3_min_base + i12_max_min;
				small_idx_t kp_idx_10 = i4.min + i3_max_base + i12_max_min;
				small_idx_t kp_idx_11 = i4.max + i3_max_base + i12_max_min;
				small_idx_t kp_idx_12 = i4.min + i3_min_base + i12_max_max;
				small_idx_t kp_idx_13 = i4.max + i3_min_base + i12_max_max;
				small_idx_t kp_idx_14 = i4.min + i3_max_base + i12_max_max;
				small_idx_t kp_idx_15 = i4.max + i3_max_base + i12_max_max;

				// ** LI4D
				real fp = st_kprimes[0][kp_idx_0] * P * p +
							st_kprimes[0][kp_idx_1] * P * q +
							st_kprimes[1][kp_idx_2] * P * r +
							st_kprimes[1][kp_idx_3] * P * s +
							st_kprimes[2][kp_idx_4] * Q * p +
							st_kprimes[2][kp_idx_5] * Q * q +
							st_kprimes[3][kp_idx_6] * Q * r +
							st_kprimes[3][kp_idx_7] * Q * s +
							st_kprimes[4][kp_idx_8] * R * p +
							st_kprimes[4][kp_idx_9] * R * q +
							st_kprimes[5][kp_idx_10] * R * r +
							st_kprimes[5][kp_idx_11] * R * s +
							st_kprimes[6][kp_idx_12] * S * p +
							st_kprimes[6][kp_idx_13] * S * q +
							st_kprimes[7][kp_idx_14] * S * r +
							st_kprimes[7][kp_idx_15] * S * s;
				#if AST_UNROLL && ACCEL
				for (int k = 0; k < NUM_KCROSS; ++k) {
					kprime_interp0[k][kpi_idx] = fp;
					kprime_interp1[k][kpi_idx] = fp;
				}
				#else
				kprime_interp0[kpi_idx] = fp;
				kprime_interp1[kpi_idx] = fp;
				#endif
				++kpi_idx;
			}
		}

		#if AST_UNROLL && ACCEL
		small_idx_t kidx = 0;
		// Loop 1.3: AST agents interp over kprime_interp
		// Unroll factor dictated by inner loop over k
		for (int j = 0; j < (N_AGENTS / IDS_PACK_FACTOR) / IDS_AGG_X; ++j) {
			#pragma HLS pipeline
			curr_ids = idshock[idshock_idx++];

			// for (int k = 0; k < IDS_PACK_FACTOR; ++k)
			// 	#pragma HLS unroll
			// 	p1b[k] = st_kcross[k][kidx];
			// hw_fr_n100_8(i2b, p1b, st_env.k[0]);

			for (int k = 0; k < IDS_PACK_FACTOR * IDS_AGG_X; ++k) {
				real p1b = st_kcross[k][kidx];
				range_t i2b = hw_findrange(st_kcross[k][kidx], st_env.k_egm, NKGRID);
				#ifdef __SYNTHESIS__
				real p0b = curr_ids.get_bit(k) ? (real)1.0 : (real)0.0;
				#else
				real p0b = (curr_ids & 0b1) ? (real)1.0 : (real)0.0;
				curr_ids >>= 1; 
				#endif

				// if (global_debug < 32) {
				// 	printf("[%d]: %g\n", global_debug, p0b);
				// 	global_debug++;
				// }
				// if (!global_debug)
				// 	fprintf(stderr, "%d\n", (int) p0b);

				range_t i1b = hw_findrange(p0b, st_env.epsilon2[k], NSTATES_ID);
				small_idx_t i1b_min_base = NKGRID * i1b.min;
				small_idx_t i1b_max_base = NKGRID * i1b.max;
				real bz = (p0b - i1b.min_val) / (i1b.max_val - i1b.min_val);
				real bw = (p1b - i2b.min_val) / (i2b.max_val - i2b.min_val);
				kc_t fpb = kprime_interp0[k][i1b_min_base + i2b.min]
						* ((real)1.0 - bz) * ((real)1.0 - bw) +
					   kprime_interp0[k][i1b_min_base + i2b.max]
						* ((real)1.0 - bz) *              bw  +
					   kprime_interp1[k][i1b_max_base + i2b.min]
						*              bz  * ((real)1.0 - bw) +
					   kprime_interp1[k][i1b_max_base + i2b.max]
						*              bz  *              bw;
				hw_rail_values(&fpb, KMAX, KMIN);
				st_kcross[k][kidx] = fpb;
				curr_sum += fpb;
				kidx++;
				#if OUT_DEBUG_DATA
				curr_sum_cnt++;
				#endif
			}
		}



		#else
		small_idx_t ids_phase = IDS_PACK_FACTOR;
		// Loop 1.3: AST agents interp over kprime_interp
		for (int j = 0; j < N_AGENTS; ++j) {
			#pragma HLS pipeline
			// Read next packed idshock value when needed
			if (++ids_phase >= IDS_PACK_FACTOR) {
				curr_ids = idshock[idshock_idx++];
				ids_phase = 0;
			}
			// HW_LI_2D_BASE:
			#ifdef __SYNTHESIS__
			real p0b = curr_ids.get_bit(ids_phase) ? (real)1.0 : (real)0.0; 
			#else
			real p0b = (curr_ids & 0b1) ? (real)1.0 : (real)0.0;
			curr_ids >>= 1; 
			#endif

			real p1b = st_kcross[j];
			range_t i1b = hw_findrange(p0b, st_env.epsilon2, NSTATES_ID);
			range_t i2b = hw_findrange(p1b, st_env.k_egm, NKGRID);
			// quick can *128 and -28
			small_idx_t i1b_min_base = NKGRID * i1b.min; // always 0
			small_idx_t i1b_max_base = NKGRID * i1b.max; // always NKGRID
			real bz = (p0b - i1b.min_val) / (i1b.max_val - i1b.min_val);
			real bw = (p1b - i2b.min_val) / (i2b.max_val - i2b.min_val);
			/** Produces [1100 * 10,000]
			 * technically can replace i1b_min/max_base with 0 and NKGRID
			 * bz
			 * bw
			 */

			// real fv0 = kprime_interp0[i1b_min_base + i2b.min];
			// real fv1 = kprime_interp0[i1b_min_base + i2b.max];
			// real fv2 = kprime_interp1[i1b_max_base + i2b.min];
			// real fv3 = kprime_interp1[i1b_max_base + i2b.max];

			kc_t fpb = kprime_interp0[i1b_min_base + i2b.min]
						* ((real)1.0 - bz) * ((real)1.0 - bw) +
					   kprime_interp0[i1b_min_base + i2b.max]
						* ((real)1.0 - bz) *              bw  +
					   kprime_interp1[i1b_max_base + i2b.min]
						*              bz  * ((real)1.0 - bw) +
					   kprime_interp1[i1b_max_base + i2b.max]
						*              bz  *              bw;

			hw_rail_values(&fpb, KMAX, KMIN);
			st_kcross[j] = fpb;
			curr_sum += fpb;
		#if OUT_DEBUG_DATA
			curr_sum_cnt++;
		#endif
		}
		#endif
		// Update kmts for next iteration
		curr_kmts = (real) curr_sum * N_AGENTS_INV;
		hw_rail_values(&curr_kmts, KM_MAX, KM_MIN);
	} // end simulation steps
	st_kmts_init = curr_kmts;
	#if OUT_DEBUG_DATA
	*asts = curr_sum_cnt;
	#endif


	global_debug = 1;
	return;
    }
	
    void agents_egm(const real kmprime[NSTATES_AG * NKM_GRID])
    	#pragma HLS array_partition variable=st_kprimes complete dim=1
	#pragma HLS array_partition variable=st_env.P complete

	#if AST_UNROLL && ACCEL
	#pragma HLS array_partition variable=st_env.k complete dim=1
	#pragma HLS array_partition variable=st_env.epsilon2 complete dim=1
	#endif

	/** Lookup tables */
	static const small_idx_t li_2d_aux_idx_base[4] = {
		0,
		100,
		800,
		900
	};
	#pragma HLS array_partition variable=li_2d_aux_idx_base complete

	// Local kprime/new copies
	real kprime_new[NSTATES]; 
	real metric = 1;
	#if OUT_DEBUG_DATA
	unsigned int iter_cnt = 0;
	#endif
	
    while (metric > (real) TOLL_K) {
		#pragma HLS loop_tripcount min=20 avg=20 max=600

		#if SPREAD_SCALAR_TRICK
		real spread_scalar[2] = {VERY_SMALL_SCALAR, VERY_SMALL_SCALAR};
		#else
		real spread_scalar = VERY_SMALL_SCALAR;
		#endif

		#if EGM_ONE_LOOP
		// Reset index values for [1600] loop
		pidx_t p_idx_outer = 0b0100;
		small_idx_t hundreds_cnt = 100;
		small_idx_t kp_iter_cnt = (NSTATES_ID * NKGRID);
		small_idx_t kidx = 0;
		
		// ^^ Loop 1.1: 1600 * 4x LI2D over kprime[] --> kprime_new[]
		for (small_idx_t is = 0; is < NSTATES; ++is) {
			#if !SMALL_PL && ACCEL
			#pragma HLS pipeline
			#elif SMALL_PL_MAX_ACCEL && ACCEL
			#pragma HLS pipeline
			#endif
			
			pidx_t p_idx_inner = 0;
			emu_s_t emu_s = 0.;
			real kmp, temp_base;
			real kp = st_kprimes[8][is];

			// Index handling
			if (++kp_iter_cnt >= NSTATES_ID * NKGRID) {
				kp_iter_cnt = 0;
				kmp = kmprime[kidx++];
				temp_base = kmp * (real) env__l_bar_inv;
			}
			if (++hundreds_cnt >= 100) {
				hundreds_cnt = 0;
				p_idx_outer ^= (pidx_t) 0b0100;
			}
			if (is == (NKM_GRID * NSTATES_ID * NKGRID))
				p_idx_outer |= (pidx_t) 0b1000;
			p_idx_inner = 0;
			
			// 4x EGM 2D interpolation over kprime[]
			// Loop 1.1.1
			for (int iap = 0; iap < NSTATES_AG; ++iap) {
				real temp = temp_base * st_env.er_inv[iap];
				real irate = st_env.irate_factor[iap]
							 * hw_pow(temp, env__alpha_c);
				real imrt = env__delta_c + irate;
				real wage = st_env.wage_factor[iap] * hw_pow(temp, env__alpha);
				small_idx_t kpb = iap << 2;

				for (int iidp = 0; iidp < NSTATES_ID; ++iidp) {
					#if SMALL_PL && ACCEL
					#pragma HLS pipeline
					#endif

					#if OLD_INLINE_2D
					range_t i1 = hw_findrange(kmp, st_env.km_egm, NKM_GRID);
					range_t i2 = hw_findrange(kp, st_env.k_egm, NKGRID);
					small_idx_t idx_base = li_2d_aux_idx_base[p_idx_inner];
					small_idx_t i1_min_base = idx_base + (NSTATES_ID * NKGRID * i1.min);
					small_idx_t i1_max_base = idx_base + (NSTATES_ID * NKGRID * i1.max);

					real tz = (kmp - i1.min_val) / (i1.max_val - i1.min_val);
					real tw = (kp - i2.min_val) / (i2.max_val - i2.min_val);
	
					real fp = st_kprimes[kpb+0][i1_min_base + i2.min]
								* ((real)1.0 - tz) * ((real)1.0 - tw) +
							  st_kprimes[kpb+1][i1_min_base + i2.max]
								* ((real)1.0 - tz) *      tw  +
							  st_kprimes[kpb+2][i1_max_base + i2.min]
								*        tz  * ((real)1.0 - tw) +
							  st_kprimes[kpb+3][i1_max_base + i2.max]
								*        tz  *      tw;
					#else
					real fp = hw_li_2d_aux(kmp,
										   kp,
										   st_env.km_egm,
										   st_env.k_egm,
										   st_kprimes,
										   kpb,
										   li_2d_aux_idx_base[p_idx_inner]);
					#endif

					real cons2 = (imrt * kp
								 + wage * st_env.cons2_factor[p_idx_inner]
								 ) - fp;
					if (cons2 < 0)
						cons2 = CONS2_MIN;
					real mu2 = hw_pow(cons2, env__gamma_neg);
					emu_s += (emu_s_t) (st_env.P[p_idx_outer + p_idx_inner] * imrt * mu2);
					++p_idx_inner;
				}
			} // end 4x EGM interpolation over kprime[]
			// if (emu_s > 3.0)
			// 	printf("            ~~ emu_s %18.15lf\n", emu_s);
			// ~ Today (each 4x interpolate yields one new kprime value)
			real new_kp = st_env.wealth[is]
						  - hw_pow(env__beta * (real)emu_s, env__gamma_neg_inv);
			hw_rail_values(&new_kp, KMAX, KMIN);
			// ~ Convergence check
			
			real spread = fabs(new_kp - kp);
			#if SPREAD_SCALAR_TRICK
			#ifdef __SYNTHESIS__
			ap_uint<16> is_ap_uint = is;
			small_idx_t ssidx = is_ap_uint.get_bit(0);
			#else
			small_idx_t ssidx = is % 2;
			#endif
			if (spread > spread_scalar[ssidx])
				spread_scalar[ssidx] = spread;
			#else
			if (spread > spread_scalar)
				spread_scalar = spread;
			#endif
			kprime_new[is] = new_kp;
		}

    void sim_alm_coeff(input_t *in,
				   const real *kmts,
				   real *coeff,
				   double *metric,
				   real *R2)
    {
        small_idx_t agshock_idx = 0;
        small_idx_t ags_phase = AGS_PACK_FACTOR;
        shock_t curr_ags;
        shock_t curr_shock_val;
        real coeff_new[NCOEFF];

        // // # of bad aggregate shocks
        // int ndimbad=0;
        // // # of good aggregate shocks
        // int ndimgood=0;

        // for (int t=0; t<SIM_STEPS; t++) { // orig: NDISCARD to SIM_STEPS-1
        //     // Read new value when needed
        //     if (++ags_phase >= AGS_PACK_FACTOR) {
        //         curr_ags = in->agshock[agshock_idx++];
        //         ags_phase = 0;
        //     }
        //     curr_shock_val = curr_ags & 0b1;
        //     curr_ags >>= 1;
        //     // Discard outside of range
        //     if (t < NDISCARD || t >= SIM_STEPS - 2)
        //         continue;
        //     // Count
        //     if (curr_shock_val == 0)
        //         ndimbad++;
        //     else
        //         ndimgood++;
        // }
        
        double x_good_v[1000];
        double y_good_v[1000];
        double x_bad_v[1000];
        double y_bad_v[1000];
        
        int ibad = 0;
        int igood = 0;

        agshock_idx = 0;
        ags_phase = AGS_PACK_FACTOR;
        for (int t=0; t < SIM_STEPS; t++) {
            // Read new value when needed
            if (++ags_phase >= AGS_PACK_FACTOR) {
                curr_ags = in->agshock[agshock_idx++];
                ags_phase = 0;
            }
            curr_shock_val = curr_ags & 0b1;
            curr_ags >>= 1;
            // Discard
            if (t < NDISCARD || t >= SIM_STEPS - 2)
                continue;
            if (curr_shock_val == 0) {	
                y_bad_v[ibad] = log(kmts[t+1]);
                x_bad_v[ibad] = log(kmts[t]);
                ibad++;
            }
            else {
                y_good_v[igood] = log(kmts[t+1]);
                x_good_v[igood] = log(kmts[t]);
                igood++;
            }
        }

        double badcoeff[2] = {0, 0}; //initialize to prevent garbage values
        double goodcoeff[2] = {0, 0};

        matrixfunction(badcoeff, x_bad_v, y_bad_v, ibad);
        matrixfunction(goodcoeff, x_good_v, y_good_v, igood);

        real rbad = RSquaredCalc(badcoeff, x_bad_v, y_bad_v, ibad);
        real rgood = RSquaredCalc(goodcoeff, x_good_v, y_good_v, igood);
        
        coeff_new[0]=badcoeff[0]; //bb
        coeff_new[1]=badcoeff[1];
        coeff_new[2]=goodcoeff[0];
        coeff_new[3]=goodcoeff[1];
        R2[0] = rbad;
        R2[1] = rgood;
        
        // Update metric for convergence test
        #if (CC_NORM==1)
        double norm = 0.;
        for (int ib = 0; ib < NCOEFF; ++ib)
            norm += (coeff_new[ib] - coeff[ib])
                    * (coeff_new[ib] - coeff[ib]);
        *metric = sqrt(norm);

        #elif (CC_NORM==2)
        double spread_coeff = VERY_SMALL_SCALAR;
        for (int ib = 0; ib < NCOEFF; ++ib)
        {
            double spread_coeff_actual = fabs(coeff_new[ib] - coeff[ib]);
            if (spread_coeff_actual > spread_coeff)
                spread_coeff = spread_coeff_actual;
        }
        *metric = spread_coeff;
        #endif

        // Update ALM coefficients vector
        for (int ib = 0; ib < NCOEFF; ++ib)
            coeff[ib] = coeff_new[ib] * UPDATE_B + coeff[ib] * (1. - UPDATE_B);
        return;
    }

    void runOnfpga(output_t outputs, input_t input, env_t env, vars_t vars, perf_ctr perf[4], int len, int num_iter) {
    #pragma HLS INTERFACE ap_ctrl_chain port=return            bundle=control
    #pragma HLS INTERFACE m_axi         port=outputs     bundle=maxiport0   offset=slave
    #pragma HLS INTERFACE m_axi         port=inputs      bundle=maxiport1   offset=slave
        #pragma HLS DATAFLOW
        hls::stream<unsigned char> data_from_gmem;
        // Burst write 512-bit values to global memory over AXI interface -- just streaming in to out for testing purposes. 
        /********* need to do a size check ************/
        hls_stream::buffer(data_from_gmem, inputs, len);
        hls_stream::buffer(outputs, data_from_gmem, len);
        double metric_coeff = pow(10., 10.);
	    int iter_coeff = 0;
        real R2[2];
        
        while (metric_coeff > TOLL_COEFF && (iter_coeff < num_iter && num_iter > 0)) {


            // egm
		agents_egm(vars.kprime);
            // ast
		sim_ast(inputs.agshock, inputs.idshock, vars.kmts);

            // perf_ctr_start(&perf[2]);
            // perf_ctr_update(&perf[2]);
            // perf_ctr_start(&perf[3]);
            sim_alm_coeff(&in, vars.kmts, vars.coeff, &metric_coeff, R2);
            // perf_ctr_update(&perf[3]);
            iter_coeff = iter_coeff+1;

        } // End of model convergence loop

        outputs.metric_coeff = metric_coeff;
        outputs.coeff[0] = vars.coeff[0];
        outputs.coeff[1] = vars.coeff[1];
        outputs.R2[0] = R2[0];
        outputs.coeff[2] = vars.coeff[2];
        outputs.coeff[3] vars.coeff[3];
        outputs.R2[1] = R2[1];
        //add cross sectional kcross
        //add kmts
        //metric from egm
        //new kp from egm
        //itercoeff

        /** Performance metrics */
        // perf_ctr_update(&pctr_main);
        // printf("\n**\nMain Loop: %f s, %f cyc/iter, (%d iters)\n",
        //     get_pctr_secs(pctr_main),
        //     get_pctr_avg_cycles(pctr_main),
        //     pctr_main.iters);
        // printf("SW init time: %f s, %f cyc/iter, (%d iters)\n",
        //     get_pctr_secs(pctr_init),
        //     get_pctr_avg_cycles(pctr_init),
        //     pctr_main.iters);
        // printf("Main loop PL time: %f s, %f cyc/iter, (%d iters)\n",
        //     get_pctr_secs(pctr_hw),
        //     get_pctr_avg_cycles(pctr_hw),
        //     pctr_hw.iters);
        // printf("Main loop PS time: %f s, %f cyc/iter, (%d iters)\n",
        //     get_pctr_secs(pctr_alm_coeff),
        //     get_pctr_avg_cycles(pctr_alm_coeff),
        //     pctr_alm_coeff.iters);

        /** Exit (free memory) */
        return 0;
    }

}
