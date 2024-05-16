#pragma once
#include <math.h>

#include "definitions.h"
#include "perf.h"

void sim_alm_coeff(input_t *in,
				   const real *kmts,
				   real *coeff,
				   double *metric,
				   real *R2);
