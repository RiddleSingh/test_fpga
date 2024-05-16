The Main Development will happen in the repository

## Building
This repository includes headers for GSL-1.15, but the actual library must be installed. 

On linux:

```sudo apt-get install libgsl-dev```  

On Mac OS using Homebrew:  

```brew install gsl```

The headers for the installed GSL library should be used and the binaries linked to.

## Profiling
The C code is profiled using `clock()`, which does not have fine-grain accuracy but can express relative differences well on the order of seconds.  

To enable detailed profiling, edit the value of `#define DETAILED_PERF_METRICS` to 1 (from 0) and compile. This will accumulate `clock_t` cycle counts for each of the modules within the outer loop over each iteration, until convergence. Note that this adds considerable (many seconds) overhead to the overall runtime.  

Sample profile output:  
````
baseline (single counter): ~151.3s
baseline (+detailed perf_ctr overhead): ~161.0s

Details:
 - ALM: ~0.0s (2 avg cyc/iter)
 - AgentsEGM: ~58.1s (1M avg cyc/iter)
 - AggST: ~105.3s (2M avg cyc/iter)
 - ALM_Coeff: ~0.0s (126 avg cyc/iter)
````

Based on this, further metrics were measured on software via named loops in EGM and ST. Dominant effects are in linear interpolation functions. 
````
---- EGM perf_ctrs (secs, cycles/iter, iters) -----
  f: 104.802959 s, 0.909066 cyc/iter, (115286400 iters)
  g: 0.287015 s, 7.966664 cyc/iter, (36027 iters)

---- ST perf_ctrs (secs, cycles/iter, iters) -----
  b: 1.576092 s, 27.034168 cyc/iter, (58300 iters)
  c: 5.394431 s, 0.925288 cyc/iter, (5830000 iters)
  d: 94.985908 s, 1629.260858 cyc/iter, (58300 iters)
  e: 1.311091 s, 22.488696 cyc/iter, (58300 iters)
````

## Detailed profiling on hardware using clock()
````
# Good State Best Fit: logK' =  0.155376016818807 +  0.958724757084258 logk
# Bad State Best Fit: logK' =  0.145957031741170 +  0.959893458537024 logK
Iteration: 53 Spread: 0.000000007 Regression Coeff:  0.145957026923899  0.959893459816537  0.155376016260257  0.958724757234533

Model n. 0: Execution Time: 1752.666715 seconds Solution Time 1752.666715

---- EGM perf_ctrs (secs, cycles/iter, iters) -----
  f: 502.476901 s, 4.358510 cyc/iter, (115286400 iters)
  g: 4.249639 s, 117.957060 cyc/iter, (36027 iters)

---- ST perf_ctrs (secs, cycles/iter, iters) -----
  b: 12.743290 s, 218.581304 cyc/iter, (58300 iters)
  c: 33.850844 s, 5.806320 cyc/iter, (5830000 iters)
  d: 1007.101227 s, 17274.463585 cyc/iter, (58300 iters)
  e: 0.000000 s, nan cyc/iter, (0 iters)
