#include "simulation.h"
#include "hw.h"

/**
 * Updates coeff, calcs R2, updates convergence metric
 */
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

	// # of bad aggregate shocks
	int ndimbad=0;
	// # of good aggregate shocks
	int ndimgood=0;

	for (int t=0; t<SIM_STEPS; t++) { // orig: NDISCARD to SIM_STEPS-1
		// Read new value when needed
		if (++ags_phase >= AGS_PACK_FACTOR) {
			curr_ags = in->agshock[agshock_idx++];
			ags_phase = 0;
		}
		curr_shock_val = curr_ags & 0b1;
		curr_ags >>= 1;
		// Discard outside of range
		if (t < NDISCARD || t >= SIM_STEPS - 2)
			continue;
		// Count
		if (curr_shock_val == 0)
			ndimbad++;
		else
			ndimgood++;
	}
	
	double SSRgood,SSRbad;
	gsl_matrix *xgood = gsl_matrix_alloc(ndimgood, REGRESSORS);
	gsl_matrix *xbad  = gsl_matrix_alloc(ndimbad, REGRESSORS);
	gsl_vector *ygood = gsl_vector_alloc(ndimgood);
	gsl_vector *ybad  = gsl_vector_alloc(ndimbad);
	//gsl_vector *wgood = gsl_vector_alloc(ndimgood);
	//gsl_vector *wbad  = gsl_vector_alloc(ndimbad);
	gsl_vector *cgood = gsl_vector_alloc(REGRESSORS);
	gsl_vector *cbad = gsl_vector_alloc(REGRESSORS);
	gsl_matrix *covgood = gsl_matrix_alloc(REGRESSORS, REGRESSORS);
	gsl_matrix *covbad = gsl_matrix_alloc(REGRESSORS, REGRESSORS);
	
	//c = gsl_vector_alloc(REGRESSORS);
	//cov = gsl_matrix_alloc(REGRESSORS, REGRESSORS);
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
			gsl_vector_set(ybad, ibad, log(kmts[t+1]));
			//gsl_vector_set(wbad, ibad, 1.0);
			gsl_matrix_set(xbad, ibad, 0, 1.0);
			gsl_matrix_set(xbad, ibad, 1, log(kmts[t]));
			ibad++;
		}
		else {
			gsl_vector_set(ygood, igood, log(kmts[t+1]));
			//gsl_vector_set(wgood, igood, 1.0);
			gsl_matrix_set(xgood, igood, 0, 1.0);
			gsl_matrix_set(xgood, igood, 1, log(kmts[t]));
			igood++;
		}
	}
	
	double TSSgood = gsl_stats_tss(ygood->data,
								   ygood->stride,
								   ygood->size);
	gsl_multifit_linear_workspace *workgood;
	workgood = gsl_multifit_linear_alloc(ndimgood, REGRESSORS);
	gsl_multifit_linear(xgood, ygood, cgood, covgood, &SSRgood, workgood);
	gsl_multifit_linear_free(workgood);
	
	double TSSbad = gsl_stats_tss(ybad->data,ybad->stride,ybad->size);
	gsl_multifit_linear_workspace *workbad;
	workbad = gsl_multifit_linear_alloc(ndimbad, REGRESSORS);
	gsl_multifit_linear(xbad, ybad, cbad, covbad, &SSRbad, workbad);
	gsl_multifit_linear_free(workbad);
	
	coeff_new[0]=gsl_vector_get(cbad, 0);
	coeff_new[1]=gsl_vector_get(cbad, 1);
	coeff_new[2]=gsl_vector_get(cgood, 0);
	coeff_new[3]=gsl_vector_get(cgood, 1);
	
	R2[0] = 1- SSRbad / TSSbad;
	R2[1] = 1- SSRgood / TSSgood;
	
	#if VERBOSE
	// maybe @todo: change function signature to avoid printing within this fn
	printf("\n# SSRb/g [%18.15lf, %18.15lf]\n# TSSb/g [%18.15lf, %18.15lf]\n",
		   SSRbad, SSRgood, TSSbad, TSSgood);
	printf("# Good State Best Fit: logK' = %18.15lf + %18.15lf logk,  R2 = %18.15lf\n",
		   gsl_vector_get(cgood,0), gsl_vector_get(cgood,1), R2[1]);
	printf("# Bad  State Best Fit: logK' = %18.15lf + %18.15lf logK,  R2 = %18.15lf\n",
		   gsl_vector_get(cbad,0), gsl_vector_get(cbad,1), R2[0]);
	#endif
	
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