#pragma once
#include <math.h>
#include <gsl/gsl_multifit.h>
#include <gsl/gsl_statistics_double.h>

#include "definitions.h"
#include "perf.h"

void sim_alm_coeff(input_t *in,
				   const real *kmts,
				   real *coeff,
				   double *metric,
				   real *R2);