#pragma once

#ifdef __SDSCC__
#include "sds_lib.h"
#define U96_CYCLES_PER_SEC 1200000000 // 1.2e9
#endif

#include <time.h>

typedef struct perf_ctr {
	clock_t start;
	unsigned long long cycles;
	unsigned long long last;
	unsigned int iters;
} perf_ctr;

#define INIT_PERF_CTR { 0, 0, 0 }

void perf_ctr_start(perf_ctr *pctr);
void perf_ctr_update(perf_ctr *pctr);
double get_pctr_secs(const perf_ctr pctr);
double get_pctr_avg_cycles(const perf_ctr pctr);
double get_pctr_last_secs(const perf_ctr pctr);
