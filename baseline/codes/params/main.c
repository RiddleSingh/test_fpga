/*
 Date: 2017-12-13
 File: main.c (Version: 1.0)
 Description: Main File: 
 1. Loads Parameters,
 2. Initializes Grids on Capital and TFP, 
 3. Initializes Value Function and Wealth Function
 4. Launches the FPGA
 5. Prints the Results
 6. Returns 0 if Successful
 Copyright: Alessandro Peri, peri.alessandro@gmail.com
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include "usr_decl.h"
#include "auxiliary.h"

#ifdef FPGA_DEBUG_MODE_ON
const int FPGA_ITERATIONS = 1352;
#endif

int main()
{
    char filename [150] = {'\0'};
    char modelnumber[20] = {'\0'};
    
    // Load parameters
    parameters params;
    Parameters_load(&params);
    
    printf("alpha: %lf\n",params.alpha);
    printf("beta: %lf\n",params.beta);
    printf("delta: %lf\n",params.delta);
    printf("eta: %lf\n",params.eta);
    printf("mu: %lf\n",params.mu);
    printf("rho: %lf\n",params.rho);
    printf("sigma: %lf\n",params.sigma);
    printf("lambda: %lf\n",params.lambda);
    printf("nz: %d\n",params.nz);
    printf("nk: %d\n",params.nk);
    printf("TOLL_V: %lf\n",params.TOLL_V);
    
    // Initialization of Grids Size
    int nz = params.nz;
    int nk = params.nk;
    
#if (GLOBAL_SEARCH == 1)
    REAL ** ParmsSobol;
    ParmsSobol = NULL;
    
    ParmsSobol = allocate2dREAL(N_MODEL,N_PARMS_TBC);
    int check_nodes = Model1SobolNodes(ParmsSobol);
    if (check_nodes!=1)
    {
        printf("ERROR. Unable to load Sobol Nodes");
        exit(EXIT_FAILURE);
    }
#endif
    
    for(int i_m=0;i_m<N_MODEL;i_m++){
        
#if (GLOBAL_SEARCH == 1)
        int i_new = 0;
        params.alpha    = ParmsSobol[i_m][i_new] * 0.3   + (1-ParmsSobol[i_m][i_new]) * 0.4  ;i_new++;
        params.delta    = ParmsSobol[i_m][i_new] * 0.005 + (1-ParmsSobol[i_m][i_new]) * 0.015;i_new++;
        params.eta      = ParmsSobol[i_m][i_new] * 1.5   + (1-ParmsSobol[i_m][i_new]) * 2.5  ;i_new++;
        params.rho      = ParmsSobol[i_m][i_new] * 0.9   + (1-ParmsSobol[i_m][i_new]) * 0.98 ;i_new++;
        params.sigma    = ParmsSobol[i_m][i_new] * 0.001 + (1-ParmsSobol[i_m][i_new]) * 0.009;i_new++;
#endif
        
        //printf("Writing the Parameters.\n");
        memset(filename,'\0',sizeof(filename));
        memset(modelnumber,'\0',sizeof(modelnumber));
        
        strcpy(filename,MAINPATH);
        strcat(filename,"/Results/Parameters");
        sprintf(modelnumber,"%d",i_m);
        strcat(filename,modelnumber);
        strcat(filename,".txt");
        
        FILE *fp;
        fp = fopen(filename, "w");
        fprintf(fp,"%19.17lf\n",params.eta);
        fprintf(fp,"%19.17lf\n",params.beta);
        fprintf(fp,"%19.17lf\n",params.alpha);
        fprintf(fp,"%19.17lf\n",params.delta);
        fprintf(fp,"%19.17lf\n",params.mu);
        fprintf(fp,"%19.17lf\n",params.rho);
        fprintf(fp,"%19.17lf\n",params.sigma);
        fprintf(fp,"%19.17lf\n",params.lambda);
        fprintf(fp,"%d\n",params.nk);
        fprintf(fp,"%d\n",params.nz);
        fprintf(fp,"%19.17lf\n",params.TOLL_V);
        fclose(fp);
        
        //printf("Beta: %llx\n",*((uint64_t *) &params.beta));
        
        // Allocation of memory
        clock_t tic = clock();
        size_t sizeK = nk*sizeof(REAL);
        size_t sizeZ = nz*sizeof(REAL);
        size_t sizeP = nz*nz*sizeof(REAL);
        size_t sizeV = nk*nz*sizeof(REAL);
        size_t sizeG = nk*nz*sizeof(int);
        size_t sizeW = nk*nz*sizeof(REAL);
        
        // Dynamic Memory Allocation
        REAL *hK= (double *) malloc(sizeK);
        REAL *hZ= (double *) malloc(sizeZ);
        REAL *hP= (double *) malloc(sizeP);
        REAL *hV0= (double *) malloc(sizeV);
        REAL *hV= (double *) malloc(sizeV);
        int  *hG= (int *) malloc(sizeG);
        REAL *hW= (double *) malloc(sizeW);
        
        // Initializations
        Tauchen(&params, hZ, hP);
        InitializationK(&params, hZ, hK);
        InitializationV(&params, hZ, hV0);
        InitializationW(&params,hZ,hK,hW);
        
        // Print Initializations if READWRITE_INIT is defined
#ifdef READWRITE_INIT
        
        FILE *fpK,*fpP,*fpV,*fpK_hex,*fpP_hex,*fpV_hex,*fpW_hex;
        
        memset(filename,'\0',sizeof(filename));strcpy(filename,MAINPATH);strcat(filename,"/Results/");
        strcat(filename,"k");sprintf(modelnumber,"%d",i_m);strcat(filename,modelnumber);strcat(filename,".txt");
        fpK = fopen(filename,"w");
        
        memset(filename,'\0',sizeof(filename));strcpy(filename,MAINPATH);strcat(filename,"/Results/");
        strcat(filename,"P");sprintf(modelnumber,"%d",i_m);strcat(filename,modelnumber);strcat(filename,".txt");
        fpP = fopen(filename,"w");
        
        memset(filename,'\0',sizeof(filename));strcpy(filename,MAINPATH);strcat(filename,"/Results/");
        strcat(filename,"V0");sprintf(modelnumber,"%d",i_m);strcat(filename,modelnumber);strcat(filename,".txt");
        fpV = fopen(filename,"w");
        
        memset(filename,'\0',sizeof(filename));strcpy(filename,MAINPATH);strcat(filename,"/Results/");
        strcat(filename,"k_hex");sprintf(modelnumber,"%d",i_m);strcat(filename,modelnumber);strcat(filename,".txt");
        fpK_hex = fopen(filename,"w");
        
        memset(filename,'\0',sizeof(filename));strcpy(filename,MAINPATH);strcat(filename,"/Results/");
        strcat(filename,"P_hex");sprintf(modelnumber,"%d",i_m);strcat(filename,modelnumber);strcat(filename,".txt");
        fpP_hex = fopen(filename,"w");
        
        memset(filename,'\0',sizeof(filename));strcpy(filename,MAINPATH);strcat(filename,"/Results/");
        strcat(filename,"V0_hex");sprintf(modelnumber,"%d",i_m);strcat(filename,modelnumber);strcat(filename,".txt");
        fpV_hex = fopen(filename,"w");
        
        memset(filename,'\0',sizeof(filename));strcpy(filename,MAINPATH);strcat(filename,"/Results/");
        strcat(filename,"W_hex");sprintf(modelnumber,"%d",i_m);strcat(filename,modelnumber);strcat(filename,".txt");
        fpW_hex = fopen(filename,"w");
        
        
        for(int ik=0;ik<nk;ik++){
            for(int iz=0;iz<nz;iz++){
                
                //double val;
                //uint64_t val_hex;
                
                fprintf(fpV,"%10.8lf ",hV0[ik*nz + iz]);
                fprintf(fpV_hex,"%llx ",*((uint64_t *) &hV0[ik*nz + iz]));
                fprintf(fpW_hex,"%llx ",*((uint64_t *) &hW[ik*nz + iz]));

            }
            
            fprintf(fpV,"\n");
            fprintf(fpV_hex,"\n");
            fprintf(fpW_hex,"\n");
            fprintf(fpK,"%10.8lf ",hK[ik]);
            fprintf(fpK_hex,"%llx ",*((uint64_t *) &hK[ik])); //val = hK[ik];val_hex= *reinterpret_cast<uint64_t *>(&val);fprintf(fpk,"%llx\n",val_hex);
            
        }
        
        for(int iz=0;iz<nz;iz++){
            for(int izp=0;izp<nz;izp++){
                fprintf(fpP,"%10.8lf ",hP[iz*nz+izp]);
                fprintf(fpP_hex,"%llx ",*((uint64_t *) &hP[iz*nz+izp]));
            }
            fprintf(fpP,"\n");
            fprintf(fpP_hex,"\n");
        }
        
        
        fclose(fpK);
        fclose(fpP);
        fclose(fpV);
        fclose(fpK_hex);
        fclose(fpP_hex);
        fclose(fpV_hex);
        fclose(fpW_hex);
        
        
#endif // END READWRITE_INIT
        
        
       // ----------------------------------------- LAUNCH THE FPGA ---------------------------------- //
#ifdef FPGA_MODE_ON
        printf("\nCopy the files to fpga and launch\n");
        
        // Initialization of FPGA type
        FPGA_setting_type FPGA_;
        
        FPGA_.nk   = params.nk;
        FPGA_.nz   = params.nz;
        FPGA_.beta = params.beta;
        FPGA_.eta  = params.eta;
        FPGA_.TOLL_V  = params.TOLL_V;
        FPGA_.K    = hK;
        FPGA_.P    = hP;
        FPGA_.V0   = hV0;
        FPGA_.W    = hW;
        FPGA_.V    = hV;
        FPGA_.G    = hG;
        FPGA_.Iterations = 0;
        FPGA_.tic = tic;
        FPGA_.startTime = 0.;
        
        
        // Launch FPGA
        int rc = fpgamemcpyandgo(&params,&FPGA_);
        
        printf("Uscito\n");
        
#endif
        
        clock_t toc = clock();
        double totTime = (double)(toc - tic) / CLOCKS_PER_SEC;
        printf("\nExecution time: %f seconds\n",totTime);
        
        // ----------------------------------------- END FPGA EXECUTION ---------------------------------- //
        
        // PRINT THE RESULTS
        printf("writing the files\n");
         
        // Write to file (row major)
        FILE *fileStartTime, *fileSolTime, *fileReadingBackTime, *fileTotalTime, *fileValue, *filePolicy;
        FILE *fileIterations;
        
        // Startime
        memset(filename,'\0',sizeof(filename));strcpy(filename,MAINPATH);strcat(filename,"/Results/");
        strcat(filename,"startTimeFPGA_");sprintf(modelnumber,"%d",i_m);strcat(filename,modelnumber);strcat(filename,".txt");
        fileStartTime = fopen(filename,"w");
        
        // Soltime
        memset(filename,'\0',sizeof(filename));strcpy(filename,MAINPATH);strcat(filename,"/Results/");
        strcat(filename,"solTimeFPGA_");sprintf(modelnumber,"%d",i_m);strcat(filename,modelnumber);strcat(filename,".txt");
        fileSolTime = fopen(filename,"w");
        
        // ReadingBackTime
        memset(filename,'\0',sizeof(filename));strcpy(filename,MAINPATH);strcat(filename,"/Results/");
        strcat(filename,"ReadingBackTimeFPGA_");sprintf(modelnumber,"%d",i_m);strcat(filename,modelnumber);strcat(filename,".txt");
        fileReadingBackTime = fopen(filename,"w");
        
        // TotalTime
        memset(filename,'\0',sizeof(filename));strcpy(filename,MAINPATH);strcat(filename,"/Results/");
        strcat(filename,"totalTimeFPGA_");sprintf(modelnumber,"%d",i_m);strcat(filename,modelnumber);strcat(filename,".txt");
        fileTotalTime = fopen(filename,"w");
        
        // Value Function
        memset(filename,'\0',sizeof(filename));strcpy(filename,MAINPATH);strcat(filename,"/Results/");
        strcat(filename,"valFunFPGA_");sprintf(modelnumber,"%d",i_m);strcat(filename,modelnumber);strcat(filename,".txt");
        fileValue = fopen(filename,"w");
        
        // Policy Function
        memset(filename,'\0',sizeof(filename));strcpy(filename,MAINPATH);strcat(filename,"/Results/");
        strcat(filename,"polFunFPGA_");sprintf(modelnumber,"%d",i_m);strcat(filename,modelnumber);strcat(filename,".txt");
        filePolicy = fopen(filename,"w");
        
        // Iterations
        memset(filename,'\0',sizeof(filename));strcpy(filename,MAINPATH);strcat(filename,"/Results/");
        strcat(filename,"IterationsFPGA_");sprintf(modelnumber,"%d",i_m);strcat(filename,modelnumber);strcat(filename,".txt");
        fileIterations = fopen(filename,"w");
        
        
        // Saving Results
#ifdef FPGA_MODE_ON
        fprintf(fileStartTime,"%lf",FPGA_.startTime);
        fprintf(fileSolTime,"%lf",FPGA_.solTime);
        fprintf(fileReadingBackTime,"%lf",FPGA_.ReadingBackTime);
        fprintf(fileTotalTime,"%lf",totTime);
        fprintf(fileIterations,"%d",FPGA_.Iterations);
#endif
        
        for(int iz = 0 ; iz < nz ; ++iz){
            for(int ik = 0 ; ik < nk ; ++ik){
                fprintf(fileValue,"%32.30lf\n",hV[ik*nz+iz]);
                fprintf(filePolicy,"%d\n",hG[ik*nz+iz]);
            }
        }
        
        // Close Files
        fclose(fileStartTime);
        fclose(fileSolTime);
        fclose(fileReadingBackTime);
        fclose(fileTotalTime);
        fclose(fileValue);
        fclose(filePolicy);
        fclose(fileIterations);
        
        // Free Dynamic Memory
        free(hK);
        free(hZ);
        free(hP);
        free(hV0);
        free(hW);
        free(hV);
        free(hG);

        
    } // i_m


#if (GLOBAL_SEARCH == 1)
    // Free Memory
    deallocate2dREAL(&(ParmsSobol),N_MODEL);
    ParmsSobol = NULL;
#endif
    
    return 0;
    
}
