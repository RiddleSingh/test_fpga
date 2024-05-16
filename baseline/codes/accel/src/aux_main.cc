/**
 * Auxiliary main() functions
 *	SDx won't play nice with multiple main in same src
 *	So for now, 'make aux'
 */
#include "app.h"
#include "init.h"
#include <math.h>


// env.alpha * env.ag[2]
// (1. - env.alpha) * env.ag[2]
// ((1.-taxrate) 
	//    * env.epsilon[iidp]
	//    * env.l_bar
	//    + (1. - env.epsilon[iidp])
	//    * env.mu)

int aux_init()
{
	env_t env;
	input_t in;
	vars_t vars;

	init_all(&env, &in, &vars);
	printf("** kprime: \n");
	for (int i = 0; i < NSTATES; i++)
		printf("%.17g\n", vars.kprime_a[i]);

	printf("** kcross: \n");
	printf("%.17g\n", vars.kcross_a[0]);

	real increment;
	printf("** linear_space: km\n");
	increment = (real) ((KM_MAX) - (KM_MIN)) / (NKM_GRID - 1);
	for (int i = 0; i < NKM_GRID; ++i)
		printf("%.17g\n", KM_MIN + i * increment);

	real x[NKGRID];
	real y[NKGRID];
	real xmin = 0;
	real xmax = 0.5;
	increment = (real) ((xmax - xmin)) / (NKGRID - 1);
	printf("** linear_space: k\n");
	for (int i = 0; i < NKGRID; ++i)
		x[i] = xmin + i * increment;
	for (int i = 0; i < NKGRID; ++i) {
		y[i] = pow(x[i], 7.) / pow(x[NKGRID - 1], 7.);
		printf("%.17g\n", KMIN + (KMAX - KMIN) * y[i]);
	}



	printf("** env.km:\n");
	for (int i = 0; i < NKM_GRID; ++i)
		printf("%.17g\n", log(env.km[i]));

	printf("** wealth:\n");
	for (int i = 0; i < NSTATES; ++i)
		printf("%.17g\n", env.wealth[i]);
	
	printf("** pow(10., -10.):\n");
	printf("%.17g\n", pow(10., -10.));

	printf("** irate_factor\n");
	for (int i = 0; i < NSTATES_AG; ++i)
		printf("%.17g\n", env.alpha * env.ag[i]);

	printf("** wage_factor\n");
	for (int i = 0; i < NSTATES_AG; ++i)
		printf("%.17g\n", (1. - env.alpha) * env.ag[i]);
	
	printf("** cons2_factor\n");
	for (int i = 0; i < NSTATES_AG; ++i)
		for (int j = 0; j < NSTATES_ID; ++j)
			printf("%.17g\n", ((1.-env.trate[i]) 
								 * env.epsilon[j]
								 * env.l_bar
								 + (1. - env.epsilon[j])
								 * env.mu));

	return 0;
}

#ifndef __SDSCC__
int main()
{
	aux_init();
	return 0;
}
#endif