/*
 Date: 2017-12-13
 File: auxiliary.h (Version: 1.0)
 Description: Auxiliary Functions:
    1. allocate2dREAL: Dynamic Allocation of 2D arrays
    2. deallocate2dREAL: Dynamic DeAllocation of 2D arrays
    3. Model1SobolNodes: Read Sobol Nodes from Outside File
 Copyright: Alessandro Peri, peri.alessandro@gmail.com
 */

#ifndef __FILE_AUX_FUNCS_H_SEEN__
#define __FILE_AUX_FUNCS_H_SEEN__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "usr_decl.h"

REAL ** allocate2dREAL(size_t dim_1,size_t dim_2)
{
    REAL ** array;
    int i_1;
    
    // Check valid dimension: (avoid allocation of size 0 bytes)
    assert(dim_1>0);
    assert(dim_2>0);
    
    // DIMENSION PER DIMENSION
    
    // ----------------------------------------------------------------------------------------------
    array = NULL;
    array= (REAL**)malloc(dim_1 * sizeof(REAL*));
    
    if(array != NULL)
    {
        // ----------------------------------------------------------------------------------------------
        for(i_1=0;i_1<dim_1;i_1++)
        {
            array[i_1] = NULL;
            array[i_1]= (REAL*)malloc(dim_2 * sizeof(REAL));
            
            
            if(array[i_1]== NULL)
            {
                //printf("Memory not assigned");
                exit(-1);
                
            } // End if memory not assigned
            
        } // i_1
        
    }
    else // if memory not assigned
    {
        //printf("Memory not assigned");
        exit(-1);
    }
    
    return array;
}
//void deallocate2dREAL(REAL *** array,size_t dim_1);
void deallocate2dREAL(     REAL *** array,size_t dim_1)
{
    int i_1;
    
    for(i_1=0;i_1<dim_1;i_1++)
    {
        if((*array)[i_1]!=NULL)
        {
            free((*array)[i_1]);
            (*array)[i_1]=NULL;
            
        }
    }
    free((*array));
    (*array) = NULL;
}

int Model1SobolNodes(double ** ParmsSobol)
{
    
    // **************************************************************************************************
    
    //                          CALIBRATION NODES 1: READING SOBOL NODES FROM FILE
    
    // **************************************************************************************************
    
    printf("\n ----------------------------------------- SOBOL NODES -------------------------------------------------------");
    printf("\n1. Reading Sobol Nodes from file: \n");
    
    double value;
    int i,j;
    char filename[150] ={'\0'};
    char N_char   [20] ={'\0'};
    char M_char   [20] ={'\0'};
    
    // --------------------------------------------------------------------------------------------
    // Name the Sobol Nodes file:~/sobol_M_N.txt
    strcpy(filename,MAINPATH);
    strcat(filename,"sobol_");
    sprintf(N_char,"%d",N_MODEL);
    sprintf(M_char,"%d",N_PARMS_TBC);
    strcat(filename,M_char);
    strcat(filename,"_");
    strcat(filename,N_char);
    strcat(filename,".txt");
    printf("%s",filename);
    
    // --------------------------------------------------------------------------------------------
    // Read the Sobol Nodes
    if((GLOBAL_SEARCH==1) & (N_MODEL == 1))
    {
        fprintf(stderr,"\nERROR: Global Search and N_MODEL == 1 incompatible.\n\n\n\n");
         exit(EXIT_FAILURE);
    }
    
    
    FILE *fp;
    fp = fopen(filename, "r");
    if(fp == NULL)
    {
        fprintf(stderr,"Sobol filename: %s",filename);
        fprintf(stderr,"\n1.1. Cannot read ~/sobol_M_N.txt");
        exit(EXIT_FAILURE);
    }
    else
    {
        fprintf(stderr,"\n1.1. Open Sobol file.txt: ~/sobol_M_N.txt: DONE.");
    }
    
    
    // --------------------------------------------------------------------------------------------
    // Check there are enough nodes
    i = 0;
    while(fscanf(fp,"%lf",&value)== 1)
    {
        i = i+1;
        //printf("(%d,%lf) ",i,value);
        
    }
    
    printf("\n1.2. Check number of elements read from ~/sobol_M_N.txt is consistent:");
    if (i!= N_PARMS_TBC*N_MODEL)
    {
        printf(" ERROR: %d != %d\n",i, N_PARMS_TBC*N_MODEL);
        exit(EXIT_FAILURE);
        
    }
    else{
        
        printf("DONE\n");
        
        // Rewind the fp. Sets the file position to the beginning of the file of the given stream.
        rewind(fp);
        
        
        for(i=0;i<N_MODEL;i++)
            for(j=0;j<N_PARMS_TBC;j++)
            {
                if(fscanf(fp, "%lf", &ParmsSobol[i][j])!=1){
                    printf("\n Cannot fscanf ~/sobol_M_N.txt\n");
                    exit(EXIT_FAILURE);
                    
                }
            }
    }
    
    
    
    
    fclose(fp);
    fp = NULL;
    
    
    
    /*
     #if SHOWSOBOLNODES
     
     for(i=0;i<N_MODEL;i++){
     
        for(j=0;j<N_PARMS_TBC;j++)
        {
            printf("%lf ",ParmsSobol[i][j]);
        }
        printf("\n");
     }
     
     #endif
     */
    
    
    // ------------------------------ END CALIBRATION NODES: READING SOBOL NODES FROM FILE ----------------------- //
    
    
    
    
    
    
    return 1;
    
}

#endif
