#pragma once


#ifdef __SDSCC__
#include "sds_lib.h"
#define U96_CYCLES_PER_SEC 1200000000 // 1.2e9
#endif

#include <time.h>

/**
 * Options
 */
#define DETAILED_PERF_METRICS 1
#define PERF_SPECIAL 0

/**
 * C Performance counters implementation
 */
typedef struct perf_ctr {
#ifdef __SDSCC__
	unsigned long long start;
#else
	clock_t start;
#endif
    unsigned long long cycles;
    unsigned int iters;
} perf_ctr;

#define INIT_PERF_CTR { 0, 0, 0 }

// GLobal counters
perf_ctr pctr_b = INIT_PERF_CTR;
perf_ctr pctr_c = INIT_PERF_CTR;
perf_ctr pctr_d = INIT_PERF_CTR;
perf_ctr pctr_e = INIT_PERF_CTR;
perf_ctr pctr_f = INIT_PERF_CTR;
perf_ctr pctr_g = INIT_PERF_CTR;

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
	if (pctr->start == 0)
	{
		perf_ctr_start(pctr);
	}
	else
	{
		// Update relative to the last start
	#ifdef __SDSCC__
		pctr->cycles += (sds_clock_counter() - pctr->start);
	#else
		pctr->cycles += (unsigned long long)(clock() - pctr->start);
	#endif
		pctr->iters += 1;
	}
    return;
}

/**
 * get_pctr_time:
 *  Converts perf_ctr struct to time
 */
REAL get_pctr_secs(const perf_ctr pctr) {
#ifdef __SDSCC__
	return (REAL)(pctr.cycles) / U96_CYCLES_PER_SEC;
#else
    return (REAL)(pctr.cycles) / CLOCKS_PER_SEC;
#endif
}

/**
 * get_pctr_avg_cycles:
 *  Gets average cycles per iteration
 */
REAL get_pctr_avg_cycles(const perf_ctr pctr) {
    return (REAL)(pctr.cycles) / (pctr.iters);
}