````
```
---- perf_ctrs (secs, cycles/iter, iters) -----
  alm: 0.000585 s, 10.833333 cyc/iter, (54 iters)
  agents_egm: 536.829296 s, 9941283.259259 cyc/iter, (54 iters)
  agg_st: 1059.975760 s, 19629180.740741 cyc/iter, (54 iters)
  alm_coeff: 0.178400 s, 3303.703704 cyc/iter, (54 iters)
````

## Questions
- Capital Distribution in main.c

## EGM / ST quick summary
** AgentsEGM **
While not converged:
  - Loop A: Use full kprime[1600] to produce kprimen[j] in order  
		> Full unroll should be possible (kprime, kmprime sequential)
		> (Deep?) Pipeline possible here: mostly reading constants, then feeding into interp2Daux
  - Loop G: swap old, new kprime (unroll potential, otherwise depends on A)
		> Should just swap pointers here
  - Update convergence metric  

** AggST ** 
Foreach timestep:
  - Loop B: sum-reduce of full kcross[10,000] to produce kmts[t] 
		> Think this can be eliminated
  - Loop C: use single kmts[t], full kprime[1600] to produce kprime_interp[j] in order for j in [0, 200)
		> Full unroll seems possible
  - Loop D: use full kprime_interp[200], and in-order kcross[k] to update itself in order (via kcross1 currently)
		> Full unroll seems *possible* though not practical for 10,000 iterations
		> Have the sum-reduce here

	This is like:
		Swap pointers?

		kmts_t --> [loop C] --> kprime_interp[200] --> [loop D] --> kmts_t'

		kmts_t only used on one of the dimensions of linear_interp4D; p[1] search in km[] array which is ... constant?
		And km[] only has four elements in it.


Note: can also just start summing when new kcross is produced (and there is an initial sum of the default values, which are all kss)

Seems like the only output from AggST is the kmts[t] vector for each timestep? And kcross for itself.

## AgentsEGM : produce a converged kprime[1600]
````
while (!converge)
{
	// Loop A
	for (ia in 0..NSTATES_AG==2)
	{
		// Loop B
		for (ikm in 0..NKM_GRID==4)
		{
			// Loop C
			for (iid in 0..NSTATES_ID==2)
			{
				// Loop D
				for (ik in 0..NKGRID==100)
				{
					REAL kprime = kprime[idx]
					REAL EMUs = 0.;
					// Loop E
					for (iap in 0..NSTATES_AG==2)
					{
						// Loop F
						for (iidp in 0..NSTATES_ID==2)
						{
							linear_interp2Daux
							/**
							 * Reads:
							 *	kprime[1600] (M_->f) RANDOM
							 *	>> findrange: epsilon2[2], k[100]
							 * Writes:
							 *	kprime2_interpolated
							 *
							 * This loop can be fully unrolled w/ sum reduce for EMUs +=
							 */
						}
					}
					// "Today"
					REAL consn = fn(EMUs)
					kprimen[idx] = fn(consn)
					// Check convergence
					REAL spread = fabs(kprimen[idx] - kprime[idx])
					spread_scalar = (spread > spread_scalar) ? spread : spread_scalar;
				}
			}
		}
	} // --> produces in order kprimen[1600], spread_scalar
	// each of the new kprimen[idx] values depends on complete old kprime[1600]

	// Loop G ("update guess")
	// Natural stream consumer of Loop A
	for (is in 0..NSTATES==1600)
	{
		// cons  = fn(kprime[is]) ???
		kprime[is] = fn(kprime[is], kprimen[is])
	} // --> produces kprime[1600] in order for use in next step (until convergence)
	// This should be rotating buffer instead, and this can be unrolled

	// Update convergence metric
	// This depends on Loop A running to completion
	metric = spread_scalar
}
````
Variable reference
````
kmprime[8]
kprime[1600]

trate[2]		Constant init
ag[2]			Constant init
er[2]			Constant init
epsilon[2]		Constant init
P[16]			Constant init
wealth[1600]	Constant init

cons[1600]		Seems unnecessary
kprime[1600]	Loop G in EGM produces until convergence; prealloc'd but updated
kprimen[1600]	Do not need: replace with streaming
````

## AggregateST : using converged kprime[1600], 
````
	// Loop A
	for (t in 0..T==1100) // simulation timesteps
	{
		// Loop B
		for (j = 0..N_AGENTS==10,000)
		{
			mean += kcross[j];
		} // this produces a sum

		// Calculate actual mean w/ railing
		kmts[t] = mean/N_AGENTS; 

		// Loop C
		for (ik = 0..NKGRID==100)
		{
			// Loop C.2
			for (iid = 0..NSTATES_ID==2)
			{
				linear_interp4D
				/**
				 * Reads:
				 *	kprime[1600] (M_->f) RANDOM
				 *	agshock[t] (read from file)
				 *	kmts[t] 
				 *	epsilon2[iid] (constant init)
				 *	k[ik]
				 *	>> findrange: ag2[2], km[4], epsilon2[2], k[100]
				 * Writes:
				 *	kprime_interp[idx]
				 */
			}
		} // --> produces whole kprime_interp[200]

		// Loop D
		for (j = 0..N_AGENTS==10,000)
		{
			linear_interp2D
			/**
			 * Reads:
			 *	kprime_interp (M2_->f) RANDOM
			 *	idshock[IX(t,j)]
			 *	kcross[j]
			 *	>> findrange: epsilon2[2], k[100]
			 * Writes:
			 *	kcross1[j] (railing)
		} // --> produces kcross1 in order; consumes kcross[j] in order ** NATURAL STREAMING **

		// Loop E
		for (j = 0..N_AGENTS==10,000)
		{
			kcross[j] = kcross1[j]
		} // --> produces whole kcross[j] (should be pointer swap)
	}
````
Variable reference
````
agshock[1100]			Constant init
epsilon2[2]				Constant init
k[100]					Constant init (in InitK)
kmts[1100]				Used in ALMCoeff
kprime_interp[200]		Local only to AggST, dependency across timesteps

idshock[1100 * 10,000]	Read from file
kcross[10,000]			Initialized but changes
````

## Reference of interpolation objects
````
    // Interpolation Matrix for Agents Problem
    Parms_->M4_.x1 = Parms_->ag2;
    Parms_->M4_.d1 = NSTATES_AG == 2
    Parms_->M4_.x2 = Parms_->km;
    Parms_->M4_.d2 = NKM_GRID == 4
    Parms_->M4_.x3 = Parms_->epsilon2;
    Parms_->M4_.d3 = NSTATES_ID == 2
    Parms_->M4_.x4 = Parms_->k;
    Parms_->M4_.d4 = NKGRID == 100
    Parms_->M4_.f  = Parms_->kprime;
    
    Parms_->M2_.x1 = Parms_->epsilon2;
    Parms_->M2_.d1 = NSTATES_ID; ==2 @@ can simplify
    Parms_->M2_.x2 = Parms_->k;
    Parms_->M2_.d2 = NKGRID; == 100
    Parms_->M2_.f  = Parms_->kprime_interp;
    
    Parms_->I4_.x1 = Parms_->ag2;
    Parms_->I4_.d1 = NSTATES_AG;
    Parms_->I4_.x2 = Parms_->km;
    Parms_->I4_.d2 = NKM_GRID;
    Parms_->I4_.x3 = Parms_->epsilon2;
    Parms_->I4_.d3 = NSTATES_ID;
    Parms_->I4_.x4 = Parms_->k;
    Parms_->I4_.d4 = NKGRID;
    Parms_->I4_.f  = Parms_->V;
    
    Parms_->I2_.x1 = Parms_->km;
    Parms_->I2_.d1 = NKM_GRID; == 4
    Parms_->I2_.x2 = Parms_->k;
    Parms_->I2_.d2 = NKGRID; == 100
    Parms_->I2_.f  = Parms_->kprime;
    
    Parms_->I1_.x1 = Parms_->km;
    Parms_->I1_.d1 = NKM_GRID;
    Parms_->I1_.f  = Parms_->V;
````

KC_Unroll test:

# Good State Best Fit: logK' =  0.155376016818807 +  0.958724757084258 logk
# Bad State Best Fit: logK' =  0.145957031741170 +  0.959893458537024 logK
Iteration: 53 Spread: 0.000000007 Regression Coeff:  0.145957026923899  0.959893459816537  0.155376016260257  0.958724757234533

Without unroll:
# Good State Best Fit: logK' =  0.155376016818807 +  0.958724757084258 logk
# Bad State Best Fit: logK' =  0.145957031741170 +  0.959893458537024 logK
Iteration: 53 Spread: 0.000000007 Regression Coeff:  0.145957026923899  0.959893459816537  0.155376016260257  0.958724757234533


// Look into using NEON 
// Can it do multiple single precision (are there 4 single precision floating point units?)