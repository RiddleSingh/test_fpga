#include "hw.h"

// void hw_init_kcross(const hw_env_t &env, real kcross[N_AGENTS])
// {
// 	for (int j = 0; j < N_AGENTS; ++j)
// 		kcross[j] = env.kss;
// 	return;
// }

// void hw_init_prealloc(hw_env_t &env, real kprime[NSTATES])
// {
// 	real taxrate, irate, wage;
// 	real q, p;
// 	real pow_base;
// 	small_idx_t idx = 0;
// 	small_idx_t cidx_base = 0;

// 	for (int ia = 0; ia < NSTATES_AG; ++ia) {
// 		#pragma HLS pipeline
// 		q = env.er_inv[ia] * env.l_bar_inv;
// 		taxrate = q * env.mu * (1. - env.er[ia]);
// 		env.irate_factor[ia] = env.alpha * env.ag[ia];
// 		env.wage_factor[ia] = (1. - env.alpha) * env.ag[ia];

// 		for (int iid = 0; iid < NSTATES_ID; ++iid) {
// 			p = ((1. - taxrate) * env.epsilon[iid] * env.l_bar)
// 				+ ((1. - env.epsilon[iid]) * env.mu);
// 			env.cons2_factor[idx++] = p;
// 		}
		
// 		idx = 0;
// 		for (int ikm = 0; ikm < NKM_GRID; ++ikm) {
// 			env.log_env_km[ikm] = log(env.km[ikm]);
// 			pow_base = env.km[ikm] * q;
// 			irate = env.alpha 
// 					* env.ag[ia]
// 					* powf(pow_base, env.alpha - 1);
// 			wage = (1. - env.alpha)
// 				   * env.ag[ia]
// 				   * powf(pow_base, env.alpha);
// 			for (int iid = 0; iid < NSTATES_ID; ++iid) {
// 				p = env.cons2_factor[cidx_base + iid];
// 				for (int ik = 0; ik < NKGRID; ++ik) {
// 					env.wealth[idx] = (irate + 1. - env.delta)
// 									  * env.k[ik]
// 									  + wage
// 									  * p;
// 					kprime[idx] = 0.9 * env.k[ik];
// 				}
// 			}
// 		}
// 		cidx_base += 2; // roll over for ia==1
// 	}
// 	return;
// }

/**
 * hw_init_all
 */
// void hw_init_all(hw_env_t &env,
// 				 real kprime[NSTATES],
// 				 real kcross[N_AGENTS])
// {
// 	hw_init_env(env);
// 	hw_init_kcross(env, kcross);
// 	hw_init_prealloc(env, kprime);
// }

/**
 * hw_init_precalc
 */


