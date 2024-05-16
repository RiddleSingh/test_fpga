//
//  main.c
//  fpga-ks-c
//
//  Created by Alessandro Peri on 08/09/2019.
//  Copyright © 2019 Alessandro Peri. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "usr_decl.h"
#include "auxiliary.h"
#include <time.h>
#include "perf.h"


/**
 * Main loop
 */
int main(){
    
    printf("Replication for 'Solving the incomplete markets model with aggregate uncertainty using the Krusell-Smith algorithm from the special JEDC issue edited by Den Haan, Judd and Juillard (2008)\n");
    FILE* cfile = fopen("checks/kcross.txt", "w");
    fclose(cfile);
    cfile = fopen("checks/kmprime.txt", "w");
    fclose(cfile);
    cfile = fopen("checks/kprime.txt", "w");
    fclose(cfile);
    cfile = fopen("checks/mean.txt", "w");
    fclose(cfile);
    cfile = fopen("checks/r2bg.txt", "w");
    fclose(cfile);
    cfile = fopen("checks/coeffs.txt", "w");
    fclose(cfile);

    // Definition of Parameters Types
    Parameters_type Parms_;
    Parms_.ur  = (REAL *) malloc(NSTATES_AG*sizeof(REAL));
    Parms_.er  = (REAL *) malloc(NSTATES_AG*sizeof(REAL));
    InitParms(&Parms_);
    
    // Initialization Transition Matrix
    Parms_.P = (REAL *) malloc(16*sizeof(REAL));
    InitTransition(&Parms_);
    // Aggregate and Idiosyncratics Shocks
    Parms_.idshock = (int *) malloc(T*N_AGENTS*sizeof(int));
    Parms_.agshock = (int *) malloc(T*sizeof(int));
    
    InitIdAg(&Parms_);
    Parms_.epsilon  = (REAL *) malloc(NSTATES_ID*sizeof(REAL));
    Parms_.epsilon2 = (REAL *) malloc(NSTATES_ID*sizeof(REAL));
    
    Parms_.ag  = (REAL *) malloc(NSTATES_AG*sizeof(REAL));
    Parms_.ag2 = (REAL *) malloc(NSTATES_AG*sizeof(REAL));
    Init_parmshocks(&Parms_);
    
    // Capital Grid
    Parms_.k = (REAL *) malloc(NKGRID*sizeof(REAL));
    Parms_.km = (REAL *) malloc(NKM_GRID*sizeof(REAL));
    Parms_.kprime  = (REAL *) malloc(NSTATES*sizeof(REAL));
    Parms_.kprimen = (REAL *) malloc(NSTATES*sizeof(REAL));
    Parms_.cons  = (REAL *) malloc(NSTATES*sizeof(REAL));
    Parms_.kprime_interp = (REAL *) malloc(NKGRID*NSTATES_ID*sizeof(REAL));
    Parms_.V = (REAL *) malloc(NSTATES*sizeof(REAL));
    Parms_.Vinterp = (REAL *) malloc(NSTATES*sizeof(REAL));
    Parms_.Vimp = (REAL *) malloc(NSTATES*sizeof(REAL));
    Parms_.G = (int *) malloc(NSTATES*sizeof(int));
    Parms_.EV = (REAL *) malloc(NSTATES*sizeof(REAL));
    
    
    Parms_.kcross = (REAL *) malloc(N_AGENTS*sizeof(REAL));
    Parms_.kcross_l = (REAL *) malloc(N_AGENTS*sizeof(REAL));
    InitK(&Parms_);
    
    // OLS Regression
    Parms_.coeff = (REAL *) malloc(NCOEFF*sizeof(REAL));
    Parms_.coeff1 = (REAL *) malloc(NCOEFF*sizeof(REAL));
    Parms_.coeff[0]=0;
    Parms_.coeff[1]=1;
    Parms_.coeff[2]=0;
    Parms_.coeff[3]=1;
    
    Parms_.R2 = (REAL *) malloc(NSTATES_AG*sizeof(REAL));
    
    
    Parms_.kmts  = (REAL *) malloc(T*sizeof(REAL));
    
    // Prellocation Aggregate/Individual Matrixes
    Parms_.wage  = (REAL *) malloc(NSTATES_AG*NKM_GRID*sizeof(REAL));
    Parms_.irate = (REAL *) malloc(NSTATES_AG*NKM_GRID*sizeof(REAL));
    Parms_.kmprime = (REAL *) malloc(NSTATES_AG*NKM_GRID*sizeof(REAL));
    Parms_.trate = (REAL *) malloc(NSTATES_AG*sizeof(REAL));
    Parms_.wealth = (REAL *) malloc(NSTATES*sizeof(REAL));
    
    // Auxiliary Interpolation Matrixies
    // Agents Problem
    Parms_.I4_.p= (REAL *) malloc(4*sizeof(REAL));
    Parms_.I2_.p= (REAL *) malloc(2*sizeof(REAL));
    Parms_.I1_.p= (REAL *) malloc(1*sizeof(REAL));
    
    Parms_.M4_.p  = (REAL *) malloc(4*sizeof(REAL));
    Parms_.M2_.p  = (REAL *) malloc(2*sizeof(REAL));
    
    InitPrealloc(&Parms_);

    // Performance counters
    perf_ctr pctr_main = INIT_PERF_CTR;
    perf_ctr pctr_alm = INIT_PERF_CTR;
    // perf_ctr pctr_agents = INIT_PERF_CTR; not currently using!
    perf_ctr pctr_agents_egm = INIT_PERF_CTR;
    perf_ctr pctr_agg_st = INIT_PERF_CTR;
    perf_ctr pctr_alm_coeff = INIT_PERF_CTR;

#ifdef PRINTIT
    checkwithmatlab(&Parms_);
#endif
    printf("Solving the Model\n");
 
#if DETAILED_PERF_METRICS
    perf_ctr_start(&pctr_main);
#else
    clock_t tic = clock();
#endif

    REAL metric_coeff = pow(10.,10.); //difference between coefficients B of the the ALM on% successive iterations; initially, set to a large number
    int num_iter = 0;
    int iter_coeff = 0;
    while(metric_coeff>TOLL_COEFF){
    if (iter_coeff >= num_iter & num_iter != 0) {
        break;
    }
    //for (int iter_coeff = 0; iter_coeff < num_iter; iter_coeff = iter_coeff+1) {

        // Aggregate Law of Motion
    #if DETAILED_PERF_METRICS
        perf_ctr_start(&pctr_alm);
    #endif
        AggregatesLawMotion(&Parms_);
        for(int i=0;i<8;i++){
            FILE* cfile = fopen("checks/kmprime.txt", "a");
            fprintf(cfile, "%lf\n", Parms_.kmprime[i]);
            fclose(cfile);
        }
    #if DETAILED_PERF_METRICS
        perf_ctr_update(&pctr_alm);
    #endif
        // Agents Problem
#ifdef VFISOLVER
        // VFI
    #if DETAILED_PERF_METRICS
        perf_ctr_start(&pctr_agents);
    #endif
        Agents(&Parms_);
    #if DETAILED_PERF_METRICS
        perf_ctr_update(&pctr_agents);
    #endif
#else
        // Endogenous grid method
    #if DETAILED_PERF_METRICS
        perf_ctr_start(&pctr_agents_egm);
    #endif
        AgentsEGM(&Parms_);
        FILE* cfile = fopen("kprime.txt", "w");
        for(int is=0;is<NSTATES;is++){
            fprintf(cfile, "%lf\n", Parms_.kprime[is]);
        }
        fclose(cfile);
    #if DETAILED_PERF_METRICS
        perf_ctr_update(&pctr_agents_egm);
    #endif
#endif
        //loadprime(&Parms_);
#ifdef PRINTCHECK
        printf("\nV\n");
        for(int ia=0;ia<NSTATES_AG;ia++)
            for(int iid=0;iid<NSTATES_ID;iid++)
            {
                printf("(:,:,%d,%d)\n",ia+1,iid+1);
                for(int ik=0;ik<NKGRID;ik++)
                {
                    for(int ikm=0;ikm<NKM_GRID;ikm++)
                        printf("%19.15lf ",Parms_.V[IXV(ia,ikm,iid,ik)]);
                    printf("\n");
                }
                printf("\n");
                
            }
#endif
        // Aggregate Transition
    #if DETAILED_PERF_METRICS
        perf_ctr_start(&pctr_agg_st);
    #endif

        for(int is=0;is<N_AGENTS;is++){
            Parms_.kcross_l[is] = Parms_.kcross[is];
        }
        Aggregate_ST(&Parms_);
        cfile = fopen("kcross.txt", "w");
        for(int is=0;is<N_AGENTS;is++){
            fprintf(cfile, "%.15lf\n", Parms_.kcross_l[is]);
        }
        fclose(cfile);
    #if DETAILED_PERF_METRICS
        perf_ctr_update(&pctr_agg_st);
        perf_ctr_start(&pctr_alm_coeff);
    #endif
        // Predicting the Distribution
        ALMCoefficients(&Parms_);
    #if DETAILED_PERF_METRICS
        perf_ctr_update(&pctr_alm_coeff);
    #endif
        // Compute the Convergence Metric
        // Convergence in Euclidean Norm
#if(CONVERGENCE_COEF_NORM==1)
        REAL norm=0.;
        for(int ib=0;ib<NCOEFF;ib++)
            norm +=(Parms_.coeff1[ib]-Parms_.coeff[ib])*(Parms_.coeff1[ib]-Parms_.coeff[ib]);
        metric_coeff = sqrt(norm);
#elif(CONVERGENCE_COEF_NORM==2)
        // Convergence in max(abs))
        REAL spread_coeff=-99999999999999;
        for(int ib=0;ib<NCOEFF;ib++)
        {
            REAL spread_coeff_actual = fabs(Parms_.coeff1[ib]-Parms_.coeff[ib]);
            if(spread_coeff_actual>spread_coeff)
            {
                spread_coeff = spread_coeff_actual;
            }
            
        }
        
        // Metric coeff
        metric_coeff = spread_coeff;
#endif

        #if !KCTEST
        if (metric_coeff>TOLL_COEFF*100){
            // Replace the old with new capital distribution
            for(int j=0;j<N_AGENTS;j++)
                Parms_.kcross[j] = Parms_.kcross_l[j];
        }
        #endif
        cfile = fopen("updated_kcross.txt", "w");
        for(int is=0;is<N_AGENTS;is++){
            fprintf(cfile, "%.15lf\n", Parms_.kcross[is]);
        }
        fclose(cfile);
        // Update the vector of the ALM coefficients
        for(int ib=0;ib<NCOEFF;ib++)
            Parms_.coeff[ib]= Parms_.coeff1[ib]*UPDATE_B + Parms_.coeff[ib]*(1.-UPDATE_B);
        
        iter_coeff++;
#ifdef SHOWMETRIC_DCOEF
        printf("\nIteration: %d Spread: %10.9lf Regression Coeff: Bad: %18.15lf %18.15lf R2: %18.15lf, Good: %18.15lf %18.15lf R2: %18.15lf\n",iter_coeff,metric_coeff,Parms_.coeff[0],Parms_.coeff[1],Parms_.R2[0],Parms_.coeff[2],Parms_.coeff[3],Parms_.R2[1]);
#endif
    } // Loop over aggregate law of motion

    // Performance metrics (main)
#if DETAILED_PERF_METRICS
    perf_ctr_update(&pctr_main);
    REAL totTime = get_pctr_secs(pctr_main);
#else
    clock_t toc = clock();
    REAL totTime = (REAL)(toc - tic) / CLOCKS_PER_SEC;
#endif

    Parms_.solTime=totTime;
    printf("\nModel n. %d: Execution Time: %f seconds Solution Time %f\n",0,totTime,Parms_.solTime);

    // Performance metrics (detail)
#if DETAILED_PERF_METRICS
    printf("\n---- perf_ctrs (secs, cycles/iter, iters) -----\n");
    printf("  alm: %f s, %f cyc/iter, (%d iters)\n",
           get_pctr_secs(pctr_alm),
           get_pctr_avg_cycles(pctr_alm),
           pctr_alm.iters
    );
    printf("  agents_egm: %f s, %f cyc/iter, (%d iters)\n",
           get_pctr_secs(pctr_agents_egm),
           get_pctr_avg_cycles(pctr_agents_egm),
           pctr_agents_egm.iters
    );
    printf("  agg_st: %f s, %f cyc/iter, (%d iters)\n",
           get_pctr_secs(pctr_agg_st),
           get_pctr_avg_cycles(pctr_agg_st),
           pctr_agg_st.iters
    );
    printf("  alm_coeff: %f s, %f cyc/iter, (%d iters)\n",
           get_pctr_secs(pctr_alm_coeff),
           get_pctr_avg_cycles(pctr_alm_coeff),
           pctr_alm_coeff.iters
    );
#endif

#ifdef PERF_SPECIAL
    printf("\n---- EGM perf_ctrs (secs, cycles/iter, iters) -----\n");
    printf("  f: %f s, %f cyc/iter, (%d iters)\n",
           get_pctr_secs(pctr_f),
           get_pctr_avg_cycles(pctr_f),
           pctr_f.iters
    );
    printf("  g: %f s, %f cyc/iter, (%d iters)\n",
           get_pctr_secs(pctr_g),
           get_pctr_avg_cycles(pctr_g),
           pctr_g.iters
    );

    printf("\n---- ST perf_ctrs (secs, cycles/iter, iters) -----\n");
    printf("  b: %f s, %f cyc/iter, (%d iters)\n",
           get_pctr_secs(pctr_b),
           get_pctr_avg_cycles(pctr_b),
           pctr_b.iters
    );
    printf("  c: %f s, %f cyc/iter, (%d iters)\n",
           get_pctr_secs(pctr_c),
           get_pctr_avg_cycles(pctr_c),
           pctr_c.iters
    );
    printf("  d: %f s, %f cyc/iter, (%d iters)\n",
           get_pctr_secs(pctr_d),
           get_pctr_avg_cycles(pctr_d),
           pctr_d.iters
    );
    printf("  e: %f s, %f cyc/iter, (%d iters)\n",
           get_pctr_secs(pctr_e),
           get_pctr_avg_cycles(pctr_e),
           pctr_e.iters
    );
#endif

    printf("\n");
    
    // Deallocate Memory
    free(Parms_.ur);
    free(Parms_.er);
    free(Parms_.P);
    free(Parms_.idshock);
    free(Parms_.agshock);
    free(Parms_.epsilon);
    free(Parms_.epsilon2);
    free(Parms_.ag);
    free(Parms_.ag2);
    free(Parms_.k);
    free(Parms_.km);
    free(Parms_.kprime);
    free(Parms_.kprimen);
    free(Parms_.cons);
    free(Parms_.kprime_interp);
    free(Parms_.V);
    free(Parms_.Vinterp);
    free(Parms_.Vimp);
    free(Parms_.G);
    free(Parms_.EV);
    free(Parms_.kcross);
    free(Parms_.kcross_l);
    free(Parms_.coeff);
    free(Parms_.coeff1);
    free(Parms_.R2);
    free(Parms_.kmts);
    free(Parms_.wage);
    free(Parms_.irate);
    free(Parms_.kmprime);
    free(Parms_.trate);
    free(Parms_.wealth);
    free(Parms_.I4_.p);
    free(Parms_.I2_.p);
    free(Parms_.I1_.p);
    free(Parms_.M4_.p);
    free(Parms_.M2_.p);
    
    return 0;
}
