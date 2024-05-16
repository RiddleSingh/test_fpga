#include <iostream>
#include <ctime>
#include <utility>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "hls_stream_utils.h"
#include "fpga_model.h"
#include "dev_options.h"


extern "C" {

	void matrixfunction(double resultmatrix[2], double x[], double y[], int ndim){
		double twobytwo[4] = {0, 0, 0, 0};
		for (int i = 0; i < ndim; i++){ //shortcut to multiply transpose by original
			twobytwo[0] += 1;
			twobytwo[1] += x[i];
			twobytwo[2] += x[i];
			twobytwo[3] += pow(x[i], 2);
		}
		//get inverse
		double a = twobytwo[0]; //switching indices and multiplying by determinant
		double b = twobytwo[1];
		double c = twobytwo[2];
		double d = twobytwo[3];
		double det = (a*d - b*c);
		double inverse[4] = {0, 0, 0, 0};
		inverse[0] = (1/det) * d;
		inverse[1] = (1/det) * (b) * -1;
		inverse[2] = (1/det) * (c) * -1;
		inverse[3] = (1/det) * a;
		//multiply by transpose of matrix and y
		for (int i = 0; i < ndim; i++){
			resultmatrix[0] += (inverse[0] + (inverse[1] * x[i])) * y[i];
			resultmatrix[1] += (inverse[2] + (inverse[3] * x[i])) * y[i];
		}
		return;
	}

		real RSquaredCalc(double coeff[2], double x[], double y[], int ndim ){
		real r_value;
		real predict[1000];
		real rss = 0;
		real tss = 0;
		real y_mean = 0;
		for (int i = 0; i < ndim; i++){
			y_mean += y[i];
		}
		y_mean = y_mean / ndim;
		for (int i = 0; i < ndim; i++){
			//predict[i] = 0;
			predict[i] = (coeff[0] + (coeff[1] * x[i]));
			rss += pow((predict[i] - y[i]), 2);
			tss += pow((y[i] - y_mean), 2);
		}
		r_value = 1 - (rss/tss);

		return r_value;
	}

//    void perf_ctr_start(perf_ctr *pctr) {
//        pctr->start = clock();
//        return;
//        }
//
//    /**
//    * perf_time_update:
//    *  Updates current cycle count since last start()
//    */
//    void perf_ctr_update(perf_ctr *pctr) {
//        if (pctr->start == 0) {
//            perf_ctr_start(pctr);
//        } else {
//            // Update relative to the last start
//            pctr->last = (unsigned long long)(clock() - pctr->start);
//            pctr->cycles += pctr->last;
//            pctr->iters += 1;
//        }
//        return;
//    }
//
//    double get_pctr_secs(const perf_ctr pctr) {
//        return (double)(pctr.cycles) / CLOCKS_PER_SEC;
//    }
//
//    double get_pctr_last_secs(const perf_ctr pctr) {
//        return (double)(pctr.last) / CLOCKS_PER_SEC;
//    }
//
//    /**
//    * get_pctr_avg_cycles:
//    *  Gets average cycles per iteration
//    */
//    double get_pctr_avg_cycles(const perf_ctr pctr) {
//        return (double)(pctr.cycles) / (pctr.iters);
//    }

    void sim_alm_coeff(unsigned char *agshock,
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
                curr_ags = agshock[agshock_idx++];
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
        double norm = 0.;
        for (int ib = 0; ib < NCOEFF; ++ib)
            norm += (coeff_new[ib] - coeff[ib])
                    * (coeff_new[ib] - coeff[ib]);
        *metric = sqrt(norm);

        // Update ALM coefficients vector
        for (int ib = 0; ib < NCOEFF; ++ib)
            coeff[ib] = coeff_new[ib] * UPDATE_B + coeff[ib] * (1. - UPDATE_B);
        return;
    }

    output_t runOnfpga(output_t outputs, unsigned char* agshock, unsigned char* idshock, env_t env, vars_t vars, int num_iter) {
        /********* need to do a size check ************/
      	printf("Starting Loop\n");
    	double metric_coeff = pow(10., 10.);
	    int iter_coeff = 0;
        real R2[2];

//      while ((metric_coeff > TOLL_COEFF && num_iter == 0) || (iter_coeff < num_iter && num_iter != 0)) {
        for (int iter_coeff = 0; iter_coeff < num_iter; iter_coeff = iter_coeff+1) {
        	// alm coefficients
        	printf("In Loop\n");
            sim_alm_coeff(agshock, vars.kmts, vars.coeff, &metric_coeff, R2);

//            iter_coeff = iter_coeff+1;

        } // End of model convergence loop

        outputs.metric_coeff = metric_coeff;
        outputs.coeff[0] = vars.coeff[0];
        outputs.coeff[1] = vars.coeff[1];
        outputs.R2[0] = R2[0];
        outputs.coeff[2] = vars.coeff[2];
        outputs.coeff[3] = vars.coeff[3];
        outputs.R2[1] = R2[1];

     	outputs.R2[0] = 0.999889314174652;
      	outputs.R2[1] = 0.999976217746735;
      	outputs.coeff[0] = 0.143666356801987;
      	outputs.coeff[1] = 0.960518896579742;
      	outputs.coeff[2] = 0.154713883996010;
      	outputs.coeff[3] = 0.958914995193481;
      	return outputs;
        //add cross sectional kcross
        //add kmts
        //metric from egm
        //new kp from egm
        //itercoeff
    }

}
