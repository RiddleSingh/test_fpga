#include "perf.h"

/**
 * perf_time_start:
 *  Resets starting point
 */
void perf_ctr_start(perf_ctr *pctr) {
#ifdef __SDSCC__
	pctr->start = sds_clock_counter();
#else
	pctr->start = clock();
#endif
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
	#ifdef __SDSCC__
		pctr->last = (sds_clock_counter() - pctr->start);
	#else
		pctr->last = (unsigned long long)(clock() - pctr->start);
	#endif
		pctr->cycles += pctr->last;
		pctr->iters += 1;
	}
	return;
}

/**
 * get_pctr_time:
 *  Converts perf_ctr struct to time
 */
double get_pctr_secs(const perf_ctr pctr) {
#ifdef __SDSCC__
	return (double)(pctr.cycles) / U96_CYCLES_PER_SEC;
#else
	return (double)(pctr.cycles) / CLOCKS_PER_SEC;
#endif
}

double get_pctr_last_secs(const perf_ctr pctr) {
#ifdef __SDSCC__
	return (double)(pctr.last) / U96_CYCLES_PER_SEC;
#else
	return (double)(pctr.last) / CLOCKS_PER_SEC;
#endif
}

/**
 * get_pctr_avg_cycles:
 *  Gets average cycles per iteration
 */
double get_pctr_avg_cycles(const perf_ctr pctr) {
	return (double)(pctr.cycles) / (pctr.iters);
}
